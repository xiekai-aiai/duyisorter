#ifndef TOPBARSTATUSWIDGET_H
#define TOPBARSTATUSWIDGET_H

#include <QWidget>
#include <QThread>
#include <QPixmap>
#include <QTimer>
#include <QRect>

/* 运行态顶部状态栏高度（主界面布局按此高度让位） */
#define TOPBAR_STATUS_HEIGHT 80

/*!
 * \brief AI实时状态轮询线程
 * 运行期间周期性读取各AI单元的丢包数/超时数，避免阻塞UI线程
 */
class AiStatusPollThread : public QThread
{
    Q_OBJECT
public:
    explicit AiStatusPollThread(QObject* parent = 0);
    void startPolling();
    void stopPolling();

signals:
    void statusUpdated(quint32 discard_num, quint32 timeout_num);

protected:
    void run();

private:
    volatile bool m_stopped;
};

/*!
 * \brief 开始工作后的顶部实时状态栏
 * 空闲态由主界面的工具按钮显示，本控件只在运行态覆盖整个顶部区域：
 * 相机丢包卡片 + 推理延迟超时卡片 + 停止工作按钮 + 时钟
 */
class TopBarStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopBarStatusWidget(QWidget* parent = 0);
    ~TopBarStatusWidget();

    bool isRunning() const { return m_running; }

public slots:
    void setRunning(bool running);
    void onStatusUpdated(quint32 discard_num, quint32 timeout_num);

signals:
    void stopRequested();

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private slots:
    void onAnimTimeout();

private:
    QPixmap loadAsset(const QString& name) const;
    void drawNineSlice(QPainter& painter, const QPixmap& pixmap, const QRect& target,
                       int src_slice, int dst_margin) const;
    void drawPanel(QPainter& painter, const QRect& panel, bool alarm) const;
    void drawCard(QPainter& painter, const QRect& area, const QString& title,
                  const QString& subtitle, quint32 value, const QString& unit,
                  const QString& status, bool alarm) const;
    void drawSegments(QPainter& painter, const QRect& panel, bool alarm) const;
    void drawStopButton(QPainter& painter, const QRect& button) const;
    void drawClock(QPainter& painter, const QRect& area) const;

    QRect panelRect() const;
    QRect metricRect(int index) const;
    QRect stopRect() const;
    QRect clockRect() const;

private:
    bool m_running;             // 是否处于运行态
    bool m_pressed_stop;        // 停止按钮按下标志
    int m_fade;                 // 入场/退场透明度 0~255
    int m_fade_target;          // 目标透明度
    int m_phase;                // 分段条流光相位
    quint32 m_discard_num;      // 相机丢包数
    quint32 m_timeout_num;      // 推理延迟超时数

    QTimer m_anim_timer;        // 流光/淡入淡出动画
    QTimer m_clock_timer;       // 时钟刷新
    AiStatusPollThread* m_poller;

    QPixmap m_dot_normal;       // dotNormal.png
    QPixmap m_dot_alarm;        // dotAlarm.png
    QPixmap m_seg_on;           // segOn.png
    QPixmap m_seg_off;          // segOff.png
    QPixmap m_seg_on_alarm;     // segOnAlarm.png
    QPixmap m_btn_stop;         // btnStop.png
    QPixmap m_btn_stop_pressed; // btnStopPressed.png
    QPixmap m_icon_stop;        // iconStop.png
    QPixmap m_topbar_bg;        // topbarBg.png
    QPixmap m_icon_clock;       // iconClock.png
};

#endif // TOPBARSTATUSWIDGET_H
