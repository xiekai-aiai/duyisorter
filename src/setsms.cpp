/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setsms.cpp
 * \brief       设置短信息界面源文件
 * \date        2015.01.14
 */
#include "setsms.h"

SetSms::SetSms(QWidget *parent) :
    QWidget(parent)
{
    initLocalParams();
    getSmsConfig();
    createPage();
    connectSigAndSlt();

    /* 发送关回显命令 */
    sendAT(AT_CLOSE_ECHO);
}

/* 创建页面 */
void SetSms::createPage()
{
    QGridLayout *mainLay = new QGridLayout(this);

    /* 控件实例化 */
    m_revNumLbl = new myLabel(myLan.receiver_num);
    m_centerNumLbl = new myLabel(myLan.sms_center_num);
    m_mesLbl = new myLabel(myLan.sms_content);

    m_revNumEdit = new myLineEdit(m_strRevNum);
    m_revNumEdit->setFixedHeight(BTN_HEIGHT); 
    m_centerNumEdit = new myLineEdit(m_strCenterNum);
    m_centerNumEdit->setFixedHeight(BTN_HEIGHT);
    m_mesEdit = new myLineEdit();
    m_mesEdit->setFixedHeight(BTN_HEIGHT);

    m_testBtn = new myPushButton(myLan.test_signal, QIcon());
    m_testBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_obtainAutoBtn = new myPushButton(myLan.obtain_auto, QIcon());
    m_obtainAutoBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_sendBtn = new myPushButton(myLan.sms_send, QIcon());
    m_sendBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    mainLay->addWidget(m_revNumLbl, 0, 0);
    mainLay->addWidget(m_revNumEdit, 0, 1);
    mainLay->addWidget(m_testBtn, 0, 2);
    mainLay->addWidget(m_centerNumLbl, 1, 0);
    mainLay->addWidget(m_centerNumEdit, 1, 1);
    mainLay->addWidget(m_obtainAutoBtn, 1, 2);
    mainLay->addWidget(m_mesLbl, 2, 0);
    mainLay->addWidget(m_mesEdit, 2, 1);
    mainLay->addWidget(m_sendBtn, 2, 2);
}

/*!
 * \brief SetSms::initLocalParams
 *        初始化局部参数
 */
void SetSms::initLocalParams()
{
    m_timer = new QTimer;
    m_nTimes = 0;

    for (int i = 0; i < MAX_ALARM; i++) {
        m_pAlarm[i] = 0;
    }
}

/* 连接信号与槽 */
void SetSms::connectSigAndSlt()
{
    connect(m_revNumEdit, SIGNAL(pressed()), this, SLOT(onRevNumEditPressed()));
    connect(m_centerNumEdit, SIGNAL(pressed()), this, SLOT(onCenterNumEditPressed()));
    connect(m_sendBtn, SIGNAL(pressed()), this, SLOT(onSendBtnPressed()));
    connect(m_mesEdit, SIGNAL(pressed()), this, SLOT(onMesEditPressed()));
    connect(m_testBtn, SIGNAL(pressed()), this, SLOT(onTestBtnPressed()));
    connect(m_obtainAutoBtn, SIGNAL(pressed()), this, SLOT(onObtainAutoBtnPressed()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerOutSlt()));
}

/* 获取设置短信配置信息 */
void SetSms::getSmsConfig()
{
    QFile file(SMS_CONFIG_FILE);

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug("ERR: Sms Config Open Error!");
        return;
    }

    QTextStream out(&file);
    out>>m_strRevNum;
    out>>m_strCenterNum;

    file.close();
}

/* 保存设置短信配置信息 */
void SetSms::saveSmsConfig()
{
    QFile file(SMS_CONFIG_FILE);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug("ERR: Sms Config Open Error!");
        return;
    }

    QTextStream in(&file);
    in<<m_strRevNum;
    in<<"\n";
    in<<m_strCenterNum;

    file.close();
}

/* 刷新页面的显示 */
void SetSms::refreshDisplay()
{
    if (struCnfg.nLang == LANG_CHS) {
        m_centerNumLbl->show();
        m_centerNumEdit->show();
        m_obtainAutoBtn->show();
    } else {
        m_centerNumLbl->hide();
        m_centerNumEdit->hide();
        m_obtainAutoBtn->hide();
    }
}

/* 编辑收件人号码 */
void SetSms::onRevNumEditPressed()
{
    myInputPanel *inputPanel = new myInputPanel(textType, 0, 0, 0);
    inputPanel->setJudgeEn(false);
    inputPanel->setText(m_strRevNum);
    int nRet = inputPanel->exec();

    if (nRet == QDialog::Accepted) {
        m_strRevNum = inputPanel->getText();
        if(m_strRevNum == "-1") {
            m_strRevNum = "";
        }
        m_revNumEdit->setText(m_strRevNum);
        saveSmsConfig();
    }
}

/* 编辑短信中心号码 */
void SetSms::onCenterNumEditPressed()
{
    myInputPanel *inputPanel = new myInputPanel(textType, 0, 0, 0);
    inputPanel->setJudgeEn(false);
    inputPanel->setText(m_strCenterNum);
    int nRet = inputPanel->exec();

    if (nRet == QDialog::Accepted) {
        m_strCenterNum = inputPanel->getText();
        if(m_strCenterNum == "-1") {
            m_strCenterNum = "";
        }
        m_centerNumEdit->setText(m_strCenterNum);
        saveSmsConfig();
    }
}

/* 发送短信息 */
void SetSms::onSendBtnPressed()
{
    if (struCnfg.nLang == LANG_CHS) {
        sendSmsCh();
    } else {
        sendSmsEn();
    }
}

/* 编辑短信息 */
void SetSms::onMesEditPressed()
{
    myInputMethod *input = new myInputMethod(myLan.input, m_strMes);
    int nRet = input->exec();

    if (nRet == QDialog::Accepted) {
        m_strMes = input->getText();
        m_mesEdit->setText(m_strMes);
    }
}

/* 测试信号质量 */
void SetSms::onTestBtnPressed()
{
    QString str;
    sendAT(AT_CLOSE_ECHO);
    myFlow.msleep(100);
    sendAT(AT_TEST_SIGNAL);
    myFlow.msleep(100);
    MySerial.com1Write(CMD_INT_SMS_READ, INT, struGsh.nLevel, 0, 0, 0, 0, 0, 0, 3);
    myFlow.msleep(10);
    str = MySerial.smsComRead();
    qDebug()<<str;

    /* 获取信号强度 */
    if (str.size() == 21) {
        int a = str.at(8).toLatin1()-0x30;
        int b = str.at(9).toLatin1()-0x30;
        int nStrength = a*10+b;

        /* 判断信号强度是否合法 */
        if (nStrength < 0 || nStrength > 99) {
            return;
        }

        if (nStrength == 0) {
            m_sendBtn->setRedColor(RED);
        } else if (nStrength == 1) {
            m_sendBtn->setRedColor(GREEN);
        } else if (nStrength > 1 && nStrength < 31) {
            m_sendBtn->setRedColor(GREEN);
            m_obtainAutoBtn->setRedColor(GREEN);
        } else if (nStrength == 31) {
            m_sendBtn->setRedColor(GREEN);
            m_obtainAutoBtn->setRedColor(GREEN);
            m_testBtn->setRedColor(GREEN);
        }

        /* 若定时器未开启则开启定时器 */
        if(!m_timer->isActive()) {
            m_timer->start(3000);
        }
    }
}

/* 自动获取短信中心号码 */
void SetSms::onObtainAutoBtnPressed()
{
    QString str;
    sendAT(AT_CLOSE_ECHO);
    myFlow.msleep(100);
    sendAT(AT_OBTAIN_CENTER_NUM);
    myFlow.msleep(100);
    MySerial.com1Write(CMD_INT_SMS_READ, INT, struGsh.nLevel, 0, 0, 0, 0, 0, 0, 3);
    myFlow.msleep(10);
    str = MySerial.smsComRead();

    if (str.size() > 0) {
        QString strNum = str.mid(13, 11);
        m_strCenterNum = strNum;
        m_centerNumEdit->setText(m_strCenterNum);
        saveSmsConfig();
    }
}

/* 定时器到时 */
void SetSms::onTimerOutSlt()
{
    m_sendBtn->setRedColor(DEF);
    m_testBtn->setRedColor(DEF);
    m_obtainAutoBtn->setRedColor(DEF);

    /* 若定时器已开启则关闭定时器 */
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

/* 根据设置的类型发送AT命令 */
void SetSms::sendAT(int nType)
{
    QString strCmd;
    int nLen;

    switch(nType) {
    case AT_CLOSE_ECHO:     // 关回显
        strCmd = "ATE0\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_SET_SMS_PDU:    // 设置短信格式为PDU
        strCmd = "AT+CMGF=0\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_SET_SMS_TEXT:   // 设置短信格式为TEXT
        strCmd = "AT+CMGF=1\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_SMS_LEN:        // 发送短信长度指令
        nLen = strlen(m_sendCont.toStdString().c_str())/2;
        strCmd = QString("%1%2\r").arg("AT+CMGS=").arg(nLen-9);
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_SMS_CONTENT:    // 发送短信内容指令
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, (m_sendCont+"\x01a"));
        break;

    case AT_READ_SMS_CAP:   // 短信容量查询指令
        strCmd = "AT+CPMS?\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_SET_NOTICE:     // 设置新短信通知指令
        strCmd = "AT+CNMI=2,1\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_READ_UNREAD:    // 读取未读短信
        strCmd = "AT+CMGL=0\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_OBTAIN_CENTER_NUM:  // 获取短信中心号码
        strCmd = "AT+CSCA?\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    case AT_TEST_SIGNAL:        // 测试信号强度
        strCmd = "AT+CSQ\r";
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
        break;

    default:
        break;
    }
}

/* 发送短信息 */
void SetSms::sendSmsCh(QString str)
{
    /* 若收件人号码或短信内容为空则退出 */
    if (m_strRevNum == "") {
        return;
    }

    /*-------------- 对短信息内容进行PDU编码 --------------*/
    /**
     * 08: 信息长度(包括91)
     * 91: 国际格式号码
     * 68: 中国地区(86)
    ********************/
    m_sendCont = "089168";
    convertCenterNum();

    /**
     * 1100: 固定格式
     * 0d:   目标地址数字个数
     * 91:   国际格式号码
     * 68:   中国地区(86)
    ********************/
    m_sendCont += "11000D9168";
    convertRevNum();

    /**
     * 00: 普通GSM类型,点对点方式
     * 08: UCS2编码
     * 00: 有效期
    ********************/
    m_sendCont += "000800";
    convertMes(str);
    /*--------------------------------------------------*/

    /* 发送短信息 */
    sendAT(AT_SET_SMS_PDU);
    myFlow.msleep(100);
    sendAT(AT_SMS_LEN);
    myFlow.msleep(100);
    sendAT(AT_SMS_CONTENT);
}

/* 发送英文短信息 */
void SetSms::sendSmsEn(QString str)
{
    QString strCmd;

    /* 1.设置短信格式 */
    sendAT(AT_SET_SMS_TEXT);
    myFlow.msleep(100);
    
    /* 2.发送收件人号码 */
    strCmd = QString("AT+CMGS=\"%1\"\r").arg(m_strRevNum);
    MySerial.smsComWrite(CMD_INT_SMS_WRITE, strCmd);
    myFlow.msleep(100);

    /* 3.发送短信内容 */
    if (str == "") {
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, (m_strMes+"\x01a"));
    } else {
        MySerial.smsComWrite(CMD_INT_SMS_WRITE, (str+"\x01a"));
    }
}

/* 读取短信息 */
void SetSms::readSms()
{
}

/* PDU：收件人号码 */
void SetSms::convertRevNum()
{
    /* 去除输入的86 */
    if (m_strRevNum.mid(0, 2) == "86") {
        int nLen = m_strRevNum.length();
        m_strRevNum = m_strRevNum.mid(2, nLen);
    }

    /* 为奇数位后面加F */
    QString strRcvNum = m_strRevNum;
    if (m_strRevNum.length()%2 != 0) {
        strRcvNum += "F";
    }

    /* 奇数位偶数位交换 */
    QString strTmp;
    for (int i = 0; i < strRcvNum.length(); i+=2) {
        strTmp += strRcvNum.mid(i+1, 1)+strRcvNum.mid(i, 1);
    }

    m_sendCont += strTmp;
}

/* PDU：短信中心号码 */
void SetSms::convertCenterNum()
{
    /* 去除输入的86 */
    if (m_strCenterNum.mid(0, 2) == "86") {
        int nLen = m_strCenterNum.length();
        m_strCenterNum = m_strCenterNum.mid(2, nLen);
    }

    /* 为奇数位后面加F */
    QString strCenterNum = m_strCenterNum;
    if (m_strCenterNum.length()%2 != 0) {
        strCenterNum += "F";
    }

    /* 奇数位偶数位交换 */
    QString strTmp;
    for (int i = 0; i < strCenterNum.length(); i+=2) {
        strTmp += strCenterNum.mid(i+1, 1)+strCenterNum.mid(i, 1);
    }

    m_sendCont += strTmp;
}

/* PDU：短信内容 */
void SetSms::convertMes(QString str)
{
    QString strMes, strTmp;
    QString sHex;
    int nLen;

    if (str == "") {
        m_strMes = tr(m_strMes.toStdString().c_str());
        strMes = stringToUnicode(m_strMes);
    } else {
        str = tr(str.toStdString().c_str());
        strMes = stringToUnicode(str);
    }

    nLen = strMes.length()/2;
    sHex.setNum(nLen, 16);

    if(sHex.length() == 1) {
        sHex = "0"+sHex;
    }

    strTmp = QString("%1%2").arg(sHex).arg(strMes);
    m_sendCont += strTmp.toUpper();
}

/* 字符串转Unicode编码 */
QString SetSms::stringToUnicode(QString str)
{
    const QChar *q;
    QChar qTmp;
    QString strTmp;
    QString strRet;
    int num, len;

    q = str.unicode();
    len = str.count();

    for (int i = 0; i < len; i++) {
        qTmp = (QChar)*q++;
        num = qTmp.unicode();

        if (num < 255) {
            /* 英文或数字前加"00" */
            strRet += "00";
        }

        /* 变成十六进制数 */
        strTmp = strTmp.setNum(num, 16);
        strRet += strTmp;
    }

    return strRet;
}

/*!
 * \brief SetSms::getStringFromAlarm
 *        根据报警信息获取相应字符串
 * \param nAlarmId
 * \return
 */
QString SetSms::getStringFromAlarm(int nAlarmId)
{
    QString str = "";
    switch(nAlarmId) {
    case ALARM_ID_PRESSURE:
        if (struCnfg.nLang == LANG_CHS) {
            str = "气压报警\n";
        } else {
            str = "Alarm of Pressure\n";
        }
        break;

    case ALARM_ID_BLOWER:
        if (struCnfg.nLang == LANG_CHS) {
            str = "风机报警\n";
        } else {
            str = "Alarm of Blower\n";
        }
        break;

    case ALARM_ID_LEVEL:
        if (struCnfg.nLang == LANG_CHS) {
            str = "料位报警\n";
        } else {
            str = "Alarm of Feeder Level\n";
        }
        break;

    case ALARM_ID_COMM:
        if (struCnfg.nLang == LANG_CHS) {
            str = "通信报警\n";
        } else {
            str = "Alarm of Communication\n";
        }
        break;

    default:
        break;
    }

    return str;
}

/*!
 * \brief SetSms::checkAlarmIsSame
 *        检查报警状态是否相同
 * \return
 */
bool SetSms::checkAlarmIsSame()
{
    bool bRet = true;

    /** 气压报警 */
    if (struCnfg.nAlarmEnable[ALARM_ID_PRESSURE] == 1) {
        if (struGsh.bAlarmPressure == 1
                && m_pAlarm[ALARM_ID_PRESSURE] != struGsh.bAlarmPressure) {
            m_pAlarm[ALARM_ID_PRESSURE] = struGsh.bAlarmPressure;
            bRet = false;
        }
    }

    /** 风机报警 */
    if (struCnfg.nAlarmEnable[ALARM_ID_BLOWER] == 1) {
        if (struGsh.bAlarmBlower == 1
                && m_pAlarm[ALARM_ID_BLOWER] != struGsh.bAlarmBlower) {
            m_pAlarm[ALARM_ID_BLOWER] = struGsh.bAlarmBlower;
            bRet = false;
        }
    }

    /** 料位报警 */
    if (struGsh.nAlarmLevel != ALARM_LEVEL_NULL) {
        if (m_pAlarm[ALARM_ID_LEVEL] != struGsh.nAlarmLevel) {
            m_pAlarm[ALARM_ID_LEVEL] = struGsh.nAlarmLevel;
            bRet = false;
        }
    }

    /** 通信报警 */
    if (struGsh.nCommStat == 1) {
        if (m_pAlarm[ALARM_ID_COMM] != struGsh.nCommStat) {
            m_pAlarm[ALARM_ID_COMM] = struGsh.nCommStat;
            bRet = false;
        }
    }

    return bRet;
}

/*!
 * \brief SetSms::packAlarmStr
 *        根据报警状态组织字符串
 */
void SetSms::packAlarmStr()
{
    m_strAlarm = "";

    /** 气压报警 */
    if (struCnfg.nAlarmEnable[ALARM_ID_PRESSURE] == 1 && struGsh.bAlarmPressure == 1) {
        m_strAlarm += getStringFromAlarm(ALARM_ID_PRESSURE);
    }

    /** 风机报警 */
    if (struCnfg.nAlarmEnable[ALARM_ID_BLOWER] == 1 && struGsh.bAlarmBlower == 1) {
        m_strAlarm += getStringFromAlarm(ALARM_ID_BLOWER);
    }

    /** 料位报警 */
    if (struGsh.nAlarmLevel != ALARM_LEVEL_NULL) {
        m_strAlarm += getStringFromAlarm(ALARM_ID_LEVEL);
    }

    /** 通信报警 */
    if (struGsh.nCommStat) {
        m_strAlarm += getStringFromAlarm(ALARM_ID_COMM);
    }
}

/*!
 * \brief SetSms::sendAlarmSms
 *        有报警时发送提示短信
 */
void SetSms::sendAlarmSms()
{
    /** 若超过最大发送次数则退出 */
    if (m_nTimes >= MAX_ALARM_TIMES) {
        return;
    }

    /** 若报警状态相同则退出 */
    if (checkAlarmIsSame()) {
        return;
    }

    /** 根据当前报警状态组织字符串 */
    packAlarmStr();

    /** 根据当前语言发送信息 */
    if (struCnfg.nLang == LANG_CHS) {
        sendSmsCh(m_strAlarm);
    } else {
        sendSmsEn(m_strAlarm);
    }

    m_nTimes++;
}

/*!
 * \brief SetSms::updateAlarmStatus
 *        更新报警状态
 */
void SetSms::updateAlarmStatus()
{
    if (struCnfg.nAlarmEnable[ALARM_ID_PRESSURE] == 1) {
        m_pAlarm[ALARM_ID_PRESSURE] = struGsh.bAlarmPressure;
    }

    /** 风机报警 */
    if (struCnfg.nAlarmEnable[ALARM_ID_BLOWER] == 1) {
        m_pAlarm[ALARM_ID_BLOWER] = struGsh.bAlarmBlower;
    }

    /** 通信报警 */
    m_pAlarm[ALARM_ID_COMM] = struGsh.nCommStat;
}
