#ifndef TOPBARSTATUSWIDGET_H
#define TOPBARSTATUSWIDGET_H

#include <QWidget>
#include <QThread>
#include <QPixmap>
#include <QTimer>
#include <QRect>
#include <QString>
#include <QVector>

/* 运行态顶部状态栏高度（主界面布局按此高度让位） */
#define TOPBAR_STATUS_HEIGHT 60

/* 单次查询相比上次查询，丢包数/延迟超时数增长超过该值即判为异常 */
#define AI_STATUS_DELTA_ALARM 10

/*!
 * \brief AI实时状态轮询线程
 * 运行期间周期性读取各AI单元状态，并缓存每块板卡上一次查询值；
 * 单次查询丢包数或延迟超时数增量超过阈值时生成异常描述，交由顶部状态栏显示
 */
class AiStatusPollThread : public QThread
{
    Q_OBJECT
public:
    explicit AiStatusPollThread(QObject* parent = 0);
    void startPolling();
    void stopPolling();

signals:
    /*! alarm_text 为空表示正常，否则为“通道x前视出现丢包，通道x后视推理异常”形式的描述 */
    void statusUpdated(const QString& alarm_text);

protected:
    void run();

private:
    volatile bool m_stopped;
    QVector<quint32> m_prev_discard;    // 各板卡上次查询的丢包数
    QVector<quint32> m_prev_timeout;    // 各板卡上次查询的延迟超时数
    QVector<bool> m_has_prev;           // 各板卡是否已有基准值
};

/*!
 * \brief 开始工作后的顶部实时状态显示
 * 只覆盖“开始工作”按钮左侧区域，直接显示状态图标、状态文字和绿色跑马灯；
 * “开始工作”按钮与右侧时钟继续复用主界面的真实控件，位置保持不变
 */
class TopBarStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopBarStatusWidget(QWidget* parent = 0);
    ~TopBarStatusWidget();

    bool isRunning() const { return m_running; }

    /*! 指定主界面“开始工作”按钮，状态区右边界按其位置对齐 */
    void setWorkButton(QWidget* button);

public slots:
    void setRunning(bool running);
    void onStatusUpdated(const QString& alarm_text);

protected:
    void paintEvent(QPaintEvent* event);

private slots:
    void onAnimTimeout();

private:
    QPixmap loadAsset(const QString& dir, const QString& name) const;
    void drawStatus(QPainter& painter) const;
    void drawSegments(QPainter& painter) const;

private:
    bool m_running;             // 是否处于运行态
    int m_fade;                 // 入场/退场透明度 0~255
    int m_fade_target;          // 目标透明度
    int m_phase;                // 分段条流光相位
    QString m_alarm_text;       // 异常描述，空表示正常

    QTimer m_anim_timer;        // 流光/淡入淡出动画
    AiStatusPollThread* m_poller;
    QWidget* m_work_button;     // 主界面“开始工作”按钮，用于确定状态区右边界

    QPixmap m_state_normal;     // stateNormal.png    正常状态图标
    QPixmap m_state_alarm;      // stateAlarm.png     异常状态图标
    QPixmap m_seg_on;           // segOn.png          跑马灯亮段
    QPixmap m_seg_off;          // segOff.png         跑马灯暗段
    QPixmap m_topbar_bg;        // topbarBg.png       顶栏底色
};

#endif // TOPBARSTATUSWIDGET_H
