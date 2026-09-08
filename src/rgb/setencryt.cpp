#include <QApplication>
#include <QtGui>
#include <QProcess>
#include "setencryt.h"
#include <time.h>
#include <setaes.h>

myMacInfo myMachineInfo;
myMacInfo::myMacInfo(QObject *parent) :
    QObject(parent)
{
  QString machineNo = "";
  int  encyStatus = 0; //0-未加密， 1-完全解密， 2-部分解密
  QString encyCode = "";
  //基准时间
  QDateTime encyTime = QDateTime::fromString(QString("19700101"), "yyyyMMdd");

}

void myMacInfo::getEncyInfo(){

}
// 0 完全解密，解密未到期，未加密  1 部分解密到期
int myMacInfo::macEncyCheck(){
    int result = 0;
    QSqlQuery sql_query;

    QString machineNo;
    QString encyStatus;
    QString encyCode;
    QString encyDays;
    QString encyTime;

    if(getEncyFile() == 1) result = 0;

    sql_query.exec("SELECT * FROM sfio limit 1;");
    while(sql_query.next())
    {
        machineNo = sql_query.value(0).toString();
        encyStatus = sql_query.value(1).toString();
        encyCode = sql_query.value(2).toString();
        encyDays = sql_query.value(3).toString();
        encyTime = sql_query.value(4).toDateTime().toString("yyyy-MM-dd");
    }
    if(machineNo.isEmpty() && encyStatus.isEmpty()) return 0;
    qDebug()<<"machineNo:"<<machineNo<<"encyStatus:"<<encyStatus
           <<"encyCode:"<<encyCode<<"encyDays:"
          <<encyDays<<"encyTime"<<encyTime<<flush;

    if(encyStatus == "1") {
        result = 0;
    }else{
        QDateTime time = QDateTime::currentDateTime();
        QString timeStr = time.toString("yyyy-MM-dd");

        QDateTime nowDays = QDateTime::fromString(timeStr, "yyyy-MM-dd");
        QDateTime encyptDays = QDateTime::fromString(encyTime, "yyyy-MM-dd").addDays(encyDays.toInt());
        QDateTime encyptTimes = QDateTime::fromString(encyTime, "yyyy-MM-dd");
//        uint snowDays = nowDays.toTime_t();
//        uint sencyptDays = encyptDays.addDays(encyDays.toInt()).toTime_t();

        qDebug()<<"nowDays:"<<nowDays<<"encyptTimes:"<<encyptTimes<<"sencyptDays:"<<encyptDays<<flush;

        qDebug()<<"yes:"<<nowDays.daysTo(encyptDays)<<flush;
        //现在时间小于加密时间，手动调整时间，机器加密
        if(nowDays.daysTo(encyptTimes)>0){
            result = 1;
        }else{
            if(nowDays.daysTo(encyptDays)>=0){
                    result = 0;
            }else{
                result = 1;
            }
        }

    }
    return result;
}

int myMacInfo::getEncyFile(){
    int result = 0;
    for(int i=0; i<3; i++){
        if (QFileInfo(SYS_DB_INFO).exists()) {
           result = 1;
           break;
        }
    }
    return result;
}



QString myMacInfo::getMacAddress(){
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表

     int nCnt = nets.count();

     QString strMacAddr = "";

     for(int i = 0; i < nCnt; i ++)

     {

         // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址

         if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))

         {

             strMacAddr = nets[i].hardwareAddress();

             break;

         }

     }
     qDebug() << "Mac:" << strMacAddr;
     return strMacAddr;
}

/**************加密程序********/
setEncryt::setEncryt(QWidget *parent) :
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
    connect(this,SIGNAL(setCurrentTimeSig())  ,this,SLOT(setCurrentTimeSlt()));
}

/*!
 * \brief setEncryt::timeOutSlt
 */
void setEncryt::timeOutSlt()
{
    timer->stop();
    emit inputDelayCodeSig();
}
/*!
 * \brief setEncryt::timeOutSlt
 */
void setEncryt::inputDelayCodeSlt()
{
    str = QString("%1").arg(myLan.dc_input);
    myInputMethod inputDlg(str,"");
    inputDlg.setType(KB_DC);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
      str = inputDlg.getText();
//      qDebug()<<str;
      emit explainDelayCodeSig();
    }
    else {
        reject();
    }
}

bool setEncryt::dealWithEncyCode(QStringList strDecryList){
    bool result = true;
    SETAES aes;

    if((strDecryList.size() !=4)){
        myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
//        msgBox.setText("加密码非法");
        msgBox.setText(myLan.illegal_password_encryption);
        msgBox.exec();
        emit inputDelayCodeSig();
        result  = false;
        return result;
    }else{
//        if((strDecryList[1].toInt() !=1 && strDecryList[1].toInt() !=0) ){
//            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
//            msgBox.setText("加密码非法1");
//            msgBox.exec();
//            emit inputDelayCodeSig();
//            result  = false;

//        }

        int intKey = strDecryList[1].toInt();
        int serialNo = aes.numberDecrypt(strDecryList[0].toInt(),intKey);
        int encrydateInt = aes.numberDecrypt(strDecryList[2].toInt(),intKey)-1000;
        int encrydateAsureInt = aes.numberDecrypt(strDecryList[3].toInt(),intKey+1123);

        qDebug()<<"serialNo:"<<serialNo<<endl;
        qDebug()<<"intKey:"<<intKey<<endl;
        qDebug()<<"encrydateInt:"<<encrydateInt<<endl;
        qDebug()<<"encrydateAsureInt:"<<encrydateAsureInt<<endl;

        if(!(serialNo>0 && serialNo<=100000)){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_encryption+"1");
            msgBox.exec();
            emit inputDelayCodeSig();
            result  = false;
            return result;

        }

        if(!(encrydateInt>0 && encrydateInt<=7200)){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_encryption+"2");
            msgBox.exec();
            emit inputDelayCodeSig();
            result  = false;
            return result;

        }
        if(!(intKey>0 && intKey<100000) ){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_encryption+"3");
            msgBox.exec();
            emit inputDelayCodeSig();
            result  = false;
            return result;

        }

        if(encrydateInt !=encrydateAsureInt){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_encryption+"9");
            msgBox.exec();
            emit inputDelayCodeSig();
            result  = false;
            return result;
        }

    }
    return result;

}

/*!
 * \brief setEncryt::输入加密码
 */
void setEncryt::explainDelayCodeSlt()
{
    //获取机器序列号
    //1、判断是否首次加密，如果是首次加密，获取本次加密码序列号为机器序列号
    //   需要把机器序列号和加密码存到sfio和encycodeio中

    QSqlQuery sql_query;
    if(qDataBaseInfo.checkIsTableDataExist(sql_query,"sfio") == 0){
        QSqlQuery sql_query1;

//        string strKey = "encrypted_123456";
        SETAES aes;
//        QString strDecryData = QString::fromStdString(aes.DecryptAES(strKey, str.toStdString()));





        qDebug()<<str<<flush;
        QStringList strDecryList = str.split("_");
        qDebug()<<strDecryList<<flush;

        if(!dealWithEncyCode(strDecryList)){
            emit inputDelayCodeSig();
            return;
        }


        int intKey = strDecryList[1].toInt();
        int serialNo = aes.numberDecrypt(strDecryList[0].toInt(),intKey);
        int encrydateInt = aes.numberDecrypt(strDecryList[2].toInt(),intKey)-1000;
        int encyStatus = 0;
        if(encrydateInt == 7200){
            encyStatus = 1;
        }


//        emit setCurrentTimeSig();
//        myMessageBox msgBox(MSG_QUES,"");
        mySetTime setTime;
        int ret = setTime.exec();
        if (ret == QDialog::Accepted) {
            sql_query.prepare("insert into sfio(machineNo, encyStatus, encyCode,encyDays, encyTime) values (?,?,?,?,?)");
            QString machineNo = QString::number(serialNo);

//            int encyStatus = strDecryList[1].toInt();
            QString encyCode = str;
            QString encyDays =  QString::number(encrydateInt);
            QString encyTime = setTime.getDateTime();

            bool result = qDataBaseInfo.addSfio(sql_query, machineNo, encyStatus, encyCode,encyDays, QDateTime::fromString(encyTime, "yyyyMMddhhmmss"));

            sql_query1.prepare("insert into encycodeio(machineNo, encyCode,encyTime) values (?,?,?)");
            QString encyTime1 = setTime.getDateTime();

            bool result1 = qDataBaseInfo.addEncycodeio(sql_query1, machineNo,  encyCode,QDateTime::fromString(encyTime1, "yyyyMMddhhmmss"));

            if(result&&result1){
//                qDebug()<<result<<flush;
                myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
                msgBox.setText(myLan.encryption_successful);
                msgBox.exec();
                accept();
                return;
            }else{
                myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
//                msgBox.setText("加密过程错误");
                msgBox.setText(myLan.encryption_process_error);
                msgBox.exec();
                emit inputDelayCodeSig();
                return;
            }


        }


    }
    else{
    //1、判断是否首次加密，如果不是首次加密，则需要判断本次加密机器序列号是否同老数据一致
    //   若一致，需要把机器序列号和加密码存到encycodeio中

        QSqlQuery sql_query;
        QSqlQuery sql_query1;

        QString machineNo;
        QString encyCode;

        sql_query.exec("SELECT * FROM sfio limit 1;");
        while(sql_query.next())
        {
            machineNo = sql_query.value(0).toString();
        }
//        string strKey = "encrypted_123456";
        SETAES aes;
//        QString strDecryData = QString::fromStdString(aes.DecryptAES(strKey, str.toStdString()));
//        qDebug()<<strDecryData<<flush;
//        QStringList strDecryList = strDecryData.split("_");
//        qDebug()<<strDecryList<<flush;

//        qDebug()<<str<<flush;
        QStringList strDecryList = str.split("_");
//        qDebug()<<strDecryList<<flush;


        if(!dealWithEncyCode(strDecryList)) return;

        int intKey = strDecryList[1].toInt();
        int serialNo = aes.numberDecrypt(strDecryList[0].toInt(),intKey);
        int encrydateInt = aes.numberDecrypt(strDecryList[2].toInt(),intKey)-1000;
        int encyStatus = 0;
        if(encrydateInt == 7200){
            encyStatus = 1;
        }

        if(machineNo.toInt() !=serialNo){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_encryption+"4");
            msgBox.exec();
            emit inputDelayCodeSig();
        } else{
            sql_query1.exec("SELECT * FROM encycodeio ;");
            while(sql_query1.next())
            {
                encyCode = sql_query1.value(1).toString();
                if(encyCode == str){
                    myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
                    msgBox.setText(myLan.illegal_password_encryption+"5");
                    msgBox.exec();
                    emit inputDelayCodeSig();
                    return;
                }
            }

            mySetTime setTime;
            int ret = setTime.exec();
            if (ret == QDialog::Accepted) {
                QSqlQuery sql_query2;

                sql_query2.prepare("insert into encycodeio(machineNo, encyCode,encyTime) values (?,?,?)");
                QString encyTime1 = setTime.getDateTime();
                bool result2 = qDataBaseInfo.addEncycodeio(sql_query2, QString::number(serialNo),  str, QDateTime::fromString(encyTime1, "yyyyMMddhhmmss"));

                QSqlQuery sql_query3;

                sql_query3.prepare(" update sfio set "
                                  "encyStatus = :encyStatus,"
                                  "encyCode = :encyCode, "
                                  "encyDays = :encyDays, "
                                  "encyTime = :encyTime "
                                  "where machineNo = :machineNo");
                QString machineNo = QString::number(serialNo);
//                int encyStatus = encyStatus;
                QString encyCode = str;
                QString encyDays = QString::number(encrydateInt);
                QString encyTime = setTime.getDateTime();
                bool result3 = qDataBaseInfo.updateSfio(sql_query3,  machineNo, encyStatus, encyCode,encyDays, QDateTime::fromString(encyTime, "yyyyMMddhhmmss"));

                if(result2&&result3){
//                    qDebug()<<result2<<"||"<<result3<<flush;
                    myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
                    msgBox.setText(myLan.encryption_successful);
                    msgBox.exec();
                    accept();
                    return;
                }else{
                    myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
                    //                msgBox.setText("加密过程错误");
                    msgBox.setText(myLan.encryption_process_error);
                    msgBox.exec();
                    emit inputDelayCodeSig();
                    return;
                }

            }
        }

    }




//    int counter = 0;
//    time_t curtime;
//    FILE *fp;
//    int ret = 0;
//    struct tm *stru_curtime;
//    char tmpCurDate[64];

//    const char *delay_char = str.toAscii();
//    memcpy(paramDelayCode.delay_code,delay_char, DCCRYPT_MAX_STR);

//    myMessageBox msgBox(MSG_QUES,"");
//    myMessageBox msgBox1(MSG_INFO,myLan.dc_illegal_used);
//    switch(dc_decrypt(paramDelayCode.delay_code, paramDelayCode.limit, paramDelayCode.product_sn))
//    {
//	      case 0:
//        case -1:
//        case -2:
//        case -9:
//           str = QString("%1: %2\n%3: %4%5%6%7-%8%9-%10%11").arg(myLan.dc_productsn).arg(paramDelayCode.product_sn).arg(myLan.dc_limit).arg(paramDelayCode.limit[0]).arg(paramDelayCode.limit[1]).arg(paramDelayCode.limit[2]).arg(paramDelayCode.limit[3]).arg(paramDelayCode.limit[4]).arg(paramDelayCode.limit[5]).arg(paramDelayCode.limit[6]).arg(paramDelayCode.limit[7]);

//            msgBox.setText(str);
//            ret = msgBox.exec();

//            if (ret == QDialog::Accepted) {
//                /*机器编号不匹配*/
//                if (strlen(paramDelayCode.delay_code_eeprom) == 0) {
//                    bFlagSetNewTime = true;
//                } else if (strcmp(paramDelayCode.product_sn, paramDelayCode.product_sn_cur) != 0) {
//                    msgBox1.setText(myLan.dc_productsn_mismatch);
//                    msgBox1.exec();
//                    emit inputDelayCodeSig();
//                    return;
//                }
//                /*累计计时加密方式*/
//                if ((int)(atoi(paramDelayCode.limit)/100000) == 888) {
//                    /*加密码是否已经使用过*/
//                    if (paramDelayCode.delaycodeMatch(paramDelayCode.delay_code)) {
//                        msgBox1.exec();
//                        emit inputDelayCodeSig();
//                        return;
//                    } else {/*保存加密码*/
//                        if(!paramDelayCode.delaycodeSave(paramDelayCode.delay_code)) {
//                            msgBox1.setText(myLan.dc_save_erro);
//                            msgBox1.exec();
//                            emit inputDelayCodeSig();
//                            return;
//                        }
//                    }
//                    counter = 0;
//                    bFlagSetNewTime = false;
//                }
//                /*彻底解密*/
//                if (strcmp(paramDelayCode.limit, "20990101") == 0) {
//                        memset(paramDelayCode.limit_cur         ,0, DCCRYPT_MAX_STR);
//                        memset(paramDelayCode.product_sn_cur    ,0, DCCRYPT_MAX_STR);
//                    memset(paramDelayCode.delay_code_eeprom,0, sizeof(paramDelayCode.delay_code_eeprom));
//                    paramDelayCode.ena = 0xFF;
//                } else {
//                    strcpy(paramDelayCode.delay_code_eeprom, paramDelayCode.delay_code);
//                    paramDelayCode.ena = 0xAA;
//                }

//                /* get current time */
//                if ((fp = fopen(CNF_FILE_ADDTIME, "w"))) {
//                    fprintf(fp, "%d",counter);
//                    fclose(fp);
//                }

//                time(&curtime);
//                stru_curtime = localtime(&curtime);
//                strftime(tmpCurDate,64,"%Y%m%d",stru_curtime);
//                int nTmpCurDate = atoi(tmpCurDate);
//                tmpCurDate[0] = nTmpCurDate/1000000;
//                tmpCurDate[1] = (nTmpCurDate%1000000)/10000;
//                tmpCurDate[2] = (nTmpCurDate%10000)/100;
//                tmpCurDate[3] = nTmpCurDate%100;

//                if ((fp = fopen(CNF_FILE_CURTIME, "w"))) {
//                    fwrite(&curtime, sizeof(time_t), 1, fp);
//                    fclose(fp);
//                }
//                if (bFlagSetNewTime) {//启动设置时间
//                    emit setCurrentTimeSig();
//                    return;
//                } else {
//                    reject();
//                }
//            } else {
//                reject();
//            }
//            break;
//        case -1:
//        case -2:
//        case -9:
//        case 0:
//            msgBox1.setText(myLan.dc_illegal);
//            msgBox1.exec();
//            emit inputDelayCodeSig();
//            return;
//    }
}

void setEncryt::setCurrentTimeSlt()
{
    mySetTime setTime;
    int ret = setTime.exec();
    if (ret == QDialog::Accepted) {
        accept();

    }
}

/**************解密代码********/

setDccryt::setDccryt(QWidget *parent) :
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
 * \brief setDccryt::timeOutSlt
 */
void setDccryt::timeOutSlt()
{
    timer->stop();
    emit inputDelayCodeSig();
}

void setDccryt::inputDelayCodeSlt()
{
    str = QString("%1").arg(myLan.dc_input);
    myInputMethod inputDlg(str,"");
    inputDlg.setType(KB_DC);

    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted){
      str = inputDlg.getText();
      qDebug()<<str;
      emit explainDelayCodeSig();

    }
    else {
        emit inputDelayCodeSig();
    }
}

bool setDccryt::dealWithEncyCode(QStringList strDecryList){
    bool result = true;
    SETAES aes;

    if((strDecryList.size() !=4)){
        myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
        msgBox.setText(myLan.illegal_password_decryption);
        msgBox.exec();
//        emit inputDelayCodeSig();
        result  = false;
        return result;

    }else{

        int intKey = strDecryList[1].toInt();
        int serialNo = aes.numberDecrypt(strDecryList[0].toInt(),intKey);
        int encrydateInt = aes.numberDecrypt(strDecryList[2].toInt(),intKey)-1000;
        int encrydateAsureInt = aes.numberDecrypt(strDecryList[3].toInt(),intKey+1123);

        qDebug()<<"serialNo:"<<serialNo<<endl;
        qDebug()<<"intKey:"<<intKey<<endl;
        qDebug()<<"encrydateInt:"<<encrydateInt<<endl;
        qDebug()<<"encrydateAsureInt:"<<encrydateAsureInt<<endl;

        if(!(serialNo>0 && serialNo<=100000) ){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_decryption+"1");
            msgBox.exec();
//            emit inputDelayCodeSig();
            result  = false;
            return result;


        }
        if(!(encrydateInt>0 && encrydateInt<=7200)){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_decryption+"2");
            msgBox.exec();
//            emit inputDelayCodeSig();
            result  = false;
            return result;

        }
        if(!(intKey>0 && intKey<100000)  ){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_decryption+"3");
            msgBox.exec();
//            emit inputDelayCodeSig();
            result  = false;
            return result;


        }
        if(encrydateInt !=encrydateAsureInt){
            myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
            msgBox.setText(myLan.illegal_password_decryption+"9");
            msgBox.exec();
//            emit inputDelayCodeSig();
            result  = false;
            return result;
        }
    }
    return result;

}

void setDccryt::explainDelayCodeSlt()
{

    QSqlQuery sql_query;
    QSqlQuery sql_query1;

    QString machineNo;
    QString encyCode;

    sql_query.exec("SELECT * FROM sfio limit 1;");
    while(sql_query.next())
    {
        machineNo = sql_query.value(0).toString();
    }
//    string strKey = "encrypted_123456";
//    SETAES aes;
//    QString strDecryData = QString::fromStdString(aes.DecryptAES(strKey, str.toStdString()));
//    qDebug()<<strDecryData<<flush;
//    QStringList strDecryList = strDecryData.split("_");
//    qDebug()<<strDecryList<<flush;

    QStringList strDecryList = str.split("_");
//    qDebug()<<strDecryList<<flush;

    if(!dealWithEncyCode(strDecryList)) {
        emit inputDelayCodeSig();
        return;
    }
     SETAES aes;

    int intKey = strDecryList[1].toInt();
    int serialNo = aes.numberDecrypt(strDecryList[0].toInt(),intKey);
    int encrydateInt = aes.numberDecrypt(strDecryList[2].toInt(),intKey)-1000;

    int encyStatus = 0;
    if(encrydateInt == 7200){
        encyStatus = 1;
    }

    if(machineNo.toInt() !=serialNo){
        myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
        msgBox.setText(myLan.illegal_password_decryption+"4");
        msgBox.exec();
        emit inputDelayCodeSig();
        return;
    } else{
        sql_query1.exec("SELECT * FROM encycodeio ;");
        while(sql_query1.next())
        {
            encyCode = sql_query1.value(1).toString();
            if(encyCode == str){
                myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
                msgBox.setText(myLan.illegal_password_decryption+"5");
                msgBox.exec();
                emit inputDelayCodeSig();
                return;
            }
        }

        mySetTime setTime;
        int ret = setTime.exec();
        if (ret == QDialog::Accepted) {
            QSqlQuery sql_query2;
            QSqlQuery sql_query;

            sql_query.prepare(" update sfio set "
                              "encyStatus = :encyStatus,"
                              "encyCode = :encyCode, "
                              "encyDays = :encyDays, "
                              "encyTime = :encyTime "
                              "where machineNo = :machineNo");
            QString machineNo = QString::number(serialNo);
//            int encyStatus = strDecryList[1].toInt();
            QString encyCode = str;
            QString encyDays = QString::number(encrydateInt);
            QString encyTime = setTime.getDateTime();

            bool result = qDataBaseInfo.updateSfio(sql_query,  machineNo, encyStatus, encyCode,encyDays, QDateTime::fromString(encyTime, "yyyyMMddhhmmss"));


            sql_query2.prepare("insert into encycodeio(machineNo, encyCode,encyTime) values (?,?,?)");
            QString encyTime1 = setTime.getDateTime();

            bool result2 = qDataBaseInfo.addEncycodeio(sql_query2, QString::number(serialNo),  str, QDateTime::fromString(encyTime1, "yyyyMMddhhmmss"));

            if(result&&result2){
                myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
//                msgBox.setText("解密成功");
                msgBox.setText(myLan.decryption_successful);
                msgBox.exec();
                accept();
                return;
            }else{
                myMessageBox msgBox(MSG_INFO,myLan.dc_illegal);
//                msgBox.setText("解密过程错误");
                msgBox.setText(myLan.decryption_process_error);
                msgBox.exec();
                emit inputDelayCodeSig();
                return;
            }
        }
    }

//    switch(dc_decrypt(paramDelayCode.delay_code, paramDelayCode.limit, paramDelayCode.product_sn))
//    {
//        case 0:
//            qDebug()<<paramDelayCode.delay_code;
//           str = QString("%1: %2\n%3: %4%5%6%7-%8%9-%10%11").arg(myLan.dc_productsn).arg(paramDelayCode.product_sn).arg(myLan.dc_limit).arg(paramDelayCode.limit[0]).arg(paramDelayCode.limit[1]).arg(paramDelayCode.limit[2]).arg(paramDelayCode.limit[3]).arg(paramDelayCode.limit[4]).arg(paramDelayCode.limit[5]).arg(paramDelayCode.limit[6]).arg(paramDelayCode.limit[7]);

//            msgBox.setText(str);
//            ret = msgBox.exec();

//            if (ret == QDialog::Accepted) {
//            /*机器编号不匹配*/
//            if (strcmp(paramDelayCode.product_sn, paramDelayCode.product_sn_cur)) {
//                    msgBox1.setText(myLan.dc_productsn_mismatch);
//                msgBox1.exec();
//                emit inputDelayCodeSig();
//                return;
//            }
//            /*累计计时加密方式*/
//            if ((int)(atoi(paramDelayCode.limit)/100000) == 888) {
//                /*加密码是否已经使用过*/
//                if (paramDelayCode.delaycodeMatch(paramDelayCode.delay_code)) {
//                    msgBox1.exec();
//                    emit inputDelayCodeSig();
//                    return;
//                } else {/*保存加密码*/
//                    if (!paramDelayCode.delaycodeSave(paramDelayCode.delay_code)) {
//                        msgBox1.setText(myLan.dc_save_erro);
//                        msgBox1.exec();
//                        emit inputDelayCodeSig();
//                        return;
//                    }
//                }
//                counter = 0;
//            }
//            /*加密时间为无效时间*/
//            if (atoi(paramDelayCode.curDate) > atoi(paramDelayCode.limit)) {
//                    msgBox1.setText(myLan.dc_illegal);
//                msgBox1.exec();
//                emit inputDelayCodeSig();
//                return;
//            }
//            /*彻底解密*/
//            if (strcmp(paramDelayCode.limit, "20990101") == 0) {
//                    memset(paramDelayCode.limit_cur         ,0, DCCRYPT_MAX_STR);
//                    memset(paramDelayCode.product_sn_cur    ,0, DCCRYPT_MAX_STR);
//                memset(paramDelayCode.delay_code_eeprom, 0, sizeof(paramDelayCode.delay_code_eeprom));
//                paramDelayCode.ena = 0xFF;
//            }
//            else {
//                strcpy(paramDelayCode.delay_code_eeprom, paramDelayCode.delay_code);
//                paramDelayCode.ena = 0xAA;
//            }

//            //! 如果出现系统时间出错，重新设置时间
//            if (paramDelayCode.limitCnt == 5){
//               qDebug("Set New Time");
//                mySetTime setTime;
//                setTime.exec();
//            }

//            /* get current time */
//            if ((fp = fopen(CNF_FILE_ADDTIME, "w"))) {
//                fprintf(fp, "%d",counter);
//                fclose(fp);
//            }

//            time(&curtime);
//            stru_curtime = localtime(&curtime);
//            strftime(tmpCurDate,64,"%Y%m%d",stru_curtime);
//            int nTmpCurDate = atoi(tmpCurDate);
//            tmpCurDate[0] = nTmpCurDate/1000000;
//            tmpCurDate[1] = (nTmpCurDate%1000000)/10000;
//            tmpCurDate[2] = (nTmpCurDate%10000)/100;
//            tmpCurDate[3] = nTmpCurDate%100;

//            if ((fp = fopen(CNF_FILE_CURTIME, "w"))) {
//                fwrite(&curtime, sizeof(time_t), 1, fp);
//                fclose(fp);
//            }
//            accept();
//            }
//            else {
//             emit inputDelayCodeSig();
//             return;
//            }
//            break;
//        case -1:
//        case -2:
//        case -9:
//            msgBox1.setText(myLan.dc_illegal);
//            msgBox1.exec();
//            emit inputDelayCodeSig();
//            return;
//    }
}

