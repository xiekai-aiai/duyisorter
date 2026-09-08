/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymessagebox.h
 * \brief       消息框控件头文件
 * \date        2015.01.14
 */
#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QDialog>
#include <QWidget>
#include <QListWidget>
#include "mypushbutton.h"
#include "globalconfig.h"
#include "mylabel.h"
#include "mylanguage.h"
#include "myicon.h"

#define MSG_QUES    0       //  询问操作
#define MSG_INFO    1       //  信息提示
#define MSG_WARN    2       //  警告操作
#define MSG_ABOUT   3       //  关于信息
#define MSG_TIPS    4       //  提示信息

/*!
 * \brief 消息对话框控件类
 */
class myMessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit myMessageBox(int style, QString text,QWidget *parent = 0);
    void setText(QString text);

signals:
    
public slots:
    void onOkBtnClicked();
    void onCancelBtnClicked();

private:
    myPushButton    *okBtn;
    myPushButton    *cancelBtn;
    myLabel         *textLabel;
    myLabel         *iconLabel;
};

class myInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit myInfoWidget(QWidget *parent = 0);
    ~myInfoWidget();
    void setLabelText(QString text);
    void delayShow(void);
    void hideCentralWidget(void);
    void showSecs(int secs = 2);
    void delayHide(void);
    void setVisableCancelBtn(bool isShow = false);

public slots:
    void onCancelBtnClicked(void);
signals:
    void sCancelBtnClicked();

private:
    QFont font;
    QWidget *widget;
    QLabel *warnningLabel, *iconLabel;
    g_Config *config;
    myPushButton *cancelBtn;//此按钮可控制串口采集图像时取消收集图像数据
};

class myAIInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit myAIInfoWidget(QWidget *parent = 0);
    ~myAIInfoWidget();
    void setLabelText(QString text);
    void delayShow(void);
    void hideCentralWidget(void);
    void showSecs(int secs = 2);
    void delayHide(void);
    void setVisableCancelBtn(bool isShow = false);
    void setMsgHeightRatio(int ratio);

public slots:
    void onCancelBtnClicked(void);
signals:
    void sCancelBtnClicked();

private:
    QFont font;
    QWidget *widget;
    QLabel *warnningLabel, *iconLabel;
    g_Config *config;
    myPushButton *cancelBtn;//此按钮可控制串口采集图像时取消收集图像数据
};

class myInfoListWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit myInfoListWidget(QString text, QWidget *parent = 0);
        ~myInfoListWidget();
    void setlistInfoText(QStringList infoList, QString text);

    public slots:
        void onOkBtnClicked();
    signals:

    private:
        QFont font;
        QWidget *widget;
        QListWidget *m_listWidget;
        g_Config *config;
        myPushButton *okBtn;//此按钮可控制串口采集图像时取消收集图像数据
    };

extern myInfoWidget *infoWidget;
extern myAIInfoWidget *aiInfoWidget;
extern QLabel *titleLabel;
extern myInfoListWidget *infoList;

#endif // MYMESSAGEBOX_H
