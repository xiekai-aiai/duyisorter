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
#include <QImage>

#include "cmdworker.h"
#include "cmdudpmanager.h"
#include "aihelper.h"
#include "globalparams.h"
#include "mylanguage.h"
#include "sortertypes.h"

/* 素材目录（生成指令中约定的文件名） */
#define TOPBAR_ASSET_DIR ":/res/png2/topbar_anim/"

/* 状态面板与按钮布局参数（1024x768 基准）
   运行态顶部为“一个面板 + 一条跑马灯”，面板内左右两个指标 */
#define PANEL_LEFT       8
#define PANEL_TOP        6
#define PANEL_WIDTH      610
#define PANEL_HEIGHT     62
#define METRIC_WIDTH     (PANEL_WIDTH / 2)
#define STOP_LEFT        630
#define STOP_WIDTH       150
#define STOP_HEIGHT      50
#define SEGMENT_COUNT    30
#define SEGMENT_WIDTH    14
#define SEGMENT_LIT_LEN  8

/* 去掉素材四周的透明留白：卡片素材画布带大边距，直接绘制会把边框缩到内侧。
   alpha 阈值取 60，保留描边与 1px 柔边，同时裁掉低透明度的外发光留白 */
static QPixmap trimTransparentMargins(const QPixmap& src)
{
    if (src.isNull())
    {
        return src;
    }

    const QImage image = src.toImage().convertToFormat(QImage::Format_ARGB32);
    int left = image.width();
    int top = image.height();
    int right = -1;
    int bottom = -1;

    for (int y = 0; y < image.height(); ++y)
    {
        const QRgb* line = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x)
        {
            if (qAlpha(line[x]) <= 60)
            {
                continue;
            }
            if (x < left)   { left = x; }
            if (x > right)  { right = x; }
            if (y < top)    { top = y; }
            if (y > bottom) { bottom = y; }
        }
    }

    if (right < left || bottom < top)
    {
        return src;
    }

    return src.copy(QRect(left, top, right - left + 1, bottom - top + 1));
}

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

    m_dot_normal       = loadAsset("dotNormal.png");
    m_dot_alarm        = loadAsset("dotAlarm.png");
    m_seg_on           = loadAsset("segOn.png");
    m_seg_off          = loadAsset("segOff.png");
    m_seg_on_alarm     = loadAsset("segOnAlarm.png");
    m_btn_stop         = trimTransparentMargins(loadAsset("btnStop.png"));
    m_btn_stop_pressed = trimTransparentMargins(loadAsset("btnStopPressed.png"));
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
    update(panelRect());
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
        // 只刷新面板内的跑马灯区域
        update(panelRect());
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

QRect TopBarStatusWidget::panelRect() const
{
    return QRect(PANEL_LEFT, PANEL_TOP, PANEL_WIDTH, PANEL_HEIGHT);
}

QRect TopBarStatusWidget::metricRect(int index) const
{
    return QRect(PANEL_LEFT + index * METRIC_WIDTH, PANEL_TOP, METRIC_WIDTH, PANEL_HEIGHT);
}

QRect TopBarStatusWidget::stopRect() const
{
    return QRect(STOP_LEFT, (TOPBAR_STATUS_HEIGHT - STOP_HEIGHT) / 2,
                 STOP_WIDTH, STOP_HEIGHT);
}

QRect TopBarStatusWidget::clockRect() const
{
    return QRect(width() - 152, 0, 144, height());
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

    const bool discard_alarm = (m_discard_num > 0);
    const bool timeout_alarm = (m_timeout_num > 0);
    const bool any_alarm = discard_alarm || timeout_alarm;
    // 中文界面下内部只显示中文，不显示英文副标题与英文状态
    const bool chinese = (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT);

    // 一个矩形外框 + 一条跑马灯
    drawPanel(painter, panelRect(), any_alarm);

    // 面板内左右两个指标
    drawCard(painter, metricRect(0), myLan.ai_front_discard_num,
             chinese ? QString() : QString("PACKET LOSS"),
             m_discard_num, QString::fromUtf8("帧"),
             chinese ? (discard_alarm ? myLan.abnormal : myLan.normal)
                     : (discard_alarm ? QString("ABNORMAL") : QString("NORMAL")),
             discard_alarm);
    drawCard(painter, metricRect(1), myLan.ai_front_delay_num,
             chinese ? QString() : QString("INFER TIMEOUT"),
             m_timeout_num, myLan.times,
             chinese ? (timeout_alarm ? myLan.abnormal : myLan.normal)
                     : (timeout_alarm ? QString("ABNORMAL") : QString("NORMAL")),
             timeout_alarm);

    drawSegments(painter, panelRect(), any_alarm);

    drawStopButton(painter, stopRect());
    drawClock(painter, clockRect());
}

void TopBarStatusWidget::drawPanel(QPainter& painter, const QRect& panel, bool alarm) const
{
    const QColor accent = alarm ? QColor(255, 79, 94) : QColor(53, 224, 138);

    // 直角矩形外框（不使用弧形/圆角包围框）
    QLinearGradient fill(panel.topLeft(), panel.bottomLeft());
    fill.setColorAt(0.0, QColor(11, 27, 43, 245));
    fill.setColorAt(1.0, QColor(6, 18, 30, 245));
    const QRectF body = QRectF(panel).adjusted(0.5, 0.5, -0.5, -0.5);
    painter.setBrush(fill);
    painter.setPen(QPen(alarm ? QColor(180, 68, 78) : QColor(42, 127, 150), 1));
    painter.drawRect(body);

    // 顶边高亮，矩形框内的强调线
    painter.setPen(QPen(accent, 1));
    painter.drawLine(QPointF(body.left(), body.top()), QPointF(body.right(), body.top()));

    // 左右指标分隔线
    const int divider_x = PANEL_LEFT + METRIC_WIDTH;
    painter.setPen(QPen(QColor(38, 110, 130), 1));
    painter.drawLine(divider_x, panel.top() + 14, divider_x, panel.bottom() - 22);
}

void TopBarStatusWidget::drawCard(QPainter& painter, const QRect& card, const QString& title,
                                  const QString& subtitle, quint32 value, const QString& unit,
                                  const QString& status, bool alarm) const
{
    const QColor accent = alarm ? QColor(255, 79, 94) : QColor(53, 224, 138);

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

    // 标题（右侧留出数值区域，过长时省略）；无副标题时垂直居中
    const bool has_subtitle = !subtitle.isEmpty();
    QFont title_font = font();
    title_font.setPixelSize(15);
    title_font.setBold(true);
    painter.setFont(title_font);
    painter.setPen(QColor(234, 246, 255));
    const QRect title_rect(card.left() + 34, card.top() + (has_subtitle ? 6 : 8),
                           112, has_subtitle ? 20 : 30);
    const QFontMetrics title_metrics(title_font);
    painter.drawText(title_rect, Qt::AlignLeft | Qt::AlignVCenter,
                     title_metrics.elidedText(title, Qt::ElideRight, title_rect.width()));

    // 英文副标题（中文界面不显示）
    if (has_subtitle)
    {
        QFont sub_font = font();
        sub_font.setPixelSize(8);
        sub_font.setBold(false);
        sub_font.setLetterSpacing(QFont::AbsoluteSpacing, 1.0);
        painter.setFont(sub_font);
        painter.setPen(QColor(78, 143, 166));
        painter.drawText(QRect(card.left() + 34, card.top() + 27, 112, 12),
                         Qt::AlignLeft | Qt::AlignVCenter, subtitle);
    }

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
    painter.drawText(QRect(card.left() + 146, card.top() + 4, 72, 40),
                     Qt::AlignRight | Qt::AlignVCenter, value_text);

    // 单位
    QFont unit_font = font();
    unit_font.setPixelSize(11);
    unit_font.setBold(false);
    painter.setFont(unit_font);
    painter.setPen(QColor(127, 166, 181));
    painter.drawText(QRect(card.left() + 220, card.top() + 28, 30, 16),
                     Qt::AlignLeft | Qt::AlignVCenter, unit);

    // 正常 / 异常（中文界面）或 NORMAL / ABNORMAL
    QFont status_font = font();
    status_font.setPixelSize(10);
    status_font.setBold(true);
    status_font.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    painter.setFont(status_font);
    painter.setPen(accent);
    painter.drawText(QRect(card.left() + 236, card.top() + 8, 52, 16),
                     Qt::AlignRight | Qt::AlignVCenter, status);
}

void TopBarStatusWidget::drawSegments(QPainter& painter, const QRect& panel, bool alarm) const
{
    const int left = panel.left() + 34;
    const int right = panel.right() - 16;
    const int gap = qMax(2, (right - left - SEGMENT_COUNT * SEGMENT_WIDTH) / (SEGMENT_COUNT - 1));
    const int y = panel.bottom() - 13;
    const int lit_start = m_phase % SEGMENT_COUNT;

    for (int i = 0; i < SEGMENT_COUNT; ++i)
    {
        const QRect rect(left + i * (SEGMENT_WIDTH + gap), y, SEGMENT_WIDTH, 6);
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
        // 素材已裁掉透明留白，按高度一半切九宫格，左右半圆帽等比例缩放
        drawNineSlice(painter, bg, button, bg.height() / 2, button.height() / 2);
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
                     Qt::AlignLeft | Qt::AlignVCenter, myLan.working_start);
}

void TopBarStatusWidget::drawClock(QPainter& painter, const QRect& area) const
{
    const QDateTime now = QDateTime::currentDateTime();
    const QRect icon_rect(area.left() + 4, area.center().y() - 19, 38, 38);

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
        painter.drawLine(center, QPoint(center.x(), center.y() - 11));
        painter.drawLine(center, QPoint(center.x() + 8, center.y() + 4));
    }

    QFont time_font = font();
    time_font.setPixelSize(11);
    time_font.setBold(false);
    painter.setFont(time_font);
    painter.setPen(QColor(159, 195, 210));
    painter.drawText(QRect(area.left() + 46, area.top() + 16, 96, 18),
                     Qt::AlignLeft | Qt::AlignVCenter, now.toString("yyyy-MM-dd"));

    time_font.setPixelSize(16);
    time_font.setBold(true);
    painter.setFont(time_font);
    painter.setPen(QColor(234, 246, 255));
    painter.drawText(QRect(area.left() + 46, area.top() + 35, 96, 26),
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
        || target.width() < 2 * margin || target.height() < 2 * margin)
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
