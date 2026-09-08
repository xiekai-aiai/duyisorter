#ifndef EJECTTIMESTHREAD_H
#define EJECTTIMESTHREAD_H

#include "globalparams.h"

/* 定义最大超过吹气限制次数 */
const int MAX_ERROR_TIMES = 5;
/*!
 * \brief 获取吹气频率线程类
 */
class EjectTimesThread : public QThread
{
    Q_OBJECT

public:
    explicit EjectTimesThread();
    ~EjectTimesThread();  

    void run(void);                     // 读取吹气次数线程运行函数
    void startEmitSignal(void);         // 开始发送信号
    void stopEmitSignal(void);          // 停止发送信号
    void checkEjectTimesLimit(void);    // 检查吹气次数限制
    int getTickGroup(int chute);        // 根据滑槽获取当前剔除组
    int getEjectTimes(int flag);        // 前后视的吹气次数统计
    void stopStatRun(void);             // 停止线程

signals:
    /* 线程信号 */
    void readDataFinished();            // 串口数据读取完成信号
    void startWipeSig();                // 开始清灰信号

public slots:
    /* 线程槽函数 */
    void readSerialData(void);          // 从串口读取吹气次数

private:
    /* 线程私有数据 */
    bool m_bIsEmit;                     // 标志是否发送信号至吹气次数界面
    int m_nChuteNum;                    // 当前滑槽数
    int m_nUnitAddr;                      // 当前统计的通道中前置板地址
    int m_nErrorTimes;                  // 某一滑槽超过吹气限制的次数
    int m_nRunFlag;                     // 吹气次数统计功能的使能标志
    int typeMachine;                    // 0：米，1：杂粮，2：茶叶

    /* 定义前后视索引号 */
    enum {
        SM_EJECT_MAJOR_FRONT = 0,   // 主配前视
        SM_EJECT_MAJOR_REAR,        // 主配后视
        SM_EJECT_ASSIST_FRONT,      // 辅配前视
        SM_EJECT_ASSIST_REAR        // 辅配后视
    };

};
/* 线程读取的吹气次数数据,+1为统计的当前通道的总吹气次数 */
extern int pEjectTimesPerSec[60*4 + 1];

extern EjectTimesThread *g_ejectTimesThread;

#endif // EJECTTIMESTHREAD_H
