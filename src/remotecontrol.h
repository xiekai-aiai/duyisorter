/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        remotecontrol.h
 * \brief       远程控制界面头文件
 *              子页面包括网络配置、设置服务器、设置短信及调试管理。
 * \date        2015.01.14
 */
#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QWidget>
#include <QtCore>
#include <QtNetwork>
#include <QRegExp>
#include <QTextBrowser>
#include <QCalendarWidget>
#include <QDateTimeEdit>
#include "mypushbutton.h"
#include "mylabel.h"
#include "mylineedit.h"
#include "mylanguage.h"
#include "myinputmethod.h"
#include "myinputpanel.h"
#include "mymessagebox.h"
#include "globalflow.h"
#include "common/mygroupbox.h"
#include "common/myqrcode/myqrcode.h"
#include "common/myqrcode/myqrwidget.h"
#include "mydelaycode.h"
#include "mywid.h"

/*!
 * \enum 枚举网络配置类型
 */
enum {
    NET_IP,         //! IP地址
    NET_MASK,       //! 子网掩码
    NET_GATEWAY,    //! 网关
    NET_DNS,        //! DNS服务器
    NET_MAC,        //! MAC地址
    NET_ROUTE,      //! 路由表
    NET_DEVID,      //! 设备ID
    NET_SERVER      //! 服务器地址
};

//! 最多从设备数量
#define MAX_SLAVE   4

/*!
 * \brief 网络连接测试线程
 * 即在OpenVPN运行时每5s发送一次ping命令,判断服务器是否有效
 */
class pingThread : public QThread
{
    Q_OBJECT
public:
    explicit pingThread();
    bool serverIsAlive(void);

protected:
    void run(void);

signals:
    void pingFailSig();

public slots:
    void stopPing(void);

private:
    bool m_bIsRunning;
    QProcess *m_cmd;
};

/*!
 * \brief 远程协助界面类
 */
class MyRemoteControl : public QWidget
{
    Q_OBJECT
public:
    explicit MyRemoteControl(QWidget *parent = 0);

    void createRemoteControlPage(void);             // 创建远程控制页面
    void setConfFile(void);                         // 根据用户设置修改配置文件
    void setPasswordFile(void);                     // 根据用户设置修改用户名密码文件
    void getConfFromFile(void);                     // 进入远程升级界面时从本地文件中读取配置信息
    void getPasswordFromFile(void);                 // 进入远程升级界面时从本地文件中读取用户名密码
    bool getLocalVirtualAddr(void);                 // 获取本地虚拟网络地址
    bool connetToServer(void);                      // 判断屏连接服务器是否成功
    QString getVpnIpAddress();

    
signals:
    void backToFactoryMainPageSig();
    
public slots:
    void onServerUnaliveSlt(void);                  // 响应服务器失效的槽函数

    /* 远程控制页面控件对应的槽函数 */
    void onServerIPLineEditPressed(void);           // 响应服务器地址输入框被按下事件
    void onPortLineEditPressed(void);               // 响应服务器端口号被按下事件
    void onPortLineEditNCPressed(void);             // 响应网管服务器端口号被按下事件
    void onUsernameLineEditPressed(void);           // 响应用户名输入框被按下事件
    void onPasswordLineEditPressed(void);           // 响应密码输入框被按下事件
    void onConnectBtnPressed(void);                 // 响应连接按钮按下事件
    void onDisconnectBtnPressed(void);              // 响应断开连接按钮按下事件
    void onBackBtnPressed(void);                    // 响应返回按钮按下事件

private:
    /* 远程控制页面检查服务器是否工作的线程 */
    pingThread *m_thread;
    /* 远程控制页面相关控件 */
    myLabel *m_serverIPLabel;
    myLineEdit *m_serverIPLineEdit;
    myLabel *m_portLabel;
    myLineEdit *m_portLineEdit;
    myLabel *m_portLabelNC;
    myLineEdit *m_portLineEditNC;       //! 与网管通信的端口号
    myLabel *m_usernameLabel;
    myLineEdit *m_usernameLineEdit;
    myLabel *m_passwordLabel;
    myLineEdit *m_passwordLineEdit;
    myLabel *m_localVirtualAddrLabel;
    myLineEdit *m_localVirtualAddrLineEdit;
    myPushButton *m_connectBtn;
    myPushButton *m_disconnectBtn;
};

/*!
 * \brief 本机网络配置界面类
 */
class MyNetConfig: public QWidget
{
	Q_OBJECT
public:
    explicit MyNetConfig(QWidget *parent = 0);

    QString getNetText(int index);       //! 获取网络配置选项文本
    void getNetSetting(QString setting); //! 获取网络配置文件参数
    void setNetSetting(QString setting); //! 保存网络配置文件参数
	void getNetConfig();
	void setNetConfig(int index);
	QString getDNS();
	QString getGateway();
	QString getRoute();
signals:

public slots:
	void getHostAddress();
	void setHostAddress(int index);

private:
	QVector<myLabel*>    m_hostLabel;
	QVector<myLineEdit*> m_hostValue;
	QSignalMapper        *m_sigMapper;
};

/*!
 * \brief 大数据管理界面类
 */
class MyBigData : public QWidget
{
    Q_OBJECT

public:
    explicit MyBigData(QWidget *parent = 0);
    ~MyBigData();

signals:

public slots:
    void generateQRcode();
    void setServer();

private:
    QString generateQRString();

    myGroupBox *m_serverCbx;
    myGroupBox *m_qrcodeCbx;
    myLabel *m_qrCodeLabel;
    myLabel *m_serverLabel;
    myLineEdit *m_serverValue;
};

/*!
 * \brief 日志查看界面类
 */
class MyLoggerViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MyLoggerViewer(QWidget *parent = 0);
    ~MyLoggerViewer();
    void updateText();
    void updateTextByDate();
    void updateTextByEventSeq();
    void reloadLogFile();
    qint64 getIndexOfCharacter(const char *str, quint64 len, char c);

signals:

private slots:
    void onDateTimeEditFromChanged(QDateTime dateTime);
    void onDateTimeEditToChanged(QDateTime dateTime);
    void OnEventChanged(int eventSeq);
    void onReloadLogFileBtnClicked();

private:
    QTextBrowser *qText;
    QDateTimeEdit *dateTimeEditFrom;
    QDateTimeEdit *dateTimeEditTo;
    MyComboBox *eventTypeCombo;
    myPushButton *reLoadFileBtn;
    QDateTime dateTimeFrom;
    QDateTime dateTimeTo;
    int maxLine;    //! 界面显示的最大记录数

    QFile qFile;
    char *tmpFileData;
    int dateLen;
    QString FirstdateInCurFile;     //! 获取当前记录文件的第一条记录的时间
    int isBackFileHandled;          //! 标示备份的日志文件（mylog.1）是否被处理 0:未处理,1:已处理
    QVector<QString> qLog;          //! 日期区间内的全部信息
    int logEventSeq;
};

/*!
 * \brief 从设备列表类
 */
class MySlaveList : public QWidget
{
    Q_OBJECT

public:
    explicit MySlaveList(QWidget *parent = 0);
    ~MySlaveList();

    void refreshDisplay(void);      //! 刷新显示

signals:

public slots:
    void onLineEditPressed(int nIndex);     //! 编辑从设备列表

private:
    myLabel *m_label[MAX_SLAVE];
    myLineEdit *m_lineEdit[MAX_SLAVE];
    QSignalMapper *m_sigMap;
};

#endif // REMOTECONTROL_H
