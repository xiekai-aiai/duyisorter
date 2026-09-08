/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymessagebox.cpp
 * \brief       消息框控件源文件
 * \date        2015.01.14
 */
#include "mymessagebox.h"

myMessageBox::myMessageBox(int style, QString text,QWidget *parent) :
    QDialog(parent)
{
    int margin = 25;
    int nLabelHei;

    switch(LCD_WIDTH) {
    case 1024:
        margin = 10;
        nLabelHei = 120;
        break;
    case 640:
        margin = 5;
        nLabelHei = 80;
        break;
    }

    this->setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(LCD_WIDTH/4,LCD_HEIGHT/4,LCD_WIDTH/2,LCD_HEIGHT/2);
//    setStyleSheet(g_style2);
    QPalette palette;
    QPixmap pixmap1(":/res/png/messageBack.png");
    pixmap1 = pixmap1.scaled(LCD_WIDTH/2+5,LCD_HEIGHT/2+5);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    setPalette(palette);
    setAutoFillBackground(true);

    textLabel   = new myLabel(text, this);
    textLabel->setGeometry(QRect(margin,this->height()/2-30,this->width()-2*margin,nLabelHei));
    textLabel->setAlignment(Qt::AlignHCenter);
    iconLabel   = new myLabel("", this);
    iconLabel->setGeometry(QRect(margin,margin,50,50));
    iconLabel->setScaledContents(true);
    okBtn      =  new myPushButton(myLan.ok, QIcon(), this);
    okBtn->setGeometry(QRect(width()-BTN_WIDTH-margin,height()-60,BTN_WIDTH,BTN_HEIGHT));
    QString g_style1 = "background-color: rgb(124, 186, 224);""outline: none;""border-radius: 8;";

    okBtn->setStyleSheet(g_style1);

    cancelBtn  =  new myPushButton(myLan.cancel, QIcon(), this);
    cancelBtn->setGeometry(QRect(margin,this->height()-60,BTN_WIDTH,BTN_HEIGHT));
    QString g_style2 = "background-color: rgb(96, 156, 194);""outline: none;""border-radius: 8;";
    cancelBtn->setStyleSheet(g_style2);


    QPixmap pixmap;

    switch(style) {
    case MSG_QUES:
        pixmap = QPixmap(":/res/png/Message_Help.png");
        iconLabel->setPixmap(pixmap);
        break;
    case MSG_TIPS:
        pixmap = QPixmap(":/res/png/Message_Info.png");
        cancelBtn->setHidden(true);
        break;
    case MSG_INFO:
        pixmap = QPixmap(":/res/png/Message_Info.png");
        okBtn->setHidden(true);
        cancelBtn->setHidden(true);
        break;
    case MSG_WARN:
        pixmap = QPixmap(":/res/png/Message_Warning.png");
        okBtn->setHidden(true);
        cancelBtn->setText(myLan.back);
        break;
    case MSG_ABOUT:
        pixmap = QPixmap(":/res/png/Message_Info.png");
        okBtn->setHidden(true);
        cancelBtn->setText(myLan.back);
        break;
    }
    iconLabel->setPixmap(pixmap);

    connect(okBtn    ,   SIGNAL(pressed()), this,   SLOT(onOkBtnClicked()));
    connect(cancelBtn,  SIGNAL(pressed()),  this,   SLOT(onCancelBtnClicked()));

    if (style == MSG_INFO) {
      QTimer *timer=new QTimer(this);
      connect(timer,SIGNAL(timeout()),this,SLOT(close()));
      timer->start(3000);
    }
}

//  确定按钮
void myMessageBox::onOkBtnClicked()
{
    accept();
}

//  取消按钮
void myMessageBox::onCancelBtnClicked()
{
    reject();
}

void myMessageBox::setText(QString text)
{
    textLabel->setText(text);
}

myInfoWidget::myInfoWidget(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT);
    config = g_Config::getInstance();
    int margin = 25;

    widget = new QWidget(this);
    widget->setGeometry(width()/4, height()/4, width()/2, height()/2);
//    widget->setStyleSheet(g_style2);

    QPalette palette;
    QPixmap pixmap1(":/res/png/messageBack.png");
    pixmap1 = pixmap1.scaled(LCD_WIDTH/2+10,LCD_HEIGHT/2+10);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    widget->setPalette(palette);
    widget->setAutoFillBackground(true);

    warnningLabel = new QLabel(widget);
    warnningLabel->setFont(config->getFont());
    warnningLabel->setGeometry(margin, widget->height()/2, widget->width()-2*margin, 60);
    warnningLabel->setAlignment(Qt::AlignHCenter);
    iconLabel = new QLabel(widget);
    iconLabel->setGeometry(margin,margin,50,50);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(":/res/png/Message_Info.png"));
    cancelBtn = new myPushButton(myLan.cancel,myIcon.Action_Cancel,this);
    cancelBtn->setGeometry(QRect(width() / 2 + 15, height() / 2 + 60, BTN_WIDTH, BTN_HEIGHT));
    cancelBtn->hide();

    connect(cancelBtn,SIGNAL(pressed()),this,SLOT(onCancelBtnClicked()));
}

/* 析构函数*/
myInfoWidget::~myInfoWidget()
{
}

void myInfoWidget::setVisableCancelBtn(bool isShow)
{
    if (isShow) {
        cancelBtn->show();
    } else {
        cancelBtn->hide();
    }
}

void myInfoWidget::onCancelBtnClicked(void)
{
    cancelBtn->hide();
    emit sCancelBtnClicked();
}

/* 设置标签上的提醒文本 */
void myInfoWidget::setLabelText(QString text)
{
    widget->show();
    warnningLabel->setText(text);
}

/* 延迟一定时间后再显示 */
void myInfoWidget::delayShow()
{
    this->show();
    QTime dieTime = QTime::currentTime().addMSecs(10);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

/* 隐藏提示框，仅覆盖当前页面 */
void myInfoWidget::hideCentralWidget()
{
    widget->hide();
}

/* 显示secs秒后隐藏 */
void myInfoWidget::showSecs(int secs)
{
    this->show();
    QTime dieTime = QTime::currentTime().addSecs(secs);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
    this->hide();
}

/* 延迟一定时间后隐藏 */
void myInfoWidget::delayHide()
{
    QTime dieTime = QTime::currentTime().addSecs(1);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
    this->hide();
}

myAIInfoWidget::myAIInfoWidget(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT);
    config = g_Config::getInstance();
    int margin = 25;

    widget = new QWidget(this);
    widget->setGeometry(width()/4, height()/4, width()/2, height()/2);
//    widget->setStyleSheet(g_style2);

    QPalette palette;
    QPixmap pixmap1(":/res/png/messageBack.png");
    pixmap1 = pixmap1.scaled(LCD_WIDTH/2+10,LCD_HEIGHT/2+10);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    widget->setPalette(palette);
    widget->setAutoFillBackground(true);

    warnningLabel = new QLabel(widget);
    warnningLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    warnningLabel->setGeometry(margin, widget->height()/2, widget->width()-2*margin, 320);
    warnningLabel->setAlignment(Qt::AlignHCenter);
    iconLabel = new QLabel(widget);
    iconLabel->setGeometry(margin,margin,50,50);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(":/res/png/Message_Info.png"));
    cancelBtn = new myPushButton(myLan.cancel,myIcon.Action_Cancel,this);
    cancelBtn->setGeometry(QRect(width() / 2 + 15, height() / 2 + 60, BTN_WIDTH, BTN_HEIGHT));
    cancelBtn->hide();

    connect(cancelBtn,SIGNAL(pressed()),this,SLOT(onCancelBtnClicked()));
}

/* 析构函数*/
myAIInfoWidget::~myAIInfoWidget()
{
}

void myAIInfoWidget::setMsgHeightRatio(int ratio){
    int margin = 25;
    if(ratio <= 2){
        ratio = 2;
    }
    if(ratio <= 3 && ratio > 2){
        ratio = 3;
    }
    if(ratio <= 4 && ratio > 3){
        ratio = 4;
    }
    warnningLabel->setGeometry(margin, widget->height()/ratio, widget->width()-2*margin, 320);

}

void myAIInfoWidget::setVisableCancelBtn(bool isShow)
{
    if (isShow) {
        cancelBtn->show();
    } else {
        cancelBtn->hide();
    }
}

void myAIInfoWidget::onCancelBtnClicked(void)
{
    cancelBtn->hide();
    emit sCancelBtnClicked();
}

/* 设置标签上的提醒文本 */
void myAIInfoWidget::setLabelText(QString text)
{
    widget->show();
    warnningLabel->setText(text);
}

/* 延迟一定时间后再显示 */
void myAIInfoWidget::delayShow()
{
    this->show();
    QTime dieTime = QTime::currentTime().addMSecs(10);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

/* 隐藏提示框，仅覆盖当前页面 */
void myAIInfoWidget::hideCentralWidget()
{
    widget->hide();
}

/* 显示secs秒后隐藏 */
void myAIInfoWidget::showSecs(int secs)
{
    this->show();
    QTime dieTime = QTime::currentTime().addSecs(secs);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
    this->hide();
}

/* 延迟一定时间后隐藏 */
void myAIInfoWidget::delayHide()
{
    QTime dieTime = QTime::currentTime().addSecs(1);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
    this->hide();
}

myInfoListWidget::myInfoListWidget(QString text,QWidget *parent) :
    QWidget(parent)
{
    config = g_Config::getInstance();
    this->setGeometry(LCD_WIDTH/2,LCD_HEIGHT/2-LCD_STATUS_HEIGHT+100,LCD_WIDTH/2,LCD_HEIGHT/2-100);
//    this->setStyleSheet(g_style1);
    QPalette palette;
    QPixmap pixmap1(":/res/png/messageBack.png");
    pixmap1 = pixmap1.scaled(LCD_WIDTH/2+5,LCD_HEIGHT/2-95);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    setPalette(palette);
    setAutoFillBackground(true);

    widget = new QWidget(this);
    widget->setFixedSize(this->size());
    QFont fontStatusInfo;
    fontStatusInfo.setPixelSize(20);

    okBtn  =  new myPushButton(myLan.ok, QIcon(), this);
    okBtn->setMaximumHeight(BTN_HEIGHT);
    okBtn->setFixedWidth(DEFAULT_BTN_WIDTH);
    okBtn->setIconSize(QSize(25,25));
    okBtn->setFont(config->getFont(FONT_SIZE_16));
    QString g_style1 = "background-color: rgb(124, 186, 224);""outline: none;""border-radius: 8;";

    okBtn->setStyleSheet(g_style1);
    connect(okBtn,  SIGNAL(pressed()),  this,   SLOT(onOkBtnClicked()));

    m_listWidget = new QListWidget(this);
    m_listWidget->setFont(fontStatusInfo);
    m_listWidget->setGridSize(QSize(m_listWidget->width(),40));
    m_listWidget->setIconSize(QSize(30,30));
    m_listWidget->setFrameShape(QListWidget::NoFrame);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(20,20,20,10);
    layout->setRowStretch(0,5);
    layout->setRowStretch(1,1);
    layout->addWidget(m_listWidget,0,0,1,3);
    layout->addWidget(okBtn,1,2,1,1,Qt::AlignRight);
    this->setLayout(layout);
}

/* 析构函数*/
myInfoListWidget::~myInfoListWidget()
{
}

//设置提示信息，text为标红信息段
void myInfoListWidget::setlistInfoText(QStringList infoList, QString text){
    m_listWidget->clear();
    QListWidgetItem *item = new QListWidgetItem(QIcon(":/res/png/Message_Warning_red.png"),text);
    item->setTextColor(QColor(255,0,0));
    m_listWidget->addItem(item);

    for(int i=0; i<infoList.length(); i++){
        QListWidgetItem *item = new QListWidgetItem(QIcon(),infoList.at(i));
        m_listWidget->addItem(item);
    }
    m_listWidget->scrollToTop();
}

//  确定按钮
void myInfoListWidget::onOkBtnClicked()
{
    this->hide();

}

myInfoListWidget *infoList;
myInfoWidget *infoWidget;
myAIInfoWidget *aiInfoWidget;
QLabel *titleLabel;
