#include "topbarstatuswidget.h"

#include <QPainter>
#include <QFile>
#include <QCoreApplication>
#include <QFont>
#include <QFontMetrics>
#include <QHostAddress>
#include <QLinearGradient>
#include <QString>
#include <QStringList>

#include "cmdworker.h"
#include "cmdudpmanager.h"
#include "aihelper.h"
#include "globalparams.h"
#include "mylanguage.h"
#include "sortertypes.h"

/* 素材目录：状态图标在 topbar_anim2，分段条/顶栏底色在 topbar_anim */
#define TOPBAR_ASSET_DIR2  ":/res/png2/topbar_anim2/"
#define TOPBAR_ASSET_DIR   ":/res/png2/topbar_anim/"

/* 状态区布局参数（1024x768 基准）
   状态区只覆盖“开始工作”按钮左侧，按钮与时钟由主界面真实控件显示 */
#define STATUS_LEFT        18       // 状态图标左边距
#define STATUS_ICON_SIZE   26       // 状态图标尺寸
#define STATUS_ICON_GAP    12       // 图标与文字间距
#define STATUS_RIGHT_GAP   16       // 文字/跑马灯右边距
#define STATUS_FALLBACK_W  600      // 未取到按钮位置时的兜底宽度
#define WORK_RIGHT_GAP     12       // 状态区与“开始工作”按钮的间距
#define SEGMENT_COUNT      30
#define SEGMENT_WIDTH      14
#define SEGMENT_LIT_LEN    8

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
    // 每次开始工作重新建立基准，首轮查询只记录不告警
    m_prev_discard.clear();
    m_prev_timeout.clear();
    m_has_prev.clear();

    while (!m_stopped)
    {
        const int unit_total = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        const QByteArray request = cmdworker::AiStatusRequest();

        if (m_prev_discard.size() != unit_total)
        {
            m_prev_discard.fill(0, unit_total);
            m_prev_timeout.fill(0, unit_total);
            m_has_prev.fill(false, unit_total);
        }

        QStringList issues;

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

            // 每块板卡与上一次查询值对比，增量超阈值判为异常
            if (m_has_prev[idx])
            {
                // 计数回退视为板卡重启，只更新基准不告警
                const quint32 discard_delta = status_info.discard_num_ >= m_prev_discard[idx]
                                              ? status_info.discard_num_ - m_prev_discard[idx] : 0;
                const quint32 timeout_delta = status_info.timeout_num_ >= m_prev_timeout[idx]
                                              ? status_info.timeout_num_ - m_prev_timeout[idx] : 0;
                const int channel = idx / 2 + 1;
                const QString view = (idx % 2 == 0) ? QString::fromUtf8("前视")
                                                    : QString::fromUtf8("后视");

                if (discard_delta > AI_STATUS_DELTA_ALARM)
                {
                    issues.append(QString::fromUtf8("通道%1%2出现丢包").arg(channel).arg(view));
                }
                if (timeout_delta > AI_STATUS_DELTA_ALARM)
                {
                    issues.append(QString::fromUtf8("通道%1%2推理异常").arg(channel).arg(view));
                }
            }

            m_prev_discard[idx] = status_info.discard_num_;
            m_prev_timeout[idx] = status_info.timeout_num_;
            m_has_prev[idx] = true;
        }

        emit statusUpdated(issues.isEmpty() ? QString()
                                            : issues.join(QString::fromUtf8("，")) + QString::fromUtf8("。"));

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
      m_fade(0),
      m_fade_target(0),
      m_phase(0),
      m_poller(0),
      m_work_button(0)
{
    setFixedHeight(TOPBAR_STATUS_HEIGHT);

    m_state_normal  = loadAsset(TOPBAR_ASSET_DIR2, "stateNormal.png");
    m_state_alarm   = loadAsset(TOPBAR_ASSET_DIR2, "stateAlarm.png");
    m_seg_on        = loadAsset(TOPBAR_ASSET_DIR, "segOn.png");
    m_seg_off       = loadAsset(TOPBAR_ASSET_DIR, "segOff.png");
    m_topbar_bg     = loadAsset(TOPBAR_ASSET_DIR, "topbarBg.png");

    m_anim_timer.setInterval(80);
    connect(&m_anim_timer, SIGNAL(timeout()), this, SLOT(onAnimTimeout()));

    m_poller = new AiStatusPollThread(this);
    connect(m_poller, SIGNAL(statusUpdated(QString)),
            this, SLOT(onStatusUpdated(QString)), Qt::QueuedConnection);

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

void TopBarStatusWidget::setWorkButton(QWidget* button)
{
    m_work_button = button;
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
        // 状态区只占“开始工作”按钮左侧区域，按钮和时钟保持空闲态位置不动
        if (m_work_button != 0 && parentWidget() != 0)
        {
            QWidget* row = m_work_button->parentWidget();
            if (row != 0)
            {
                const QPoint row_pos = row->mapTo(parentWidget(), QPoint(0, 0));
                const QPoint btn_pos = m_work_button->mapTo(parentWidget(), QPoint(0, 0));

                int right = btn_pos.x() - WORK_RIGHT_GAP;
                if (right < row_pos.x() + 200)
                {
                    right = row_pos.x() + STATUS_FALLBACK_W;
                }
                setGeometry(row_pos.x(), row_pos.y(),
                            right - row_pos.x(), row->height());
            }
        }

        // 每次开始工作清除上次异常描述，首轮查询仅建立基准
        m_alarm_text.clear();
        m_phase = 0;
        m_fade = 0;
        m_fade_target = 255;

        m_anim_timer.start();
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

void TopBarStatusWidget::onStatusUpdated(const QString& alarm_text)
{
    if (!m_running)
    {
        return;
    }

    if (m_alarm_text == alarm_text)
    {
        return;
    }

    m_alarm_text = alarm_text;
    update();
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
            hide();
            return;
        }
    }
    else if (!m_running)
    {
        // 已完全淡出，停止刷新
        m_anim_timer.stop();
        hide();
        return;
    }

    if (m_running && m_fade >= m_fade_target)
    {
        ++m_phase;
        update();
    }
}

QPixmap TopBarStatusWidget::loadAsset(const QString& dir, const QString& name) const
{
    const QString res_path = dir + name;
    if (QFile::exists(res_path))
    {
        return QPixmap(res_path);
    }

    // 兼容素材未打进qrc时，从可执行文件同级的res目录读取
    const QString disk_path = QCoreApplication::applicationDirPath()
                              + "/res/png2/" + dir.section('/', -2, -2) + "/" + name;
    if (QFile::exists(disk_path))
    {
        return QPixmap(disk_path);
    }

    return QPixmap();
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

    // 背景与主界面使用同一张背景图、同一取样位置，盖住左侧工具按钮且无割裂感
    bool bg_painted = false;
    if (parentWidget() != 0)
    {
        const QPixmap parent_bg = parentWidget()->palette()
                                  .brush(parentWidget()->backgroundRole()).texture();
        if (!parent_bg.isNull() && parent_bg.size() == parentWidget()->size())
        {
            painter.drawPixmap(rect(), parent_bg, geometry());
            bg_painted = true;
        }
    }

    if (!bg_painted)
    {
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
    }

    drawStatus(painter);
    drawSegments(painter);
}

void TopBarStatusWidget::drawStatus(QPainter& painter) const
{
    const bool alarm = !m_alarm_text.isEmpty();
    const QColor accent = alarm ? QColor(255, 79, 94) : QColor(53, 224, 138);

    // 状态图标与文字整体在高度上居中，与“开始工作”按钮中心对齐
    const int center_y = height() / 2;
    const QRect icon_rect(STATUS_LEFT, center_y - STATUS_ICON_SIZE / 2,
                          STATUS_ICON_SIZE, STATUS_ICON_SIZE);
    const QPixmap& icon = alarm ? m_state_alarm : m_state_normal;
    if (!icon.isNull())
    {
        painter.drawPixmap(icon_rect, icon);
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(accent, 2));
        painter.drawRoundedRect(icon_rect.adjusted(1, 1, -1, -1), 4, 4);

        if (alarm)
        {
            painter.drawLine(icon_rect.center().x(), icon_rect.top() + 7,
                             icon_rect.center().x(), icon_rect.bottom() - 11);
            painter.setBrush(accent);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(icon_rect.center().x(), icon_rect.bottom() - 6), 1.6, 1.6);
        }
        else
        {
            painter.drawLine(icon_rect.left() + 7, icon_rect.center().y(),
                             icon_rect.center().x() - 1, icon_rect.bottom() - 8);
            painter.drawLine(icon_rect.center().x() - 1, icon_rect.bottom() - 8,
                             icon_rect.right() - 6, icon_rect.top() + 8);
        }
    }

    // 状态文字：正常“正常运行”，异常显示具体问题（通道x前视出现丢包 / 推理异常）
    const bool chinese = (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT);
    const QString text = alarm ? m_alarm_text
                               : (chinese ? myLan.normal + QString::fromUtf8("运行") : myLan.normal);

    const int text_left = icon_rect.right() + 1 + STATUS_ICON_GAP;
    const int text_width = qMax(0, width() - text_left - STATUS_RIGHT_GAP);

    // 异常描述可能较长，18px 放不下时自动降到 15px，仍放不下再省略
    int pixel_size = 18;
    QFont text_font = font();
    text_font.setPixelSize(pixel_size);
    text_font.setBold(true);
    QFontMetrics text_metrics(text_font);
    if (alarm && text_metrics.horizontalAdvance(text) > text_width)
    {
        pixel_size = 15;
        text_font.setPixelSize(pixel_size);
        text_metrics = QFontMetrics(text_font);
    }

    painter.setFont(text_font);
    painter.setPen(accent);
    painter.drawText(QRect(text_left, center_y - 15, text_width, 30),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     text_metrics.elidedText(text, Qt::ElideRight, text_width));
}

void TopBarStatusWidget::drawSegments(QPainter& painter) const
{
    const int left = STATUS_LEFT;
    const int right = width() - STATUS_RIGHT_GAP;
    const int gap = qMax(2, (right - left - SEGMENT_COUNT * SEGMENT_WIDTH) / (SEGMENT_COUNT - 1));
    const int y = height() - 13;
    const int lit_start = m_phase % SEGMENT_COUNT;

    for (int i = 0; i < SEGMENT_COUNT; ++i)
    {
        const QRect rect(left + i * (SEGMENT_WIDTH + gap), y, SEGMENT_WIDTH, 6);
        const bool lit = ((i - lit_start + SEGMENT_COUNT) % SEGMENT_COUNT) < SEGMENT_LIT_LEN;
        const QPixmap& pm = lit ? m_seg_on : m_seg_off;

        if (!pm.isNull())
        {
            painter.drawPixmap(rect, pm);
        }
        else
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(lit ? QColor(53, 214, 160) : QColor(18, 48, 60));
            painter.drawRoundedRect(rect, 2.5, 2.5);
        }
    }
}
