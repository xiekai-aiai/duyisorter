#ifndef SETENCRYT_H
#define SETENCRYT_H
#include <QtGui>
#include <QDialog>
#include "QtNetwork/qnetworkinterface.h"
#include "globalparams.h"
#include "common/myinputmethod.h"
#include "common/mymessagebox.h"
#include "common/mysettime.h"
#include "src/qdatabase.h"

class myMacInfo : public QObject
{
public:
    explicit myMacInfo(QObject *parent = 0);
    QString machineNo;
    int encyStatus;
    QString encyCode;
    QDateTime  encyTime;

    int macEncyCheck();  //检查机器加密状态
    void getEncyInfo();  //检查机器加密信息
    QString getMacAddress();  //获取机器Mac地址
    int getEncyFile();   // 判断SYS_DB_INFO文件


};
extern myMacInfo myMachineInfo;

/*!
 * \brief 加密类
 */
class setEncryt : public QDialog
{
     Q_OBJECT
public:
    explicit setEncryt(QWidget *parent = 0);
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
private :
    bool dealWithEncyCode(QStringList);
};

/*!
 * \brief 解密类
 */

class setDccryt : public QDialog
{
    Q_OBJECT
public:
    explicit setDccryt(QWidget *parent = 0);
    QTimer *timer;
    QString str;
signals:
    void inputDelayCodeSig();
    void explainDelayCodeSig();

public slots:
    void timeOutSlt();
    void inputDelayCodeSlt();
    void explainDelayCodeSlt();
private :
    bool dealWithEncyCode(QStringList);
};

#endif // SETENCRYT_H
