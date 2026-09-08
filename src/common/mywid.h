/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mywid.h
 * \brief       定制控件头文件
 * \date        2015.01.14
 */
#ifndef MY_WIDGET_H
#define MY_WIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QToolButton>
#include <QRadioButton>
#include <QComboBox>
#include <QButtonGroup>
#include <QTabBar>
#include "mypushbutton.h"
#include "mylabel.h"
#include "mycustomcheckbox.h"
#include "mypushbutton.h"
#include "mylineedit.h"
#include "myinputpanel.h"
#include "myinputmethod.h"
#include "mygroupbox.h"
#include "mymessagebox.h"
#include "mylistwidget.h"
#include "globalconfig.h"
#include "mycurve.h"
#include "mylistwidgetitem.h"
#include "myvaluebar.h"
QT_BEGIN_NAMESPACE
class QScrollBar;
class QLCDNumber;
class QSignalMapper;
class QToolButton;
class QLabel;
QT_END_NAMESPACE

class MyComboBox : public QComboBox
{
    Q_OBJECT

public:
    MyComboBox(QWidget *parent = 0);

private:
    g_Config *config;
};

class MyTabBar : public QTabBar
{
    Q_OBJECT

public:
    MyTabBar(QWidget *parent = 0);
    void setTabBarType(int type);
    int  getTabBarType();

private:
    int tabBarType;//0:可见光识别组的tab条；1：红外识别组的tab条
    g_Config *config;
};


/****************************************************************************
**
** declaration of class MyValueBar
**
****************************************************************************/
class MyValueBar : public QWidget
{
    Q_OBJECT

public:
    MyValueBar(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = 0);

signals:
    void valueChanged(int value);

public slots:
    void setValue(int value);
    void setMaximum(int value);
    void setMinimum(int value);
    void setRange(int min, int max);
    int  getValue();
    void onMinusBtnClicked();
    void onPlusBtnClicked();

private:
    myPushButton *minusBtn;
    myPushButton *plusBtn;
    QSlider *scrollBar;
    QLCDNumber *lcdNumber;
};



/****************************************************************************
**
** declaration of class MyCheckBox
**
****************************************************************************/
class MyCheckBox : public QWidget
{
    Q_OBJECT

public:
    MyCheckBox(const QString text, bool isChecked = false, QBoxLayout::Direction dir = QBoxLayout::LeftToRight, QWidget *parent = 0);
    bool isChecked();
    void setText(QString text);
    void setChecked(bool checked);

signals:
    void pressed();
    void toggled(bool checked);

public slots:
    void click();

private:
    myPushButton    *checkBtn;
    myLabel         *checkLabel;
    bool            checked;
};

/****************************************************************************
**
** declaration of class MyButtonGroup
**
****************************************************************************/
class MyButtonGroup : public QWidget
{
    Q_OBJECT

public:
    MyButtonGroup(const QStringList &list, QWidget *parent = 0);
    void setText(QStringList list);
    int size();
    int currentIndex();

signals:
    void currentChanged(int);

public slots:
    void setCurrentIndex(int index);
    void setChecked(int btnId);

private:
    QVector<QToolButton *>  btns;
    QSignalMapper           *sigMapper;

    int m_nCurrent;
    g_Config *config;
};

/****************************************************************************
**
** definition of class MyRadioGroup
**
****************************************************************************/
class MyRadioGroup : public QWidget
{
    Q_OBJECT

public:
    MyRadioGroup(const QStringList& list, Qt::Orientation orientation = Qt::Vertical, QWidget *parent = 0);
    int  size() const;
    int  currentIndex() const;

signals:
    void currentChanged(int index);

public slots:
    void setCurrentIndex(int index);
    void setChecked(int index);

private:
    QVector<myPushButton *> btns;
    QVector<myLabel *>      labels;
    QSignalMapper           *sigMapper;
    QBoxLayout              *layout;

    int m_nCurrent;
};

#endif
