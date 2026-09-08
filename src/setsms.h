/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setsms.h
 * \brief       设置短信息界面头文件
 * \date        2015.01.14
 */
#ifndef SETSMS_H
#define SETSMS_H

#include <QWidget>
#include <QLayout>
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/myinputpanel.h"
#include "common/myinputmethod.h"
#include "globalflow.h"

/* 定义保存收件人号码机短信中心号码文件路径 */
#define SMS_CONFIG_FILE APP_PATH "/userdata/cnf/cnf.sms"

/* 枚举AT命令类型 */
enum {
    AT_CLOSE_ECHO = 0,      // 关回显
    AT_SET_SMS_PDU,         // 设置短信格式为PDU
    AT_SET_SMS_TEXT,        // 设置短信格式为TEXT
    AT_SMS_LEN,             // 发送短信长度
    AT_SMS_CONTENT,         // 发送短信内容
    AT_READ_SMS_CAP,        // 读取短信容量
    AT_SET_NOTICE,          // 设置短信提醒
    AT_READ_UNREAD,         // 读取未读短信
    AT_OBTAIN_CENTER_NUM,   // 获取短信中心号码
    AT_TEST_SIGNAL          // 测试信号质量
};

/** 定义报警类型 */
const int ALARM_TYPE = 4;

/** 定义每次开机最大报警次数 */
const int MAX_ALARM_TIMES = 5;

/*!
 * \brief 设置短信息界面类
 */
class SetSms : public QWidget
{
    Q_OBJECT
public:
    explicit SetSms(QWidget *parent = 0);

    void createPage(void);          // 创建页面
    void initLocalParams(void);     // 初始化局部参数
    void connectSigAndSlt(void);    // 连接信号与槽
    void getSmsConfig(void);        // 获取设置短信配置信息
    void saveSmsConfig(void);       // 保存设置短信配置信息
    void refreshDisplay(void);      // 刷新页面的显示

    /* 短信协议部分接口 */
    void sendAT(int nType);         // 根据设置的类型发送AT命令
    void sendSmsCh(QString str = "");   // 发送中文短信息
    void sendSmsEn(QString str = "");   // 发送英文短信息
    void readSms(void);             // 读取短信息
    void convertRevNum(void);       // PDU：收件人号码
    void convertCenterNum(void);    // PDU：短信中心号码
    void convertMes(QString str = "");          // PDU：短信内容
    QString stringToUnicode(QString str);       // 字符串转Unicode编码
    QString getStringFromAlarm(int nAlarmId);   // 根据报警信息获取相应字符串
    bool checkAlarmIsSame(void);    // 检查报警状态是否相同
    void packAlarmStr(void);        // 根据报警状态组织字符串
    void sendAlarmSms(void);        // 有报警时发送提示短信
    void updateAlarmStatus(void);   // 更新报警状态
    
signals:
    
public slots:
    /* 设置短信页面槽函数 */
    void onRevNumEditPressed(void);         // 编辑收件人号码
    void onCenterNumEditPressed(void);      // 编辑短信中心号码
    void onSendBtnPressed(void);            // 发送短信息
    void onMesEditPressed(void);            // 编辑短信息
    void onTestBtnPressed(void);            // 测试信号质量
    void onObtainAutoBtnPressed(void);      // 自动获取短信中心号码
    void onTimerOutSlt(void);               // 定时器到时

private:
    /* 设置短信页面控件 */
    myLabel *m_revNumLbl;
    myLabel *m_centerNumLbl;
    myLabel *m_mesLbl;
    myLineEdit *m_revNumEdit;
    myLineEdit *m_centerNumEdit;
    myLineEdit *m_mesEdit;
    myPushButton *m_testBtn;
    myPushButton *m_obtainAutoBtn;
    myPushButton *m_sendBtn;

    /* 设置短信页面数据 */
    QString m_strRevNum;
    QString m_strCenterNum;
    QString m_strMes;
    QString m_sendCont;
    QTimer  *m_timer;
    int     m_nTimes;
    int     m_pAlarm[MAX_ALARM];
    QString m_strAlarm;
};

#endif // SETSMS_H
