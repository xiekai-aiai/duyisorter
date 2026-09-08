/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mydelaycode.h
 * \brief       延迟付款加密头文件
 * \date        2015.01.14
 */
#ifndef MYDELAYCODE_H
#define MYDELAYCODE_H

#include <QtGui>
#include <QDialog>
#include "common/libdccrypt/dccrypt.h"
#include "globalparams.h"
#include "common/myinputmethod.h"
#include "common/mymessagebox.h"
#include "common/mysettime.h"
#ifdef Q_OS_UNIX
    #include "common/myeeprom/myeeprom.h"
#endif

#define CNF_FILE_CURTIME      "userdata/cnf/cnf.curtime"
#define CNF_FILE_ADDTIME      "userdata/cnf/cnf.addtime"
#define CNF_FILE_DELAYCODE    "userdata/cnf/cnf.delaycode"
#define BASE_ADDR_DELAY	0x00000000

/*!
 * \brief 延迟付款类
 */
class myDelayCode : public QObject
{
public:
    explicit myDelayCode(QObject *parent = 0);
    char curDate[64];
    char limit_cur[DCCRYPT_MAX_STR];
    char product_sn_cur[DCCRYPT_MAX_STR];
    char limit[DCCRYPT_MAX_STR];
    char product_sn[DCCRYPT_MAX_STR];
    char delay_code[DCCRYPT_MAX_STR];
    char delay_code_eeprom[DCCRYPT_MAX_STR];
    char ena;
    char size;
    char limitCnt;

    int lcd_width;
    int lcd_height;
    bool m_bDelayCodeType;              /*!<< 加密类型：false代表为累计计时，true为系统时间计时*/
    int  m_nDelayCodeTime;              /*!<< 延时加密码剩余时间 */
    int  m_nEnableDelayCodeInfo;

    int delayCodeCheck();
    int delaycodeMatch(const char *str);		// delay code match
    int delaycodeSave(char *str);		// delay code save
    int delayCodeGetTime();             /*!<< 加密剩余时间获取*/
};
extern myDelayCode paramDelayCode;

/*!
 * \brief 延迟付款解密类
 */
class myDccrypt : public QDialog
{
    Q_OBJECT
public:
    explicit myDccrypt(QWidget *parent = 0);
    QTimer *timer;
    QString str;

    bool verifyUSB(void);                               /*!< 函数：验证权限卡 */
signals:
    void inputDelayCodeSig();
    void explainDelayCodeSig();
    
public slots:
    void timeOutSlt();
    void inputDelayCodeSlt();
    void explainDelayCodeSlt();
};

/*!
 * \brief 延迟付款加密类
 */
class myEncrypt : public QDialog
{
    Q_OBJECT
public:
    explicit myEncrypt(QWidget *parent = 0);
    bool bFlagSetNewTime;
    QTimer *timer;
    QString str;

signals:
    void inputDelayCodeSig();
    void explainDelayCodeSig();
    void setCurrentTimeSig();

public slots:
    void timeOutSlt();
    void inputDelayCodeSlt();
    void explainDelayCodeSlt();
    void setCurrentTimeSlt();

};
#endif // MYDELAYCODE_H
