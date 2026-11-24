/*!
 * \file        loadingwindow.cpp
 * \brief       启动画面实现：底板 + 光环动画 + 5 组检测进度 + 品牌区
 * \date        2026.11.12
 */
#include "loadingwindow.h"
#include "globalflow.h"

#include <QPainter>
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFontMetrics>
#include <QImage>
#include <QColor>
#include <QtGlobal>

namespace {

/** 光环贴层（1:1，不缩放） */
const int RING_X = 232;
const int RING_Y = 16;
const int RING_SIZE = 560;
const int RING_FRAMES = 40;

/** 5 段进度条：单段 96x8、间距 12、整体居中 */
const int SEG_W = 96;
const int SEG_H = 8;
const int SEG_GAP = 12;
const int SEG_Y = 603;
const int SEG_X0 = (LCD_WIDTH - (BOOT_STAGE_COUNT * SEG_W
                                 + (BOOT_STAGE_COUNT - 1) * SEG_GAP)) / 2;

/** 当前检测段呼吸闪烁：每 2 个光环帧（150ms）推进一个相位
 *  相位顺序 亮(本体+柔光) → 中间(仅本体) → 暗(空闲灰) → 中间 → 亮… */
const int BLINK_TICKS = 2;
const int BLINK_PHASES = 4;

/** 文字基线 */
const int CENTER_BASELINE = 579;
const int DETAIL_BASELINE = 634;

/** 9-slice 左右圆角宽度 */
const int SEG_CORNER = 5;

/*!
 * \brief 9-slice 绘制段条：左右圆角固定，中段拉伸
 */
void drawNineSlice(QPainter &p, int x, int y, int w, int h, const QPixmap &src)
{
    if (src.isNull() || w <= SEG_CORNER * 2)
    {
        return;
    }

    const int sw = src.width();
    const int sh = src.height();

    p.drawPixmap(x, y, src, 0, 0, SEG_CORNER, sh);
    p.drawPixmap(x + w - SEG_CORNER, y, src, sw - SEG_CORNER, 0, SEG_CORNER, sh);
    p.drawPixmap(QRect(x + SEG_CORNER, y, w - SEG_CORNER * 2, h),
                 src,
                 QRect(SEG_CORNER, 0, sw - SEG_CORNER * 2, sh));
}

} // namespace

LoadingWindow::LoadingWindow(QWidget *parent)
    : QWidget(parent)
    , m_ringIdx(0)
    , m_ringTimer(nullptr)
    , m_blinkPhase(0)
    , m_blinkTick(0)
{
    //! 置顶：避免桌面残留窗口（终端/文件管理器）盖住启动画面
    // 不用 X11BypassWindowManagerHint（会导致输入事件分发异常，触摸失效）
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint
                   | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT);
    setFixedSize(LCD_WIDTH, LCD_HEIGHT);
    // 隐藏鼠标指针：仅 ARM 板卡；x86_64 调试版保留光标（见 MainWidget 同名处理）
#if defined(__aarch64__) || defined(__arm__)
    setCursor(Qt::BlankCursor);
#endif

    m_bg.load(":/res/start/loading_bg.png");
    m_logo.load(":/res/start/loading_qt_kit/brand_logo.png");

    for (int i = 0; i < BOOT_STAGE_COUNT; ++i)
    {
        m_state[i] = BOOT_STATUS_IDLE;
    }

    m_centerFont.setFamily("Noto Sans SC");
    m_centerFont.setPixelSize(24);
    m_centerFont.setWeight(QFont::Medium);
    m_centerFont.setLetterSpacing(QFont::AbsoluteSpacing, 2);

    m_ringTimer = new QTimer(this);
    m_ringTimer->setInterval(75);
    connect(m_ringTimer, &QTimer::timeout, this, &LoadingWindow::onRingTick);
    m_ringTimer->start();
}

LoadingWindow::~LoadingWindow()
{
    if (m_ringTimer)
    {
        m_ringTimer->stop();
    }
}

void LoadingWindow::onStageChanged(int stage, int status, const QString &detail)
{
    if (stage < 0 || stage >= BOOT_STAGE_COUNT)
    {
        return;
    }

    m_state[stage] = status;

    switch (status)
    {
    case BOOT_STATUS_ACTIVE:
        //! 中央只显示当前动作，检查结果由段条颜色和下方失败行体现
        if (!detail.isEmpty())
        {
            m_centerText = detail;
        }
        //! 进入新阶段时从亮相位开始呼吸
        m_blinkPhase = 0;
        m_blinkTick = 0;
        break;

    case BOOT_STATUS_ERROR:
        if (!detail.isEmpty())
        {
            m_errors << detail;
        }
        break;

    default:
        break;
    }

    update();
    //! 立即刷新，保证 initAll() 阻塞期间光环与文字仍能更新
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void LoadingWindow::onRingTick()
{
    m_ringIdx = (m_ringIdx + 1) % RING_FRAMES;

    //! 只刷新光环区域，避免整屏重绘
    update(RING_X, RING_Y, RING_SIZE, RING_SIZE);

    //! 检测中的段跟随节拍呼吸闪烁，仅局部刷新该行
    if (++m_blinkTick >= BLINK_TICKS)
    {
        m_blinkTick = 0;
        m_blinkPhase = (m_blinkPhase + 1) % BLINK_PHASES;
        const int segTotalW = BOOT_STAGE_COUNT * SEG_W
                + (BOOT_STAGE_COUNT - 1) * SEG_GAP;
        update(SEG_X0 - 4, SEG_Y - 8, segTotalW + 8, 24);
    }
}

void LoadingWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);

    if (!m_bg.isNull())
    {
        p.drawPixmap(0, 0, m_bg);
    }

    QPixmap ring = ringFrame();
    if (!ring.isNull())
    {
        p.drawPixmap(RING_X, RING_Y, ring);
    }

    drawSegments(p);
    drawCenterText(p);
    drawDetailText(p);
    drawBrand(p);
}

void LoadingWindow::drawSegments(QPainter &p)
{
    for (int k = 0; k < BOOT_STAGE_COUNT; ++k)
    {
        const int x = SEG_X0 + k * (SEG_W + SEG_GAP);
        int status = m_state[k];
        bool showGlow = (status == BOOT_STATUS_OK || status == BOOT_STATUS_ERROR);

        //! 检测中的段三态呼吸：亮(本体+柔光) / 中间(仅本体) / 暗(空闲灰)
        if (status == BOOT_STATUS_ACTIVE)
        {
            switch (m_blinkPhase)
            {
            case 0:     // 亮：叠 seg_active_glow.png
                showGlow = true;
                break;
            case 1:     // 中间：仅清晰条本体
            case 3:
                break;
            default:    // 暗：按空闲灰绘制
                status = BOOT_STATUS_IDLE;
                break;
            }
        }

        //! 柔光层 104x24，水平外扩 4px，垂直居中于段条
        if (showGlow)
        {
            QPixmap glow = segGlowPixmap(m_state[k]);
            if (!glow.isNull())
            {
                p.drawPixmap(x - 4, SEG_Y - 8, glow);
            }
        }

        drawNineSlice(p, x, SEG_Y, SEG_W, SEG_H, segPixmap(status));
    }
}

void LoadingWindow::drawCenterText(QPainter &p)
{
    if (m_centerText.isEmpty())
    {
        return;
    }

    QFontMetrics fm(m_centerFont);
    const int maxWidth = LCD_WIDTH - 64;
    QString text = m_centerText;
    if (fm.horizontalAdvance(text) > maxWidth)
    {
        text = fm.elidedText(text, Qt::ElideRight, maxWidth);
    }

    //! 柔光按文本缓存，避免每帧重复模糊
    if (m_glowSource != text)
    {
        QColor glowColor(0x22, 0xD3, 0xEE);
        glowColor.setAlpha(190);
        m_centerGlow = buildGlow(text, m_centerFont, glowColor, 8);
        m_glowSource = text;
    }

    const int x = (LCD_WIDTH - fm.horizontalAdvance(text)) / 2;

    if (!m_centerGlow.isNull())
    {
        p.drawPixmap(x - 8, CENTER_BASELINE - fm.ascent() - 8, m_centerGlow);
    }

    p.setFont(m_centerFont);
    p.setPen(QColor(0xE8, 0xFA, 0xFF));
    p.drawText(QPointF(x, CENTER_BASELINE), text);
}

void LoadingWindow::drawDetailText(QPainter &p)
{
    if (m_errors.isEmpty())
    {
        return;
    }

    QFont font("Noto Sans SC");
    font.setPixelSize(16);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 1);

    QFontMetrics fm(font);
    const int maxWidth = LCD_WIDTH - 64;

    p.setFont(font);
    p.setPen(QColor(0xFF, 0x7A, 0x8A));

    QString line1 = m_errors.join(QString::fromUtf8("；"));
    if (fm.horizontalAdvance(line1) <= maxWidth)
    {
        const int x = (LCD_WIDTH - fm.horizontalAdvance(line1)) / 2;
        p.drawText(QPointF(x, DETAIL_BASELINE), line1);
        return;
    }

    //! 文字过长时按分号拆成两排显示
    QString acc;
    int split = 0;
    for (int i = 0; i < m_errors.size(); ++i)
    {
        const QString candidate = acc.isEmpty()
                ? m_errors.at(i)
                : acc + QString::fromUtf8("；") + m_errors.at(i);
        if (!acc.isEmpty() && fm.horizontalAdvance(candidate) > maxWidth)
        {
            break;
        }
        acc = candidate;
        split = i + 1;
    }
    line1 = acc;
    if (fm.horizontalAdvance(line1) > maxWidth)
    {
        line1 = fm.elidedText(line1, Qt::ElideRight, maxWidth);
    }

    QString line2;
    if (split < m_errors.size())
    {
        line2 = m_errors.mid(split).join(QString::fromUtf8("；"));
        if (fm.horizontalAdvance(line2) > maxWidth)
        {
            line2 = fm.elidedText(line2, Qt::ElideRight, maxWidth);
        }
    }

    if (line2.isEmpty())
    {
        const int x = (LCD_WIDTH - fm.horizontalAdvance(line1)) / 2;
        p.drawText(QPointF(x, DETAIL_BASELINE), line1);
    }
    else
    {
        const int x1 = (LCD_WIDTH - fm.horizontalAdvance(line1)) / 2;
        const int x2 = (LCD_WIDTH - fm.horizontalAdvance(line2)) / 2;
        p.drawText(QPointF(x1, DETAIL_BASELINE - 4), line1);
        p.drawText(QPointF(x2, DETAIL_BASELINE + 18), line2);
    }
}

void LoadingWindow::drawBrand(QPainter &p)
{
    updateBrandText();

    if (!m_logo.isNull())
    {
        p.drawPixmap(32, 684, m_logo);
    }

    //! 产品名与版本号合并成一行，放在右侧版本号位置，同色同字号
    QString brandText = m_productText;
    if (!m_versionText.isEmpty())
    {
        if (!brandText.isEmpty())
        {
            brandText += QString::fromUtf8("  ");
        }
        brandText += m_versionText;
    }

    if (brandText.isEmpty())
    {
        return;
    }

    QFont infoFont("Noto Sans SC");
    infoFont.setPixelSize(13);

    p.setFont(infoFont);
    p.setPen(QColor(0x6F, 0xA8, 0xBC));

    QFontMetrics fm(infoFont);
    const int w = fm.horizontalAdvance(brandText);
    p.drawText(QPointF(992 - w, 716), brandText);
}

void LoadingWindow::updateBrandText()
{
    //! 每次重绘都按当前语言取产品名，保证启动页中英文切换生效
    m_productText = tr("AI色选机");

    if (m_versionText.isEmpty() && !myString.sAppVersion.isEmpty())
    {
        m_versionText = myString.sAppVersion;
        if (!m_versionText.startsWith('V', Qt::CaseInsensitive))
        {
            m_versionText.prepend('V');
        }
    }
}

QPixmap LoadingWindow::ringFrame()
{
    //! 按需解码并缓存：同一帧只解一次，避免每 75ms 重复解码 PNG 造成卡顿
    if (m_ringCache.size() != RING_FRAMES)
    {
        m_ringCache.resize(RING_FRAMES);
    }

    if (m_ringCache.at(m_ringIdx).isNull())
    {
        const QString path =
            QString(":/res/start/loading_qt_kit/loading_ring_seq/ring_%1.png")
                .arg(m_ringIdx, 2, 10, QChar('0'));
        m_ringCache[m_ringIdx] = QPixmap(path);
    }

    return m_ringCache.at(m_ringIdx);
}

QPixmap LoadingWindow::segPixmap(int status)
{
    int idx = 0;

    if (status == BOOT_STATUS_ACTIVE)
    {
        idx = 1;
    }
    else if (status == BOOT_STATUS_OK)
    {
        idx = 2;
    }
    else if (status == BOOT_STATUS_ERROR)
    {
        idx = 3;
    }

    if (m_seg[idx].isNull())
    {
        static const char *names[4] = { "idle", "active", "ok", "error" };
        m_seg[idx].load(QString(":/res/start/loading_qt_kit/seg_%1.png")
                            .arg(QString::fromLatin1(names[idx])));
    }

    return m_seg[idx];
}

QPixmap LoadingWindow::segGlowPixmap(int status)
{
    int idx = 0;

    if (status == BOOT_STATUS_ACTIVE)
    {
        idx = 1;
    }
    else if (status == BOOT_STATUS_OK)
    {
        idx = 2;
    }
    else if (status == BOOT_STATUS_ERROR)
    {
        idx = 3;
    }

    if (m_segGlow[idx].isNull())
    {
        static const char *names[4] = { "idle", "active", "ok", "error" };
        m_segGlow[idx].load(QString(":/res/start/loading_qt_kit/seg_%1_glow.png")
                                .arg(QString::fromLatin1(names[idx])));
    }

    return m_segGlow[idx];
}

/*!
 * \brief 生成文字柔光：1/4 下采样后平滑放大，近似高斯模糊
 */
QPixmap LoadingWindow::buildGlow(const QString &text, const QFont &font,
                                 const QColor &color, int radius)
{
    QFontMetrics fm(font);
    const int w = fm.horizontalAdvance(text) + radius * 2;
    const int h = fm.height() + radius * 2;

    if (w <= 0 || h <= 0)
    {
        return QPixmap();
    }

    QImage img(w, h, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter ip(&img);
    ip.setFont(font);
    ip.setPen(color);
    ip.drawText(QPointF(radius, radius + fm.ascent()), text);
    ip.end();

    const QImage small = img.scaled(qMax(1, w / 4), qMax(1, h / 4),
                                    Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation);
    const QImage blurred = small.scaled(w, h,
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);

    return QPixmap::fromImage(blurred);
}
