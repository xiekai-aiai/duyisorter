/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mywid.cpp
 * \brief       定制控件源文件
 * \date        2015.01.14
 */
#include <QtGui>
#include "mywid.h"

MyComboBox::MyComboBox(QWidget *parent)
    : QComboBox(parent)
{
    config = g_Config::getInstance();
    setFont(config->getFont());

    setStyleSheet("QComboBox {"
                      "background-color: #ebebeb;"
                      "border-width: 2px;"
                      "border-color: #afafaf;"
                      "border-style: solid;"
                      "border-radius: 10;"
                      "padding-left: 3px;"
                      "padding-right: 20px;}"/* space for the arrow */
                      "min-width: 40px;"
                  "QComboBox QAbstractItemView::item{min-height: 40px;}"
                  );
}

MyTabBar::MyTabBar(QWidget *parent)
    : QTabBar(parent)
{
    config = g_Config::getInstance();
    setFont(config->getFont());
    if (LCD_HEIGHT == 768) {
        setStyleSheet("QTabBar::tab {"
                      "min-height: 45;}");
    } else {
        setStyleSheet("QTabBar::tab {"
                      "min-height: 32;}");
    }
    tabBarType = 0;//默认可见光识别组的tabBar
}

void MyTabBar::setTabBarType(int type)
{
    tabBarType = type;
}

int MyTabBar::getTabBarType()
{
    return tabBarType;
}

/****************************************************************************
**
** definition of class MyValueBar
**
****************************************************************************/
MyValueBar::MyValueBar(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
    minusBtn = new myPushButton(myIcon.Action_Minus, QSize(BTN_HEIGHT, BTN_HEIGHT));
    plusBtn  = new myPushButton(myIcon.Action_Plus, QSize(BTN_HEIGHT, BTN_HEIGHT));

    scrollBar = new QSlider(orientation);
    scrollBar->setFocusPolicy(Qt::StrongFocus);
    if (orientation == Qt::Horizontal) {
        scrollBar->setStyleSheet("QSlider::groove:horizontal{border: 1px solid #999999;height: 45px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);     margin: 2px 0; }  QSlider::handle:horizontal {background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);     border: 1px solid #5c5c5c;     width: 45px;     margin: -2px 0;border-radius: 3px; }");
        scrollBar->setFixedHeight(BTN_HEIGHT);
    }
    else {
        scrollBar->setStyleSheet("QSlider::groove:vertical{border: 1px solid #999999;width: 45px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);     margin: 2px 0; }  QSlider::handle:vertical {background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);     border: 1px solid #5c5c5c;     height: 20px;     margin: -2px 0;border-radius: 3px; }");
        scrollBar->setFixedWidth(BTN_HEIGHT);
    }

    lcdNumber = new QLCDNumber;
    lcdNumber->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    /* signals to slots */
    connect(minusBtn, SIGNAL(clicked()), this, SLOT(onMinusBtnClicked()));
    connect(plusBtn, SIGNAL(clicked()), this, SLOT(onPlusBtnClicked()));
    connect(scrollBar, SIGNAL(valueChanged(int)), lcdNumber, SLOT(display(int)));
    connect(scrollBar, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

    /* layout */
    QBoxLayout::Direction direction;

    if (orientation == Qt::Horizontal)
        direction = QBoxLayout::LeftToRight;
    else
        direction = QBoxLayout::TopToBottom;

    QBoxLayout *layout = new QBoxLayout(direction);

    if (orientation == Qt::Horizontal) {
        layout->addWidget(minusBtn);
        layout->addWidget(scrollBar);
        layout->addWidget(plusBtn);
        layout->addWidget(lcdNumber);
    }
    else {
        layout->addWidget(plusBtn);
        layout->addWidget(scrollBar);
        layout->addWidget(minusBtn);
        layout->addWidget(lcdNumber);
    }
    setLayout(layout);
}

void MyValueBar::setValue(int value)
{
    scrollBar->setValue(value);
}

void MyValueBar::setMaximum(int value)
{
    scrollBar->setMaximum(value);
}

void MyValueBar::setMinimum(int value)
{
    scrollBar->setMinimum(value);
}

void MyValueBar::setRange(int min, int max)
{
    scrollBar->setRange(min, max);
}

int MyValueBar::getValue()
{
    return lcdNumber->value();
}

void MyValueBar::onMinusBtnClicked()
{
    int value = scrollBar->value();

    if (value > scrollBar->minimum()) {
        value--;
        scrollBar->setValue(value);
    }
}

void MyValueBar::onPlusBtnClicked()
{
    int value = scrollBar->value();
    if (value < scrollBar->maximum()) {
        value++;
        scrollBar->setValue(value);
    }
}

/****************************************************************************
**
** definition of class MyCheckBox
**
****************************************************************************/
MyCheckBox::MyCheckBox(const QString text, bool isChecked, QBoxLayout::Direction direction, QWidget *parent)
    : QWidget(parent)
{
    /* buttons init */
    checkLabel = new myLabel(text);
    checkBtn = new myPushButton(QIcon(), QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));

    /* buttons update */
    setChecked(isChecked);

    /* signals to slots */
    connect(checkBtn, SIGNAL(pressed()), this, SLOT(click()));

    /* layout */
    QBoxLayout *mainLayout = new QBoxLayout(direction);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(checkBtn);
    mainLayout->addWidget(checkLabel);
    setLayout(mainLayout);
}

void MyCheckBox::click()
{
    setChecked(!checked);
    emit pressed();
}

void MyCheckBox::setChecked(bool isChecked)
{
    if (checked != isChecked) {
        /* update params */
        checked = isChecked;

        /* update display */
        if (checked) {
            checkBtn->setIcon(myIcon.Action_Select);
        } else {
            checkBtn->setIcon(QIcon());
        }

        /* trigger */
        emit toggled(checked);
    }
}

bool MyCheckBox::isChecked()
{
    return checked;
}

void MyCheckBox::setText(QString text)
{
    checkLabel->setText(text);
}


/****************************************************************************
**
** definition of class MyButtonGroup
**
****************************************************************************/
MyButtonGroup::MyButtonGroup(const QStringList & list, QWidget *parent)
    : QWidget(parent)
{
    if (list.size() == 0)
        return;

    /* page init */
    config = g_Config::getInstance();
    for (int i = 0; i < list.size(); i++) {
        btns.append(new QToolButton);
        btns[i]->setText(list[i]);
        btns[i]->setFont(config->getFont());
        btns[i]->setMinimumHeight(ICON_HEI);
        btns[i]->setMaximumHeight(BTN_HEIGHT);
        btns[i]->setMinimumWidth(80);
        btns[i]->setCheckable(true);
    }

    /* page update */
    m_nCurrent = 0;
    setChecked(m_nCurrent);

    /* signals to slots */
    sigMapper = new QSignalMapper;
    for (int i = 0; i < btns.size(); i++) {
        sigMapper->setMapping(btns[i], i);
        connect(btns[i], SIGNAL(pressed()), sigMapper, SLOT(map()));
    }
    connect(sigMapper, SIGNAL(mapped(int)), this, SLOT(setCurrentIndex(int)));
    connect(this,      SIGNAL(currentChanged(int)), this, SLOT(setChecked(int)));

    /* layout */
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    for(int i = 0; i < btns.size(); i++) {
        mainLayout->addWidget(btns[i]);
    }
    setLayout(mainLayout);
}

int MyButtonGroup::size()
{
    return btns.size();
}

int MyButtonGroup::currentIndex(void)
{
    return m_nCurrent;
}

void MyButtonGroup::setText(QStringList list)
{
    for (int i = 0; i < size(); i++) {
        btns[i]->setText(list[i]);
    }
}

void MyButtonGroup::setCurrentIndex(int index)
{
    if (m_nCurrent != index) {
        m_nCurrent = index;

        emit currentChanged(index);
    }
    setChecked(index);
}

void MyButtonGroup::setChecked(int btnId)
{
    for (int i = 0; i < btns.size(); i++) {
        if (i == btnId) {
            btns[i]->setChecked(true);
            btns[i]->setStyleSheet("background-color: #036eb8; border-radius: 5;");
        } else {
            btns[i]->setChecked(false);
            btns[i]->setStyleSheet("background-color: #ebebeb; border-radius: 5;");
        }
    }
}

/****************************************************************************
**
** definition of class MyRadioGroup
**
****************************************************************************/
MyRadioGroup::MyRadioGroup(const QStringList &list, Qt::Orientation orientation, QWidget *parent)
    : QWidget (parent)
{
    /* page init */
    if (list.size() == 0)
        return;

    for (int i = 0; i < list.size(); i++) {
        btns.append(new myPushButton(QIcon(), QSize(SMALL_BTN_WIDTH, BTN_HEIGHT)));
        labels.append(new myLabel(list[i]));
    }

    /* page update */
    m_nCurrent = 0;
    setChecked(m_nCurrent);

    /* signals mapper */
    sigMapper = new QSignalMapper;
    for (int i = 0; i < btns.size(); i++) {
        sigMapper->setMapping(btns[i], i);
        connect(btns[i], SIGNAL(pressed()), sigMapper, SLOT(map()));
    }
    connect(sigMapper, SIGNAL(mapped(int)),         this, SLOT(setCurrentIndex(int)));
    connect(this,      SIGNAL(currentChanged(int)), this, SLOT(setChecked(int)));

    /* btns layout */
    QVector<QHBoxLayout *> btnLayout;
    for (int i = 0; i < btns.size(); i++) {
        btnLayout.append(new QHBoxLayout);
        btnLayout[i]->setContentsMargins(0, 0, 0, 0);
        btnLayout[i]->addWidget(btns[i]);
        btnLayout[i]->addWidget(labels[i]);
    }

    /* main layout */
    QBoxLayout::Direction direction;
    if (orientation == Qt::Horizontal)
        direction = QBoxLayout::LeftToRight;
    else
        direction = QBoxLayout::TopToBottom;

    layout = new QBoxLayout(direction);
    layout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < btns.size(); i++) {
        layout->addLayout(btnLayout[i]);
    }
    setLayout(layout);
}

int MyRadioGroup::size() const
{
    return btns.size();
}

int MyRadioGroup::currentIndex() const
{
    return m_nCurrent;
}

void MyRadioGroup::setCurrentIndex(int index)
{
    if (m_nCurrent != index) {
        m_nCurrent = index;

        /* trigger */
        emit currentChanged(index);
    }
}

void MyRadioGroup::setChecked(int index)
{
    for (int i = 0; i < btns.size(); i++) {
        if (i == index) {
            btns[i]->setIcon(myIcon.Action_Select);
        } else {
            btns[i]->setIcon(QIcon());
        }
    }
}
