/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mysensinputpanel.h
 * \brief       灵敏度数字输入框控件头文件
 * \date        2015.01.14
 */
#ifndef MYSENSINPUTPANEL_H
#define MYSENSINPUTPANEL_H

#include <QDialog>
#include "globalconfig.h"
#include "mypushbutton.h"
#include "mylineedit.h"
#include "myslider.h"
#include "mylcdnumber.h"
#include "mylabel.h"
#include "globalparams.h"

enum sensInputType{
    floatSensType,
    intSensType,
    passwdSensType,
    textSensType
    };

/*!
 * \brief 灵敏度数字输入框控件类
 */
class mySensInputPanel : public QDialog
{
    Q_OBJECT

public:
    mySensInputPanel(sensInputType type,int nMin,int nMax, int index, int address, double nDisp, QWidget *parent = 0);
    ~mySensInputPanel();

    void setValue(double value);
    double getValue(void);
    QString getText(void);
    void setTitle(QString text);
    void setText(QString text);
    void setInputType(sensInputType type);
    void setValueMin(int nMin);
    void setValueMax(int nMax);
    void initTmpData(void);         // 初始化临时变量
    int getSens(int nIndex);        // 获取灵敏度

private slots:
    void sendChar(int indexOfCharToSend);
    void buttonC1Function(void);
    void buttonC2Function(void);
    void buttonC3Function(void);
    void updataTabBar(void);
    void updateTabBarStackWidget(int nTabIndex);

private:
    g_Config *config;
    QFont font;
    sensInputType     nFlagMode;        // 浮点模式,整型模式
    QSignalMapper *signalMapper;
    QVector<myPushButton *> button;     // 11个字母

    myLabel      *titleLabel;           // title
    QTabBar      *tabBarIdentify;
    myLineEdit   *textLbe;
    myPushButton *buttonC1;             // DEL
    myPushButton *buttonC2;             // Ok
    myPushButton *buttonC3;             // Cancel
    QSpacerItem *horizontalSpacer;

    void createButtons(void);
    void checkDotBtn(void);
    QVBoxLayout *mainVerticalLayout;

    QHBoxLayout *horizontalLayout_0;
    QHBoxLayout *horizontalLayout_1;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_5;
    QGridLayout *gridLayout_6;
    QGridLayout *gridLayout_7;

    int nMinNum;
    int nMaxNum;
    double nDispNum;
    int    nValueDisp;
    int    nChannelIndex;   // 当前滑槽组别索引
    int    nAddress;
    bool   b_FlagCLear;
    int m_nTmpData[4];      // 存储灵敏度的临时变量
    int m_nPreIndex;        // 记录上一次标签的索引
};

#endif // MYSENSINPUTPANEL_H
