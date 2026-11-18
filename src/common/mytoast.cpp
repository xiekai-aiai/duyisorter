/*!
 * \file        mytoast.cpp
 * \brief       轻量提示条实现
 * \date        2026.11.12
 */
#include "mytoast.h"

#include <QPainter>
#include <QTimer>
#include <QFontMetrics>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace {

const int TOAST_H     = 52;     //! 与 toast_bg.png 高度一致
const int ICON_SIZE   = 24;
const int PAD_LEFT    = 24;
const int PAD_RIGHT   = 24;
const int ICON_GAP    = 12;
const int TOP_MARGIN  = 64;     //! 距窗口顶部偏移，避开标题栏
const int HOLD_MS     = 1200;   //! 停留时长
const int FADE_MS     = 260;    //! 淡出时长
const int SLICE_L     = 24;     //! 9-slice 左右固定区
const int MIN_WIDTH   = 240;
const int MAX_WIDTH   = 640;

} // namespace

myToast::myToast(QWidget *parent)
    : QWidget(parent)
    , m_holdTimer(nullptr)
    , m_effect(nullptr)
    , m_fade(nullptr)
{
    m_bg.load(":/res/png2/button/toast_bg.png");
    m_icon.load(":/res/png2/button/icon_check.png");

    //! 纯展示控件：不拦截点击，不参与布局
    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_effect = new QGraphicsOpacityEffect(this);
    m_effect->setOpacity(1.0);
    setGraphicsEffect(m_effect);

    m_fade = new QPropertyAnimation(m_effect, "opacity", this);
    m_fade->setDuration(FADE_MS);
    m_fade->setStartValue(1.0);
    m_fade->setEndValue(0.0);
    connect(m_fade, &QPropertyAnimation::finished, this, &QWidget::hide);

    m_holdTimer = new QTimer(this);
    m_holdTimer->setSingleShot(true);
    m_holdTimer->setInterval(HOLD_MS);
    connect(m_holdTimer, &QTimer::timeout, this, [this]() { m_fade->start(); });

    hide();
}

void myToast::showToast(const QString &text)
{
    if (text.isEmpty())
    {
        return;
    }

    m_text = text;

    QFont font("Noto Sans SC");
    font.setPixelSize(16);
    QFontMetrics fm(font);

    int w = PAD_LEFT + ICON_SIZE + ICON_GAP + fm.horizontalAdvance(m_text) + PAD_RIGHT;
    w = qBound(MIN_WIDTH, w, MAX_WIDTH);
    setFixedSize(w, TOAST_H);

    if (parentWidget())
    {
        move((parentWidget()->width() - w) / 2, TOP_MARGIN);
    }

    //! 连续点击时重新计时，避免上一次的淡出把新提示带走
    m_holdTimer->stop();
    m_fade->stop();
    m_effect->setOpacity(1.0);
    show();
    raise();
    m_holdTimer->start();
}

void myToast::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);

    //! 底板 9-slice：左右 24px 固定，中段水平拉伸
    if (!m_bg.isNull())
    {
        const int sw = m_bg.width();
        const int sh = m_bg.height();
        const int h = qMin(sh, height());

        p.drawPixmap(0, 0, m_bg, 0, 0, SLICE_L, sh);
        p.drawPixmap(width() - SLICE_L, 0, m_bg, sw - SLICE_L, 0, SLICE_L, sh);
        p.drawPixmap(QRect(SLICE_L, 0, width() - SLICE_L * 2, h),
                     m_bg,
                     QRect(SLICE_L, 0, sw - SLICE_L * 2, sh));
    }

    if (!m_icon.isNull())
    {
        p.drawPixmap(PAD_LEFT, (TOAST_H - ICON_SIZE) / 2, ICON_SIZE, ICON_SIZE, m_icon);
    }

    QFont font("Noto Sans SC");
    font.setPixelSize(16);
    p.setFont(font);
    p.setPen(QColor(0xE8, 0xF6, 0xFF));

    const int textX = PAD_LEFT + ICON_SIZE + ICON_GAP;
    const int textW = width() - textX - PAD_RIGHT;
    p.drawText(QRect(textX, 0, textW, TOAST_H),
               Qt::AlignVCenter | Qt::AlignLeft,
               m_text);
}
