/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mysettime.cpp
 * \brief       设置时间控件源文件
 * \date        2015.01.14
 */
#include "mysettime.h"

mySetTime::mySetTime(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, LCD_TITLE_HEIGHT+1, LCD_WIDTH, LCD_CONTENT_HEIGTH-2));
    setStyleSheet(g_style1);

    gridLayout = new QGridLayout(this);
    gridLayout->setHorizontalSpacing(50);

    timeSignalMapper = new QSignalMapper(this);
    if(struCnfg.nLang == LANG_ENG ){
        warning = new myLabel("Please Confirm Current System Time?");
    } else{
        warning = new myLabel("请确认当前系统时间是否正确?(Please Confirm Current System Time?)");
    }
    warning->setMaximumHeight(BTN_HEIGHT);
    gridLayout->addWidget(warning,0,0,1,3);
    for (int i = 0; i < 6; i++) {
        dateLabel[i] = new myLabel("", this);
        dateLbe[i]   = new myLineEdit("", this);
        switch(i){
        case 0:
            dateLabel[i]->setText(myLan.year);
            break;
        case 1:
            dateLabel[i]->setText(myLan.month);
            break;
        case 2:
            dateLabel[i]->setText(myLan.day);
            break;
        case 3:
            dateLabel[i]->setText(myLan.hour);
            break;
        case 4:
            dateLabel[i]->setText(myLan.minute);
            break;
        case 5:
            dateLabel[i]->setText(myLan.sec);
            break;
        default:
            break;
        }

        dateLbe[i]->setReadOnly(true);
        dateLbe[i]->setMaximumHeight(BTN_HEIGHT);
        if (i < 3){
            gridLayout->addWidget(dateLabel[i],i/3+1,i%3,1,1,Qt::AlignCenter);
            gridLayout->addWidget(dateLbe[i]  ,i/3+2,i%3,1,1);
        } else {
            gridLayout->addWidget(dateLabel[i],i/3+2,i%3,1,1,Qt::AlignCenter);
            gridLayout->addWidget(dateLbe[i]  ,i/3+3,i%3,1,1);
        }

        /* 消息栈 */
        timeSignalMapper->setMapping(dateLbe[i], i);
        connect(dateLbe[i], SIGNAL(pressed()), timeSignalMapper, SLOT(map()));
    }

    QString  str;
    QDateTime  dateTime = QDateTime::currentDateTime();

    data[0] = dateTime.date().year();
    data[1] = dateTime.date().month();
    data[2] = dateTime.date().day();
    data[3] = dateTime.time().hour();
    data[4] = dateTime.time().minute();
    data[5] = dateTime.time().second();

    for (int i = 0; i < 6; i++) {
        str = QString("%1").arg(data[i]);
        dateLbe[i]->setText(str);
    }

    nextBtn = new myPushButton(myLan.back, myIcon.Action_Back, this);
    nextBtn->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));

    dateHBLayout = new QHBoxLayout();
    dateHBLayout->addStretch();
    dateHBLayout->addWidget(nextBtn);

    gridLayout->addLayout(dateHBLayout, 5, 0, 1,3);

    /*槽函数*/
    connect(timeSignalMapper, SIGNAL(mapped(int)), this, SLOT(getTimeIndex(int)));
    connect(nextBtn , SIGNAL(clicked()), this, SLOT(accept()));
}

QString mySetTime::getTime(){
    return dateLbe[0]->text()+dateLbe[1]->text()+dateLbe[2]->text();
}

QString mySetTime::getTime1(){
    return dateLbe[0]->text()+dateLbe[1]->text()+dateLbe[2]->text()+dateLbe[3]->text()+dateLbe[4]->text()+dateLbe[5]->text();
}

QString mySetTime::getDateTime(){
    QString month;
    QString day;
    QString hour;
    QString minitue;
    QString second;


    if(dateLbe[1]->text().toInt() <10){
         month = "0"+dateLbe[1]->text();
    }else{
        month = dateLbe[1]->text();
    }

    if(dateLbe[2]->text().toInt() <10){
        day = "0"+dateLbe[2]->text();
    }else{
        day = dateLbe[2]->text();
    }

    if(dateLbe[3]->text().toInt() <10){
        hour = "0"+dateLbe[3]->text();
    } else{
        hour = dateLbe[3]->text();
    }

    if(dateLbe[4]->text().toInt() <10){
        minitue = "0"+dateLbe[4]->text();
    }else{
        minitue = dateLbe[4]->text();
    }

    if(dateLbe[5]->text().toInt() <10){
        second = "0"+dateLbe[5]->text();
    } else{
        second = dateLbe[5]->text();
    }
    qDebug()<<"getDateTime:"<<dateLbe[0]->text()+month+day+hour+minitue+second<<flush;

    return dateLbe[0]->text()+month+day+hour+minitue+second;
}


void mySetTime::getTimeIndex(int index)
{
    int ret;
    QString str;
    int nMin,nMax;
    switch(index) {
    case 0://year
        nMin = 0;
        nMax = 2049;
        break;
    case 1://month
        nMin = 1;
        nMax = 12;
        break;
    case 2://day
        nMin = 1;
        nMax = 31;
        break;
    case 3://hour
        nMin = 0;
        nMax = 59;
        break;
    case 4://mintue
        nMin = 0;
        nMax = 59;
        break;
    case 5://second
        nMin = 0;
        nMax = 59;
        break;
    }

    myInputPanel inputDlg(intType,nMin,nMax,data[index]);
    ret = inputDlg.exec();

    if (ret == QDialog::Accepted) {
        data[index] = inputDlg.getValue();
        str = QString("%1").arg(data[index]);
        dateLbe[index]->setText(str);
#ifdef Q_OS_UNIX
        char time[64];
        sprintf(time, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"", data[0], data[1], data[2],
                data[3], data[4], data[5]);
        system(time);
        popen("hwclock -w","r");
#endif
    }
}
