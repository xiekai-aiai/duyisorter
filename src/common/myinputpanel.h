/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myinputpanel.h
 * \brief       数字输入框控件头文件
 * \date        2015.01.14
 */
#ifndef MYINPUTPANEL_H
#define MYINPUTPANEL_H

#include <QDialog>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QGridLayout>
#include "globalconfig.h"
#include "mypushbutton.h"
#include "mylineedit.h"
#include "myslider.h"
#include "mylcdnumber.h"
#include "mylabel.h"
#include "mylanguage.h"
#include "myicon.h"

enum inputType {
    floatType,
    intType,
    passwdType,
    textType
};

/*!
 * \brief 数字输入框控件类
 */
class myInputPanel : public QDialog
{
    Q_OBJECT
    
public:
    myInputPanel(inputType type, int nMin, int nMax, double nDisp, QWidget *parent = 0);
    ~myInputPanel();

    void setValue(double value);
    double getValue(void);
    QString getText(void);
    void setTitle(QString text);
    void setText(QString text);
    void setInputType(inputType type);
    void setValueMin(int nMin);
    void setValueMax(int nMax);

    void setInputNegEn(bool bFlag);     // 设置是否可输入负数
    void setJudgeEn(bool bFlag);        // 设置是否判断最大值及最小值

private slots:
    void sendChar(int indexOfCharToSend);
    void buttonC1Function(void);
    void buttonC2Function(void);
    void buttonC3Function(void);
    void onMinusBtnClickedSlt(void);
    void onAddBtnClickedSlt(void);
    void onValueSliderChangeSlt(void);

private:
    g_Config *config;
    inputType     nFlagMode;             //浮点模式,整型模式
    QSignalMapper *signalMapper;
    QVector<myPushButton *> button;      //11个字母

    myLabel      *titleLabel;            //title
    myLineEdit   *textLbe;
    myPushButton *buttonC1;              //DEL
    myPushButton *buttonC2;              //Ok
    myPushButton *buttonC3;              //Cancel

    mySlider        *valueSlider;
    myPushButton    *minusBtn;
    myPushButton    *addBtn;
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
    bool   b_FlagCLear;
    bool m_bInputNegSign;   // 可输入负号标志
    bool m_bJudgeEn;        // 判断大小标志
};

#endif // MYINPUTPANEL_H
