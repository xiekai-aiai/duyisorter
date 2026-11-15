/*!
 * \file        loadingwindow.h
 * \brief       启动画面：底板 + 光环动画 + 5 组检测进度 + 品牌区
 * \date        2026.11.12
 */
#ifndef LOADINGWINDOW_H
#define LOADINGWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QVector>

#include "bootstage.h"

class QTimer;
class QPainter;

/*!
 * \brief 自绘启动窗口，替代 QLabel + QMovie
 */
class LoadingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingWindow(QWidget *parent = nullptr);
    ~LoadingWindow();

public slots:
    /*!
     * \brief 接收开机检测状态上报
     * \param stage   BOOT_STAGE_*
     * \param status  BOOT_STATUS_*
     * \param detail  正常/异常文案（ERROR 时同时追加到失败行）
     */
    void onStageChanged(int stage, int status, const QString &detail);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onRingTick();

private:
    void drawSegments(QPainter &p);
    void drawCenterText(QPainter &p);
    void drawDetailText(QPainter &p);
    void drawBrand(QPainter &p);

    QPixmap ringFrame();
    QPixmap segPixmap(int status);
    QPixmap segGlowPixmap(int status);
    QPixmap buildGlow(const QString &text, const QFont &font, const QColor &color, int radius);
    void updateBrandText();

    QPixmap m_bg;                   //! 静态底板
    QPixmap m_logo;                 //! 品牌 LOGO
    QVector<QPixmap> m_ringCache;   //! 光环帧缓存（按需解码，只解一次）
    int     m_ringIdx;              //! 当前帧序号
    QTimer *m_ringTimer;
    int     m_blinkPhase;           //! 检测段闪烁相位：0亮 1中间 2暗 3中间
    int     m_blinkTick;            //! 相位计时

    QPixmap m_seg[4];               //! idle/active/ok/error 段条
    QPixmap m_segGlow[4];           //! 对应柔光层

    int     m_state[BOOT_STAGE_COUNT];  //! 各分组状态
    QString m_centerText;               //! 中央阶段文字
    QStringList m_errors;               //! 失败项（累计）
    QPixmap m_centerGlow;               //! 缓存的中央文字柔光
    QString m_glowSource;               //! 柔光对应的文本
    QFont   m_centerFont;               //! 中央文字字体

    QString m_productText;          //! 产品名
    QString m_versionText;          //! 版本号
};

#endif // LOADINGWINDOW_H
