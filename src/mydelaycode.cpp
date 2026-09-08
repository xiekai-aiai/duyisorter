/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mydelaycode.cpp
 * \brief       延迟付款加密源文件
 * \date        2015.01.14
 */
#include "mydelaycode.h"
#include <time.h>
#include <QApplication>
#include <QtGui>
#include <QProcess>

myDelayCode paramDelayCode;
myDelayCode::myDelayCode(QObject *parent) :
    QObject(parent)
{
    memset(limit_cur        ,0, DCCRYPT_MAX_STR);
    memset(product_sn_cur   ,0, DCCRYPT_MAX_STR);
    memset(limit            ,0, DCCRYPT_MAX_STR);
    memset(product_sn       ,0, DCCRYPT_MAX_STR);
    memset(delay_code       ,0, DCCRYPT_MAX_STR);
    memset(delay_code_eeprom,0, DCCRYPT_MAX_STR);
    ena  = 0;
    size = 0;
    limitCnt = 255;

    lcd_width  = 640;
    lcd_height = 480;
    m_bDelayCodeType = false;
    m_nDelayCodeTime = 0;
    m_nEnableDelayCodeInfo = 0;
}

int myDelayCode::delayCodeCheck()
{
    char curYear[64];
    char curMonth[64];
    char curDay[64];
    char curHour[64];
    char curMin[64];
    char curSec[64];
    time_t curtime,savedtime = 0;
    struct tm *stru_curtime;
    FILE *fp;
    int counter;
    /* 加密使能 */
#ifdef Q_OS_UNIX
    for (int i = 0; i < 3; ) {
        if (myeeprom_read(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR, &ena, 1)) {
            if (ena == 0xFF) {
                 i++;
                 continue;
            } else {
                break;
            }
        } else {
            i++;
            if (i == 3)
                return 0;
            }
        }

	    qDebug()<<ena;
	    /*加密长度*/
	    if (ena == 0xAA) {
            for (int i = 0; i < 3; ) {
                if (myeeprom_read(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR+1, &size, 1)) {
                    if (size == 0xFF) {
                        i++;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    i++;
                    if (i == 3) {
                        return 0;
                    }
                }
		    }
        } else {
            return 0;//无加密
	    }
	    qDebug()<<(int)size;
	    /*读取加密码*/
	    if (size != 0xFF) {
            for (int i = 0; i < 3; ) {
                if(myeeprom_read(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR+2, delay_code_eeprom,size)){
                    if (delay_code_eeprom[0] == 0xFF
                            && delay_code_eeprom[1] == 0xFF
                            && delay_code_eeprom[2] == 0xFF
                            && delay_code_eeprom[3] == 0xFF) {
                        i++;
                        if (i == 3) {
                            return 0;
                        }
                        continue;
                    } else {
                        break;
                    }
                } else {
                    i++;
                    if (i == 3) {
                        return 0;
                    }
                }
		    }
        } else {
		    return 0;
	    }
#endif
#ifdef win32
    if (strlen(delay_code_eeprom) == 0){
        return 0;
#endif
    /*启动定时保存参数功能*/
    QProcess *process = new QProcess;
    QStringList strList;
    strList<<"";
//    process->startDetached("./MyTimer",strList);

    /*检测加密方式，累计计时加密及系统时间加密，判断加密是否到期*/
    qDebug()<<delay_code_eeprom;
    int ret_dc = 0;
//    int ret_dc = dc_decrypt(delay_code_eeprom,limit_cur,product_sn_cur);
    /* 非法期限码，解密时作为无效 */
    if (ret_dc != 0) {
       paramDelayCode.ena = 0xFF;
#ifdef Q_OS_UNIX
                for(int i = 0; i < 3;i++) {
                    myeeprom_write(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR, &paramDelayCode.ena, 1);
                //	myeeprom_read(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR, &paramDelayCode.ena, 1);
                    qDebug("%d\n",paramDelayCode.ena);
                    usleep(5000);
                }

#endif
        return 0;
    }
    qDebug()<<atoi(limit_cur);
    qDebug()<<product_sn_cur;
    if ((int)(atoi(limit_cur)/100000) == 888) {
           if ((fp = fopen(CNF_FILE_ADDTIME,"r"))){
                fscanf(fp,"%d",&counter);
                fclose(fp);
            }
           if (counter < atoi(limit_cur+3)*3600){
                    return 0;
                }
     } else {
        //! 系统加密
        char cTmpSaveTime[64];   //! 0-1:代表年，2：代表月，3:代表日
        int  nTmpSaveTime = 19700101;   //! 保存时间
#ifdef Q_OS_UNIX
        //! 读取eeprom内保存累计使用限制，为FF时，正常，小于FF时异常
        if(myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &limitCnt,1)){
            qDebug("limit cnt %d",limitCnt);
            if (limitCnt != 0xFF) {
                limitCnt++;
                myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &limitCnt,1);
            }
        }
        else {
            qDebug("erro");
            return 0;
        }

        //! 判断系统时间是否正常
        if (limitCnt == 0xFF) {
            //! 读取eeprom内保存的系统时间
            if(myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, cTmpSaveTime,4)){
                if (cTmpSaveTime[0] <= 20 && cTmpSaveTime[1] <= 99 && cTmpSaveTime[2] <= 12 && cTmpSaveTime[3] <= 31){
                    nTmpSaveTime = cTmpSaveTime[0]*1000000+cTmpSaveTime[1]*10000+cTmpSaveTime[2]*100+cTmpSaveTime[3];
                    qDebug("saveTime %d",nTmpSaveTime);
                }
            }

            //! 读取当前系统时间
            time(&curtime);
            stru_curtime = localtime(&curtime);
            strftime(curDate,64,"%Y%m%d",stru_curtime);

            //! 比较当前时间与加密时间关系
            if (atoi(curDate) < atoi(limit_cur)) {
                //! 比较保存系统时间与当前时间关系,并保存当前时间
                if (nTmpSaveTime > atoi(curDate)){
                    limitCnt = 0;
                    myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &limitCnt,1);
                    myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &limitCnt,1);
                    qDebug("limit cnt %d",limitCnt);
                }
                else {
                    char tmpCurDate[64];
                    int nTmpCurDate = atoi(curDate);
                    tmpCurDate[0] = nTmpCurDate/1000000;
                    tmpCurDate[1] = (nTmpCurDate%1000000)/10000;
                    tmpCurDate[2] = (nTmpCurDate%10000)/100;
                    tmpCurDate[3] = nTmpCurDate%100;
                    myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
                    myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
                    qDebug("%d",(int)tmpCurDate[0]);
                    qDebug("%d",(int)tmpCurDate[1]);
                    qDebug("%d",(int)tmpCurDate[2]);
                    qDebug("%d",(int)tmpCurDate[3]);
            }

            qDebug("time also %d",atoi(limit_cur)-atoi(curDate));
                return 0;
            }
            qDebug("timeout");
        }
        else {
            if (limitCnt < 5){
                return 0;
            }
            else {
                limitCnt = 5;
            }
        }
#endif
    }
    return 1;
}

int myDelayCode::delaycodeMatch(const char *str)
{
    FILE *fp;
    char line[DCCRYPT_MAX_STR] = "";

    if (!(fp = fopen(CNF_FILE_DELAYCODE, "r"))) {
        fprintf(stderr, "unable to open %s\n", CNF_FILE_DELAYCODE);
        return 0;
    }

    while (!feof(fp)) {
        fscanf(fp,"%s", line);
        printf("%s\n",line);
        printf("%s\n",str);
        if (strcmp(line, str) == 0) {
            return 1;
        }
    }

    fclose(fp);

    return 0;
}

int myDelayCode::delaycodeSave(char *str)
{
    FILE *fp;

    if (!(fp = fopen(CNF_FILE_DELAYCODE, "a"))) {
        fprintf(stderr, "unable to open %s\n", CNF_FILE_DELAYCODE);
        return 0;
    }

    fprintf(fp, "%s\n", str);
    fclose(fp);

    return 1;
}

/*!
 * \brief myDelayCode::delayCodeGetTime
 * \return
 */
int myDelayCode::delayCodeGetTime()
{
    FILE *fp;
    int counter;
    time_t curtime;
    struct tm *stru_curtime;
    time_t limit_curtime;
    struct tm stru_limit_curtime;

    if (ena != 0xAA) {
        return 0;
    }
    /*检测加密方式，获取加密剩余时间*/
    qDebug()<<delay_code_eeprom;
    //dc_decrypt(delay_code_eeprom,limit_cur,product_sn_cur);
    qDebug()<<atoi(limit_cur);
    qDebug()<<product_sn_cur;
    if ((int)(atoi(limit_cur)/100000) == 888) {
        m_bDelayCodeType = false;
        if ((fp = fopen(CNF_FILE_ADDTIME,"r"))){
            fscanf(fp,"%d",&counter);
            fclose(fp);
        }
        if (counter < atoi(limit_cur+3)*3600) {
            m_nDelayCodeTime = atoi(limit_cur+3)*3600-counter;
            qDebug("time-%d(h)",m_nDelayCodeTime);
        } else {
           return 0;
        }
     } else {
        m_bDelayCodeType = true;
        time(&curtime);
        stru_curtime = localtime(&curtime);

        if (atoi(curDate) < atoi(limit_cur)) {
        stru_limit_curtime.tm_year = (int)(atoi(limit_cur)/10000)-1900;
        qDebug()<<stru_limit_curtime.tm_year;
        int tmp = (int)(atoi(limit_cur+4)/100)-1;
        if (tmp < 0) {
            tmp = 0;
        }
        stru_limit_curtime.tm_mon  = tmp;
        qDebug()<<stru_limit_curtime.tm_mon;
        stru_limit_curtime.tm_mday  = (int)(atoi(limit_cur)%100);
        qDebug()<<stru_limit_curtime.tm_mday;
        stru_limit_curtime.tm_hour  = 23;
        qDebug()<<stru_limit_curtime.tm_hour;
        stru_limit_curtime.tm_min   = 59;
        qDebug()<<stru_limit_curtime.tm_min;
        stru_limit_curtime.tm_sec   = 59;
        qDebug()<<stru_limit_curtime.tm_sec;

        limit_curtime = mktime(&stru_limit_curtime);

        m_nDelayCodeTime = (int)abs(difftime(limit_curtime,curtime));
        qDebug("limit_time-%d(d)",atoi(limit_cur));
        qDebug("cur_time-%d(d)",atoi(curDate));
        qDebug("time-%d(s)",m_nDelayCodeTime);
        } else {
            return 0;
        }
    }
    return 1;
}

/**************解密代码********/
myDccrypt::myDccrypt(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, LCD_TITLE_HEIGHT+1, LCD_WIDTH, LCD_CONTENT_HEIGTH-2));
    setStyleSheet(g_style1);

    /*自运行程序*/
    timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeOutSlt()));
    timer->start(100);

    /*槽函数*/
    connect(this,SIGNAL(inputDelayCodeSig())  ,this,SLOT(inputDelayCodeSlt()));
    connect(this,SIGNAL(explainDelayCodeSig()),this,SLOT(explainDelayCodeSlt()));
}

/*!
 * \brief myDccrypt::verifyUSB
 * \return
 */
bool myDccrypt::verifyUSB()
{
#ifdef Q_OS_UNIX
//    unsigned char usbkey[64] = {};

//    //! verify usb key
//    //! prevent re-open
//    if (myUsbDev != NULL)
//        usb.myusbFree();

//    //! open usb device node
//    usb.myusbInit();
//    if (myUsbDev  == NULL) {
//        return false;
//    }

//    //! read usb buffer
//    usb_bulk_read(myUsbDev, USB_EP_IN, (char *)usbkey, 6, 500);
//    if (usbkey[0] != 0x35 || usbkey[1] != 0xB7 || usbkey[2] != 0xC4
//            || usbkey[3] != 0x5A || usbkey[4] != 0x99 || usbkey[5] != 0xEF) {
//        usb.myusbFree();
//        return false;
//    }

//    usb.myusbFree();
//    return true;
#else
    return (QDateTime::currentDateTime().toTime_t()%2) ? true : false;
#endif
}

/*!
 * \brief myDccrypt::timeOutSlt
 */
void myDccrypt::timeOutSlt()
{
    timer->stop();
    emit inputDelayCodeSig();
}

void myDccrypt::inputDelayCodeSlt()
{
    str = QString("%1").arg(myLan.dc_input);
    const char *text = str.toLatin1();
    str.sprintf("%s %s%s", text, paramDelayCode.product_sn_cur, paramDelayCode.limit_cur+2);

    myInputMethod inputDlg(str,"");
    inputDlg.setType(KB_DC);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted){
      str = inputDlg.getText();
      qDebug()<<str;
      //! 检测是否到出错次数（5次）
      if (paramDelayCode.limitCnt == 5){
          //! 检测是否插入权限卡
          if (verifyUSB()) {
              emit explainDelayCodeSig();
          }
          else {
              emit inputDelayCodeSig();
          }
      }
      else {
          emit explainDelayCodeSig();
      }
    }
    else {
        emit inputDelayCodeSig();
    }
}

void myDccrypt::explainDelayCodeSlt()
{
    int counter = 0;
    time_t curtime;
    FILE *fp;
    int ret = 0;
    struct tm *stru_curtime;
    char tmpCurDate[64];

    const char *delay_char = str.toLatin1();
    memcpy(paramDelayCode.delay_code    , delay_char, DCCRYPT_MAX_STR);
    memset(paramDelayCode.limit         ,0, DCCRYPT_MAX_STR);
    memset(paramDelayCode.product_sn    ,0, DCCRYPT_MAX_STR);

    myMessageBox msgBox(MSG_QUES,"");
    myMessageBox msgBox1(MSG_INFO,myLan.dc_illegal_used);

//    switch(dc_decrypt(paramDelayCode.delay_code, paramDelayCode.limit, paramDelayCode.product_sn))
    switch(0)
    {
        case 0:
		    qDebug()<<paramDelayCode.delay_code;
           str = QString("%1: %2\n%3: %4%5%6%7-%8%9-%10%11").arg(myLan.dc_productsn).arg(paramDelayCode.product_sn).arg(myLan.dc_limit).arg(paramDelayCode.limit[0]).arg(paramDelayCode.limit[1]).arg(paramDelayCode.limit[2]).arg(paramDelayCode.limit[3]).arg(paramDelayCode.limit[4]).arg(paramDelayCode.limit[5]).arg(paramDelayCode.limit[6]).arg(paramDelayCode.limit[7]);

		    msgBox.setText(str);
		    ret = msgBox.exec();

		    if (ret == QDialog::Accepted) {
			/*机器编号不匹配*/
			if (strcmp(paramDelayCode.product_sn, paramDelayCode.product_sn_cur)) {
                    msgBox1.setText(myLan.dc_productsn_mismatch);
			    msgBox1.exec();
			    emit inputDelayCodeSig();
			    return;
			}
			/*累计计时加密方式*/
			if ((int)(atoi(paramDelayCode.limit)/100000) == 888) {
                /*加密码是否已经使用过*/
                if (paramDelayCode.delaycodeMatch(paramDelayCode.delay_code)) {
                    msgBox1.exec();
                    emit inputDelayCodeSig();
                    return;
                } else {/*保存加密码*/
                    if (!paramDelayCode.delaycodeSave(paramDelayCode.delay_code)) {
                        msgBox1.setText(myLan.dc_save_erro);
                        msgBox1.exec();
                        emit inputDelayCodeSig();
                        return;
                    }
                }
			    counter = 0;
			}
			/*加密时间为无效时间*/
			if (atoi(paramDelayCode.curDate) > atoi(paramDelayCode.limit)) {
                    msgBox1.setText(myLan.dc_illegal);
			    msgBox1.exec();
			    emit inputDelayCodeSig();
			    return;
			}
			/*彻底解密*/
			if (strcmp(paramDelayCode.limit, "20990101") == 0) {
    			    memset(paramDelayCode.limit_cur         ,0, DCCRYPT_MAX_STR);
    			    memset(paramDelayCode.product_sn_cur    ,0, DCCRYPT_MAX_STR);
			    memset(paramDelayCode.delay_code_eeprom, 0, sizeof(paramDelayCode.delay_code_eeprom));
			    paramDelayCode.ena = 0xFF;
			}
			else {
			    strcpy(paramDelayCode.delay_code_eeprom, paramDelayCode.delay_code);
			    paramDelayCode.ena = 0xAA;
			}

            //! 如果出现系统时间出错，重新设置时间
            if (paramDelayCode.limitCnt == 5){
               qDebug("Set New Time");
                mySetTime setTime;
                setTime.exec();
            }

			/* get current time */
			if ((fp = fopen(CNF_FILE_ADDTIME, "w"))) {
			    fprintf(fp, "%d",counter);
			    fclose(fp);
			}

			time(&curtime);
            stru_curtime = localtime(&curtime);
            strftime(tmpCurDate,64,"%Y%m%d",stru_curtime);
            int nTmpCurDate = atoi(tmpCurDate);
            tmpCurDate[0] = nTmpCurDate/1000000;
            tmpCurDate[1] = (nTmpCurDate%1000000)/10000;
            tmpCurDate[2] = (nTmpCurDate%10000)/100;
            tmpCurDate[3] = nTmpCurDate%100;

			if ((fp = fopen(CNF_FILE_CURTIME, "w"))) {
			    fwrite(&curtime, sizeof(time_t), 1, fp);
			    fclose(fp);
			}
#ifdef Q_OS_UNIX
			/* to eeprom */
		    	for(int i = 0; i < 3;i++) {        
				myeeprom_write(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR, &paramDelayCode.ena, 1);
				qDebug("%d\n",paramDelayCode.ena);
				paramDelayCode.size = strlen(paramDelayCode.delay_code_eeprom);
				myeeprom_write(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR+1, &paramDelayCode.size, 1);
				qDebug("%d\n",paramDelayCode.size);
				myeeprom_write(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR+2, paramDelayCode.delay_code_eeprom, strlen(paramDelayCode.delay_code_eeprom));
				qDebug("%s\n",paramDelayCode.delay_code_eeprom);
				usleep(5000);
			}
                //! 修改系统时间出错次数为0xFF
                paramDelayCode.limitCnt = 0xFF;
                myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &paramDelayCode.limitCnt,1);
                myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &paramDelayCode.limitCnt, 1);
                qDebug("%d",(int)paramDelayCode.limitCnt);

                //! 保存当前时间
                myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
                myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
                qDebug("%d",(int)tmpCurDate[0]);
                qDebug("%d",(int)tmpCurDate[1]);
                qDebug("%d",(int)tmpCurDate[2]);
                qDebug("%d",(int)tmpCurDate[3]);
#endif
			accept();
		    }
		    else {
			 emit inputDelayCodeSig();
			 return;
		    }
		    break;
	    case -1:
	    case -2:
	    case -9:
            msgBox1.setText(myLan.dc_illegal);
            msgBox1.exec();
            emit inputDelayCodeSig();
            return;
    }
}

/*加密部分*/
myEncrypt::myEncrypt(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, LCD_TITLE_HEIGHT+1, LCD_WIDTH, LCD_CONTENT_HEIGTH-2));
    setStyleSheet(g_style1);

    bFlagSetNewTime = true;
    /*自运行程序*/
    timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeOutSlt()));//timeoutslot()为自定义槽
    timer->start(100);

    /*槽函数*/
    connect(this,SIGNAL(inputDelayCodeSig())  ,this,SLOT(inputDelayCodeSlt()));
    connect(this,SIGNAL(explainDelayCodeSig()),this,SLOT(explainDelayCodeSlt()));
    connect(this,SIGNAL(setCurrentTimeSig())  ,this,SLOT(setCurrentTimeSlt()));
}


void myEncrypt::timeOutSlt()
{
    timer->stop();
    emit inputDelayCodeSig();
}

void myEncrypt::inputDelayCodeSlt()
{
    /* get current limit and product_sn */
   if (strlen(paramDelayCode.delay_code_eeprom) > 0) {
//        dc_decrypt(paramDelayCode.delay_code_eeprom, paramDelayCode.limit_cur, paramDelayCode.product_sn_cur);
    	bFlagSetNewTime = false;
    }

    str = QString("%1").arg(myLan.dc_input);
    const char *text = str.toLatin1();
    str.sprintf("%s\n%s%s", text, paramDelayCode.product_sn_cur, paramDelayCode.limit_cur+2);
//    qDebug()<<"str"<<str<<flush;

    myInputMethod inputDlg(str,"");
    inputDlg.setType(KB_DC);
    if (!paramDelayCode.m_nEnableDelayCodeInfo) {
    	inputDlg.showEnableDelayCodeCbx(false);
    }
    else {
    	inputDlg.showEnableDelayCodeCbx(true);
    }
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
      str = inputDlg.getText();
      qDebug()<<str;
      paramDelayCode.m_nEnableDelayCodeInfo = inputDlg.getDelayCodeInfoValue();
      qDebug()<<paramDelayCode.m_nEnableDelayCodeInfo;
      emit explainDelayCodeSig();
    }
    else {
        paramDelayCode.m_nEnableDelayCodeInfo = inputDlg.getDelayCodeInfoValue();
        qDebug()<<paramDelayCode.m_nEnableDelayCodeInfo;
        reject();
    }
}

void myEncrypt::explainDelayCodeSlt()
{
    int counter = 0;
    time_t curtime;
    FILE *fp;
    int ret = 0;
    struct tm *stru_curtime;
    char tmpCurDate[64];

    const char *delay_char = str.toLatin1();
    memcpy(paramDelayCode.delay_code,delay_char, DCCRYPT_MAX_STR);

    myMessageBox msgBox(MSG_QUES,"");
    myMessageBox msgBox1(MSG_INFO,myLan.dc_illegal_used);
    switch(0)
    {
//	    case 0:
        case -1:
        case -2:
        case -9:
           str = QString("%1: %2\n%3: %4%5%6%7-%8%9-%10%11").arg(myLan.dc_productsn).arg(paramDelayCode.product_sn).arg(myLan.dc_limit).arg(paramDelayCode.limit[0]).arg(paramDelayCode.limit[1]).arg(paramDelayCode.limit[2]).arg(paramDelayCode.limit[3]).arg(paramDelayCode.limit[4]).arg(paramDelayCode.limit[5]).arg(paramDelayCode.limit[6]).arg(paramDelayCode.limit[7]);

		    msgBox.setText(str);
		    ret = msgBox.exec();

            if (ret == QDialog::Accepted) {
                /*机器编号不匹配*/
                if (strlen(paramDelayCode.delay_code_eeprom) == 0) {
                    bFlagSetNewTime = true;
                } else if (strcmp(paramDelayCode.product_sn, paramDelayCode.product_sn_cur) != 0) {
                    msgBox1.setText(myLan.dc_productsn_mismatch);
                    msgBox1.exec();
                    emit inputDelayCodeSig();
                    return;
                }
                /*累计计时加密方式*/
                if ((int)(atoi(paramDelayCode.limit)/100000) == 888) {
                    /*加密码是否已经使用过*/
                    if (paramDelayCode.delaycodeMatch(paramDelayCode.delay_code)) {
                        msgBox1.exec();
                        emit inputDelayCodeSig();
                        return;
                    } else {/*保存加密码*/
                        if(!paramDelayCode.delaycodeSave(paramDelayCode.delay_code)) {
                            msgBox1.setText(myLan.dc_save_erro);
                            msgBox1.exec();
                            emit inputDelayCodeSig();
                            return;
                        }
                    }
                    counter = 0;
                    bFlagSetNewTime = false;
                }
                /*彻底解密*/
                if (strcmp(paramDelayCode.limit, "20990101") == 0) {
                        memset(paramDelayCode.limit_cur         ,0, DCCRYPT_MAX_STR);
                        memset(paramDelayCode.product_sn_cur    ,0, DCCRYPT_MAX_STR);
                    memset(paramDelayCode.delay_code_eeprom,0, sizeof(paramDelayCode.delay_code_eeprom));
                    paramDelayCode.ena = 0xFF;
                } else {
                    strcpy(paramDelayCode.delay_code_eeprom, paramDelayCode.delay_code);
                    paramDelayCode.ena = 0xAA;
                }

                /* get current time */
                if ((fp = fopen(CNF_FILE_ADDTIME, "w"))) {
                    fprintf(fp, "%d",counter);
                    fclose(fp);
                }

                time(&curtime);
                stru_curtime = localtime(&curtime);
                strftime(tmpCurDate,64,"%Y%m%d",stru_curtime);
                int nTmpCurDate = atoi(tmpCurDate);
                tmpCurDate[0] = nTmpCurDate/1000000;
                tmpCurDate[1] = (nTmpCurDate%1000000)/10000;
                tmpCurDate[2] = (nTmpCurDate%10000)/100;
                tmpCurDate[3] = nTmpCurDate%100;

                if ((fp = fopen(CNF_FILE_CURTIME, "w"))) {
                    fwrite(&curtime, sizeof(time_t), 1, fp);
                    fclose(fp);
                }
#ifdef Q_OS_UNIX
                /* to eeprom */
                /*保存时间到EEPROM，3次*/
                for(int i = 0; i < 3;i++) {
                    myeeprom_write(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR, &paramDelayCode.ena, 1);
                    qDebug("%d\n",paramDelayCode.ena);
                    paramDelayCode.size = strlen(paramDelayCode.delay_code_eeprom);
                    myeeprom_write(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR+1, &paramDelayCode.size, 1);
                    qDebug("%d\n",paramDelayCode.size);
                    myeeprom_write(BASE_ADDR_DELAY+i*DCCRYPT_MAX_STR+2, paramDelayCode.delay_code_eeprom, strlen(paramDelayCode.delay_code_eeprom));
                    qDebug("%s\n",paramDelayCode.delay_code_eeprom);
                    usleep(5000);
                }

                //! 修改系统时间出错次数为0xFF
                paramDelayCode.limitCnt = 0xFF;
                myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &paramDelayCode.limitCnt,1);
                myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR, &paramDelayCode.limitCnt, 1);
                qDebug("%d",(int)paramDelayCode.limitCnt);

                //! 保存当前时间
                myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
                myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
                qDebug("%d",(int)tmpCurDate[0]);
                qDebug("%d",(int)tmpCurDate[1]);
                qDebug("%d",(int)tmpCurDate[2]);
                qDebug("%d",(int)tmpCurDate[3]);
#endif
                if (bFlagSetNewTime) {//启动设置时间
                    emit setCurrentTimeSig();
                    return;
                } else {
                    reject();
                }
            } else {
                reject();
		    }
		    break;
//	    case -1:
//	    case -2:
//	    case -9:
        case 0:
            msgBox1.setText(myLan.dc_illegal);
            msgBox1.exec();
            emit inputDelayCodeSig();
            return;
    }
}

void myEncrypt::setCurrentTimeSlt()
{
    struct tm *stru_curtime;
    time_t curtime;
    char tmpCurDate[64];

    mySetTime setTime;
    int ret = setTime.exec();
    if (ret == QDialog::Accepted) {
        accept();

#ifdef Q_OS_UNIX
        time(&curtime);
        stru_curtime = localtime(&curtime);
        strftime(tmpCurDate,64,"%Y%m%d",stru_curtime);
        int nTmpCurDate = atoi(tmpCurDate);
        tmpCurDate[0] = nTmpCurDate/1000000;
        tmpCurDate[1] = (nTmpCurDate%1000000)/10000;
        tmpCurDate[2] = (nTmpCurDate%10000)/100;
        tmpCurDate[3] = nTmpCurDate%100;

        myeeprom_write(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
        myeeprom_read(BASE_ADDR_DELAY+3*DCCRYPT_MAX_STR+1, tmpCurDate, 4);
        qDebug("%d",(int)tmpCurDate[0]);
        qDebug("%d",(int)tmpCurDate[1]);
        qDebug("%d",(int)tmpCurDate[2]);
        qDebug("%d",(int)tmpCurDate[3]);
#endif
    }
}
