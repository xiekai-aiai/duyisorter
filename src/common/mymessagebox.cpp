/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymessagebox.cpp
 * \brief       消息框控件源文件
 * \date        2026.11.21
 */
#include "mymessagebox.h"
#include <QFrame>
#include <QLabel>

myMessageBox::myMessageBox(int style, QString text,QWidget *parent) :
    QDialog(parent)
{
    const int dlgW = 600;
    const int dlgH = 340;
    const int btnW = 108;
    const int btnH = 32;

    this->setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(dlgW, dlgH);
    setGeometry((LCD_WIDTH - dlgW)/2, (LCD_HEIGHT - dlgH)/2, dlgW, dlgH);

    /* 科技风深色背景 */
    QPalette palette;
    QPixmap pixmap1(":/res/png2/dialog/dlgBg.png");
    pixmap1 = pixmap1.scaled(dlgW, dlgH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    setPalette(palette);
    setAutoFillBackground(true);

    QFont titleFont = g_Config::getInstance()->getFont();
    titleFont.setPixelSize(15);
    QFont textFont = g_Config::getInstance()->getFont();
    textFont.setPixelSize(20);

    /* 标题栏 */
    QLabel *titleLabel = new QLabel(tr("系统提示"), this);
    titleLabel->setGeometry(QRect(24, 9, 260, 28));
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #CFEFF5; background: transparent;");

    QFrame *titleLine = new QFrame(this);
    titleLine->setGeometry(QRect(0, 46, dlgW, 1));
    titleLine->setStyleSheet("background-color: #143C4C; border: none;");

    iconLabel   = new myLabel("", this);
    iconLabel->setGeometry(QRect((dlgW - 48)/2, 78, 48, 48));
    iconLabel->setScaledContents(true);

    textLabel   = new myLabel(text, this);
    textLabel->setGeometry(QRect(40, 156, dlgW - 80, 100));
    textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    textLabel->setWordWrap(true);
    textLabel->setFont(textFont);

    /* 深色科技风按钮（保留文字，随语言切换） */
    QString btnStyle = QString(
        "QPushButton {"
        "  background-color: #0B2230;"
        "  border: 1px solid #35D6E6;"
        "  border-radius: 2px;"
        "  color: #CFEFF5;"
        "  font-size: 14px;"
        "  padding: 0px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #10384C;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #071B26;"
        "  border-color: #2AB5C4;"
        "}");

    okBtn      =  new myPushButton(myLan.ok, QIcon(), this);
    okBtn->setGeometry(QRect(dlgW - btnW - 24, dlgH - btnH - 16, btnW, btnH));
    okBtn->setStyleSheet(btnStyle);

    cancelBtn  =  new myPushButton(myLan.cancel, QIcon(), this);
    cancelBtn->setGeometry(QRect(24, dlgH - btnH - 16, btnW, btnH));
    cancelBtn->setStyleSheet(btnStyle);

    QPixmap pixmap;

    switch(style) {
    case MSG_QUES:
        pixmap = QPixmap(":/res/png2/dialog/ico_info.png");
        break;
    case MSG_TIPS:
        pixmap = QPixmap(":/res/png2/dialog/ico_info.png");
        cancelBtn->setHidden(true);
        break;
    case MSG_INFO:
        pixmap = QPixmap(":/res/png2/dialog/ico_info.png");
        okBtn->setHidden(true);
        cancelBtn->setHidden(true);
        break;
    case MSG_WARN:
        pixmap = QPixmap(":/res/png2/dialog/ico_warn.png");
        okBtn->setHidden(true);
        cancelBtn->setText(myLan.back);
        break;
    case MSG_ABOUT:
        pixmap = QPixmap(":/res/png2/dialog/ico_info.png");
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

    const int dlgW = 600;
    const int dlgH = 340;
    const int dlgX = (LCD_WIDTH - dlgW)/2;
    const int dlgY = (LCD_HEIGHT - dlgH)/2;

    widget = new QWidget(this);
    widget->setGeometry(dlgX, dlgY, dlgW, dlgH);

    /* 科技风深色背景 */
    QPalette palette;
    QPixmap pixmap1(":/res/png2/dialog/dlgBg.png");
    pixmap1 = pixmap1.scaled(dlgW, dlgH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    widget->setPalette(palette);
    widget->setAutoFillBackground(true);

    QFont titleFont = config->getFont();
    titleFont.setPixelSize(15);
    QFont msgFont = config->getFont();
    msgFont.setPixelSize(20);

    QLabel *titleLabel = new QLabel(tr("系统提示"), widget);
    titleLabel->setGeometry(QRect(24, 9, 260, 28));
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #CFEFF5; background: transparent;");

    QFrame *titleLine = new QFrame(widget);
    titleLine->setGeometry(QRect(0, 46, dlgW, 1));
    titleLine->setStyleSheet("background-color: #143C4C; border: none;");

    iconLabel = new QLabel(widget);
    iconLabel->setGeometry(QRect((dlgW - 48)/2, 78, 48, 48));
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(":/res/png2/dialog/ico_info.png"));

    warnningLabel = new QLabel(widget);
    warnningLabel->setFont(msgFont);
    warnningLabel->setGeometry(40, 156, dlgW - 80, 100);
    warnningLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    warnningLabel->setWordWrap(true);
    warnningLabel->setStyleSheet("color: #EAF6FA; background: transparent;");

    cancelBtn = new myPushButton(myLan.cancel, QIcon(), this);
    cancelBtn->setGeometry(QRect(dlgX + dlgW - 108 - 24, dlgY + dlgH - 32 - 16, 108, 32));
    cancelBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #0B2230;"
        "  border: 1px solid #35D6E6;"
        "  border-radius: 2px;"
        "  color: #CFEFF5;"
        "  font-size: 14px;"
        "  padding: 0px;"
        "}"
        "QPushButton:hover { background-color: #10384C; }"
        "QPushButton:pressed { background-color: #071B26; border-color: #2AB5C4; }");
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

    const int dlgW = 600;
    const int dlgH = 340;
    const int dlgX = (LCD_WIDTH - dlgW)/2;
    const int dlgY = (LCD_HEIGHT - dlgH)/2;

    widget = new QWidget(this);
    widget->setGeometry(dlgX, dlgY, dlgW, dlgH);

    /* 科技风深色背景 */
    QPalette palette;
    QPixmap pixmap1(":/res/png2/dialog/dlgBg.png");
    pixmap1 = pixmap1.scaled(dlgW, dlgH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    widget->setPalette(palette);
    widget->setAutoFillBackground(true);

    QFont titleFont = config->getFont();
    titleFont.setPixelSize(15);
    QFont msgFont = config->getFont();
    msgFont.setPixelSize(20);

    QLabel *titleLabel = new QLabel(tr("系统提示"), widget);
    titleLabel->setGeometry(QRect(24, 9, 260, 28));
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #CFEFF5; background: transparent;");

    QFrame *titleLine = new QFrame(widget);
    titleLine->setGeometry(QRect(0, 46, dlgW, 1));
    titleLine->setStyleSheet("background-color: #143C4C; border: none;");

    iconLabel = new QLabel(widget);
    iconLabel->setGeometry(QRect((dlgW - 48)/2, 78, 48, 48));
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(":/res/png2/dialog/ico_info.png"));

    warnningLabel = new QLabel(widget);
    warnningLabel->setFont(msgFont);
    warnningLabel->setGeometry(40, 156, dlgW - 80, 100);
    warnningLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    warnningLabel->setWordWrap(true);
    warnningLabel->setStyleSheet("color: #EAF6FA; background: transparent;");

    cancelBtn = new myPushButton(myLan.cancel, QIcon(), this);
    cancelBtn->setGeometry(QRect(dlgX + dlgW - 108 - 24, dlgY + dlgH - 32 - 16, 108, 32));
    cancelBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #0B2230;"
        "  border: 1px solid #35D6E6;"
        "  border-radius: 2px;"
        "  color: #CFEFF5;"
        "  font-size: 14px;"
        "  padding: 0px;"
        "}"
        "QPushButton:hover { background-color: #10384C; }"
        "QPushButton:pressed { background-color: #071B26; border-color: #2AB5C4; }");
    cancelBtn->hide();

    connect(cancelBtn,SIGNAL(pressed()),this,SLOT(onCancelBtnClicked()));
} 

/* 析构函数*/
myAIInfoWidget::~myAIInfoWidget()
{
}

void myAIInfoWidget::setMsgHeightRatio(int ratio){
    /* 新版弹窗固定文本区域，ratio 仅保留接口兼容 */
    Q_UNUSED(ratio);
    warnningLabel->setGeometry(40, 146, widget->width() - 80, 130);
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
