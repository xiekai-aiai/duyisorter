/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myvaluebar.h
 * \brief       数字显示控制控件头文件
 * \date        2015.01.14
 */
#ifndef MYVALUEBAR_H
#define MYVALUEBAR_H

#include <QWidget>
#include <QLayout>
#include "globalconfig.h"
#include "mypushbutton.h"
#include "mylcdnumber.h"
#include "myinputpanel.h"

/*!
 * \brief 数字显示控制控件类
 */
class myValueBar : public QWidget
{
    Q_OBJECT
    
public:
    myValueBar(int min, int max, double slowChgValue, double fastChgValue,
                        double initValue, QWidget *parent = 0);
    myValueBar(double *arr, int arrElementTotal, double initValue, QWidget *parent = 0);
    ~myValueBar();
    void setValue(double value);
    double getValue(void);
    void hideFastChangeBtns(void);
    void setMaxValue(int value);
    void setMinValue(int value);
    void setChgFastValue(int value);
    void setChgValue(int value);
    void hideBtns(void);
    void showBtns(void);
    void setIsPop(bool flag);
    void setDigitalCount(int num);
    void setLcdNumSize(QSize size);
    void setLcdNumPressEnable(bool flag);
    void setEnableChanged(bool flag);       // 设置显示的值可否改变
    void setDigitalCountByMax(void);        // 根据最大值设置lcdNum显示数字的位数

signals:
    void pressed();
    void valueChanged(int);
    void lcdNumPressed();

private slots:
    void onMinusFastBtnClicked();
    void onMinusBtnClicked();
    void onPlusBtnClicked();
    void onPlusFastBtnClicked();
    void onLcdNumberPressed();

private:
    g_Config *config;
    myPushButton *plusBtn;
    myPushButton *plusFastBtn;
    myPushButton *minusBtn;
    myPushButton *minusFastBtn;
    myLCDNumber *lcdNum;
    double chgValue;
    double chgFastValue;
    int maxValue;
    int minValue;

    /* 标志是否弹出对话框 */
    bool m_bIsPop;
    bool m_bEnableChanged;

    /*使能ValueBar的不同功能*/
    int style;              //0,普通的valueBar;1,限定值的valueBar
    double tmpArr[20];      //限定值的数组
    int currentArrSuffix;   //当前数组下标，即ValueBar的当前显示值
    int arrEleTotal;        //限定值个数
};

#endif // MYVALUEBAR_H
