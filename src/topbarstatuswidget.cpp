#include "topbarstatuswidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDateTime>
#include <QFile>
#include <QCoreApplication>
#include <QFont>
#include <QFontMetrics>
#include <QHostAddress>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QString>

#include "cmdworker.h"
#include "cmdudpmanager.h"
#include "aihelper.h"
#include "globalparams.h"
#include "mylanguage.h"
#include "sortertypes.h"

/* 素材目录（生成指令中约定的文件名） */
#define TOPBAR_ASSET_DIR ":/res/png2/topbar_anim/"

/* 状态卡与按钮布局参数（1024x768 基准） */
#define CARD_LEFT        8
#define CARD_TOP         8
#define CARD_WIDTH       296
#define CARD_HEIGHT      58
#define CARD_GAP         8
#define STOP_WIDTH       186
#define STOP_HEIGHT      56
#define SEGMENT_COUNT    18
#define SEGMENT_LIT_LEN  5

//////////////////////////////// AiStatusPollThread ////////////////////////////////

AiStatusPollThread::AiStatusPollThread(QObject* parent)
    : QThread(parent), m_stopped(true)
{
}

void AiStatusPollThread::startPolling()
{
    m_stopped = false;
    if (!isRunning())
    {
        start();
    }
}

void AiStatusPollThread::stopPolling()
{
    m_stopped = true;
}

void AiStatusPollThread::run()
{
    while (!m_stopped)
    {
        quint32 discard_num = 0;
        quint32 timeout_num = 0;
        const int unit_total = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        const QByteArray request = cmdworker::AiStatusRequest();

        for (int idx = 0; idx < unit_total && !m_stopped; ++idx)
        {
            const QString ip = ai_helper::GetAiIpByIndex(idx);
            QByteArray response;

            // 轮询用短超时，AI离线时不长时间占用UDP通道
            if (!CmdUdpManager::instance().onSendCommand(QHostAddress(ip), AI_UPD_CMD_PORT,
                                                         request, response, 500))
            {
                continue;
            }

            CmdPackage cmd_pkg;
            if (!cmdworker::ParseCmdPkg(response, cmd_pkg))
            {
                continue;
            }

            const AiStatusInfo status_info = cmdworker::AiStatusResponse(cmd_pkg);
            discard_num += status_info.discard_num_;
            timeout_num += status_info.timeout_num_;
        }

        emit statusUpdated(discard_num, timeout_num);

        // 2秒轮询间隔，分段休眠保证停止时能及时退出
        for (int i = 0; i < 20 && !m_stopped; ++i)
        {
            msleep(100);
        }
    }
}

//////////////////////////////// TopBarStatusWidget ////////////////////////////////

TopBarStatusWidget::TopBarStatusWidget(QWidget* parent)
    : QWidget(parent),
      m_running(false),
      m_pressed_stop(false),
      m_fade(0),
      m_fade_target(0),
      m_phase(0),
      m_discard_num(0),
      m_timeout_num(0),
      m_poller(0)
{
    setFixedHeight(TOPBAR_STATUS_HEIGHT);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    m_card_normal      = loadAsset("cardMetricNormal.png");
    m_card_alarm       = loadAsset("cardMetricAlarm.png");
    m_dot_normal       = loadAsset("dotNormal.png");
    m_dot_alarm        = loadAsset("dotAlarm.png");
    m_seg_on           = loadAsset("segOn.png");
    m_seg_off          = loadAsset("segOff.png");
    m_seg_on_alarm     = loadAsset("segOnAlarm.png");
    m_btn_stop         = loadAsset("btnStop.png");
    m_btn_stop_pressed = loadAsset("btnStopPressed.png");
    m_icon_stop        = loadAsset("iconStop.png");
    m_topbar_bg        = loadAsset("topbarBg.png");
    m_icon_clock       = loadAsset("iconClock.png");

    m_anim_timer.setInterval(80);
    connect(&m_anim_timer, SIGNAL(timeout()), this, SLOT(onAnimTimeout()));

    m_clock_timer.setInterval(1000);
    connect(&m_clock_timer, SIGNAL(timeout()), this, SLOT(update()));

    m_poller = new AiStatusPollThread(this);
    connect(m_poller, SIGNAL(statusUpdated(quint32,quint32)),
            this, SLOT(onStatusUpdated(quint32,quint32)), Qt::QueuedConnection);

    hide();
}

TopBarStatusWidget::~TopBarStatusWidget()
{
    if (m_poller)
    {
        m_poller->stopPolling();
        m_poller->wait();
    }
}

void TopBarStatusWidget::setRunning(bool running)
{
    if (m_running == running)
    {
        return;
    }

    m_running = running;

    if (running)
    {
        // 每次开始工作重新计数，等待AI状态回传
        m_discard_num = 0;
        m_timeout_num = 0;
        m_pressed_stop = false;
        m_phase = 0;
        m_fade = 0;
        m_fade_target = 255;

        m_anim_timer.start();
        m_clock_timer.start();
        if (m_poller)
        {
            m_poller->startPolling();
        }

        show();
        raise();
        update();
    }
    else
    {
        if (m_poller)
        {
            m_poller->stopPolling();
        }
        m_fade_target = 0;
        m_anim_timer.start();
    }
}

void TopBarStatusWidget::onStatusUpdated(quint32 discard_num, quint32 timeout_num)
{
    if (!m_running)
    {
        return;
    }

    m_discard_num = discard_num;
    m_timeout_num = timeout_num;
    update(cardRect(0).united(cardRect(1)));
}

void TopBarStatusWidget::onAnimTimeout()
{
    if (m_fade < m_fade_target)
    {
        m_fade = qMin(m_fade_target, m_fade + 48);
        update();
    }
    else if (m_fade > m_fade_target)
    {
        m_fade = qMax(m_fade_target, m_fade - 48);
        update();

        if (m_fade <= 0)
        {
            m_anim_timer.stop();
            m_clock_timer.stop();
            hide();
            return;
        }
    }
    else if (!m_running)
    {
        // 已完全淡出，停止刷新
        m_anim_timer.stop();
        m_clock_timer.stop();
        hide();
        return;
    }

    if (m_running && m_fade >= m_fade_target)
    {
        ++m_phase;
        // 只刷新两张卡片的流光区域
        update(cardRect(0).united(cardRect(1)));
    }
}

QPixmap TopBarStatusWidget::loadAsset(const QString& name) const
{
    const QString res_path = QString(TOPBAR_ASSET_DIR) + name;
    if (QFile::exists(res_path))
    {
        return QPixmap(res_path);
    }

    // 兼容素材未打进qrc时，从可执行文件同级的res目录读取
    const QString disk_path = QCoreApplication::applicationDirPath()
                              + "/res/png2/topbar_anim/" + name;
    if (QFile::exists(disk_path))
    {
        return QPixmap(disk_path);
    }

    return QPixmap();
}

QRect TopBarStatusWidget::cardRect(int index) const
{
    return QRect(CARD_LEFT + index * (CARD_WIDTH + CARD_GAP), CARD_TOP, CARD_WIDTH, CARD_HEIGHT);
}

QRect TopBarStatusWidget::stopRect() const
{
    return QRect(cardRect(1).right() + 1 + CARD_GAP, 12, STOP_WIDTH, STOP_HEIGHT);
}

QRect TopBarStatusWidget::clockRect() const
{
    return QRect(width() - 196, 0, 184, height());
}

void TopBarStatusWidget::paintEvent(QPaintEvent*)
{
    if (m_fade <= 0)
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setOpacity(qBound(0, m_fade, 255) / 255.0);

    // 顶栏底色
    if (!m_topbar_bg.isNull())
    {
        painter.drawPixmap(rect(), m_topbar_bg);
    }
    else
    {
        QLinearGradient bg(0, 0, 0, height());
        bg.setColorAt(0.0, QColor(4, 16, 28));
        bg.setColorAt(1.0, QColor(8, 26, 40));
        painter.fillRect(rect(), bg);
    }

    // 相机丢包
    const bool discard_alarm = (m_discard_num > 0);
    drawCard(painter, cardRect(0), myLan.ai_front_discard_num, "CAMERA PACKET LOSS",
             m_discard_num, QString::fromUtf8("帧"),
             discard_alarm ? "ABNORMAL" : "NORMAL", discard_alarm);

    // 推理延迟超时
    const bool timeout_alarm = (m_timeout_num > 0);
    drawCard(painter, cardRect(1), myLan.ai_front_delay_num, "INFER TIMEOUT",
             m_timeout_num, myLan.times,
             timeout_alarm ? "ABNORMAL" : "NORMAL", timeout_alarm);

    drawStopButton(painter, stopRect());
    drawClock(painter, clockRect());
}

void TopBarStatusWidget::drawCard(QPainter& painter, const QRect& card, const QString& title,
                                  const QString& subtitle, quint32 value, const QString& unit,
                                  const QString& status, bool alarm) const
{
    const QColor accent = alarm ? QColor(255, 79, 94) : QColor(53, 224, 138);
    const QPixmap& bg = alarm ? m_card_alarm : m_card_normal;

    if (!bg.isNull())
    {
        drawNineSlice(painter, bg, card, 48, 16);
    }
    else
    {
        // 外发光
        for (int i = 3; i >= 1; --i)
        {
            QColor glow = accent;
            glow.setAlpha(18 - i * 4);
            painter.setPen(QPen(glow, 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(QRectF(card).adjusted(-i, -i, i, i), 6, 6);
        }

        // 卡片底
        QLinearGradient fill(card.topLeft(), card.bottomLeft());
        fill.setColorAt(0.0, QColor(11, 27, 43, 245));
        fill.setColorAt(1.0, QColor(6, 18, 30, 245));
        const QRectF body = QRectF(card).adjusted(0.5, 0.5, -0.5, -0.5);
        painter.setBrush(fill);
        painter.setPen(QPen(alarm ? QColor(180, 68, 78) : QColor(42, 127, 150), 1));
        painter.drawRoundedRect(body, 4, 4);

        // 四角高亮
        const int len = 8;
        painter.setPen(QPen(accent, 1));
        painter.drawLine(QPointF(body.left(), body.top() + len), QPointF(body.left(), body.top()));
        painter.drawLine(QPointF(body.left(), body.top()), QPointF(body.left() + len, body.top()));
        painter.drawLine(QPointF(body.right() - len, body.top()), QPointF(body.right(), body.top()));
        painter.drawLine(QPointF(body.right(), body.top()), QPointF(body.right(), body.top() + len));
        painter.drawLine(QPointF(body.left(), body.bottom() - len), QPointF(body.left(), body.bottom()));
        painter.drawLine(QPointF(body.left(), body.bottom()), QPointF(body.left() + len, body.bottom()));
        painter.drawLine(QPointF(body.right() - len, body.bottom()), QPointF(body.right(), body.bottom()));
        painter.drawLine(QPointF(body.right(), body.bottom()), QPointF(body.right(), body.bottom() - len));
    }

    // 状态指示灯
    const QRect dot_rect(card.left() + 12, card.top() + 12, 14, 14);
    const QPixmap& dot_pm = alarm ? m_dot_alarm : m_dot_normal;
    if (!dot_pm.isNull())
    {
        painter.drawPixmap(dot_rect, dot_pm);
    }
    else
    {
        QRadialGradient radial(dot_rect.center(), 10);
        radial.setColorAt(0.0, QColor(accent.red(), accent.green(), accent.blue(), 170));
        radial.setColorAt(0.45, accent);
        radial.setColorAt(1.0, QColor(accent.red(), accent.green(), accent.blue(), 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(radial);
        painter.drawEllipse(dot_rect.center(), 10, 10);
    }

    // 中文标题
    QFont title_font = font();
    title_font.setPixelSize(15);
    title_font.setBold(true);
    painter.setFont(title_font);
    painter.setPen(QColor(234, 246, 255));
    const QRect title_rect(card.left() + 34, card.top() + 6, card.width() - 180, 20);
    const QFontMetrics title_metrics(title_font);
    painter.drawText(title_rect, Qt::AlignLeft | Qt::AlignVCenter,
                     title_metrics.elidedText(title, Qt::ElideRight, title_rect.width()));

    // 英文副标题
    QFont sub_font = font();
    sub_font.setPixelSize(8);
    sub_font.setBold(false);
    sub_font.setLetterSpacing(QFont::AbsoluteSpacing, 1.0);
    painter.setFont(sub_font);
    painter.setPen(QColor(78, 143, 166));
    painter.drawText(QRect(card.left() + 34, card.top() + 27, card.width() - 180, 12),
                     Qt::AlignLeft | Qt::AlignVCenter, subtitle);

    // 实时数值
    const QString value_text = QString::number(value);
    QFont value_font = font();
    value_font.setBold(true);
    if (value_text.length() >= 4)
    {
        value_font.setPixelSize(22);
    }
    else if (value_text.length() == 3)
    {
        value_font.setPixelSize(26);
    }
    else
    {
        value_font.setPixelSize(30);
    }
    painter.setFont(value_font);
    painter.setPen(accent);
    painter.drawText(QRect(card.left() + 120, card.top() + 4, 94, 40),
                     Qt::AlignRight | Qt::AlignVCenter, value_text);

    // 单位
    QFont unit_font = font();
    unit_font.setPixelSize(11);
    unit_font.setBold(false);
    painter.setFont(unit_font);
    painter.setPen(QColor(127, 166, 181));
    painter.drawText(QRect(card.left() + 218, card.top() + 28, 34, 16),
                     Qt::AlignLeft | Qt::AlignVCenter, unit);

    // NORMAL / ABNORMAL
    QFont status_font = font();
    status_font.setPixelSize(10);
    status_font.setBold(true);
    status_font.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    painter.setFont(status_font);
    painter.setPen(accent);
    painter.drawText(QRect(card.left() + 236, card.top() + 8, 52, 16),
                     Qt::AlignRight | Qt::AlignVCenter, status);

    drawSegments(painter, card, alarm);
}

void TopBarStatusWidget::drawSegments(QPainter& painter, const QRect& card, bool alarm) const
{
    const int left = card.left() + 34;
    const int right = card.right() - 10;
    const int seg_width = 10;
    const int gap = qMax(2, (right - left - SEGMENT_COUNT * seg_width) / (SEGMENT_COUNT - 1));
    const int y = card.bottom() - 10;
    const int lit_start = m_phase % SEGMENT_COUNT;

    for (int i = 0; i < SEGMENT_COUNT; ++i)
    {
        const QRect rect(left + i * (seg_width + gap), y, seg_width, 5);
        const bool lit = ((i - lit_start + SEGMENT_COUNT) % SEGMENT_COUNT) < SEGMENT_LIT_LEN;
        const QPixmap& pm = lit ? (alarm ? m_seg_on_alarm : m_seg_on) : m_seg_off;

        if (!pm.isNull())
        {
            painter.drawPixmap(rect, pm);
        }
        else
        {
            QColor color;
            if (lit)
            {
                color = alarm ? QColor(255, 91, 110) : QColor(53, 214, 160);
            }
            else
            {
                color = QColor(18, 48, 60);
            }
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.drawRoundedRect(rect, 2.5, 2.5);
        }
    }
}

void TopBarStatusWidget::drawStopButton(QPainter& painter, const QRect& button) const
{
    const QPixmap& bg = m_pressed_stop ? m_btn_stop_pressed : m_btn_stop;

    if (!bg.isNull())
    {
        drawNineSlice(painter, bg, button, 88, 26);
    }
    else
    {
        // 外发光
        for (int i = 4; i >= 1; --i)
        {
            QColor glow(255, 91, 110);
            glow.setAlpha(m_pressed_stop ? 10 : (22 - i * 3));
            painter.setPen(QPen(glow, 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(QRectF(button).adjusted(-i, -i, i, i),
                                    button.height() / 2.0, button.height() / 2.0);
        }

        QLinearGradient fill(button.topLeft(), button.bottomLeft());
        fill.setColorAt(0.0, m_pressed_stop ? QColor(110, 26, 38, 230) : QColor(59, 17, 25, 200));
        fill.setColorAt(1.0, m_pressed_stop ? QColor(74, 16, 25, 230) : QColor(38, 10, 16, 200));
        painter.setBrush(fill);
        painter.setPen(QPen(QColor(255, 91, 110), 1));
        painter.drawRoundedRect(QRectF(button).adjusted(0.5, 0.5, -0.5, -0.5),
                                button.height() / 2.0, button.height() / 2.0);
    }

    // 停止图标
    const QRect icon_rect(button.left() + 20, button.center().y() - 15, 30, 30);
    if (!m_icon_stop.isNull())
    {
        painter.drawPixmap(icon_rect, m_icon_stop);
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(255, 91, 110), 2));
        painter.drawEllipse(icon_rect.adjusted(1, 1, -1, -1));

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(234, 246, 255));
        painter.drawRoundedRect(icon_rect.adjusted(9, 9, -9, -9), 2, 2);
    }

    // 按钮文字
    QFont text_font = font();
    text_font.setPixelSize(17);
    text_font.setBold(true);
    painter.setFont(text_font);
    painter.setPen(QColor(255, 122, 138));
    painter.drawText(button.adjusted(58, 0, -10, 0),
                     Qt::AlignLeft | Qt::AlignVCenter, myLan.working_stop);
}

void TopBarStatusWidget::drawClock(QPainter& painter, const QRect& area) const
{
    const QDateTime now = QDateTime::currentDateTime();
    const QRect icon_rect(area.left() + 6, area.center().y() - 21, 42, 42);

    if (!m_icon_clock.isNull())
    {
        painter.drawPixmap(icon_rect, m_icon_clock);
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(53, 214, 230), 2));
        painter.drawEllipse(icon_rect.adjusted(2, 2, -2, -2));
        const QPoint center = icon_rect.center();
        painter.drawLine(center, QPoint(center.x(), center.y() - 12));
        painter.drawLine(center, QPoint(center.x() + 9, center.y() + 5));
    }

    QFont time_font = font();
    time_font.setPixelSize(11);
    time_font.setBold(false);
    painter.setFont(time_font);
    painter.setPen(QColor(159, 195, 210));
    painter.drawText(QRect(area.left() + 58, area.top() + 16, 120, 18),
                     Qt::AlignLeft | Qt::AlignVCenter, now.toString("yyyy-MM-dd"));

    time_font.setPixelSize(16);
    time_font.setBold(true);
    painter.setFont(time_font);
    painter.setPen(QColor(234, 246, 255));
    painter.drawText(QRect(area.left() + 58, area.top() + 36, 120, 26),
                     Qt::AlignLeft | Qt::AlignVCenter, now.toString("HH:mm:ss"));
}

void TopBarStatusWidget::drawNineSlice(QPainter& painter, const QPixmap& pixmap, const QRect& target,
                                       int src_slice, int dst_margin) const
{
    if (pixmap.isNull())
    {
        return;
    }

    const int src_w = pixmap.width();
    const int src_h = pixmap.height();
    const int slice = qMin(src_slice, qMin(src_w, src_h) / 2);
    const int margin = qMin(dst_margin, qMin(target.width(), target.height()) / 2);

    if (slice <= 0 || margin <= 0
        || target.width() <= 2 * margin || target.height() <= 2 * margin)
    {
        painter.drawPixmap(target, pixmap);
        return;
    }

    const int src_x[4] = {0, slice, src_w - slice, src_w};
    const int src_y[4] = {0, slice, src_h - slice, src_h};
    const int dst_x[4] = {target.left(), target.left() + margin,
                          target.right() + 1 - margin, target.right() + 1};
    const int dst_y[4] = {target.top(), target.top() + margin,
                          target.bottom() + 1 - margin, target.bottom() + 1};

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            const QRect src(src_x[col], src_y[row],
                            src_x[col + 1] - src_x[col], src_y[row + 1] - src_y[row]);
            const QRect dst(dst_x[col], dst_y[row],
                            dst_x[col + 1] - dst_x[col], dst_y[row + 1] - dst_y[row]);
            painter.drawPixmap(dst, pixmap, src);
        }
    }
}

void TopBarStatusWidget::mousePressEvent(QMouseEvent* event)
{
    if (m_running && event->button() == Qt::LeftButton && stopRect().contains(event->pos()))
    {
        m_pressed_stop = true;
        update(stopRect());
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void TopBarStatusWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_pressed_stop && event->button() == Qt::LeftButton)
    {
        const bool inside = stopRect().contains(event->pos());
        m_pressed_stop = false;
        update(stopRect());

        if (inside)
        {
            emit stopRequested();
        }
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}
