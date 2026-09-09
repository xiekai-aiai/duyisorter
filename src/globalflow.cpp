/*!  \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        globalflow.cpp
 * \brief       全局流程源文件
 * \date        2015.01.15
 */
#include "globalflow.h"
#include "unilog.h"

struct struCnfEngineer struCnfe, _t_struCnfe;
struct struCnfGlobal struCnfg, _t_struCnfg;
struct struCnfCamera   struCnfc, _t_struCnfc;
struct struCnfProfile struCnfp, _t_struCnfp;
struct struCnfFileStatus	struCnfs;
struct struShare struGsh;

AiCommunicate MyUpd;
GlobalFlow myFlow;
MyGlobalString myString;
StrNetConfig strNetInfo;
MyLanguage myLan;

GlobalFlow::GlobalFlow()
{
    QDateTime qCurrentDateTime = QDateTime::currentDateTime();
    turnOnTime = qCurrentDateTime.currentMSecsSinceEpoch();
}

GlobalFlow::~GlobalFlow()
{
}

qint64 GlobalFlow::getSystemTurnOnTime()
{
    return turnOnTime;
}

int GlobalFlow::getProductLineNo()
{
    int productLine = -1;
    if ((struCnfe.nMachine == MACHINE_CF)
        || (struCnfe.nEnableBeanMachine == 1))
    {
        productLine = 0;    //杂粮机型
    }
    return productLine;
}

bool GlobalFlow::mountUdisk()
{
    QDir myDir("/proc/scsi/usb-storage");
    if (!myDir.exists())
    {
        return false;
    }

    QProcess cmdCat;
    cmdCat.setReadChannelMode(QProcess::MergedChannels);
    cmdCat.start("cat", QStringList() << "/proc/partitions");
    QString strRet = cmdCat.readAll();
    if (cmdCat.waitForFinished(3000))
    {
        strRet = cmdCat.readAll();
    }
    else
    {
        cmdCat.kill();
        return false;
    }
    cmdCat.kill();

    QStringList strRetList = strRet.split(QRegExp("\\W+")).filter("sd");
    QString devName;
    if (!strRetList.isEmpty())
    {
        devName = strRetList.last();
    }
    else
    {
        return false;
    }

    QProcess cmdUmount;
    cmdUmount.setReadChannelMode(QProcess::MergedChannels);
    cmdUmount.start("umount", QStringList() << "/udisk");
    cmdUmount.waitForFinished(3000);
    cmdUmount.kill();

    QDir dir;
    if (!dir.exists("/udisk"))
    {
        if (!dir.mkdir("/udisk"))
        {
            return false;
        }
    }

    QProcess cmdMount;
    cmdMount.setReadChannelMode(QProcess::MergedChannels);
    if (devName == "sda")
    {
        if (QFile("/dev/sda").exists())
        {
            cmdMount.start("mount", QStringList() << "/dev/sda" << "/udisk");
        }
        else if (QFile("/dev/udisk1").exists())
        {
            cmdMount.start("mount", QStringList() << "/dev/udisk1" << "/udisk");
        }
    }
    else if (devName == "sda1")
    {
        if (QFile("/dev/sda1").exists())
        {
            cmdMount.start("mount", QStringList() << "/dev/sda1" << "/udisk");
        }
        else if (QFile("/dev/udisk").exists())
        {
            cmdMount.start("mount", QStringList() << "/dev/udisk" << "/udisk");
        }
    }
    else if (devName != "")
    {
        cmdMount.start("mount", QStringList() << QString("/dev/%1").arg(devName) << "/udisk");
    }
    if (cmdMount.waitForFinished(3000))
    {
        strRet = cmdMount.readAll();
    }
    else
    {
        cmdMount.kill();
        return false;
    }
    cmdMount.kill();

    if (strRet.contains("failed"))
    {
        return false;
    }

    return true;
}

/* 设置秒级的延迟 */
void GlobalFlow::sleep(int secs)
{
    QTime dieTime = QTime::currentTime().addSecs(secs);
    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

/* 设置秒秒级的延迟 */
void GlobalFlow::msleep(int mSecs)
{
    QTime dieTime = QTime::currentTime().addMSecs(mSecs);

    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

/* 获取文件大小 */
long GlobalFlow::getFileSize(char* sFileName)
{
    struct stat buf;
    if (stat(sFileName, &buf) < 0)
    {
        return 0;
    }

    return (long)buf.st_size;
}

/* 获取应用程序系统配置 */
void GlobalFlow::getSetting()
{
    QSettings setting(CNF_MY, QSettings::IniFormat);

    setting.beginGroup("Vendor");
    myString.sVendor = setting.value("MYVENDOR", "Duyi").toString();
    setting.endGroup();

    setting.beginGroup("Machine");
    myString.sMachine = setting.value("MYMACHINE", "1").toString();
    setting.endGroup();

    setting.beginGroup("Product");
    myString.sProduct = setting.value("MYPRODUCT", "1").toString();
    setting.endGroup();

    setting.beginGroup("Version");
    myString.sAppVersion = setting.value("MYAPP_VERSION", "1.00").toString();
    myString.sAppSVN = setting.value("MYAPP_SVN", "1024").toString();
    setting.endGroup();

    // 获取数据中心服务器地址
    getServerSetting();
}

/* 保存应用程序系统配置 */
void GlobalFlow::saveSetting()
{
    QSettings setting(CNF_MY, QSettings::IniFormat);

    setting.beginGroup("Machine");
    setting.setValue("MYMACHINE", struCnfe.nMachine);
    setting.endGroup();

    setting.beginGroup("Product");
    setting.setValue("MYPRODUCT", struCnfe.nProduct);
    setting.endGroup();
}

void GlobalFlow::getServerSetting()
{
    QString server = "www.duyigd.com";
    QString fileName;
    fileName.sprintf("%s%s", APP_PATH, "/userdata/cnf/cnf.bigdata");
    if (QFileInfo(fileName).exists())
    {
        QSettings setting(fileName, QSettings::IniFormat);

        server = setting.value("SERVER", "sorter.chinameyer.com").toString();
    }

    myString.sServer = server;
}

void GlobalFlow::saveServerSetting()
{
    // save to file
    QString fileName;
    fileName.sprintf("%s%s", APP_PATH, "/userdata/cnf/cnf.bigdata");
    QSettings setting(fileName, QSettings::IniFormat);
    setting.setValue("SERVER", myString.sServer);
    setting.sync();
}

int GlobalFlow::getParamsFileStaus()
{
    FILE* fp;
    QFile file;

    struCnfs.value = ALL_PARAMS_FILE_OK;

    if (getFileSize(CNF_PARAMS_FILE_STATUS) > 0)
    {
        if (!(fp = fopen(CNF_PARAMS_FILE_STATUS, "r")))
        {
            qDebug("getParamsFileStatus: can not get getParamsFileStatus parameters, using default");
            return 0;
        }
        fread(&struCnfs, sizeof(struct struCnfFileStatus), 1, fp);
        fclose(fp);

        usleep(100000);
        file.remove(CNF_PARAMS_FILE_STATUS_B);
        file.copy(CNF_PARAMS_FILE_STATUS, CNF_PARAMS_FILE_STATUS_B);
        usleep(100000);
    }
    else if (getFileSize(CNF_PARAMS_FILE_STATUS_B) > 0)
    {
        if (!(fp = fopen(CNF_PARAMS_FILE_STATUS_B, "r")))
        {
            qDebug("getParamsFileStatus_2: can not get getParamsFileStatus parameters, using default");
            return 0;
        }
        fread(&struCnfs, sizeof(struct struCnfFileStatus), 1, fp);
        fclose(fp);

        usleep(100000);
        file.remove(CNF_PARAMS_FILE_STATUS);
        file.copy(CNF_PARAMS_FILE_STATUS_B, CNF_PARAMS_FILE_STATUS);
        usleep(100000);
        qDebug("_t_cnfs.value is %d", struCnfs.value);
    }

    return 0;
}

int GlobalFlow::saveParamsFileStaus(int value)
{
    FILE* fp;
    QFile file;
    int i;
    struct struCnfFileStatus _t_Cnfs;
    bool bSaveIsOk = false;

    struCnfs.value = value;

    for (i = 0; i < 3; i++)
    {
        if (!(fp = fopen(CNF_PARAMS_FILE_STATUS, "w")))
        {
            qDebug("saveParamsFileStatus: can not save FileStatus parameters, using default");
            return false;
        }
        fwrite(&struCnfs, sizeof(struct struCnfFileStatus), 1, fp);
        fclose(fp);
        usleep(100000);

        if (!(fp = fopen(CNF_PARAMS_FILE_STATUS, "r")))
        {
            fprintf(stderr, "globalGetCamera: can not save camera parameters, using default\n");
            return false;
        }
        memset(&_t_Cnfs, 0, sizeof(struct struCnfFileStatus));
        fread(&_t_Cnfs, sizeof(struct struCnfFileStatus), 1, fp);
        fclose(fp);
        if (struCnfs.value == _t_Cnfs.value)
        {
            bSaveIsOk = true;
            file.remove(CNF_PARAMS_FILE_STATUS_B);
            file.copy(CNF_PARAMS_FILE_STATUS, CNF_PARAMS_FILE_STATUS_B);
            usleep(100000);
            break;
        }
    }
    return 0;
}

/* 初始化设别参数 */
void GlobalFlow::getGroupIdentify()
{
    int i, j, m;
    int row, col, per;
    int rowRes, colRes, perRes;

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        switch (struCnfc.nSensorType)
        {
        case SENSOR_K:  // Keda
            row = 16;
            col = 32;
            per = row * col;
            break;
        case SENSOR_T_2566: // T2
            row = 16;
            col = 32;
            per = row * col;
            break;
        case SENSOR_T_2564: // T3
            if (struCnfc.nSensorPixel == SENSOR_PIXELS_2048)
            {
                row = 32;
                col = 64;
            }
            else if (struCnfc.nSensorPixel == SENSOR_PIXELS_1024)
            {
                row = 32;
                col = 32;
            }
            else
            {
                row = 32;
                col = 32;
            }
            per = row * col;
            break;
        case SENSOR_D2: // D2
            row = 32;
            col = 32;
            per = row * col;
            break;
        default:
            row = 16;
            col = 32;
            per = row * col;
        }
        rowRes = row;
        colRes = col;
        perRes = per;
    }

    for (i = 0; i < MAX_LEVEL; i++)
    {
        for (j = 0; j < MAX_GROUP_IDTNTIFY; j++)
        {
            for (m = 0; m < 4; m++)
            {	// 0: -grey A, 1: -grey B, 2: -discolor A, 3: -discolor B
                memcpy(struCnfp.struGroupIdentify[i][j].struGreyColor[m].sName, "material", sizeof("material"));
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nRow = row;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nBurntReservedFlag = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nColumn = col;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nPercent = per;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nColor = 1;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nDiscolor = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nSensMin = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nSensMax = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nMode = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].nLightLimit = 20;

                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nDiscolor1 = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nSensMin1 = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nSensMax1 = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nMode1 = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nColor2 = 1;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nSensMin2 = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nSensMax2 = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nMode2 = 0;
                struCnfp.struGroupIdentify[i][j].struGreyColor[m].struReserved.nPercent = 0;
            }
            memcpy(struCnfp.struGroupIdentify[i][j].struCross.sName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struCross.nRow = row;
            struCnfp.struGroupIdentify[i][j].struCross.nColumn = col;
            struCnfp.struGroupIdentify[i][j].struCross.nPercent = per;
            struCnfp.struGroupIdentify[i][j].struCross.nColor = 1;
            struCnfp.struGroupIdentify[i][j].struCross.nSens = 0;

            memcpy(struCnfp.struGroupIdentify[i][j].struShape.sName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struShape.nAreaMin = 0;
            struCnfp.struGroupIdentify[i][j].struShape.nAreaMax = 0;
            struCnfp.struGroupIdentify[i][j].struShape.nMode = 0;
            memcpy(struCnfp.struGroupIdentify[i][j].struShape.sLengthName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struShape.nLengthMin = 0;
            struCnfp.struGroupIdentify[i][j].struShape.nLengthMax = 0;
            struCnfp.struGroupIdentify[i][j].struShape.nLengthMode = 1;
            struCnfp.struGroupIdentify[i][j].struShape.nLengthLimit = 30;
            struCnfp.struGroupIdentify[i][j].struShape.nAreaLimit = 30;

            /* 选杆算法 */
            memcpy(struCnfp.struGroupIdentify[i][j].struPole.sName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struPole.nArea = 60;
            struCnfp.struGroupIdentify[i][j].struPole.nPercent = 40;
            struCnfp.struGroupIdentify[i][j].struPole.nWidthMin = 0;
            struCnfp.struGroupIdentify[i][j].struPole.nWidthMax = 15;

            memcpy(struCnfp.struGroupIdentify[i][j].struPole.sPoleName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struPole.nPoleArea = 20;
            struCnfp.struGroupIdentify[i][j].struPole.nPolePercent = 20;
            struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMin = 0;
            struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMax = 15;

            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nDiscolor1 = 0;
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode1 = 0;
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin1 = 128;
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax1 = 128;
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nColor2 = 1;
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode2 = 0;
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin2 = 0;
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax2 = 0;

            /* 选片算法 */
            memcpy(struCnfp.struGroupIdentify[i][j].struSlice.sName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struSlice.nArea = 400;

            /* 保留算法 */
            memcpy(struCnfp.struGroupIdentify[i][j].struReserved.sName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_s = 140;
            struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_r = 60;
            struCnfp.struGroupIdentify[i][j].struReserved.nRow = 32;

            /* 选芽算法 */
            memcpy(struCnfp.struGroupIdentify[i][j].struLeaf.sName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struLeaf.nArea = 150;
            struCnfp.struGroupIdentify[i][j].struLeaf.nPercent = 15;
            struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin = 0;
            struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMax = 12;
            struCnfp.struGroupIdentify[i][j].struLeaf.nThreshold = 30;

            for (m = 0; m < 2; m++)
            {
                memcpy(struCnfp.struGroupIdentify[i][j].struScale[m].sName, "material", sizeof("material"));
                struCnfp.struGroupIdentify[i][j].struScale[m].nMode = 0;
                struCnfp.struGroupIdentify[i][j].struScale[m].nValue = 0;
                struCnfp.struGroupIdentify[i][j].struScale[m].nValueLimit = 30;
            }

            for (m = 0; m < 2; m++)
            {
                memcpy(struCnfp.struGroupIdentify[i][j].struBud[m].sName, "material", sizeof("material"));
                struCnfp.struGroupIdentify[i][j].struBud[m].nRow = row;         // 尺寸
                struCnfp.struGroupIdentify[i][j].struBud[m].nColumn = col;
                struCnfp.struGroupIdentify[i][j].struBud[m].nPercent = 1;
                struCnfp.struGroupIdentify[i][j].struBud[m].nPercentLimit = 30; // 参数下限
                struCnfp.struGroupIdentify[i][j].struBud[m].nArea = 0;          // 灵敏度
            }

            for (m = 0; m < 4; m++)
            {
                memcpy(struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].sName, "material", sizeof("material"));
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nRow = row;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nColumn = col;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nPercent = per;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nSensMin = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nSensMax = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nSensReserved = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nDiscolor1 = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nSensMin1 = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nSensMax1 = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nMode1 = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nColor2 = 1;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nSensMin2 = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nSensMax2 = 0;
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].struReserved.nMode2 = 0;
            }

            for (m = 0; m < 4; m++)
            {
                memcpy(struCnfp.struGroupIdentify[i][j].struIntel[m].sName, "material", sizeof("material"));
                struCnfp.struGroupIdentify[i][j].struIntel[m].nRow = row;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nColumn = col;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nPercent = per;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nRowRes = rowRes;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nSensRatio = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nIntelType = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nColumnRes = colRes;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nPercentRes = perRes;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nEnable = per;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nSens = 100;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nWholeEdge = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nCons[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nCons[1] = 1;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo1[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo1[1] = 1;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo2[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo2[1] = 1;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo3[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo3[1] = 1;

                /*  多分类 */
                struCnfp.struGroupIdentify[i][j].struIntel[m].nModeDfl = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nRatioMode = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nConsDfl[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nConsDfl[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoR[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoR[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoG[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoG[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoB[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoB[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRR[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRR[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGG[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGG[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoBB[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoBB[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRG[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRG[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRB[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRB[1] = 255;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGB[0] = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGB[1] = 255;

                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nPercent = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nDiscolor1 = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nSensMin1 = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nSensMax1 = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nMode1 = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nColor2 = 1;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nSensMin2 = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nSensMax2 = 0;
                struCnfp.struGroupIdentify[i][j].struIntel[m].struReserved.nMode2 = 0;
            }

            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteSens = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMin = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMax = 255;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteMode = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedSens = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedRatio = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedSens = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedRatio = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Sens = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Sens = 0;
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save = 0;

            /* 玉米参数 */
            memcpy(struCnfp.struGroupIdentify[i][j].struMaize.sName, "material", sizeof("material"));
            struCnfp.struGroupIdentify[i][j].struMaize.nRow = 7;//T3相机
            struCnfp.struGroupIdentify[i][j].struMaize.nColumn = 14;
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentTotal = 80;
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentGood = 80;
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad1 = 80;
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad2 = 80;
            struCnfp.struGroupIdentify[i][j].struMaize.nColor1 = 1;
            struCnfp.struGroupIdentify[i][j].struMaize.nMode1 = 0;
            struCnfp.struGroupIdentify[i][j].struMaize.nSensMin1 = 0;
            struCnfp.struGroupIdentify[i][j].struMaize.nSensMax1 = 0;
            struCnfp.struGroupIdentify[i][j].struMaize.nColor2 = 1;
            struCnfp.struGroupIdentify[i][j].struMaize.nMode2 = 0;
            struCnfp.struGroupIdentify[i][j].struMaize.nSensProport = 0;
            struCnfp.struGroupIdentify[i][j].struMaize.nSensMin2 = 0;
            struCnfp.struGroupIdentify[i][j].struMaize.nSensMax2 = 0;
            struCnfp.struGroupIdentify[i][j].struMaize.nThreshold = 140;

            /* 大米算法 */
            struCnfp.struGroupIdentify[i][j].struRice.nSens = 150;
            struCnfp.struGroupIdentify[i][j].struRice.nDarkSens = 150;
            struCnfp.struGroupIdentify[i][j].struRice.nBadDiscolor = 150;
            struCnfp.struGroupIdentify[i][j].struRice.nRow = 4;
            struCnfp.struGroupIdentify[i][j].struRice.nCol = 6;
            struCnfp.struGroupIdentify[i][j].struRice.nBadWidth = 2;
            struCnfp.struGroupIdentify[i][j].struRice.nYellowArea = 4;
            struCnfp.struGroupIdentify[i][j].struRice.nReservePercent1 = 0;
            struCnfp.struGroupIdentify[i][j].struRice.nReservePercent2 = 0;

            /* 糖果算法 */
            struCnfp.struGroupIdentify[i][j].struCandy.nRation1 = 255;
            struCnfp.struGroupIdentify[i][j].struCandy.nRation2 = 255;
            struCnfp.struGroupIdentify[i][j].struCandy.nRow = 3;
            struCnfp.struGroupIdentify[i][j].struCandy.nCycles = 0;

            /* 开心果算法 */
            struCnfp.struGroupIdentify[i][j].struPistachio.nAreaMin = 10;
            struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_1 = 80;
            struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_2 = 115;
            struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_3 = 16;

            memcpy(struCnfp.struGroupIdentify[i][j].struAi.modelId, "default", sizeof("default"));

        }
    }
}

/* 初始化剔除组参数 */
void GlobalFlow::getGroupTick()
{
    int i, j;
    int nEjectDuration, nEjectDelay, nEjectDelayBias, nEjectDegauss, nEjectTruncation, nAssistEjectDegauss, nAssistEjectTruncation, nImpured, nImpuredFlag, nEdgeCut[MAX_LEVEL][MAX_GROUP_TICK] = {};

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        nEjectDuration = 20;
        nEjectDelay = 60;

        nEjectDegauss = 1;
        nEjectTruncation = 30;
        nAssistEjectDegauss = 1;
        nAssistEjectTruncation = 30;
        nImpuredFlag = 0;
        nImpured = 0;
        break;
    default:
        nEjectDuration = 25;
        nEjectDelay = 100;
        nEjectDegauss = 0;
        nEjectTruncation = 0;
        nAssistEjectDegauss = 0;
        nAssistEjectTruncation = 0;
        nImpured = 0;
        nImpuredFlag = 0;
        break;
    }

    /* 可见光剔除组参数 */
    for (i = 0; i < MAX_LEVEL; i++)
    {
        for (j = 0; j < MAX_GROUP_TICK; j++)
        {
            struCnfp.struGroupTick[i][j].nEjectDuration = nEjectDuration;
            struCnfp.struGroupTick[i][j].nEjectDelay = nEjectDelay;
            struCnfp.struGroupTick[i][j].nEjectDelayBias = nEjectDelayBias;
            struCnfp.struGroupTick[i][j].nEjectDelayBiasCent = 0;
            struCnfp.struGroupTick[i][j].nEjectDegauss = nEjectDegauss;
            struCnfp.struGroupTick[i][j].nEjectTruncation = nEjectTruncation;
            struCnfp.struGroupTick[i][j].nAssistEjectDegauss = nAssistEjectDegauss;
            struCnfp.struGroupTick[i][j].nAssistEjectTruncation = nAssistEjectTruncation;
            struCnfp.struGroupTick[i][j].nEjectDurationCent = 0;
            struCnfp.struGroupTick[i][j].nEjectDelayCent = 0;
            struCnfp.struGroupTick[i][j].nFrontRearMode = 0;
            struCnfp.struGroupTick[i][j].nImpured = nImpured;
            struCnfp.struGroupTick[i][j].nImpuredFlag = nImpuredFlag;
            struCnfp.struGroupTick[i][j].nCorrupted = 0;
            struCnfp.struGroupTick[i][j].nExpanded = 0;
            struCnfp.struGroupTick[i][j].nShapeImpured = nImpured;
            struCnfp.struGroupTick[i][j].nShapeCorrupted = 0;
            struCnfp.struGroupTick[i][j].nShapeExpanded = 0;
            struCnfp.struGroupTick[i][j].nEjwidth = 4;
            struCnfp.struGroupTick[i][j].nEdgeCut = nEdgeCut[i][j];
            struCnfp.struGroupTick[i][j].nBadNum = 1;
            struCnfp.struGroupTick[i][j].nhuffWidth = 0;
            struCnfp.struGroupTick[i][j].nReverseSort = 0;
            struCnfp.struGroupTick[i][j].nMatArea = 255;
            struCnfp.struGroupTick[i][j].nMatWidth = 255;
            struCnfp.struGroupTick[i][j].nGoodNum = 15;
            struCnfp.struGroupTick[i][j].nEdgeRevise = 0;
            struCnfp.struGroupTick[i][j].nEnableBalance = 0;
            struCnfp.struGroupTick[i][j].nAssistEjectTime = 30;
            struCnfp.struGroupTick[i][j].nAssistDelayTime = 60;
            struCnfp.struGroupTick[i][j].nRemoveJoint = 0;
            struCnfp.struGroupTick[i][j].nShapeLogicMode = 0;

            struCnfp.struGroupTick[i][j].nEjectLenthMax = 8;
            struCnfp.struGroupTick[i][j].nEjectLenthDif = 1;
            struCnfp.struGroupTick[i][j].nEjectProportion1 = 32;
            struCnfp.struGroupTick[i][j].nEjectProportion2 = 22;
            struCnfp.struGroupTick[i][j].nEjectThresHold = 10;

            struCnfp.struGroupTick[i][j].nTickMode = 0;
            struCnfp.struGroupTick[i][j].nRounds = 4;

            struCnfp.struGroupTick[i][j].aiEjectorDelay = 10;
            struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay = 20;
            struCnfp.struGroupTick[i][j].aiEjectorBlowTime = 30;
        }

        /* 喷阀保护 */
        struCnfp.struGroupTick[i][0].nEjectTimesLimit = 250;
        struCnfp.struGroupTick[i][1].nEjectTimesLimit = 300;
        struCnfp.struGroupTick[i][2].nEjectTimesLimit = 300;
    }
}

/* 初始化算法名称 */
void  GlobalFlow::getArithmeticName()
{
    myString.sArithmeticName[ARITH_GREY_A] = myLan.material_defect_grey_a;
    myString.sArithmeticName[ARITH_GREY_B] = myLan.material_defect_grey_b;
    myString.sArithmeticName[ARITH_DISCOLOR_A] = myLan.material_defect_discolor_a;
    myString.sArithmeticName[ARITH_DISCOLOR_B] = myLan.material_defect_discolor_b;
    myString.sArithmeticName[ARITH_CROSS] = myLan.material_cross;
    //    myString.sArithmeticName[ARITH_GREY_A] = "单色1";
    //    myString.sArithmeticName[ARITH_GREY_B] = "单色2";
    //    myString.sArithmeticName[ARITH_DISCOLOR_A] = "双色1";
    //    myString.sArithmeticName[ARITH_DISCOLOR_B] = "双色2";
    //    myString.sArithmeticName[ARITH_CROSS] = "复合";

    myString.sArithmeticName[ARITH_SHAPE] = myLan.material_shape;
    myString.sArithmeticName[ARITH_SHAPE_LENGTH] = myLan.material_shape_length;
    myString.sArithmeticName[ARITH_SHAPE_POLE_A] = myLan.material_shape_pole_a;
    myString.sArithmeticName[ARITH_SHAPE_POLE_B] = myLan.material_shape_pole_b;
    myString.sArithmeticName[ARITH_SHAPE_SLICE] = myLan.material_shape_slice;
    myString.sArithmeticName[ARITH_SHAPE_LEAF] = myLan.material_shape_leaf;
    myString.sArithmeticName[ARITH_SCALE] = myLan.material_scale_params + " 1";
    myString.sArithmeticName[ARITH_SCALE_B] = myLan.material_scale_params + " 2";
    myString.sArithmeticName[ARITH_BUD_1] = myLan.material_bud_1;
    myString.sArithmeticName[ARITH_BUD_2] = myLan.material_bud_2;
    myString.sArithmeticName[ARITH_PEANUT_A] = myLan.material_defect_a;
    myString.sArithmeticName[ARITH_PEANUT_B] = myLan.material_defect_b;
    myString.sArithmeticName[ARITH_PEANUT_C] = myLan.material_defect_c;
    myString.sArithmeticName[ARITH_PEANUT_D] = myLan.material_defect_d;
    myString.sArithmeticName[ARITH_INTEL_A] = myLan.material_intelligent_mode_a;
    myString.sArithmeticName[ARITH_INTEL_B] = myLan.material_intelligent_mode_b;
    myString.sArithmeticName[ARITH_INTEL_C] = myLan.material_intelligent_mode_c;
    myString.sArithmeticName[ARITH_INTEL_D] = myLan.material_intelligent_mode_d;
    myString.sArithmeticName[ARITH_WATERMELON] = myLan.material_watermelon;
    myString.sArithmeticName[ARITH_MAIZE] = myLan.material_maize_param;
    myString.sArithmeticName[ARITH_RESERVED] = myLan.material_reserved;
    myString.sArithmeticName[ARITH_CANDY] = "糖果算法";
    myString.sArithmeticName[ARITH_PISTACHIO] = "AI算法";


    if (struCnfe.nEnableMaizeDoubleView == 1)
    {
        if (struCnfg.nLang == LANG_CHS)
        {
            myString.sArithmeticName[ARITH_GREY_A] = "深霉算法";
            myString.sArithmeticName[ARITH_INTEL_A] = "浅霉算法";
            myString.sArithmeticName[ARITH_MAIZE] = "点霉算法";
        }
    }
}

/* 发送相机参数 */
void GlobalFlow::initCamera()
{
    int i, j, k;
    int nUnitAddr;
    int nDelay;
    int tmpRowFre = 0;
    for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        /* 发送给接口板的喷嘴数量 */
        MySerial.com1Write(CMD_INT_EJECTOR_SET, INT, i, 0, 0, 0, struCnfg.nEjectorsPerChute - 1, 0, 0, 3);
    }
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            /* 发送给相机板的喷嘴数量 */
            MySerial.com1Write(CMD_UNIT_EJECTOR_SET, UNIT, i, nUnitAddr, 0, 0, struCnfg.nEjectorsPerChute - 1, 0, 0, 3);
            MySerial.com1Write(CMD_UNIT_EJECTOR_SET, UNIT, i, nUnitAddr + struGsh.nDevTypeAddrBias, 0, 0, struCnfg.nEjectorsPerChute - 1, 0, 0, 3);
        }

        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            /* 模拟增益*/
            MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, i, nUnitAddr, 1, 0, 0,
                struCnfc.struLevelCamera[i].nAnaGainRed[nUnitAddr] / 256,
                struCnfc.struLevelCamera[i].nAnaGainRed[nUnitAddr] % 256, 3);
            MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, i, nUnitAddr, 2, 0, 0,
                struCnfc.struLevelCamera[i].nAnaGainGreen[nUnitAddr] / 256,
                struCnfc.struLevelCamera[i].nAnaGainGreen[nUnitAddr] % 256, 3);
            if (struCnfc.nSensorType != SENSOR_D2)
            {
                MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, i, nUnitAddr, 4, 0, 0,
                    struCnfc.struLevelCamera[i].nAnaGainBlue[nUnitAddr] / 256,
                    struCnfc.struLevelCamera[i].nAnaGainBlue[nUnitAddr] % 256, 3);
            }

            /* 数字增益 */
            if (struCnfc.nSensorType != SENSOR_D2)
            {//D2无数字增益
                MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, i, nUnitAddr, 1, 0, 0, 0, struCnfc.struLevelCamera[i].nDigiGainUpperRed[nUnitAddr], 3);
                MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, i, nUnitAddr, 1, 0, 0, 0, struCnfc.struLevelCamera[i].nDigiGainLowRed[nUnitAddr], 3);
                MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, i, nUnitAddr, 2, 0, 0, 0, struCnfc.struLevelCamera[i].nDigiGainUpperGreen[nUnitAddr], 3);
                MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, i, nUnitAddr, 2, 0, 0, 0, struCnfc.struLevelCamera[i].nDigiGainLowGreen[nUnitAddr], 3);
                MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, i, nUnitAddr, 4, 0, 0, 0, struCnfc.struLevelCamera[i].nDigiGainUpperBlue[nUnitAddr], 3);
                MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, i, nUnitAddr, 4, 0, 0, 0, struCnfc.struLevelCamera[i].nDigiGainLowBlue[nUnitAddr], 3);
            }

            /* 偏置 */
            MySerial.com1Write(CMD_UNIT_BIASING, UNIT, i, nUnitAddr, 1, 0, 0, 0, struCnfc.struLevelCamera[i].nBiasingRed[nUnitAddr], 3);
            MySerial.com1Write(CMD_UNIT_BIASING, UNIT, i, nUnitAddr, 2, 0, 0, 0, struCnfc.struLevelCamera[i].nBiasingGreen[nUnitAddr], 3);
            if (struCnfc.nSensorType != SENSOR_D2)
            {
                MySerial.com1Write(CMD_UNIT_BIASING, UNIT, i, nUnitAddr, 4, 0, 0, 0, struCnfc.struLevelCamera[i].nBiasingBlue[nUnitAddr], 3);
            }

            /* 行频 */
            switch (struCnfc.nSensorType)
            {
            case SENSOR_K:
                MySerial.com1Write(CMD_UNIT_ROW_FREQUENCY, UNIT, i, nUnitAddr,
                    0,
                    0,
                    0,
                    (768000 / struCnfc.nSensorRowFre) / 256,
                    (768000 / struCnfc.nSensorRowFre) % 256,
                    3);
                break;
            case SENSOR_T_2566:
                MySerial.com1Write(CMD_UNIT_ROW_FREQUENCY, UNIT, i, nUnitAddr,
                    0,
                    0,
                    0,
                    ((int)((140 * struGsh.nSensCoefficient / (0.1 * struCnfc.nSensorRowFre) - 140 * 163.88) + 0.5)) / 256,
                    ((int)((140 * struGsh.nSensCoefficient / (0.1 * struCnfc.nSensorRowFre) - 140 * 163.88) + 0.5)) % 256,
                    3);
                MySerial.com1Write(CMD_UNIT_SENS_T_MODE, UNIT, i, nUnitAddr,
                    0,
                    0,
                    0,
                    0,
                    struCnfp.nSensorMode,
                    3);
                break;
            case SENSOR_T_2564:
                //! T3帧频f与界面设置的速度v计算公式为：f=((28*A)/v-100)*10，其中A=12
                tmpRowFre = (336.0 / (0.1 * struCnfc.nSensorRowFre) - 100) * 10 + 0.5;
                tmpRowFre = (tmpRowFre < 0) ? 0 : tmpRowFre;
                MySerial.com1Write(CMD_UNIT_ROW_FREQUENCY, UNIT, i, nUnitAddr, 0, 0, 0, tmpRowFre / 256, tmpRowFre % 256, 3);
                break;
            case SENSOR_D2:
                MySerial.com1Write(CMD_UNIT_ROW_FREQUENCY, UNIT, i, nUnitAddr, 0, 0, 0, tmpRowFre / 256, tmpRowFre % 256, 3);
                break;
            default:
                break;
            }

            /* 通道划分 */
            MySerial.com1Write(CMD_UNIT_CHANNEL, UNIT, i, nUnitAddr, 0, struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr] / 256,
                struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr] % 256,
                struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr] / 256,
                struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr] % 256, 3);
            /* 喷嘴象元 */
            for (k = 0; k < struCnfg.nEjectorsPerChute; k++)
            {
                MySerial.com1Write(CMD_UNIT_CHANNEL_EJECTOR, UNIT, i, nUnitAddr, k,
                    struCnfc.struLevelCamera[i].nChannelEjectorBegin[nUnitAddr][k] / 256,
                    struCnfc.struLevelCamera[i].nChannelEjectorBegin[nUnitAddr][k] % 256,
                    struCnfc.struLevelCamera[i].nChannelEjectorEnd[nUnitAddr][k] / 256,
                    struCnfc.struLevelCamera[i].nChannelEjectorEnd[nUnitAddr][k] % 256, 3);
            }
        }
    }

    int nAddr;
    int tmpAutoReferChute;
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int m = 0; m < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; m++)
            {
                nAddr = struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[m];
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, struCnfp.nAutoAdVal[i][nAddr][0] / 256, struCnfp.nAutoAdVal[i][nAddr][0] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, struCnfp.nAutoAdVal[i][nAddr][1] / 256, struCnfp.nAutoAdVal[i][nAddr][1] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, struCnfp.nAutoAdVal[i][nAddr][2] / 256, struCnfp.nAutoAdVal[i][nAddr][2] % 256, 3);
                nAddr = nAddr + struGsh.nDevTypeAddrBias;
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, struCnfp.nAutoAdVal[i][nAddr][0] / 256, struCnfp.nAutoAdVal[i][nAddr][0] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, struCnfp.nAutoAdVal[i][nAddr][1] / 256, struCnfp.nAutoAdVal[i][nAddr][1] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, struCnfp.nAutoAdVal[i][nAddr][2] / 256, struCnfp.nAutoAdVal[i][nAddr][2] % 256, 3);
            }
        }
        for (int j = 2; j < 6; j++)
        {
            for (int m = 0; m < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; m++)
            {
                nAddr = struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[m];
                if (nAddr % 2 == 0)
                {
                    tmpAutoReferChute = (struCnfg.nAutoReferChute - 1) * 2;
                }
                else
                {
                    tmpAutoReferChute = (struCnfg.nAutoReferChute - 1) * 2 + 1;
                }
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, struCnfp.nAutoAdVal[i][tmpAutoReferChute][0] / 256, struCnfp.nAutoAdVal[i][tmpAutoReferChute][0] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, struCnfp.nAutoAdVal[i][tmpAutoReferChute][1] / 256, struCnfp.nAutoAdVal[i][tmpAutoReferChute][1] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, struCnfp.nAutoAdVal[i][tmpAutoReferChute][2] / 256, struCnfp.nAutoAdVal[i][tmpAutoReferChute][2] % 256, 3);
                nAddr = nAddr + struGsh.nDevTypeAddrBias;
                tmpAutoReferChute = tmpAutoReferChute + struGsh.nDevTypeAddrBias;
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, struCnfp.nAutoAdVal[i][tmpAutoReferChute][0] / 256, struCnfp.nAutoAdVal[i][tmpAutoReferChute][0] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, struCnfp.nAutoAdVal[i][tmpAutoReferChute][1] / 256, struCnfp.nAutoAdVal[i][tmpAutoReferChute][1] % 256, 3);
                MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, struCnfp.nAutoAdVal[i][tmpAutoReferChute][2] / 256, struCnfp.nAutoAdVal[i][tmpAutoReferChute][2] % 256, 3);
            }
        }
    }

    //! 重置LED指示灯模式
    resetLedModeForLD4();

    //! 先关闭后打开校正使能
    myFlow.updateOnoff(0);
    myFlow.updateOnoff(1);
}

/* 获取传染算法延迟时间 */
void GlobalFlow::getDelayImpure()
{
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            struGsh.nEjectImpureDelay[i][j] = struGsh.delayPerRow * struCnfp.struGroupTick[i][j].nImpured;
            struGsh.nShapeImpureDelay[i][j] = struGsh.delayPerRow * struCnfp.struGroupTick[i][j].nShapeImpured;
        }
    }
}

/* 获取边缘切除延迟时间 */
void GlobalFlow::getDelayEdge()
{
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            struGsh.struTick[i][j].nEdgeCutDelay = struGsh.delayPerRow
                * (struCnfp.struGroupTick[i][j].nEdgeCut
                    + struCnfp.struGroupTick[i][j].nCorrupted * 3
                    + struCnfp.struGroupTick[i][j].nExpanded);
            struGsh.struTick[i][j].nShapeEdgeCutDelay = struGsh.delayPerRow
                * (struCnfp.struGroupTick[i][j].nEdgeCut
                    + struCnfp.struGroupTick[i][j].nShapeCorrupted * 3
                    + struCnfp.struGroupTick[i][j].nShapeExpanded);
        }
    }
    /* 临时测试 - 算法边缘切除 */
}

/* 获取算法边缘切除延时 */
void GlobalFlow::getArithDelayEdge()
{
    /*
    int id = -1;
    bool bIsCut = false;
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            // 剔除组号相同且已经切除边缘，直接跳过
            if (id == getTickGroupId(j) && bIsCut)
                continue;
            bIsCut = false;
            id = getTickGroupId(j);
            // 遍历算法序号
            for (int k = 0; k < ARITHMETIC_TOTAL; k++) {
                // 若算法未使能直接跳过
                if (struCnfe.nArithmeticSeq[k] == 0)
                    continue;
                if (struCnfp.nArithmeticEnable[k] == 0)
                    continue;

                // 若存在算法边缘切除圈数
                if (struCnfp.nArithEdgeCut[j][k] > 0) {
                    struGsh.struTick[i][id].nEdgeCutDelay += struGsh.delayPerRow*3;
                    bIsCut = true;
                    break;
                }
            }
        }
    }
    */
}

/* 计算获取物料算法尺寸延迟时间 */
void GlobalFlow::getDelayMaterialSize()
{
    int i, j, k, m, tmp, nMaxValue = 0;
    int nMaxSize[MAX_LEVEL][MAX_GROUP_IDTNTIFY];
    int nUnitAddr;
    int nArithmeticEnable[MAX_LEVEL][ARITHMETIC_TOTAL];

    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            struGsh.struTick[i][j].nMaxMatSize = 1;
        }
    }
    for (i = 0; i < MAX_LEVEL; i++)
    {
        for (j = 0; j < MAX_GROUP_IDTNTIFY; j++)
        {
            nMaxSize[i][j] = 1;
        }
    }

    memset(nArithmeticEnable, 0, sizeof(nArithmeticEnable));
    for (i = 0; i < MAX_LEVEL; i++)
    {
        for (j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            nArithmeticEnable[i][j] = struCnfp.nArithmeticEnable[j];
        }
    }

    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            for (m = 0; m < 4; m++)
            {       //  灰度A、灰度B、色差A、色差B
                if (nArithmeticEnable[i][m + ARITH_GREY_A] == 1)
                {
                    nMaxValue = 10001;
                    if ((struCnfp.struGroupIdentify[i][j].struGreyColor[m].nMode == 0 && struCnfp.struGroupIdentify[i][j].struGreyColor[m].nSensMin > 0)
                        || (struCnfp.struGroupIdentify[i][j].struGreyColor[m].nMode == 1 && struCnfp.struGroupIdentify[i][j].struGreyColor[m].nSensMax < nMaxValue))
                    {
                        nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struGreyColor[m].nRow)
                            ? struCnfp.struGroupIdentify[i][j].struGreyColor[m].nRow : nMaxSize[i][j];
                    }
                }
            }

            if (nArithmeticEnable[i][ARITH_CROSS] == 1)
            {   // 差分
                if (struCnfp.struGroupIdentify[i][j].struCross.nSens < 1001)
                {
                    nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struCross.nRow)
                        ? struCnfp.struGroupIdentify[i][j].struCross.nRow : nMaxSize[i][j];
                }
            }
            if (nArithmeticEnable[i][ARITH_CANDY])
            {        // 糖果
                nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struCandy.nRow)
                    ? struCnfp.struGroupIdentify[i][j].struCandy.nRow : nMaxSize[i][j];
            }

            for (m = 0; m < 2; m++)
            {
                if (nArithmeticEnable[i][m + ARITH_BUD_1] == 1)
                {   // 选芽1、选芽2
                    if (struCnfp.struGroupIdentify[i][j].struBud[m].nPercent > struCnfp.struGroupIdentify[i][j].struBud[m].nPercentLimit)
                    {
                        nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struBud[m].nRow)
                            ? struCnfp.struGroupIdentify[i][j].struBud[m].nRow : nMaxSize[i][j];
                    }
                }
            }

            for (m = 0; m < 3; m++)
            {
                if (nArithmeticEnable[i][m + ARITH_PEANUT_A] == 1)
                {    // A、B、C类
                    nMaxValue = 1001;
                    if (((struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nMode == 0) && (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nSensMin > 0))
                        || ((struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nMode == 1) && (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nSensMax < nMaxValue)))
                    {
                        nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nRow) ? struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m].nRow
                            : nMaxSize[i][j];
                    }
                }
            }
            if (struCnfp.nIntelMode == 0)
            {  // 普通自学习
                for (m = 0; m < 4; m++)
                {
                    if (nArithmeticEnable[i][m + ARITH_INTEL_A] == 1)
                    {     // 智能模式A、B、C、D
                        if ((struCnfp.struGroupIdentify[i][j].struIntel[m].nCons[0] != 0) || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo1[0] != 0)
                            || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo2[0] != 0)
                            || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuo3[0] != 0))
                        {
                            nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struIntel[m].nRow) ? struCnfp.struGroupIdentify[i][j].struIntel[m].nRow : nMaxSize[i][j];
                        }
                    }
                }
            }
            else
            {  //多分类
                if (myFlow.getProductLineNo() == 0)
                {
                    for (m = 0; m < 2; m++)
                    {
                        if (nArithmeticEnable[i][ARITH_INTEL_A + m] == 1)
                        {     // 智能模式
                            if (struCnfp.struGroupIdentify[i][j].struIntel[m].nIntelType == 0)
                            {// 普通智能模式
                                if (struCnfp.struGroupIdentify[i][j].struIntel[m].nModeDfl != -1)
                                {
                                    if ((struCnfp.struGroupIdentify[i][j].struIntel[m].nConsDfl[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoR[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoG[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoB[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRG[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRB[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGB[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRR[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGG[0] != 0)
                                        || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoBB[0] != 0))
                                    {
                                        nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struIntel[m].nRow) ? struCnfp.struGroupIdentify[i][j].struIntel[m].nRow : nMaxSize[i][j];
                                    }
                                }
                            }
                            else
                            {                                                            // 比例智能模式
                                if ((struCnfp.struGroupIdentify[i][j].struIntel[m].nConsDfl[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoR[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoG[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoB[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRG[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRB[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGB[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoRR[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoGG[0] != 0)
                                    || (struCnfp.struGroupIdentify[i][j].struIntel[m].nQuoBB[0] != 0))
                                {
                                    nMaxSize[i][j] = (nMaxSize[i][j] < 3) ? 3 : nMaxSize[i][j];
                                }
                            }
                        }
                    }
                }
                else
                {
                    for (m = 0; m < 2; m++)
                    {
                        if (nArithmeticEnable[i][ARITH_INTEL_A + m] == 1)
                        {
                            if (struCnfp.struGroupIdentify[i][j].struIntel[m].nIntelType == 0)
                            {// 普通智能模式
                                if (struCnfp.struGroupIdentify[i][j].struIntel[m].nModeDfl != -1)
                                {
                                    if (checkIntelArithParams(i, j, m))
                                    {
                                        nMaxSize[i][j] = (nMaxSize[i][j] < struCnfp.struGroupIdentify[i][j].struIntel[m].nRow) ? struCnfp.struGroupIdentify[i][j].struIntel[m].nRow : nMaxSize[i][j];
                                    }
                                }
                            }
                            else
                            {                                                            // 比例智能模式
                                if (checkIntelArithParams(i, j, m))
                                {
                                    nMaxSize[i][j] = (nMaxSize[i][j] < 3) ? 3 : nMaxSize[i][j];
                                }
                            }
                        }
                    }
                }
            }

            if (((nArithmeticEnable[i][ARITH_SHAPE] == 1)
                || (nArithmeticEnable[i][ARITH_SHAPE_LENGTH]) == 1)
                || (nArithmeticEnable[i][ARITH_SCALE] == 1)
                || (nArithmeticEnable[i][ARITH_SCALE_B] == 1))
            { // 形状算法+大小算法
                nMaxSize[i][j] = (nMaxSize[i][j] < 3) ? 3 : nMaxSize[i][j];
            }

            if ((nArithmeticEnable[i][ARITH_RESERVED] == 1) || nArithmeticEnable[i][ARITH_MAIZE] == 1)
            { // 玉米算法或者保留算法
                nMaxSize[i][j] = (nMaxSize[i][j] < 10) ? 10 : nMaxSize[i][j];
            }
            if (nArithmeticEnable[i][ARITH_SHAPE_POLE_A] == 1 || nArithmeticEnable[i][ARITH_SHAPE_POLE_B] == 1)
            { // 选杆算法
                nMaxSize[i][j] = (nMaxSize[i][j] < 25) ? 25 : nMaxSize[i][j];
            }
            if (((nArithmeticEnable[i][ARITH_BUD_1] == 1 || nArithmeticEnable[i][ARITH_BUD_2]) == 1) && (struCnfc.nSensorType == SENSOR_T_2564))
            { // 选芽算法
                nMaxSize[i][j] = struGsh.maxRowNumber;
            }
        }
    }

    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            switch (struCnfg.struLevelInfo[i].nViewTotal)
            {
            case SINGLE_VIEW:
                struGsh.struTick[i][j].nMaxMatSize = nMaxSize[i][j];
                break;
            case DOUBLE_VIEW:
                struGsh.struTick[i][j].nMaxMatSize = (nMaxSize[i][j * 2] < nMaxSize[i][j * 2 + 1]) ? nMaxSize[i][j * 2 + 1] : nMaxSize[i][j * 2];
                break;
            case QUADRUPLE_VIEW: // LD4 MAIZE
                tmp = nMaxSize[i][j * 4];
                tmp = (tmp > nMaxSize[i][j * 4 + 1]) ? tmp : nMaxSize[i][j * 4 + 1];
                tmp = (tmp > nMaxSize[i][j * 4 + 2]) ? tmp : nMaxSize[i][j * 4 + 2];
                tmp = (tmp > nMaxSize[i][j * 4 + 3]) ? tmp : nMaxSize[i][j * 4 + 3];
                struGsh.struTick[i][j].nMaxMatSize = tmp;
                break;
            default:
                break;
            }
        }
    }


    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            struGsh.struTick[i][j].nMatSizeDelay = (struGsh.struTick[i][j].nMaxMatSize == 1) ? 0 : struGsh.delayPerRow * ((struGsh.struTick[i][j].nMaxMatSize + 1) / 2);
        }
    }

    /* 发送最大行列尺寸值 */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getTickGroupAddr(i, j, k);
                MySerial.com1Write(CMD_UNIT_MATERIAL_SIZE_MAX, UNIT, i, nUnitAddr, 0, 0, 0, 0, struGsh.struTick[i][j].nMaxMatSize - 1, 3);
            }
        }
    }
}


/* 计算获取剔除组发送地址 */
int GlobalFlow::getTickGroupAddr(int nLevelId, int nGroupId, int nUnitIndex)
{
    int nUnitAddr = 0;
    nUnitAddr = struCnfg.struLevelInfo[nLevelId].struTickGroupInfo[nGroupId].nUnitId[nUnitIndex];

    //    if (struCnfg.struLevelInfo[nLevelId].struTickGroupInfo[nGroupId].nUnitSplice[nUnitIndex] == 0) {
    //        nUnitAddr = struCnfg.struLevelInfo[nLevelId].struTickGroupInfo[nGroupId].nUnitId[nUnitIndex];
    //    } else if (struCnfg.struLevelInfo[nLevelId].struTickGroupInfo[nGroupId].nUnitSplice[nUnitIndex] == 1) {
    //        nUnitAddr = 1 << 6 | struCnfg.struLevelInfo[nLevelId].struTickGroupInfo[nGroupId].nUnitId[nUnitIndex];
    //    } else if (struCnfg.struLevelInfo[nLevelId].struTickGroupInfo[nGroupId].nUnitSplice[nUnitIndex] == 2) {
    //        nUnitAddr = 1 << 7 | struCnfg.struLevelInfo[nLevelId].struTickGroupInfo[nGroupId].nUnitId[nUnitIndex];
    //    }
    return nUnitAddr;
}

/* 根据相机号获取该相机所在剔除组ID */
int GlobalFlow::getTickGroupId(int nUnit)
{
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitCount; j++)
        {
            if (nUnit == struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitId[j])
            {
                return i;
            }
        }
    }
    return 0;
}

/* 发送算法边缘切除圈数 */
int GlobalFlow::resetArithEdgeCut()
{
    /*
    int nUnitAddr;
    for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
        if (struCnfe.nArithmeticSeq[i] == 0)
            continue;
        if (struCnfp.nArithmeticEnable[i] == 0)
            continue;

        for (int j = 0; j < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; j++) {
            nUnitAddr = struCnfg.struLevelInfo[ONE_LEVEL].nUnitId[j];

            MySerial.com1Write(CMD_UNIT_ARITH_EDGE_CUT, UNIT, struGsh.nLevel, nUnitAddr, i, 0, 0, 0, struCnfp.nArithEdgeCut[j][i], 1);
        }
    }
    */
}

/* 发送精准剔除使能及参数 */
void GlobalFlow::resetAccurateTickParams()
{
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
            {
                int nUnitAddr = getTickGroupAddr(i, j, k);
                MySerial.com1Write(CMD_INT_TICK_MODE, INT, i, nUnitAddr,
                    0, 0, 0, 0, (struCnfp.struGroupTick[i][j].nTickMode == 1) ? 1 : 0, 3);

                MySerial.com1Write(CMD_INT_MASTER_SLAVE_RELATION, INT, i, nUnitAddr,
                    0, 0, 0, 0, (struCnfp.struGroupTick[i][j].nTickMode == 2) ? 1 : 0, 3);

                MySerial.com1Write(CMD_UNIT_TICK_MODE, UNIT, i, nUnitAddr,
                    (struCnfp.struGroupTick[i][j].nTickMode == 1) ? 1 : 0,
                    struCnfp.struGroupTick[i][j].nEjectLenthDif | (struCnfp.struGroupTick[i][j].nEjectLenthMax << 4),
                    struCnfp.struGroupTick[i][j].nEjectProportion1,
                    struCnfp.struGroupTick[i][j].nEjectProportion2,
                    struCnfp.struGroupTick[i][j].nEjectThresHold,
                    3);

                MySerial.com1Write(CMD_UNIT_REVERSE, UNIT, i, nUnitAddr,
                    (struCnfp.struGroupTick[i][j].nTickMode == 2) ? 1 : 0,
                    0,
                    0,
                    0,
                    struCnfp.struGroupTick[i][j].nRounds,
                    3);
            }
        }
    }
}

/* 发送延迟时间 */
int GlobalFlow::resetEjectTime()
{
    int i, j, k;
    float nDelayTime[MAX_LEVEL][MAX_GROUP_TICK];
    float nDelayTimeBias[MAX_LEVEL][MAX_GROUP_TICK];
    float nMinDelay = 2.55;
    int nUnitAddr = 0;

    getDelayImpure();

    getDelayEdge();

    getArithDelayEdge();

    getDelayMaterialSize();

    switch (struCnfe.nMachine)
    {
    default:
        for (i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
            {
                //! 计算色选与形选延迟较大值(包括传染,切边缘,膨胀,腐蚀)
                float fColorDelay = struGsh.nEjectImpureDelay[i][j] + struGsh.struTick[i][j].nEdgeCutDelay;
                float fShapeDelay = struGsh.nShapeImpureDelay[i][j] + struGsh.struTick[i][j].nShapeEdgeCutDelay;
                float fTmpDelay = (fColorDelay > fShapeDelay) ? fColorDelay : fShapeDelay;

                nDelayTime[i][j] = struCnfp.struGroupTick[i][j].nEjectDelay / 10.0 + struCnfp.struGroupTick[i][j].nEjectDelayCent / 100.0;
                nDelayTimeBias[i][j] = struCnfp.struGroupTick[i][j].nEjectDelayBias / 10.0 + struCnfp.struGroupTick[i][j].nEjectDelayBiasCent / 100.0;

                //! 延时时间减去色选与形选延迟较大值
                nDelayTime[i][j] -= fTmpDelay;

                /* material arithmetic row delay */
                nDelayTime[i][j] = nDelayTime[i][j] - struGsh.struTick[i][j].nMatSizeDelay;

                /* half duration for centre position*/
                nDelayTime[i][j] = nDelayTime[i][j] - (struCnfp.struGroupTick[i][j].nEjectDuration / 10.0 + struCnfp.struGroupTick[i][j].nEjectDurationCent / 100.0) / 2;

                /* delay time must bigger than 0 */
                nDelayTime[i][j] = (nDelayTime[i][j] > 0) ? nDelayTime[i][j] : 0;

#if 0
                QString strPrint = "\n吹气延迟参数:";
                strPrint.sprintf("%s\n[%d][%d]***************************\n", strPrint.toLocal8Bit().constData(), i, j);
                strPrint.sprintf("%s每行延迟:%f\n", strPrint.toLocal8Bit().constData(), struGsh.delayPerRow);
                strPrint.sprintf("%s界面延迟:%f\n", strPrint.toLocal8Bit().constData(), struCnfp.struGroupTick[i][j].nEjectDelay / 10.0 + struCnfp.struGroupTick[i][j].nEjectDelayCent / 100.0);
                strPrint.sprintf("%s传染延迟:%f\n", strPrint.toLocal8Bit().constData(), struGsh.nEjectImpureDelay[i][j]);
                strPrint.sprintf("%s边缘延迟:%f\n", strPrint.toLocal8Bit().constData(), struGsh.struTick[i][j].nEdgeCutDelay);
                strPrint.sprintf("%s物料延迟:%f\n", strPrint.toLocal8Bit().constData(), struGsh.struTick[i][j].nMatSizeDelay);
                strPrint.sprintf("%s半吹气时间:%f\n", strPrint.toLocal8Bit().constData(), struCnfp.struGroupTick[i][j].nEjectDuration / 20.0 + struCnfp.struGroupTick[i][j].nEjectDurationCent / 200.0);
                strPrint.sprintf("%s下发延迟:%f\n", strPrint.toLocal8Bit().constData(), nDelayTime[i][j]);
                strPrint.sprintf("%s***************************[%d][%d]\n", strPrint.toLocal8Bit().constData(), i, j);

                printf(qPrintable(strPrint));
                fflush(0);
#endif
            }
        }
        break;
    }

    /* eject time */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            /* get min delay */
            nMinDelay = (nMinDelay > nDelayTime[i][j]) ? nDelayTime[i][j] : nMinDelay;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getTickGroupAddr(i, j, k);
                MySerial.com1Write(CMD_INT_EJTIME, INT, i, nUnitAddr,
                    ((int)(nDelayTime[i][j] * 100 + 0.5)) % 10,
                    struCnfp.struGroupTick[i][j].nEjectDurationCent,
                    ((int)(nDelayTime[i][j] * 10 + 0.05)) / 256,
                    ((int)(nDelayTime[i][j] * 10 + 0.05)) % 256,
                    struCnfp.struGroupTick[i][j].nEjectDuration, 3);
                // 截断时间、消磁时间下发
                MySerial.com1Write(CMD_INT_EJECT_TRUNCATION, INT, i, nUnitAddr,
                    0, 0, 0,
                    struCnfp.struGroupTick[i][j].nEjectDegauss,
                    struCnfp.struGroupTick[i][j].nEjectTruncation,
                    3);
            }
        }
    }

    /* ejector on&off */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getIdentifyGroupAddr(i, j, k);
                MySerial.com1Write(CMD_UNIT_EJECT_ONOFF, UNIT, i, nUnitAddr, 0, 0, 0, 0, struGsh.bStatEjectFpga, 3);
            }
        }
    }

    //ai时发送喷阀延迟参数
    initEjectorDelayPara();

    return nMinDelay * 100;
}

/* 发送前后视算法关系 */
/* 当出现3.5 ：1.5时，此功能不能用 */
void GlobalFlow::resetFrontRearRelation()
{
    int i, j, k;
    int relation = 0;
    int relationShape = 0;
    int relationAdd = 0;
    int relationAddShape = 0;
    int bit[16] = { 0 };
    int bitShape[16] = { 0 };
    int sIndex = 0;

    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
            {
                sIndex = struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k] / 2;
                if (sIndex >= MAX_UNIT_PER_INT / 2)
                {
                    continue;
                }
                bit[sIndex] = struCnfp.struGroupTick[i][j].nFrontRearMode;
                bitShape[sIndex] = struCnfp.struGroupTick[i][j].nShapeLogicMode;
            }
        }

        relation = bit[0] | (bit[1] << 1) | (bit[2] << 2) | (bit[3] << 3) | (bit[4] << 4) | (bit[5] << 5)
            | (bit[6] << 6) | (bit[7] << 7);
        relationAdd = bit[8] | (bit[9] << 1) | (bit[10] << 2) | (bit[11] << 3);
        relationShape = bitShape[0] | (bitShape[1] << 1) | (bitShape[2] << 2) | (bitShape[3] << 3) | (bitShape[4] << 4) | (bitShape[5] << 5)
            | (bitShape[6] << 6) | (bitShape[7] << 7);
        relationAddShape = bitShape[8] | (bitShape[9] << 1) | (bitShape[10] << 2) | (bitShape[11] << 3);

        MySerial.com1Write(CMD_INT_FRONT_REAR_RELATION, INT, i, 0, 0, 0, relationAdd, 0, relation, 3);

        //! 彩色米机暂不支持形色选与或关系独立设置
        MySerial.com1Write(CMD_INT_FRONT_REAR_RELATION, INT, i, 0, 1, 0, relationAddShape, 0, relationShape, 3);

        if (struCnfg.struLevelInfo[i].nUnitLevelTotal > MAX_UNIT_PER_INT)
        {
            memset(bit, 0, sizeof(bit));
            memset(bitShape, 0, sizeof(bitShape));
            for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
            {
                for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
                {
                    sIndex = struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k] / 2;
                    if (sIndex >= MAX_UNIT_PER_INT / 2)
                    {
                        bit[sIndex - MAX_UNIT_PER_INT / 2] = struCnfp.struGroupTick[i][j].nFrontRearMode;
                        bitShape[sIndex - MAX_UNIT_PER_INT / 2] = struCnfp.struGroupTick[i][j].nShapeLogicMode;
                    }
                }
            }

            relation = bit[0] | (bit[1] << 1) | (bit[2] << 2) | (bit[3] << 3) | (bit[4] << 4) | (bit[5] << 5)
                | (bit[6] << 6) | (bit[7] << 7);
            relationShape = bitShape[0] | (bitShape[1] << 1) | (bitShape[2] << 2) | (bitShape[3] << 3) | (bitShape[4] << 4) | (bitShape[5] << 5)
                | (bitShape[6] << 6) | (bitShape[7] << 7);
            MySerial.com1Write(CMD_INT_FRONT_REAR_RELATION, INT, i + 1, 0, 0, 0, 0, 0, relation, 3);

            //! 彩色米机暂不支持形色选与或关系独立设置
            MySerial.com1Write(CMD_INT_FRONT_REAR_RELATION, INT, i + 1, 0, 1, 0, 0, 0, relationShape, 3);
        }
    }
}

/* 发送正反选模式 */
void GlobalFlow::resetReverseSort()
{
    int nUnitAddr = 0;

    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getTickGroupAddr(i, j, k);
                MySerial.com1Write(CMD_UNIT_REVERSE_SORT, UNIT, i, nUnitAddr, 0, 0, 0, 0, struCnfp.struGroupTick[i][j].nReverseSort, 3);
                //! 彩色米机暂不支持形色选正反选独立设置
                MySerial.com1Write(CMD_UNIT_REVERSE_SORT, UNIT, i, nUnitAddr, 1, 0, 0, 0, struCnfp.struGroupTick[i][j].nShapeSortMode, 3);
            }
        }
    }
}

/* 发送剔除参数 */
void GlobalFlow::resetTick()
{
    int nUnitAddr = 0;

    /* eject width, edge cut, impured, bad_num */
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getTickGroupAddr(i, j, k);
                qDebug() << "nUnitAddr" << nUnitAddr << endl;
                MySerial.com1Write(CMD_INT_EJECT_WIDTH, INT, i, nUnitAddr, 0, 0, 0, 0, struCnfp.struGroupTick[i][j].nEjwidth, 3);
                MySerial.com1Write(CMD_UNIT_EDGE_CUT, UNIT, i, nUnitAddr, 0, 0, struCnfp.struGroupTick[i][j].nEnableBalance,
                    struCnfp.struGroupTick[i][j].nEdgeRevise,
                    struCnfp.struGroupTick[i][j].nEdgeCut, 3);
                //! 形选传染
                MySerial.com1Write(CMD_UNIT_INFECT, UNIT, i, nUnitAddr, 1,
                    struCnfp.struGroupTick[i][j].nShapeCorrupted,
                    struCnfp.struGroupTick[i][j].nShapeExpanded,
                    struCnfp.struGroupTick[i][j].nShapeImpured,
                    struCnfp.struGroupTick[i][j].nImpuredFlag, 3);
                //! 色选传染
                MySerial.com1Write(CMD_UNIT_INFECT, UNIT, i, nUnitAddr, 0,
                    struCnfp.struGroupTick[i][j].nCorrupted,
                    struCnfp.struGroupTick[i][j].nExpanded,
                    struCnfp.struGroupTick[i][j].nImpured,
                    struCnfp.struGroupTick[i][j].nImpuredFlag, 3);
                MySerial.com1Write(CMD_UNIT_BAD_NUM, UNIT, i, nUnitAddr, 0, struCnfp.struGroupTick[i][j].nMatArea, struCnfp.struGroupTick[i][j].nMatWidth,
                    struCnfp.struGroupTick[i][j].nGoodNum, struCnfp.struGroupTick[i][j].nBadNum, 3);

                //! 吹气宽度
                MySerial.com1Write(CMD_UNIT_HUFF_WIDTH, UNIT, i, nUnitAddr, 0, 0, 0, 0, struCnfp.struGroupTick[i][j].nhuffWidth, 3);
            }
        }
    }

    /* arithmetic relation of front view&rear view*/
    resetFrontRearRelation();

    /* arithmetic normal sort or reserved sort */
    resetReverseSort();
}

/* 发送算法使能状态 */
void GlobalFlow::resetArithmeticEnable()
{
    int i, j;
    int nUnitAddr;
    char sParam1[MAX_LEVEL] = {}, sParam2[MAX_LEVEL] = {}, sParam3[MAX_LEVEL] = {};
    int nArithmeticEnable[MAX_LEVEL][ARITHMETIC_TOTAL];

    memset(nArithmeticEnable, 0, sizeof(nArithmeticEnable));

    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfe.nArithmeticTotal; j++)
        {
            nArithmeticEnable[i][j] = struCnfp.nArithmeticEnable[j];
        }

        sParam1[i] = (nArithmeticEnable[i][ARITH_BUD_1] << 7) | (nArithmeticEnable[i][ARITH_SCALE] << 6)
            | (nArithmeticEnable[i][ARITH_SHAPE] << 5) | (nArithmeticEnable[i][ARITH_CROSS] << 4)
            | ((nArithmeticEnable[i][ARITH_DISCOLOR_B] | nArithmeticEnable[i][ARITH_PEANUT_D]) << 3)
            | (nArithmeticEnable[i][ARITH_DISCOLOR_A] << 2) | (nArithmeticEnable[i][ARITH_GREY_B] << 1)
            | nArithmeticEnable[i][ARITH_GREY_A];

        sParam2[i] = (nArithmeticEnable[i][ARITH_INTEL_D] << 7) | (nArithmeticEnable[i][ARITH_INTEL_C] << 6)
            | (nArithmeticEnable[i][ARITH_INTEL_B] << 5) | (nArithmeticEnable[i][ARITH_INTEL_A] << 4)
            | (nArithmeticEnable[i][ARITH_PEANUT_C] << 3) | (nArithmeticEnable[i][ARITH_PEANUT_B] << 2)
            | (nArithmeticEnable[i][ARITH_PEANUT_A] << 1) | nArithmeticEnable[i][ARITH_BUD_2];
        sParam3[i] = (nArithmeticEnable[i][ARITH_CANDY] << 7) | (nArithmeticEnable[i][ARITH_RESERVED] << 6) | (nArithmeticEnable[i][ARITH_SCALE_B] << 5) | (nArithmeticEnable[i][ARITH_SHAPE_LEAF] << 4)
            | (nArithmeticEnable[i][ARITH_SHAPE_SLICE] << 3)
            | ((nArithmeticEnable[i][ARITH_SHAPE_POLE_A] | nArithmeticEnable[i][ARITH_SHAPE_POLE_B]) << 2)
            | (nArithmeticEnable[i][ARITH_MAIZE] << 1) | (nArithmeticEnable[i][ARITH_SHAPE_LENGTH]);
    }

    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        char tmp = sParam2[i];
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            int nGroupID = getIdentifyGroupID(nUnitAddr, 0);
            sParam2[i] = tmp;
            if (nGroupID > -1 && nGroupID < MAX_GROUP_IDTNTIFY)
            {
                if (struCnfp.struGroupIdentify[i][nGroupID].struIntel[0].nModeDfl == -1)
                {  // 智能A禁用
                    sParam2[i] &= 0xef;
                }

                if (struCnfp.struGroupIdentify[i][nGroupID].struIntel[1].nModeDfl == -1)
                {  // 智能B禁用
                    sParam2[i] &= 0xdf;
                }

                if (struCnfp.struGroupIdentify[i][nGroupID].struIntel[2].nModeDfl == -1)
                {  // 智能C禁用
                    sParam2[i] &= 0xbf;
                }

                if (struCnfp.struGroupIdentify[i][nGroupID].struIntel[3].nModeDfl == -1)
                {  // 智能D禁用
                    sParam2[i] &= 0x7f;
                }
            }
            MySerial.com1Write(CMD_UNIT_ARITHMETIC_ENABLE, UNIT, i, nUnitAddr,
                0, 0, sParam3[i], sParam2[i], sParam1[i], 3);

            //            MySerial.com1Write(CMD_UNIT_PISTACHIO, UNIT, i, nUnitAddr, struCnfp.nArithmeticEnable[ARITH_PISTACHIO], struCnfp.struGroupIdentify[i][nGroupID].struPistachio.nGrayThreshold_3, struCnfp.struGroupIdentify[i][nGroupID].struPistachio.nAreaMin,
            //                               struCnfp.struGroupIdentify[i][nGroupID].struPistachio.nGrayThreshold_2,
            //                               struCnfp.struGroupIdentify[i][nGroupID].struPistachio.nGrayThreshold_1,3);
        }
    }

    myFlow.initEjectorModePara();

}

/* 根据相机地址获取该相机所在背景组的所有相机地址*/
void GlobalFlow::getUnitsForBkg(QList<int>& qlUnits, int nUnit)
{
    int chuteNum = 0;

    switch (struCnfe.nMachine)
    {
    default:
        chuteNum = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2;
        break;
    }

    if (nUnit % 2 == 0)
    {     //前视
        for (int i = 0; i < chuteNum; i++)
        {
            qlUnits << i * 2;
        }
    }
    else
    {                //后视
        for (int i = 0; i < chuteNum; i++)
        {
            qlUnits << i * 2 + 1;
        }
    }
}

/* 根据相机地址及分割类型获取识别组ID*/
int GlobalFlow::getIdentifyGroupID(int nUnit, int spliceType)
{
    for (int i = 0; i < MAX_GROUP_IDTNTIFY; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[i].nUnitCount; j++)
        {
            if (struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[i].nUnitId[j] == nUnit
                && struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[i].nUnitSplice[j] == spliceType)
            {
                return i;
            }
        }
    }
    return -1;  //返回不存在的组号，调用者需要检查返回值
}


/* 计算获取识别组地址 */
int GlobalFlow::getIdentifyGroupAddr(int nLevelId, int nGroupId, int nUnitIndex)
{
    int nUnitAddr = 0;

    if (struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitSplice[nUnitIndex] == 0)
    { //通道不按0.5：0.5分隔
        nUnitAddr = struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitId[nUnitIndex];
    }
    else if (struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitSplice[nUnitIndex] == 1)
    { //分割后左半部分
        nUnitAddr = 1 << 6 | struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitId[nUnitIndex];
    }
    else if (struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitSplice[nUnitIndex] == 2)
    { //分割后右半部分
        nUnitAddr = 1 << 7 | struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitId[nUnitIndex];
    }
    return nUnitAddr;
}

/* 根据相机地址获取对应所在层背景组号 */
int GlobalFlow::getBackgroundGroupID(int nUnit)
{
    int nId = 0;
    switch (struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal)
    {
    case SINGLE_VIEW:	  // 单视
    case DOUBLE_VIEW:	  // 双视
        nId = nUnit % 2;
        break;
    case QUADRUPLE_VIEW:  // 四视
        nId = nUnit % 2;
        break;
    default:
        nId = nUnit % 2;
        break;
    }
    return nId;
}

/* 随机生成满足均分分布的特定区间随机数 */
int GlobalFlow::getUniformRandom(double min, double max)
{
    qsrand(QDateTime::currentDateTime().toTime_t());
    return ((double)qrand() / RAND_MAX) * (max - min) + min;
}

/* 随机生成大数据待采集相机模块地址 */
int GlobalFlow::getRandomLevel(int curLevel, int curUnit)
{
    if (curLevel >= struCnfg.nLevelTotal)
        return 0;
    // 先判断当前相机是否处于所在层最后
#if 1
    int count = struCnfg.struLevelInfo[curLevel].nUnitLevelTotal;
    if (curUnit == struCnfg.struLevelInfo[curLevel].nUnitId[count - 1])
        return (curLevel + 1) % struCnfg.nLevelTotal;
    else
        return curLevel;
#else
    return getUniformRandom(0, struCnfg.nLevelTotal - 1);
#endif
}

/* 随机生成大数据待采集相机单元地址 */
int GlobalFlow::getRandomUnit(int curLevel, int curUnit)
{
    int index = 0;
#if 1
    for (int i = 0; i < struCnfg.struLevelInfo[curLevel].nUnitLevelTotal; i++)
    {
        if (curUnit == struCnfg.struLevelInfo[curLevel].nUnitId[i])
        {
            index = (i + 1) % struCnfg.struLevelInfo[curLevel].nUnitLevelTotal;
        }
    }
#else
    index = getUniformRandom(0, struCnfg.struLevelInfo[curLevel].nUnitLevelTotal - 1);
#endif

    return struCnfg.struLevelInfo[curLevel].nUnitId[index];
}

/*!
 * \brief 检查智能算法参数
 * \param nArith
 * \return
 */
bool GlobalFlow::checkIntelArithParams(int nLevel, int nIdeGroup, int nArith)
{
    if ((struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nConsDfl[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoR[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoG[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoB[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoRG[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoRB[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoGB[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoRR[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoGG[0] != 0)
        || (struCnfp.struGroupIdentify[nLevel][nIdeGroup].struIntel[nArith].nQuoBB[0] != 0))
    {
        return true;
    }

    return false;
}

/* 重置背景颜色设置 */
void GlobalFlow::resetBackgroundColor()
{
    int i, j;
    int nUnitAddr = 0;
    int backgroudID;    // 背景组编号

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        for (i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
            {
                nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                backgroudID = getBackgroundGroupID(nUnitAddr);
                switch (struCnfp.nBackgroundColor[i][backgroudID])
                {
                case 0:
                case 7:
                    MySerial.com1Write(CMD_UNIT_BG_COLOR, UNIT, i, nUnitAddr, 0, (struCnfp.nBackgroundColorValue[i][backgroudID] * 3) / 256, struCnfp.nBackgroundColor[i][backgroudID],
                        (struCnfp.nBackgroundColorValue[i][backgroudID] * 3) % 256, 255 * struCnfp.nBackgroundColorPer[i][backgroudID] / 100, 3);
                    break;
                case 1:
                case 2:
                case 4:
                    MySerial.com1Write(CMD_UNIT_BG_COLOR, UNIT, i, nUnitAddr, 0, 0, struCnfp.nBackgroundColor[i][backgroudID],
                        struCnfp.nBackgroundColorValue[i][backgroudID], 255 * struCnfp.nBackgroundColorPer[i][backgroudID] / 100, 3);
                    break;
                }

                MySerial.com1Write(CMD_UNIT_BG_ADJUST, UNIT, struGsh.nLevel, nUnitAddr, struCnfp.nAutoBgEnable[struGsh.nLevel][nUnitAddr], struCnfp.nAutoBgEnable[struGsh.nLevel][nUnitAddr],
                    struCnfp.nAutoBgRange[struGsh.nLevel][nUnitAddr][0], struCnfp.nAutoBgRange[struGsh.nLevel][nUnitAddr][1],
                    struCnfp.nAutoBgRange[struGsh.nLevel][nUnitAddr][2], 3);
            }
        }
        break;
    default:
        break;
    }
}

/* 重置自动背景颜色设置 */
void GlobalFlow::resetAutoBackgroundColor()
{
    int i, j;
    int nUnitAddr = 0;
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];

            MySerial.com1Write(CMD_UNIT_BG_ADJUST, UNIT, i, nUnitAddr,
                0,
                struCnfp.nAutoBackgroundEnanble_RSC, 0, 0,
                struCnfp.nAutoBackgroundThresholdValue_RSC,
                3);
        }
    }
}


/* 根据正反选设置获取实际下发的智能物料组合序号 */
int GlobalFlow::getIntelSeq(int mod)
{
    int nMatIntelSeq = struCnfp.nMatIntelSeq;
    switch (struCnfp.nMatIntelSeq)
    {
    case 0: // 0 - good , 0 - bad
        nMatIntelSeq = 0;
        break;
    case 1: // 1 - good, 1 - bad
        nMatIntelSeq = 1;
        break;
    case 2: // 1 - good, 2 - bad 
        nMatIntelSeq = !!mod ? 4 : 2;
        break;
    case 3: // 1 - good, 3 - bad
        nMatIntelSeq = !!mod ? 6 : 3;
        break;
    case 4: // 2 - good, 1 - bad
        nMatIntelSeq = !!mod ? 2 : 4;
        break;
    case 5: // 2 - good, 2 - bad
        nMatIntelSeq = 5;
        break;
    case 6: // 3 - good, 1 - bad
        nMatIntelSeq = !!mod ? 3 : 6;
        break;
    default: // default
        break;
    }

    return nMatIntelSeq;
}

/* 重置智能物料组合序号 */
void GlobalFlow::resetIntelSeq()
{
    int nUnitAddr;
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            MySerial.com1Write(CMD_UNIT_INTEL_SET, UNIT, i, nUnitAddr, 0, 0, 0, 0, struCnfp.nMatIntelSeq, 3);
        }
    }
}

/* 发送色选识别参数 */
void GlobalFlow::resetSortParams()
{
    int i, j;

    /* 重置物料算法参数 */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            resetMaterial(i, j, 1, 0, 1);
        }
    }

    /* 智能模式好坏物料组号方式 */
    resetIntelSeq();

    /* 重置背景颜色设置 */
    resetBackgroundColor();

    /* 重置算法边缘切除 */
    resetArithEdgeCut();

    /* 重置喷阀时间 */
    resetEjectTime();

    /* 重置剔除设置 */
    resetTick();

    /* 重置算法使能 */
    resetArithmeticEnable();

    /* 产量统计使能 */
    int nUnitAddr;
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL_ALL, UNIT, 0, nUnitAddr, 0, 0, 0, 0, 1, 1);
        }
    }
}

/* 设置触摸屏背光 */
void GlobalFlow::setTsBackLight(int flag)
{
    QFile file("/dev/backlight");
    QTextStream io(&file);

    if (!file.open(QIODevice::WriteOnly))
        return;

    io << flag;
    struGsh.nBacklightStat = flag;
    file.close();
}

/* 点亮触摸屏背光，需要输入密码 */
void GlobalFlow::sendTsBackLightOn()
{
    emit backlightOnSig();
}

/*!
 * \brief 初始化RSC机型灯光配置
 */
void GlobalFlow::initLampParamsRSC()
{
    /** RSC机型灯光配置
     *  物料灯序号: 1 2 4 5
     *  背景灯序号: 3
     *  前视: 全白灯,物料灯光强为120*通道数,背景灯光强为400
     *  后视: 物料灯1和2为蓝灯,光强为20*通道数 物料灯4和5为白灯,物料灯光强为120*通道数  背景灯蓝灯,光强为400 */
    int nChuteNum = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2;
    int nLight = 120 * nChuteNum;
    for (int i = 0; i < MAX_CTRL; i++)
    {
        for (int j = 0; j < MAX_LAMP; j++)
        {
            struCnfp.struGroupCtrl[i].nLamp[j] = 1;
            struCnfp.struGroupCtrl[i].nLampLight[j] = nLight;
        }

        /** 后视物料蓝灯 */
        struCnfp.struGroupCtrl[i].nLampLight[REAR_LAMP_1] = 20 * nChuteNum;
        struCnfp.struGroupCtrl[i].nLampLight[REAR_LAMP_2] = 20 * nChuteNum;

        /** 背景灯 */
        struCnfp.struGroupCtrl[i].nLampLight[FRONT_LAMP_3_BACKGROUD] = 400;
        struCnfp.struGroupCtrl[i].nLampLight[REAR_LAMP_3_BACKGROUD] = 400;
    }
}

/*!
 * \brief 初始化RSC机型行列尺寸
 */
void GlobalFlow::initRowAndColRSC()
{
    /** 行列尺寸: 4*8 10*/
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = FIRST_FRONT; j <= THIRD_REAR; j++)
        {
            //! 灰度杂质A/B 色差杂质A/B
            for (int k = 0; k < 4; k++)
            {
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nRow = 4;
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColumn = 8;
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nPercent = 10;
            }

            //! 智能算法A/B/C/D
            for (int k = 0; k < 4; k++)
            {
                struCnfp.struGroupIdentify[i][j].struIntel[k].nRow = 4;
                struCnfp.struGroupIdentify[i][j].struIntel[k].nColumn = 8;
                struCnfp.struGroupIdentify[i][j].struIntel[k].nPercent = 10;
            }
        }
    }
}

/*!
 * \brief 初始化RSC机型色选参数
 */
void GlobalFlow::initSortParamsRSC()
{
    switch (struCnfg.nProfileMode[struCnfg.nProfile])
    {
    case RSC_DIFF_POSITIVE_SORT:
        initDiffPosSortRSC();
        break;

    case RSC_DIFF_NEGATIVE_SORT:
        initDiffNegSortRSC();
        break;

    case RSC_WHITE_POSITIVE_SORT:
        initWhitePosSortRSC();
        break;

    case RSC_WHITE_NEGATIVE_SORT:
        initWhiteNegSortRSC();
        break;

    case RSC_YELLOW_WHITE_SORT:
        initYellowWhiteSortRSC();
        break;

    case RSC_YELLOW_TRANS_SORT:
        initYellowTransSortRSC();
        break;

    case RSC_GENERAL:
        break;

    default:
        break;
    }
}

/*!
 * \brief 初始化RSC机型异色粒正选参数
 */
void GlobalFlow::initDiffPosSortRSC()
{
    //! 背景条件: 前蓝(150, 60%) 后白(240, 100%)
    struCnfp.nBackgroundColor[ONE_LEVEL][0] = 4;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][0] = 150;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][0] = 60;
    struCnfp.nBackgroundColor[ONE_LEVEL][1] = 7;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][1] = 240;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][1] = 100;

    //! 算法使能
    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;
    struCnfp.nArithmeticEnable[ARITH_GREY_B] = 1;
    struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 1;
    struCnfp.nIntelMode = 1;

    //! 算法参数: 前视灰度A选黄,灰度B选病斑; 后视智能选黄,灰度A选病斑
    QVector <int> vGroups;
    vGroups.clear();
    vGroups << FIRST_FRONT << SECOND_FRONT << THIRD_FRONT;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选病斑");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].sName, "选黄");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMin = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 3;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMin = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 12;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nSens = 0;
        int pVal[10] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
        initAIParamsRSC(vGroups.at(i), pVal);
    }

    vGroups.clear();
    vGroups << FIRST_REAR << SECOND_REAR << THIRD_REAR;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选病斑");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].sName, "选微黄");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMin = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 3;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMin = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMax = 0;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nPercent = 12;
        int pVal[10] = { -10419, -14526, 25000, 0, 0, 0, 0, 0, 0, -103169 };
        initAIParamsRSC(vGroups.at(i), pVal);
    }
}

/*!
 * \brief 初始化RSC机型异色粒反选参数
 */
void GlobalFlow::initDiffNegSortRSC()
{
    //! 背景条件: 前黑(100, 30%) 后黑(100, 30%)
    struCnfp.nBackgroundColor[ONE_LEVEL][0] = 0;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][0] = 100;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][0] = 30;
    struCnfp.nBackgroundColor[ONE_LEVEL][1] = 0;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][1] = 100;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][1] = 30;

    //! 关闭背景灯
    struCnfp.struGroupCtrl[0].nLamp[FRONT_LAMP_3_BACKGROUD] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_3_BACKGROUD] = 0;

    //! 算法使能
    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;

    //! 算法参数: 前后视都蓝选亮
    QVector <int> vGroups;
    vGroups << FIRST_FRONT << FIRST_REAR << SECOND_FRONT
        << SECOND_REAR << THIRD_FRONT << THIRD_REAR;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选透明");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMax = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 12;
    }
}

/*!
 * \brief 初始化RSC机型乳白粒正选参数
 */
void GlobalFlow::initWhitePosSortRSC()
{
    //! 背景条件: 前蓝(150, 60%) 后白(240, 100%)
    struCnfp.nBackgroundColor[ONE_LEVEL][0] = 4;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][0] = 150;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][0] = 60;
    struCnfp.nBackgroundColor[ONE_LEVEL][1] = 7;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][1] = 240;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][1] = 100;

    //! 算法使能
    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;
    struCnfp.nArithmeticEnable[ARITH_GREY_B] = 1;

    //! 关闭背景灯
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_1] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_2] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_3_BACKGROUD] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_4] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_5] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_6] = 0;

    //! 算法参数: 前视灰度A选全白,灰度B选腹白; 后视灰度A选全白,灰度B选腹白
    QVector <int> vGroups;
    vGroups << FIRST_FRONT << SECOND_FRONT << THIRD_FRONT;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选全白");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].sName, "选腹白");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMax = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 12;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColor = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nMode = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMax = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 3;
    }

    vGroups.clear();
    vGroups << FIRST_REAR << SECOND_REAR << THIRD_REAR;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选全白");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].sName, "选腹白");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMin = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 12;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColor = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMin = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 3;
    }
}

/*!
 * \brief 初始化RSC机型乳白粒反选参数
 */
void GlobalFlow::initWhiteNegSortRSC()
{
    //! 背景条件: 前黑(100, 30%) 后黑(100, 30%)
    struCnfp.nBackgroundColor[ONE_LEVEL][0] = 0;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][0] = 100;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][0] = 30;
    struCnfp.nBackgroundColor[ONE_LEVEL][1] = 0;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][1] = 100;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][1] = 30;

    //! 关闭背景灯
    struCnfp.struGroupCtrl[0].nLamp[FRONT_LAMP_3_BACKGROUD] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_3_BACKGROUD] = 0;

    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_1] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_2] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_4] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_5] = 0;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_6] = 0;

    //! 算法使能
    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;

    //! 算法参数: 前视不选; 后视蓝选亮
    QVector <int> vGroups;
    vGroups << FIRST_FRONT << SECOND_FRONT << THIRD_FRONT;
    for (int i = 0; i < vGroups.size(); i++)
    {
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMin = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMax = 0;
    }

    vGroups.clear();
    vGroups << FIRST_REAR << SECOND_REAR << THIRD_REAR;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选透明");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMax = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 12;
    }
}

/*!
 * \brief 初始化RSC机型黄白同选参数
 */
void GlobalFlow::initYellowWhiteSortRSC()
{
    //! 背景条件: 前蓝(150, 60%) 后白(240, 100%)
    struCnfp.nBackgroundColor[ONE_LEVEL][0] = 4;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][0] = 150;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][0] = 60;
    struCnfp.nBackgroundColor[ONE_LEVEL][1] = 7;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][1] = 240;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][1] = 100;

    //! 算法使能
    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;
    struCnfp.nArithmeticEnable[ARITH_GREY_B] = 1;
    struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 1;
    struCnfp.nIntelMode = 1;

    //! 算法参数: 前视灰度A选黄,灰度B选病斑; 后视智能选黄,灰度A选病斑
    QVector <int> vGroups;
    vGroups.clear();
    vGroups << FIRST_FRONT << SECOND_FRONT << THIRD_FRONT;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选病斑");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].sName, "选白");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMin = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 3;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColor = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nMode = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMax = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 12;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nSens = 0;
        int pVal[10] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
        initAIParamsRSC(vGroups.at(i), pVal);

        //! 二次及三次修改默认行列尺寸
        if (vGroups.at(i) != FIRST_FRONT)
        {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 2;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 4;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 2;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 2;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 4;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 4;
        }
    }

    vGroups.clear();
    vGroups << FIRST_REAR << SECOND_REAR << THIRD_REAR;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选病斑");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].sName, "选白");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].sName, "智能选黄");
        }

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMin = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 3;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColor = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMin = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 12;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[0].nPercent = 12;
        int pVal[10] = { -10419, -14526, 25000, 0, 0, 0, 0, 0, 0, -103169 };
        initAIParamsRSC(vGroups.at(i), pVal);
    }
}

/*!
 * \brief 初始化RSC机型黄透同选参数
 */
void GlobalFlow::initYellowTransSortRSC()
{
    //! 背景条件: 前蓝(150, 60%) 后黑(100, 30%)
    struCnfp.nBackgroundColor[ONE_LEVEL][0] = 4;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][0] = 150;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][0] = 60;
    struCnfp.nBackgroundColor[ONE_LEVEL][1] = 0;
    struCnfp.nBackgroundColorValue[ONE_LEVEL][1] = 100;
    struCnfp.nBackgroundColorPer[ONE_LEVEL][1] = 30;

    //! 关闭后视背景灯
    struCnfp.struGroupCtrl[0].nLamp[FRONT_LAMP_3_BACKGROUD] = 0;

    //! 算法使能
    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;
    struCnfp.nArithmeticEnable[ARITH_GREY_B] = 1;

    //! 算法参数: 前视灰度A选黄,灰度B选病斑; 后视灰度A选透明
    QVector <int> vGroups;
    vGroups.clear();
    vGroups << FIRST_FRONT << SECOND_FRONT << THIRD_FRONT;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选黄");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].sName, "选病斑");
        }
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMin = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 12;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMin = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 3;
    }

    vGroups.clear();
    vGroups << FIRST_REAR << SECOND_REAR << THIRD_REAR;
    for (int i = 0; i < vGroups.size(); i++)
    {
        if (struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT)
        {
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].sName, "选透明");
            strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].sName, "选病斑");
        }

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColor = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nMode = 1;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nSensMax = 350;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_A].nPercent = 12;

        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColor = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nMode = 0;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nSensMin = 250;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nRow = 4;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nColumn = 8;
        struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struGreyColor[ARITH_GREY_B].nPercent = 3;
    }
}

/*!
 * \brief 初始化RSC机型智能参数
 * \param nGroup 识别组号
 * \param vVal 智能参数
 */
void GlobalFlow::initAIParamsRSC(int nGroup, int* pVal)
{
    int nSign = pVal[0] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoR[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoR[1] = abs(pVal[0]);

    nSign = pVal[1] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoG[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoG[1] = abs(pVal[1]);

    nSign = pVal[2] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoB[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoB[1] = abs(pVal[2]);

    nSign = pVal[3] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoRG[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoRG[1] = abs(pVal[3]);

    nSign = pVal[4] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoRB[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoRB[1] = abs(pVal[4]);

    nSign = pVal[5] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoGB[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoGB[1] = abs(pVal[5]);

    nSign = pVal[6] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoRR[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoRR[1] = abs(pVal[6]);

    nSign = pVal[7] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoGG[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoGG[1] = abs(pVal[7]);

    nSign = pVal[8] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoBB[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nQuoBB[1] = abs(pVal[8]);

    nSign = pVal[9] > 0 ? 0 : 1;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nConsDfl[0] = nSign;
    struCnfp.struGroupIdentify[ONE_LEVEL][nGroup].struIntel[0].nConsDfl[1] = abs(pVal[9]);
}

/* 发送振动量 */
void GlobalFlow::resetFeeder(int flag)
{
    int i, sModuleAddr = 0;
    int nFeederTotal;
    char arg1 = 0;
    char arg2 = 0;
    char arg3 = 0;
    char arg4 = 0;
    char arg5 = 0;

    /* 标志为1时仅发送供料器使能 */
    if (flag == 1)
    {
        char feedOnoff = (struGsh.bStatFeed == 1) ? 0xAA : 0x55;
        MySerial.com2Write(CMD_CTRL_FEEDER, CTRL_GENERAL, 0, 0, 0, 0, 0, feedOnoff, 3);
        MySerial.com2Write(CMD_CTRL_FEEDER, CTRL_GENERAL, 1, 0, 0, 0, 0, feedOnoff, 3);
        return;
    }

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        nFeederTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2 + 1;
        if (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2 == 1)
        {
            nFeederTotal = 3;
        }
        break;
    default:
        break;
    }

    //! CG 机型供料控制（带使能及供料偏置）
    {
        int nOutput = 0;
        for (i = 1; i < nFeederTotal; i++)
        {
            if (struCnfg.nParamSetAlone == 1)
            {      //! 独立设置
                if (struCnfp.struGroupCtrl[sModuleAddr].nFeederEnable[i - 1])
                {
                    if (struCnfp.struGroupCtrl[sModuleAddr].nFeeder[i - 1] < 0)
                    {
                        nOutput = 0;
                    }
                    else if (struCnfp.struGroupCtrl[sModuleAddr].nFeeder[i - 1] > 1000)
                    {
                        nOutput = 1000;
                    }
                    else
                    {
                        nOutput = struCnfp.struGroupCtrl[sModuleAddr].nFeeder[i - 1];
                    }
                }
                else
                {
                    nOutput = 0;
                }
            }
            else
            {                                //! 分次设置
                if (struCnfp.struGroupCtrl[sModuleAddr].nFeederEnable[i - 1])
                {
                    nOutput = struCnfp.struGroupCtrl[sModuleAddr].nFeeder[i - 1] + struCnfp.nFeederBias[i - 1];
                    if (nOutput < 0)
                    {
                        nOutput = 0;
                    }
                    if (nOutput > 1000)
                    {
                        nOutput = 1000;
                    }
                }
                else
                {
                    nOutput = 0;
                }
            }
            arg2 = i;
            arg3 = nOutput / 256;
            arg4 = nOutput % 256;
            MySerial.com2Write(CMD_CTRL_FEEDER, CTRL_GENERAL, 0, arg1, arg2, arg3, arg4, arg5, 3);
        }
        return;
    }
}

/* 重置灯光设置 */
void GlobalFlow::resetLamp()
{
    char arg1 = 0, arg2 = 0, arg3 = 0, arg4 = 0, arg5 = 0;
    int sModuleAddr = 0;

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        //			int tmp = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
        //            if (tmp <= 2) {//CF1,CI1
        //				arg4 = (!struCnfp.struGroupCtrl[sModuleAddr].nLamp[9] << 5)
        //						|(!struCnfp.struGroupCtrl[sModuleAddr].nLamp[8] << 4)
        //						|(struCnfp.struGroupCtrl[sModuleAddr].nLamp[7] << 3)
        //						|(struCnfp.struGroupCtrl[sModuleAddr].nLamp[6] << 2)
        //						|(1 << 1)
        //						|1;
        //				arg5 = (struCnfp.struGroupCtrl[sModuleAddr].nLamp[5] << 7)
        //						|(struCnfp.struGroupCtrl[sModuleAddr].nLamp[4] << 6)
        //						|(struCnfp.struGroupCtrl[sModuleAddr].nLamp[3] << 5)
        //						|(1 << 4)
        //						|(1 << 3)
        //						|(struCnfp.struGroupCtrl[sModuleAddr].nLamp[2] << 2)
        //						|(struCnfp.struGroupCtrl[sModuleAddr].nLamp[1] << 1)
        //						|(struCnfp.struGroupCtrl[sModuleAddr].nLamp[0]);
        //			} else {
        arg4 = (!struCnfp.struGroupCtrl[sModuleAddr].nLamp[9] << 3)
            | (!struCnfp.struGroupCtrl[sModuleAddr].nLamp[8] << 2)
            | (struCnfp.struGroupCtrl[sModuleAddr].nLamp[7] << 1)
            | (struCnfp.struGroupCtrl[sModuleAddr].nLamp[6]);

        arg5 = (struCnfp.struGroupCtrl[sModuleAddr].nLamp[5] << 6)
            | (struCnfp.struGroupCtrl[sModuleAddr].nLamp[4] << 5)
            | (struCnfp.struGroupCtrl[sModuleAddr].nLamp[3] << 4)
            | (1 << 3)
            | (struCnfp.struGroupCtrl[sModuleAddr].nLamp[2] << 2)
            | (struCnfp.struGroupCtrl[sModuleAddr].nLamp[1] << 1)
            | (struCnfp.struGroupCtrl[sModuleAddr].nLamp[0]);
        //			}
        MySerial.com2Write(CMD_CTRL_LAMP, CTRL_GENERAL, 0, arg1, arg2, arg3, arg4, arg5, 3);

        /* 恒流源亮度调节 */
        for (int i = 0; i < MAX_LAMP; i++)
        {
            MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, i / 6, 0, 0, i % 6,    //每个智能恒流源底板最多控制6个灯
                struCnfp.struGroupCtrl[0].nLampLight[i] / 256,
                struCnfp.struGroupCtrl[0].nLampLight[i] % 256, 3);
        }
        break;
    default:
        break;
    }
}

/* 发送控制板参数 */
void GlobalFlow::resetControl()
{
    char arg1, arg2, arg3, arg4, arg5;
    int sModuleAddr = 0;

    arg1 = 0;
    arg2 = 0;
    arg3 = 0;
    arg4 = 0;
    arg5 = 0;

    resetFeeder(0);
    resetLamp();

    MySerial.com2Write(CMD_CTRL_FEED_VOLTAGE, CTRL_GENERAL, 0, arg1, arg2, arg3, arg4,
        struCnfg.nFeederVoltage, 3);
    MySerial.com2Write(CMD_CTRL_FEED_VOLTAGE, CTRL_GENERAL, 1, arg1, arg2, arg3, arg4,
        struCnfg.nFeederVoltage, 3);
    MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0, arg1, arg2, arg3,
        struCnfp.struGroupCtrl[sModuleAddr].nWipeDuration, 0, 3);
    MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 1, arg1, arg2, arg3,
        struCnfp.struGroupCtrl[sModuleAddr].nWipeDuration, 0, 3);

    if (struGsh.bIsMC)
    {
        myMonoFlow.sendFeederRemoteCtrlParams();
    }
}

/* 发送FPGA启动模式 */
void GlobalFlow::resetFPGAMode()
{
    int i, j;
    int nUnitAddr = 0;

    /* camera color board */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            MySerial.com1Write(COM_UNIT_UPDATE_USER, UNIT, i, nUnitAddr, 0, 0, 0, CAMERA_COLOR, struCnfp.nColorBoardMode, 3);
        }
    }

    /* interface board */
    for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        MySerial.com1Write(COM_INT_UPDATE_USER, INT, i, 0, 0, 0, 0, 0, struCnfp.nInterfaceBoardMode, 3);
    }
}

/*!
 * \brief GlobalFlow::checkAndResetFPGAMode
 * \param nIntMode
 * \param nColorMode
 * \param nInfMode
 */
void GlobalFlow::checkAndResetFPGAMode(int nIntMode, int nColorMode)
{
    if (nIntMode == struCnfp.nInterfaceBoardMode
        && nColorMode == struCnfp.nColorBoardMode)
    {
        return;
    }

    resetFPGAMode();
    myFlow.sleep(10);
    initCommunication();
}

/*!
 * \brief GlobalFlow::setBaud
 *        设置串口1波特率
 * \return
 */
void GlobalFlow::setCom1Baud()
{
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        struGsh.nIntBaud[i] = 0;
        MySerial.setComBaud(1, MY_UARTBAUD_115200);
        MySerial.setComBaud(3, MY_UARTBAUD_115200);
        if (!checkIntBoardCom(i))
        {     // 以115200查询
            MySerial.setComBaud(1, MY_UARTBAUD_921600);
            MySerial.setComBaud(3, MY_UARTBAUD_921600);
            if (!checkIntBoardCom(i))
            { // 以921600查询
                MySerial.setComBaud(1, MY_UARTBAUD_115200);
                MySerial.setComBaud(3, MY_UARTBAUD_115200);
            }
            else
            {
                struGsh.nIntBaud[i] = MY_UARTBAUD_921600;
            }
        }
        else
        {
            struGsh.nIntBaud[i] = MY_UARTBAUD_115200;
        }
    }
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        if (struGsh.nIntBaud[i] != 0)
        {
            MySerial.setComBaud(1, struGsh.nIntBaud[i]);
            MySerial.setComBaud(3, struGsh.nIntBaud[i]);
            break;
        }
    }
}

/*!
 * \brief GlobalFlow::setBaud
 *        设置串口2波特率
 * \return
 */
void GlobalFlow::setCom2Baud()
{
    for (int i = 0; i < struCnfg.nCtrlBoardTotal; i++)
    {
        struGsh.nCtrlBaud[i] = 0;
        MySerial.setComBaud(2, MY_UARTBAUD_115200);
        if (!checkCtrlBoardCom(i))
        {     // 以115200查询
            MySerial.setComBaud(2, MY_UARTBAUD_921600);
            if (!checkCtrlBoardCom(i))
            { // 以921600查询
                MySerial.setComBaud(2, MY_UARTBAUD_115200);
            }
            else
            {
                struGsh.nCtrlBaud[i] = MY_UARTBAUD_921600;
            }
        }
        else
        {
            struGsh.nCtrlBaud[i] = MY_UARTBAUD_115200;
        }
    }
    for (int i = 0; i < struCnfg.nCtrlBoardTotal; i++)
    {
        if (struGsh.nIntBaud[i] != 0)
        {
            MySerial.setComBaud(2, struGsh.nCtrlBaud[i]);
            break;
        }
    }
}

/*!
 * \brief GlobalFlow::checkIntBoardCom
 *        检查接口板通信
 * \return
 */
bool GlobalFlow::checkIntBoardCom(int nInt)
{
    /* 接口板 */
    char buf[18] = { 0 };

    for (int m = 0; m < 3; m++)
    {       // 循环次数
        MySerial.com1Write(CMD_INT_VERSION, INT, nInt, 0, 0, 0, 0, 0, 0, 3);
        usleep(50000);
        memset(buf, 0, sizeof(buf));

        if (MySerial.com1Read(buf, 18) == 18
            && buf[0] == 0xAA
            && buf[1] == 0xAA
            && buf[2] == 0xAA
            && buf[3] == 0xAA)
        {
            return true;
        }
    }

    return false;
}

/*!
 * \brief GlobalFlow::checkCtrlBoardCom
 *        检查控制板通信
 * \return
 */
bool GlobalFlow::checkCtrlBoardCom(int nCtrl)
{
    char buf[18] = { 0 };
    for (int j = 0; j < 3; j++)
    {   // 循环次数
        MySerial.com2Write(CMD_CTRL_VERSION, CTRL_GENERAL, nCtrl, 0, 0, 0, 0, 0, 3);
        usleep(50000);
        memset(buf, 0, sizeof(buf));

        if (MySerial.com2Read(buf, 18) == 18
            && buf[0] == 0xAA
            && buf[1] == 0xAA
            && buf[2] == 0xAA
            && buf[3] == 0xAA)
        {
            return true;
        }
    }

    return false;
}

/*!
 * \brief GlobalFlow::detectUsbFirmware
 *        检测USB固件
 */
void GlobalFlow::detectUsbFirmware()
{
#ifdef Q_OS_UNIX
    //    char sTmpBuf[128] = {0};
    //    int nUsbRet;

    //    if(myUsbDev != NULL){
    //        usb.myusbFree();
    //    }
    //    usb.myusbInit();
    //    if(myUsbDev == NULL){
    //        struGsh.struVer.nUsbFirmware = 255;
    //        return;
    //    }

    //    /** 根据返回值判断端口号是否存在
    //     *  无此端点：-2
    //     *  连接端点超时：-110 */
    //    nUsbRet = usb_bulk_read(myUsbDev, USB_EP_TEST_IN, sTmpBuf, 128, 100);

    //    /** 老固件使用输入端口号0x86、0x88
    //        新固件使用输入端口号0x86 */
    //    if (nUsbRet == -2) {
    //        struGsh.struVer.nUsbFirmware = 2;
    //    } else {
    //        struGsh.struVer.nUsbFirmware = 1;
    //    }
#endif
}

/*!
 * \brief GlobalFlow::detectNewUsbFirmware
 *  通过读取/proc/bus/usb/devices信息的方式查询USB固件版本
*/
void GlobalFlow::detectNewUsbFirmware()
{
#ifdef Q_OS_UNIX
    struGsh.struVer.nUsbFirmware = 255; // 默认值

    /** 老固件使用输入端口号0x86、0x88
        新固件使用输入端口号0x86 */
    QFile file("/proc/bus/usb/devices");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString info(in.readAll());
    if (info.contains("Vendor=0754 ProdID=1002"))
    {
        if (info.contains("Ad=88(I)"))
        {
            struGsh.struVer.nUsbFirmware = 1;
        }
        else
        {
            struGsh.struVer.nUsbFirmware = 2;
        }
    }
#endif
}

/*!
 * \brief GlobalFlow::checkIntBaud
 *        检查接口板波特率一致性
 * \return
 */
bool GlobalFlow::checkIntBaud()
{
    for (int i = 1; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        if (struGsh.nIntBaud[i] != struGsh.nIntBaud[i - 1])
        {
            return false;
        }
    }

    return true;
}

/*!
 * \brief GlobalFlow::checkIntBaud
 *        检查控制板波特率一致性
 * \return
 */
bool GlobalFlow::checkCtrlBaud()
{
    for (int i = 1; i < struCnfg.nCtrlBoardTotal; i++)
    {
        if (struGsh.nCtrlBaud[i] != struGsh.nCtrlBaud[i - 1])
        {
            return false;
        }
    }

    return true;
}

/* 通信自检 */
int GlobalFlow::initCommunication()
{
    int i, j, ret, nAddr = 0;
    int intResult[MAX_LEVEL];
    int colorResult[MAX_LEVEL][MAX_UNIT];
    int ctrlResult[MAX_CTRL] = { 0 };
    int firstRun = 1, reExamine = 0, err = 0;
    char buf[24];

    /** 串口1、2考虑波特率兼容性 */
    setCom1Baud();
    setCom2Baud();

    for (i = 0; i < MAX_LEVEL; i++)
    {
        intResult[i] = -1;
        for (j = 0; j < MAX_UNIT; j++)
        {
            colorResult[i][j] = -1;
        }
    }
    for (i = 0; i < MAX_CTRL; i++)
    {
        ctrlResult[i] = -1;
    }

update:
    reExamine = 0;

    /*  彩色相机板  */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            for (int m = 0; m < 3; m++)
            {       // 循环次数
                MySerial.com1Write(CMD_UNIT_CAMERA_VERSION, UNIT, i, nAddr, 0, 0, 0, CAMERA_COLOR, 0, 3);
                usleep(50000);
                memset(buf, 0, sizeof(buf));
                if ((ret = MySerial.com1Read(buf, 24)) == 24)
                {
                    struGsh.struVer.sUnit[i][j][0] = buf[17];   // 小版本
                    struGsh.struVer.sUnit[i][j][1] = buf[16];   // 大版本
                    struGsh.struVer.sUnit[i][j][2] = buf[15];   // 用户模式代号
                    struGsh.struVer.sUnit[i][j][3] = buf[14];   // 物料编码7-8
                    struGsh.struVer.sUnit[i][j][4] = buf[13];   // 物料编码5-6
                    struGsh.struVer.sUnit[i][j][5] = buf[12];   // 物料编码3-4
                    if (struGsh.struVer.sUnit[i][j][1] & 128)
                    {
                        colorResult[i][j] = 0;      // 处于工厂模式
                        reExamine = 1;
                    }
                    else
                    {
                        colorResult[i][j] = 1;      // 成功，处于用户模式
                        break;
                    }
                }
                else
                {
                    struGsh.struVer.sUnit[i][j][0] = 255;
                    struGsh.struVer.sUnit[i][j][1] = 255;
                    struGsh.struVer.sUnit[i][j][2] = 255;
                    struGsh.struVer.sUnit[i][j][3] = 255;
                    struGsh.struVer.sUnit[i][j][4] = 255;
                    struGsh.struVer.sUnit[i][j][5] = 255;
                    colorResult[i][j] = -1;          // 通信错误

                    //! 若第三次仍出错则该板通信失败
                    if (m == 2)
                    {
                        err = 1;
                    }
                }
            }
        }
    }

    /* 接口板 */
    for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        for (int m = 0; m < 3; m++)
        {       // 循环次数
            MySerial.com1Write(CMD_INT_VERSION, INT, i, 0, 0, 0, 0, 0, 0, 3);
            usleep(50000);
            memset(buf, 0, sizeof(buf));

            ret = MySerial.com1Read(buf, 24);
            if (ret == 24)
            {
                struGsh.struVer.sInterface[i][0] = buf[17];
                struGsh.struVer.sInterface[i][1] = buf[16];
                struGsh.struVer.sInterface[i][2] = buf[15];
                struGsh.struVer.sInterface[i][3] = buf[14];   // 物料编码7-8
                struGsh.struVer.sInterface[i][4] = buf[13];   // 物料编码5-6
                struGsh.struVer.sInterface[i][5] = buf[12];   // 物料编码3-4
                if (struGsh.struVer.sInterface[i][1] & 128)
                {
                    intResult[i] = 0;
                    reExamine = 1;
                }
                else
                {
                    intResult[i] = 1;
                    break;
                }
            }
            else
            {
                struGsh.struVer.sInterface[i][0] = 255;
                struGsh.struVer.sInterface[i][1] = 255;
                struGsh.struVer.sInterface[i][2] = 255;
                struGsh.struVer.sInterface[i][3] = 255;   // 物料编码7-8
                struGsh.struVer.sInterface[i][4] = 255;   // 物料编码5-6
                struGsh.struVer.sInterface[i][5] = 255;   // 物料编码3-4
                intResult[i] = -1;

                //! 若第三次仍出错则该板通信失败
                if (m == 2)
                {
                    err = 1;
                }
            }
        }
    }

    if ((reExamine == 1) && (firstRun == 1))
    {
        firstRun = 0;

        /* camera color board */
        for (i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
            {
                nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                if (colorResult[i][j] == 0)
                {
                    MySerial.com1Write(COM_UNIT_UPDATE_USER, UNIT, i, nAddr, 0, 0, 0, CAMERA_COLOR, struCnfp.nColorBoardMode, 3);
                }
            }
        }

        /* interface board */
        for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
        {
            if (intResult[i] == 0)
            {
                MySerial.com1Write(COM_INT_UPDATE_USER, INT, i, 0, 0, 0, 0, 0, struCnfp.nInterfaceBoardMode, 3);
            }
        }
#ifdef Q_OS_UNIX
        myFlow.sleep(10);
#endif
        goto update;
    }

    /* 控制板 */
    for (i = 0; i < struCnfg.nCtrlBoardTotal; i++)
    {
        for (j = 0; j < 3; j++)
        {
            MySerial.com2Write(CMD_CTRL_VERSION, CTRL_GENERAL, i, 0, 0, 0, 0, 0, 3);
            usleep(50000);
            memset(buf, 0, sizeof(buf));

            ret = MySerial.com2Read(buf, 18);

            if (ret == 18 && buf[0] == 0xAA && buf[1] == 0xAA
                && buf[2] == 0xAA && buf[3] == 0xAA)
            {
                struGsh.struVer.sControl[i][0] = buf[11];
                struGsh.struVer.sControl[i][1] = buf[10];
                break;
            }
            else
            {
                struGsh.struVer.sControl[i][0] = 255;
                struGsh.struVer.sControl[i][1] = 255;
                //! 若第三次仍出错则该板通信失败
                if (j == 2)
                {
                    err = 1;
                }
            }
        }
    }

    //! 恒流源板
    myMonoFlow.checkLightSourceBoard();

    //! USB固件版本查询
    detectNewUsbFirmware();

    //! 检查接口板和控制板波特率一致性
    if (!checkIntBaud() || !checkCtrlBaud())
    {
        err = 1;
    }

    return err;
}

/*获取色选模式中组名*/
void GlobalFlow::getColorSelectGroupName()
{
    int i;
    QString sLevelName[3];

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        sLevelName[0] = "";
        break;
    default:
        break;
    }

    for (i = 0;i < struCnfg.nLevelTotal; i++)
    {
        myString.sColorSelectGroupName[i] = sLevelName[i];
    }
}

/*获取色选模式名称*/
void GlobalFlow::getColorSelectModeName()
{
    int i;
    QString sColorModeName[4];

    sColorModeName[0] = myLan.black_tea;
    sColorModeName[1] = myLan.white_bar_tea;
    sColorModeName[2] = myLan.yeloow_green_bar_piece;
    sColorModeName[3] = myLan.red_bar_red_piece;
    for (i = 0;i < MAX_COLOR_MODE; i++)
    {
        myString.sColorSelectModeName[i] = sColorModeName[i];
    }
}

/* 获取识别组每个组组名 */
void GlobalFlow::getIdentifyGroupName()
{
    int i, j;
    QString sLevelName[3];

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        sLevelName[0] = "";
        break;
    default:
        break;
    }

    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            switch (struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nNameIndex)
            {
            case FIRST_FRONT:   // 一次前视
                myString.sIdentifyGroupName[i][j] = myLan.first_front;
                break;
            case FIRST_REAR:    // 一次后视
                myString.sIdentifyGroupName[i][j] = myLan.first_rear;
                break;
            case SECOND_FRONT:  // 二次前视
                myString.sIdentifyGroupName[i][j] = myLan.second_front;
                break;
            case SECOND_REAR:   // 二次后视
                myString.sIdentifyGroupName[i][j] = myLan.second_rear;
                break;
            case THIRD_FRONT:   // 三次前视
                myString.sIdentifyGroupName[i][j] = myLan.third_front;
                break;
            case THIRD_REAR:    // 三次后视
                myString.sIdentifyGroupName[i][j] = myLan.third_rear;
                break;
            case FOURTH_FRONT:	// 四次前视
                myString.sIdentifyGroupName[i][j] = myLan.fourth_front;
                break;
            case FOURTH_REAR:	// 四次后视
                myString.sIdentifyGroupName[i][j] = myLan.fourth_rear;
                break;
            case FIRST_UPPER:   // 一次上视
                myString.sIdentifyGroupName[i][j] = myLan.first_upper;
                break;
            case FIRST_DOWN:    // 一次下视
                myString.sIdentifyGroupName[i][j] = myLan.first_lower;
                break;
            case SECOND_UPPER:   // 二次上视
                myString.sIdentifyGroupName[i][j] = myLan.second_upper;
                break;
            case SECOND_DOWN:    // 二次下视
                myString.sIdentifyGroupName[i][j] = myLan.second_lower;
                break;
            case FIRST_DOWN_FRONT_ML:
                myString.sIdentifyGroupName[i][j] = myLan.first_lower_layer + "-" + myLan.upper;
                break;
            case FIRST_UP_FRONT_ML:
                myString.sIdentifyGroupName[i][j] = myLan.first_upper_layer + "-" + myLan.upper;
                break;
            case FIRST_DOWN_REAR_ML:
                myString.sIdentifyGroupName[i][j] = myLan.first_lower_layer + "-" + myLan.lower;
                break;
            case FIRST_UP_REAR_ML:
                myString.sIdentifyGroupName[i][j] = myLan.first_upper_layer + "-" + myLan.lower;
                break;
            case SECOND_DOWN_FRONT_ML:
                myString.sIdentifyGroupName[i][j] = myLan.second_lower_layer + "-" + myLan.upper;
                break;
            case SECOND_UP_FRONT_ML:
                myString.sIdentifyGroupName[i][j] = myLan.second_upper_layer + "-" + myLan.upper;
                break;
            case SECOND_DOWN_REAR_ML:
                myString.sIdentifyGroupName[i][j] = myLan.second_lower_layer + "-" + myLan.lower;
                break;
            case SECOND_UP_REAR_ML:
                myString.sIdentifyGroupName[i][j] = myLan.second_upper_layer + "-" + myLan.lower;
                break;
            case FIRST_DOWN_FRONT:  // 一次前下
                myString.sIdentifyGroupName[i][j] = myLan.first_down_front;
                break;
            case FIRST_UP_FRONT:    // 一次前上
                myString.sIdentifyGroupName[i][j] = myLan.first_up_front;
                break;
            case FIRST_DOWN_REAR:   // 一次后下
                myString.sIdentifyGroupName[i][j] = myLan.first_down_rear;
                break;
            case FIRST_UP_REAR:     // 一次后上
                myString.sIdentifyGroupName[i][j] = myLan.first_up_rear;
                break;
            case SECOND_DOWN_FRONT: // 二次前下
                myString.sIdentifyGroupName[i][j] = myLan.second_down_front;
                break;
            case SECOND_UP_FRONT:   // 二次前上
                myString.sIdentifyGroupName[i][j] = myLan.second_up_front;
                break;
            case SECOND_DOWN_REAR:  // 二次后下
                myString.sIdentifyGroupName[i][j] = myLan.second_down_rear;
                break;
            case SECOND_UP_REAR:    // 二次后上
                myString.sIdentifyGroupName[i][j] = myLan.second_up_rear;
                break;
            case FRONT_UNIT:   // 一次前视
                myString.sIdentifyGroupName[i][j] = myLan.front_view;
                break;
            case REAR_UNIT:    // 一次后视
                myString.sIdentifyGroupName[i][j] = myLan.rear_view;
                break;
            case UNIT_NULL:
                myString.sIdentifyGroupName[i][j] = "";
                break;
            case FIRST_UP_MASTER:   // 一次上多
                myString.sIdentifyGroupName[i][j] = myLan.ld4_1st_upper_master;
                break;
            case FIRST_DOWN_MASTER: // 一次下多
                myString.sIdentifyGroupName[i][j] = myLan.ld4_1st_lower_master;
                break;
            case SECOND_UP_MASTER:  // 二次上多
                myString.sIdentifyGroupName[i][j] = myLan.ld4_2nd_upper_master;
                break;
            case SECOND_DOWN_MASTER:// 二次下多
                myString.sIdentifyGroupName[i][j] = myLan.ld4_2nd_lower_master;
                break;
            case FIRST_UP_SLAVE:    // 一次上少
                myString.sIdentifyGroupName[i][j] = myLan.ld4_1st_upper_slave;
                break;
            case FIRST_DOWN_SLAVE:  // 一次下少
                myString.sIdentifyGroupName[i][j] = myLan.ld4_1st_lower_slave;
                break;
            case SECOND_UP_SLAVE:   // 二次上少
                myString.sIdentifyGroupName[i][j] = myLan.ld4_2nd_upper_slave;
                break;
            case SECOND_DOWN_SLAVE: // 二次上少
                myString.sIdentifyGroupName[i][j] = myLan.ld4_2nd_lower_slave;
                break;
            default:
                break;
            }
            myString.sIdentifyGroupName[i][j] = sLevelName[i] + myString.sIdentifyGroupName[i][j];
        }
    }
}

/* 获取剔除组每个组组名 */
void GlobalFlow::getTickGroupName()
{
    int i, j;
    QString sLevelName[3];

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        sLevelName[0] = "";
        break;
    default:
        break;
    }
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            switch (struCnfg.struLevelInfo[i].struTickGroupInfo[j].nNameIndex)
            {
            case FIRST_UNIT:    // 一次
                myString.sTickGroupName[i][j] = myLan.first;
                break;
            case SECOND_UNIT:   //二次
                myString.sTickGroupName[i][j] = myLan.second;
                break;
            case THIRD_UNIT:    // 三次
                myString.sTickGroupName[i][j] = myLan.third;
                break;
            case FOURTH_UNIT:	// 四次
                myString.sTickGroupName[i][j] = myLan.fourth;
                break;
            case FIRST_UP_UNIT:     // 一次
                myString.sTickGroupName[i][j] = myLan.upper + " - " + myLan.first;
                break;
            case SECOND_UP_UNIT:    // 二次
                myString.sTickGroupName[i][j] = myLan.upper + " - " + myLan.second;
                break;
            case THIRD_UP_UNIT:     // 三次
                myString.sTickGroupName[i][j] = myLan.upper + " - " + myLan.third;
                break;
            case FIRST_DOWN_UNIT:   // 一次
                myString.sTickGroupName[i][j] = myLan.lower + " - " + myLan.first;
                break;
            case SECOND_DOWN_UNIT:  // 二次
                myString.sTickGroupName[i][j] = myLan.lower + " - " + myLan.second;
                break;
            case THIRD_DOWN_UNIT:   // 三次
                myString.sTickGroupName[i][j] = myLan.lower + " - " + myLan.third;
                break;
            case FRONT_UNIT:        // 一次前视
                myString.sTickGroupName[i][j] = myLan.front_view;
                break;
            case REAR_UNIT:         // 一次后视
                myString.sTickGroupName[i][j] = myLan.rear_view;
                break;
            case UNIT_NULL:         // 空
                myString.sTickGroupName[i][j] = "";
                break;
            default:
                break;
            }
            myString.sTickGroupName[i][j] = sLevelName[i] + myString.sTickGroupName[i][j];
        }
    }
}

void GlobalFlow::getLampName()
{
    int nUnitLevelTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
    QStringList strList;

    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        if (struCnfg.nEnableLampAI == 1)
        {
            if (struCnfe.nEnableCGMachine)
            {
                //! CG8及CG10整机共6个恒流源板
                //! CG4及CG6整机共4个恒流源板
                if (nUnitLevelTotal >= 16)
                {
                    if (struCnfe.nEnableHGQMachine)
                    {
                        strList << "1-1" << "2-1" << "3-1" << "4-1" << "5-1" << "6-1" //! 恒流源板1
                            << "1-2" << "2-2" << "3-2" << "4-2" << "5-2" << "6-2" //! 恒流源板2
                            << "1B-R" << "2B-G" << "3B-B" << "4" << "5" << "6" //! 恒流源板3
                            << "1-1" << "2-1" << "3-1" << "4-1" << "5-1" << "6-1" //! 恒流源板4
                            << "1-2" << "2-2" << "3-2" << "4-2" << "5-2" << "6-2" //! 恒流源板5
                            << "1B-R" << "2B-G" << "3B-B" << "4" << "5" << "6";//! 恒流源板6
                    }
                    else
                    {
                        strList << "1-1" << "2-1" << "3-1" << "4-1" << "" << "" //! 恒流源板1
                            << "1-2" << "2-2" << "3-2" << "4-2" << "" << "" //! 恒流源板2
                            << "1B-R" << "2B-G" << "3B-B" << "" << "" << "" //! 恒流源板3
                            << "1-1" << "2-1" << "3-1" << "4-1" << "" << "" //! 恒流源板4
                            << "1-2" << "2-2" << "3-2" << "4-2" << "" << "" //! 恒流源板5
                            << "1B-R" << "2B-G" << "3B-B" << "" << "" << "";//! 恒流源板6
                    }
                }
                else
                {
                    if (struCnfe.nEnableHGQMachine == 1)
                    {//黑枸杞定制
                        strList << "1" << "2" << "3" << "4" << "5" << "6"         //! 恒流源板1
                            << "" << "" << "" << "" << "" << ""             //! 恒流源板2
                            << "1B-R" << "2B-G" << "3B-B" << "4" << "5" << "6" //! 恒流源板3
                            << "1" << "2" << "3" << "4" << "5" << "6"         //! 恒流源板4
                            << "" << "" << "" << "" << "" << ""             //! 恒流源板5
                            << "1B-R" << "2B-G" << "3B-B" << "4" << "5" << "6";//! 恒流源板6
                    }
                    else
                    {
                        strList << "1" << "2" << "3" << "4" << "5" << "6"         //! 恒流源板1
                            << "" << "" << "" << "" << "" << ""             //! 恒流源板2
                            << "1B-R" << "2B-G" << "3B-B" << "" << "" << "" //! 恒流源板3
                            << "1" << "2" << "3" << "4" << "5" << "6"         //! 恒流源板4
                            << "" << "" << "" << "" << "" << ""             //! 恒流源板5
                            << "1B-R" << "2B-G" << "3B-B" << "" << "" << "";//! 恒流源板6
                    }

                }

                for (int i = 0; i < strList.size(); i++)
                {
                    myString.sLamp[i] = strList.at(i);
                }
            }
            else if (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal > 16)
            {
                myString.sLamp[0] = myLan.lamp_front + "1";
                myString.sLamp[1] = myLan.lamp_front + "2";
                myString.sLamp[2] = myLan.lamp_front + "3";
                myString.sLamp[3] = myLan.lamp_front + "4";
                myString.sLamp[4] = myLan.lamp_front + "";
                myString.sLamp[5] = myLan.lamp_front + "";

                myString.sLamp[6] = myLan.lamp_front + "5";
                myString.sLamp[7] = myLan.lamp_front + "6";
                myString.sLamp[8] = myLan.lamp_front + "7";
                myString.sLamp[9] = myLan.lamp_front + "8";
                myString.sLamp[10] = myLan.lamp_front + "";
                myString.sLamp[11] = myLan.lamp_front + "";

                myString.sLamp[12] = myLan.lamp_front_bg_red + "1";
                myString.sLamp[13] = myLan.lamp_front_bg_green + "1";
                myString.sLamp[14] = myLan.lamp_front_bg_blue + "1";
                myString.sLamp[15] = myLan.lamp_front_bg_red + "";
                myString.sLamp[16] = myLan.lamp_front_bg_green + "";
                myString.sLamp[17] = myLan.lamp_front_bg_blue + "";

                myString.sLamp[18] = myLan.lamp_front_bg_red + "2";
                myString.sLamp[19] = myLan.lamp_front_bg_green + "2";
                myString.sLamp[20] = myLan.lamp_front_bg_blue + "2";
                myString.sLamp[21] = myLan.lamp_front_bg_red + "";
                myString.sLamp[22] = myLan.lamp_front_bg_green + "";
                myString.sLamp[23] = myLan.lamp_front_bg_blue + "";

                myString.sLamp[24] = myLan.lamp_rear + "1";
                myString.sLamp[25] = myLan.lamp_rear + "2";
                myString.sLamp[26] = myLan.lamp_rear + "3";
                myString.sLamp[27] = myLan.lamp_rear + "4";
                myString.sLamp[28] = myLan.lamp_rear + "";
                myString.sLamp[29] = myLan.lamp_rear + "";

                myString.sLamp[30] = myLan.lamp_rear + "5";
                myString.sLamp[31] = myLan.lamp_rear + "6";
                myString.sLamp[32] = myLan.lamp_rear + "7";
                myString.sLamp[33] = myLan.lamp_rear + "8";
                myString.sLamp[34] = myLan.lamp_rear + "";
                myString.sLamp[35] = myLan.lamp_rear + "";

                myString.sLamp[36] = myLan.lamp_rear_bg_red + "3";
                myString.sLamp[37] = myLan.lamp_rear_bg_green + "3";
                myString.sLamp[38] = myLan.lamp_rear_bg_blue + "3";
                myString.sLamp[39] = myLan.lamp_rear_bg_red + "";
                myString.sLamp[40] = myLan.lamp_rear_bg_green + "";
                myString.sLamp[41] = myLan.lamp_rear_bg_blue + "";

                myString.sLamp[42] = myLan.lamp_rear_bg_red + "4";
                myString.sLamp[43] = myLan.lamp_rear_bg_green + "4";
                myString.sLamp[44] = myLan.lamp_rear_bg_blue + "4";
                myString.sLamp[45] = myLan.lamp_rear_bg_red + "";
                myString.sLamp[46] = myLan.lamp_rear_bg_green + "";
                myString.sLamp[47] = myLan.lamp_rear_bg_blue + "";
            }
            else
            {
                myString.sLamp[0] = myLan.lamp_front_1;
                myString.sLamp[1] = myLan.lamp_front_2;
                myString.sLamp[2] = myLan.lamp_front_3;
                myString.sLamp[3] = myLan.lamp_front_bg_red;
                myString.sLamp[4] = myLan.lamp_front_bg_green;
                myString.sLamp[5] = myLan.lamp_front_bg_blue;
                myString.sLamp[6] = myLan.lamp_rear_1;
                myString.sLamp[7] = myLan.lamp_rear_2;
                myString.sLamp[8] = myLan.lamp_rear_3;
                myString.sLamp[9] = myLan.lamp_rear_bg_red;
                myString.sLamp[10] = myLan.lamp_rear_bg_green;
                myString.sLamp[11] = myLan.lamp_rear_bg_blue;
                myString.sLamp[12] = myLan.lamp_rear + " 4";
                myString.sLamp[13] = myLan.lamp_front + " 4";
                myString.sLamp[14] = myLan.infra;
            }
        }
        else
        {
            myString.sLamp[0] = myLan.lamp_front_bg_red;
            myString.sLamp[1] = myLan.lamp_front_bg_green;
            myString.sLamp[2] = myLan.lamp_front_bg_blue;
            myString.sLamp[3] = myLan.lamp_rear_bg_red;
            myString.sLamp[4] = myLan.lamp_rear_bg_green;
            myString.sLamp[5] = myLan.lamp_rear_bg_blue;
            myString.sLamp[6] = myLan.lamp_rear_1;
            myString.sLamp[7] = myLan.lamp_rear_2;
            myString.sLamp[8] = myLan.lamp_rear_3;
            myString.sLamp[9] = myLan.lamp_rear_4;
        }
        break;
    default:
        break;
    }
}

/*!
 * \brief 根据当前模式获取名称
 * \return 方案名称
 */
QString GlobalFlow::getNameByModeRSC(int nMode)
{
    QStringList list;
    list.push_back(myLan.mode_discolor);
    list.push_back(myLan.mode_discolor_reverse);
    list.push_back(myLan.mode_milky);
    list.push_back(myLan.mode_milky_reverse);
    list.push_back(myLan.mode_yellow_milky);
    list.push_back(myLan.mode_yellow_trans);
    list.push_back(myLan.general);

    if (nMode < 0 || nMode >= RSC_SORT_MODE_TYPE)
    {
        return QString();
    }

    return list.at(nMode);
}

/*!
 * \brief GlobalFlow::initSerial
 *        初始化串口设备
 */
void GlobalFlow::initSerial()
{
    MySerial.comOpen(1);
    MySerial.comOpen(2);
    MySerial.comOpen(3);

    setCom1Baud();
    setCom2Baud();
}

/*!
 * \brief GlobalFlow::onOff
 *        开关供料
 */
void GlobalFlow::onOff()
{
    int i, j;
    if (struGsh.bStatFeed == 1)
    {
        LOG_INFO_STM("Feed off");

        //关料
        struGsh.bStatFeed = 0;
        resetFeeder(1);
        sleep(5);

        for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
        {
            MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, 0, 0, 3);
        }
        for (i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
            {
                int nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, nAddr, 0, 0, 0, 0, 0, 3);
            }
        }

        //当前是图像推理模式
        if (!struGsh.isImageSend)
        {
            imageInferOnOff(false);
        }
        else
        {
            imageSendOnOff(false);
        }

        struGsh.bFlagAutowipe = 0;      // 禁止清灰
    }
    else
    {
        //开料
        struGsh.bStatFeed = 1;
        struGsh.bFlagAutowipe = 1;           //! 使能自动清灰
        myFlow.initMachineType();
        if (struGsh.bFlagMachinetest == 2)
        {
            // 整机老化自检状态
            LOG_INFO_STM("feed switch, SelfExamine:" << struGsh.nSelfExamineFre);

            for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
            {
                MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, struGsh.nSelfExamineFre, 2, 3);
            }
            for (i = 0; i < struCnfg.nLevelTotal; i++)
            {
                for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
                {
                    int nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                    MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, nAddr, 0, 0, 0, struGsh.nSelfExamineFre, 2, 3);
                }
            }
        }
        else
        {
            LOG_INFO_STM("feed switch: Feed on");
            resetSortParams();
            for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
            {    // 正常色选工作状态
                MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, 0, 1, 3);
                for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
                {
                    int nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                    MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, nAddr, 0, 0, 0, 0, 1, 3);
                }
            }
        }

        //当前是图像推理模式
        if (!struGsh.isImageSend)
        {
            imageInferOnOff(true);
        }
        else
        {
            imageSendOnOff(true);
        }

        sleep(1);

        //! 振动器独立设置模式下开启所有振动器供料使能
        if (struCnfg.nParamSetAlone == 1)
        {
            for (int i = 0;i < MAX_FEED; i++)
            {
                struCnfp.struGroupCtrl[ONE_LEVEL].nFeederEnable[i] = 1;
            }
        }

        resetFeeder(0);
        resetFeeder(1);

        //! 使能自动清灰
        struGsh.bFlagAutowipe = 1;
    }
}

/* 打印调试的参数 */
void GlobalFlow::printArgsForTest(int indexOfTrain)
{
#if DEBUG_OUTPUT == 1
    int levelNum = 0;
    int groupNum = 0;
    QStringList tmp;
    for (groupNum = 0; groupNum < struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal; groupNum++)
    {
        tmp.clear();
        printf("identity id %d, intel %d: \n", groupNum, indexOfTrain);
        tmp
            << QString("ConsDfl[0]: %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[0])
            << QString("ConsDfl[1]: %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[1])
            << QString("QuoR[0]   : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[0])
            << QString("QuoR[1]   : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[1])
            << QString("QuoG[0]   : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[0])
            << QString("QuoG[1]   : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[1])
            << QString("QuoB[0]   : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[0])
            << QString("QuoB[1]   : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[1])
            << QString("QuoRG[0]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[0])
            << QString("QuoRG[1]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[1])
            << QString("QuoRB[0]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[0])
            << QString("QuoRB[1]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[1])
            << QString("QuoGB[0]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[0])
            << QString("QuoGB[1]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[1])
            << QString("QuoRR[0]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[0])
            << QString("QuoRR[1]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[1])
            << QString("QuoGG[0]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[0])
            << QString("QuoGG[1]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[1])
            << QString("QuoBB[0]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[0])
            << QString("QuoBB[1]  : %1\n").arg(struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[1])
            ;
        foreach(QString st, tmp)
        {
            printf("%s", qPrintable(st));
        }
    }
    fflush(0);
#endif
}

/* RS机型的开关下料 */
void GlobalFlow::onOffMono()
{
    /* 若下料已开启则关闭下料 */
    if (struGsh.bStatFeed == 1)
    {
        struGsh.bStatFeed = 0;
        resetFeeder(1);

        /* MC机型增加停下料关喷阀时间 */
        if (struGsh.bIsMC)
        {
            sleep(6);
        }
        else
        {
            sleep(4);
        }

        myMonoFlow.sendOnOffParams(0);

        /* 禁止自动清灰 */
        struGsh.bFlagAutowipe = 0;

        LOG_INFO_STM("Feed switch: Feed off");
    }
    else
    {
        // 正常开启下料过程
        /* RS机型发送所有参数 */
        myMonoFlow.sendAllParamsMono();
        myMonoFlow.sendOnOffParams(1);

        sleep(1);
        struGsh.bStatFeed = 1;
        resetFeeder(1);

        /* 使能自动清灰 */
        struGsh.bFlagAutowipe = 1;

        LOG_INFO_STM("Feed Switch: Feed on");
        LOG_INFO_STM("Scheme name: " << struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName
            << ", file: " << struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
    }
}


/* 气压报警正常后开喷阀 */
void GlobalFlow::ejectorOn()
{
    int i, j, k;
    int nUnitAddr;

    /* ejector on&off */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getIdentifyGroupAddr(i, j, k);
                MySerial.com1Write(CMD_UNIT_EJECT_ONOFF, UNIT, i, nUnitAddr, 0, 0, 0, 0, 1, 3);
            }
        }
    }
}

/* 气压不足时关喷阀*/
void GlobalFlow::ejectOff()
{
    int i, j, k;
    int nUnitAddr;

    /* ejector on&off */
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getIdentifyGroupAddr(i, j, k);
                MySerial.com1Write(CMD_UNIT_EJECT_ONOFF, UNIT, i, nUnitAddr, 0, 0, 0, 0, 0, 3);
            }
        }
    }
}

/* 校正 */
void GlobalFlow::updateOnoff(int onOff)
{
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            MySerial.com1Write(CMD_UNIT_UPDATE, UNIT, i, struCnfg.struLevelInfo[i].nUnitId[j],
                struCnfg.nProfile + 1, 0, 0, 0, onOff, 3);
        }
    }
}

bool GlobalFlow::peaMachineJudge()
{
    int notBeanMatCount = 0;
    char buf[24] = {};
    int ret = 0, nAddr;
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            for (int m = 0; m < 3; m++)
            {       // 循环次数
                MySerial.com1Write(CMD_UNIT_CAMERA_VERSION, UNIT, i, nAddr, 0, 0, 0, CAMERA_COLOR, 1, 3);
                usleep(400000);
                memset(buf, 0, sizeof(buf));
                if ((ret = MySerial.com1Read(buf, 24)) == 24)
                {
                    if (buf[14] == 0)
                    {
                        notBeanMatCount++;
                    }
                    break;
                }
            }
        }
    }

    struGsh.enableJudgeBeanMachine = false;
    if (notBeanMatCount < 1)
    {
        struGsh.isPeaMachineBeanMat = true;
        return true;
    }
    else
    {
        struGsh.isPeaMachineBeanMat = false;
        return false;
    }
}

/* 清灰操作 */
void GlobalFlow::startWipe()
{
    int nStatFeed = 0;
    int flagBacklight = 0;
    int curLevel = 0;
    int curUnit = 0;

    //! 若当前处于屏保状态，则打开背光，并保存当前屏保状态
    if (struGsh.nBacklightStat == 0)
    {
        flagBacklight = 1;
        myFlow.setTsBackLight(1);
    }

    //! 统计信息采集只有在满足特定条件下触发（启用统计使能开关且满足统计周期要求)
    if (struCnfg.nStatisticEnable == 1 && (++struGsh.nWipeTimes >= qRound(1.0 * DEF_TIMEOUT_STATISTIC / struCnfp.struGroupCtrl[0].nWipeInterval)))
    {
        struGsh.nWipeTimes = 0;
        struGsh.isBigDataUpdated = true;
        //! 随机生成大数据待采集对象 （参考当前相机所在位置）
        curLevel = struCnfg.nCurCapLevel = getRandomLevel(struCnfg.nCurCapLevel, struCnfg.nCurCapUnit);
        curUnit = struCnfg.nCurCapUnit = getRandomUnit(struCnfg.nCurCapLevel, struCnfg.nCurCapUnit);
        qDebug("level %d unit %d", curLevel, curUnit);
    }

    struGsh.isQueryingCom3 = true;

    //! 若当前处在供料状态，则关闭下料，并保存当前状态
    if (struGsh.bStatFeed == 1)
    {
        nStatFeed = 1;
        struGsh.bStatFeed = 0;

        MySerial.com2Write(CMD_CTRL_LIFT_SWITCH, CTRL_GENERAL, 0, 0, 0, 0, 0, 0, 3);//关闭提升机
        MySerial.com2Write(CMD_CTRL_LIFT_SWITCH, CTRL_GENERAL, 1, 0, 0, 0, 0, 0, 3);//关闭提升机
        myFlow.sleep(struCnfp.struGroupCtrl[0].nLiftTimeBeforeWipe);

        myFlow.resetFeeder(1);
        //供料关闭，10s后关闭喷阀
        //测试后，优化为5s
        sleep(5);
        myFlow.ejectOff();

        //! 考虑多层色选机振动器至视点距离较远
        if (struCnfg.nLevelTotal > 1)
        {
            myFlow.sleep(6);
        }
        else
        {
            myFlow.sleep(3);
        }

        /* 整机性能参数的获取流程, 信号直方图默认统计用时10s, 其他统计时间间隔为5s. 清灰前下发准备命令 */
        //! 保存清灰前背景数据
        if (struGsh.isBigDataUpdated)
        {
            MySerial.com1Write(CMD_UNIT_STATISTIC_BACKGROUND_CAPTURE, UNIT, curLevel, curUnit, 0, 0, 0, 0, 0, 3);
        }
    }

    //! 复合清灰模式
    if (struCnfg.nWipeMode == 1)
    {
        //! 清灰时间2暂借米机清灰气帘命令
        MySerial.com2Write(CMD_CTRL_WIPE_WIND_RS, CTRL_GENERAL, 0, 0, 0, 0, struCnfp.struGroupCtrl[0].nWipeDuration2 * 10, 0xAA, 3);
        myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration2);
    }

    //! 开始清灰操作
    MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0, 0, 0, 0, struCnfp.struGroupCtrl[0].nWipeDuration, 0xAA, 3);
    MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 1, 0, 0, 0, struCnfp.struGroupCtrl[0].nWipeDuration, 0xAA, 3);
    myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration);

    //! 恢复至清灰前机器供料状态
    if (nStatFeed == 1)
    {
        //! 豆类定制机型特殊处理
        if (struCnfe.nEnableBeanMachine == 1 && struGsh.enableJudgeBeanMachine)
        {
            peaMachineJudge();
            if (struGsh.isPeaMachineBeanMat == false)
            {
                for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
                {
                    MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, 0, 0, 3);
                    for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
                    {
                        MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, struCnfg.struLevelInfo[i].nUnitId[j], 0, 0, 0, 0, 0, 3);
                    }
                }
                if (flagBacklight == 1)
                {
                    flagBacklight = 0;
                    myFlow.setTsBackLight(1);
                }
                return;
            }
        }
        myFlow.resetSortParams();

        if (struGsh.isBigDataUpdated)
        {
            //! 保存清灰后背景信息
            MySerial.com1Write(CMD_UNIT_STATISTIC_BACKGROUND_CAPTURE, UNIT, curLevel, curUnit, 0, 0, 0, 0, 1, 3);
            myFlow.sleep(1);
        }

        nStatFeed = 0;
        struGsh.bStatFeed = 1;

        myFlow.ejectorOn();

        MySerial.com2Write(CMD_CTRL_LIFT_SWITCH, CTRL_GENERAL, 0, 0, 0, 0, 0, 1, 3);//打开提升机
        MySerial.com2Write(CMD_CTRL_LIFT_SWITCH, CTRL_GENERAL, 1, 0, 0, 0, 0, 1, 3);//打开提升机
        myFlow.sleep(struCnfp.struGroupCtrl[0].nLiftTimeAfterWipe);

        myFlow.resetFeeder(1);


        //! 大数据信息收集
        if (struGsh.isBigDataUpdated)
        {
            myFlow.startStatistic(curLevel, curUnit);
        }
    }

    struGsh.isQueryingCom3 = false;
    //! 恢复至清灰前机器屏保状态
    if (flagBacklight == 1)
    {
        flagBacklight = 0;
        myFlow.setTsBackLight(1);
    }
}

/*!
 * \brief 开始信息统计
 */
void GlobalFlow::startStatistic(int curLevel, int curUnit)
{
    //! 整机性能参数的获取流程
    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();

    //! 若吹气频率线程开启则关闭
    bool bIsRunning = false;
    if (g_ejectTimesThread->isRunning())
    {
        g_ejectTimesThread->stopStatRun();
        bIsRunning = true;
    }

    myFlow.sleep(2);//等待2秒，物料状态较为稳定时，通知前置开始统计直方图
    uint histogram_start = QDateTime::currentDateTime().toTime_t();

    //! 开始统计信号直方图
    MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_CAPTURE, UNIT, curLevel, curUnit, 0, 0, 0, 0, 1, 1);

    //! 1.获取清灰前背景数据
    struGsh.struStatisticInfo.bgBeforeWipeStatistic.resize(MAX_STATISTIC_BACKGROUND + 16);
    MySerial.com1Write(CMD_UNIT_STATISTIC_BACKGROUND_OBTAIN, UNIT, curLevel, curUnit, 0, 0, 0, 0, 0, 1);
    MySerial.com3Read(struGsh.struStatisticInfo.bgBeforeWipeStatistic.data(), struGsh.struStatisticInfo.bgBeforeWipeStatistic.size(), 1000);
    struGsh.struStatisticInfo.bgBeforeWipeStatistic.remove(0, 10);
    struGsh.struStatisticInfo.bgBeforeWipeStatistic.truncate(struGsh.struStatisticInfo.bgBeforeWipeStatistic.size() - 6);

    //! 2.获取清灰后背景数据
    struGsh.struStatisticInfo.bgAfterWipeStatistic.resize(MAX_STATISTIC_BACKGROUND + 16);
    MySerial.com1Write(CMD_UNIT_STATISTIC_BACKGROUND_OBTAIN, UNIT, curLevel, curUnit, 0, 0, 0, 0, 1, 1);
    MySerial.com3Read(struGsh.struStatisticInfo.bgAfterWipeStatistic.data(), struGsh.struStatisticInfo.bgAfterWipeStatistic.size(), 1000);
    struGsh.struStatisticInfo.bgAfterWipeStatistic.remove(0, 10);
    struGsh.struStatisticInfo.bgAfterWipeStatistic.truncate(struGsh.struStatisticInfo.bgAfterWipeStatistic.size() - 6);

    //! 3.物料点像素统计数据获取
    struGsh.struStatisticInfo.materialStatistic.resize(MAX_STATISTIC_MATERIAL + 16);
    MySerial.com1Write(CMD_UNIT_STATISTIC_MATERIAL_CAPTURE, UNIT, curLevel, curUnit, 0, 0, 0, 0, struCnfp.nMaterialQueryInterval, 1);
    MySerial.com3Read(struGsh.struStatisticInfo.materialStatistic.data(), struGsh.struStatisticInfo.materialStatistic.size(), 1000);
    struGsh.struStatisticInfo.materialStatistic.remove(0, 10);
    struGsh.struStatisticInfo.materialStatistic.truncate(struGsh.struStatisticInfo.materialStatistic.size() - 6);

    //! 4.吹气次数统计数据获取
    struGsh.struStatisticInfo.ejectStatistic.resize(MAX_STATISTIC_EJTIMES + 16);
    MySerial.com1Write(CMD_INT_STATISTIC_EJTIMES_CAPTURE, INT, curLevel, curUnit, 0, 0, 0, 0, struCnfp.nMaterialQueryInterval, 1);
    MySerial.com3Read(struGsh.struStatisticInfo.ejectStatistic.data(), struGsh.struStatisticInfo.ejectStatistic.size(), 1000);
    struGsh.struStatisticInfo.ejectStatistic.remove(0, 10);
    struGsh.struStatisticInfo.ejectStatistic.truncate(struGsh.struStatisticInfo.ejectStatistic.size() - 6);

    //! 查看直方图统计时间是否用尽
    uint statistic_end = (int)(QDateTime::currentDateTime().toTime_t() - histogram_start);
    if (statistic_end < struCnfp.nHistogramQueryTime)
    {
        myFlow.sleep(struCnfp.nHistogramQueryTime - statistic_end);
    }
    //! 停止统计信号直方图数据
    MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_CAPTURE, UNIT, curLevel, curUnit, 0, 0, 0, 0, 0, 1);

    //! 5.获取信号直方图数据
    struGsh.struStatisticInfo.histogramStatistic.resize(MAX_STATISTIC_HISTOGRAM + 16);
    MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_OBTAIN, UNIT, curLevel, curUnit, 0, 0, 0, 0, 0, 1);
    MySerial.com3Read(struGsh.struStatisticInfo.histogramStatistic.data(), struGsh.struStatisticInfo.histogramStatistic.size(), 1000);
    struGsh.struStatisticInfo.histogramStatistic.remove(0, 10);
    struGsh.struStatisticInfo.histogramStatistic.truncate(struGsh.struStatisticInfo.histogramStatistic.size() - 6);
    infoWidget->hide();

    //! 标记性能参数统计结束信号
    struGsh.bStatisticFinished = 1;
    struGsh.isBigDataUpdated = false;

    //! 恢复吹气频率线程状态
    if (bIsRunning)
    {
        g_ejectTimesThread->start();
    }
}

/*!
 * \brief 接口板传输模式切换设置
 */
void GlobalFlow::initInterfaceTransMode(int mode)
{
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, i, 0, 0, 0, 0, 0, mode, 3);
    }
}

/*!
 * \brief 喷阀数据组合类型设置
 */
void GlobalFlow::initMachineType()
{
    int arg = 0;
    switch (struCnfe.nMachine)
    {
    default:
        arg = 0;
        break;
    }
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        MySerial.com1Write(CMD_INT_VALVE_COMBIN_TYPE, INT, i, 0, 0, 0, 0, 0, arg, 3);
    }
}

/*!
 * \brief 重置LED指示灯（LD4机型）
 */
void GlobalFlow::resetLedModeForLD4()
{
}

/*!
 * \brief GlobalFlow::updateLampTotal
 */
void GlobalFlow::updateLampTotal()
{
    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        struCnfg.nLampTotal = 10;
        if (struCnfg.nEnableLampAI == 1)
        {
            // CG 机型：6个恒流源板 × 6灯 = 36
            struCnfg.nLampTotal = 36;
            struCnfg.nLampAINum = 6;
        }
        else
        {
            if ((struCnfe.nMachine == MACHINE_CF) && (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal > 16))
            {
                struCnfg.nLampAINum = 8;
            }
            else
            {
                struCnfg.nLampAINum = 3;
            }
        }

        break;
    default:
        break;
    }
}

/*!
 * \brief GlobalFlow::updateLightSrcLampNum
 */
void GlobalFlow::updateLightSrcLampNum()
{
    int nUnitLevelTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        if (struCnfe.nEnableCGMachine)
        {
            //! CG8及CG10整机共6个恒流源板
            //! CG4及CG6整机共4个恒流源板
            if (nUnitLevelTotal >= 16)
            {
                if (struCnfe.nEnableHGQMachine)
                {
                    struCnfg.nCurSrcBoradLampNum[0] = 6;
                    struCnfg.nCurSrcBoradLampNum[1] = 6;
                    struCnfg.nCurSrcBoradLampNum[2] = 6;
                    struCnfg.nCurSrcBoradLampNum[3] = 6;
                    struCnfg.nCurSrcBoradLampNum[4] = 6;
                    struCnfg.nCurSrcBoradLampNum[5] = 6;
                }
                else
                {
                    struCnfg.nCurSrcBoradLampNum[0] = 4;
                    struCnfg.nCurSrcBoradLampNum[1] = 4;
                    struCnfg.nCurSrcBoradLampNum[2] = 3;
                    struCnfg.nCurSrcBoradLampNum[3] = 4;
                    struCnfg.nCurSrcBoradLampNum[4] = 4;
                    struCnfg.nCurSrcBoradLampNum[5] = 3;
                }
            }
            else
            {
                if (struCnfe.nEnableHGQMachine == 1)
                {//黑枸杞定制
                    struCnfg.nCurSrcBoradLampNum[0] = 6;
                    struCnfg.nCurSrcBoradLampNum[1] = 0;
                    struCnfg.nCurSrcBoradLampNum[2] = 6;
                    struCnfg.nCurSrcBoradLampNum[3] = 6;
                    struCnfg.nCurSrcBoradLampNum[4] = 0;
                    struCnfg.nCurSrcBoradLampNum[5] = 6;
                }
                else
                {
                    struCnfg.nCurSrcBoradLampNum[0] = 6;
                    struCnfg.nCurSrcBoradLampNum[1] = 0;
                    struCnfg.nCurSrcBoradLampNum[2] = 3;
                    struCnfg.nCurSrcBoradLampNum[3] = 6;
                    struCnfg.nCurSrcBoradLampNum[4] = 0;
                    struCnfg.nCurSrcBoradLampNum[5] = 3;
                }
            }
        }
        else if (nUnitLevelTotal > 16)
        {
            //! CF10整机共8个恒流源,其它为3个
            struCnfg.nCurSrcBoradLampNum[0] = 4;
            struCnfg.nCurSrcBoradLampNum[1] = 4;
            struCnfg.nCurSrcBoradLampNum[2] = 3;
            struCnfg.nCurSrcBoradLampNum[3] = 3;
            struCnfg.nCurSrcBoradLampNum[4] = 4;
            struCnfg.nCurSrcBoradLampNum[5] = 4;
            struCnfg.nCurSrcBoradLampNum[6] = 3;
            struCnfg.nCurSrcBoradLampNum[7] = 3;
        }
        else
        {
            struCnfg.nCurSrcBoradLampNum[0] = 6;
            struCnfg.nCurSrcBoradLampNum[1] = 6;
            struCnfg.nCurSrcBoradLampNum[2] = 2;
        }
        break;
    default:
        break;
    }
}

/* 根据机型模式id获取名称,名称长度不可超过20个 */
QString GlobalFlow::getMachineName(int id)
{
    QString name = "";
    if (id == MACHINE_CF || id == MACHINE_CG)
    {
        name = "CG";
    }

    return name;

}

/* 获取产品名称 */
QString GlobalFlow::getProductName(int id)
{
    Q_UNUSED(id);
    if (struCnfe.nEnableBeanMachine == 1)
    {
        return "Bean";
    }
    return "General";
}

/**
 * @brief GlobalFlow::getLanguageList
 * 获取支持语言列表
 * @return
 */
QList<int> GlobalFlow::getLanguageList()
{
    QList<int> list;

    list << LANG_CHS
        << LANG_CHT
        << LANG_ENG
        << LANG_VIET
        << LANG_THAILAND
        << LANG_MYANMAR
        << LANG_KOREANE
        << LANG_UYGHUR
        << LANG_ARABICE
        << LANG_FARSIE
        << LANG_TURKEY
        << LANG_RUS
        << LANG_BULGARIANE
        << LANG_SLOVAKE
        << LANG_FRENCHE
        << LANG_SPAN
        << LANG_PORTUGUESEE
        << LANG_BENGALIE
        << LANG_INDONESIA
        << LANG_POLAND;

    return list;
}

/*!
 * \brief GlobalFlow::getLanguageListName
 * 获取语言列表名称
 * \param index
 * \param local
 * \return
 */
QString GlobalFlow::getLanguageListName(int index, bool local)
{
    QString text = "Unknow";
    switch (index)
    {
    case LANG_CHS:
        text = local ? "简体中文" : myLan.chinese_simplified;
        break;
    case LANG_CHT:
        text = local ? "繁体中文" : myLan.chinese_traditonal;
        break;
    case LANG_ENG:
        text = local ? "英语" : myLan.english;
        break;
    case LANG_VIET:
        text = local ? "越南语" : myLan.vietnam;
        break;
    case LANG_THAILAND:
        text = local ? "泰语" : myLan.thailand;
        break;
    case LANG_MYANMAR:
        text = local ? "缅甸语" : myLan.myanmar;
        break;
    case LANG_KOREANE:
        text = local ? "韩语" : myLan.korean;
        break;
    case LANG_TURKEY:
        text = local ? "土耳其语" : myLan.turkey;
        break;
    case LANG_UYGHUR:
        text = local ? "维吾尔语" : myLan.uyghur;
        break;
    case LANG_RUS:
        text = local ? "俄语" : myLan.russian;
        break;
    case LANG_BULGARIANE:
        text = local ? "保加利亚语" : myLan.bulgarian;
        break;
    case LANG_SLOVAKE:
        text = local ? "斯洛伐克语" : myLan.slovak;
        break;
    case LANG_FRENCHE:
        text = local ? "法语" : myLan.french;
        break;
    case LANG_SPAN:
        text = local ? "西班牙语" : myLan.spanish;
        break;
    case LANG_PORTUGUESEE:
        text = local ? "葡萄牙语" : myLan.portuguese;
        break;
    case LANG_ARABICE:
        text = local ? "阿拉伯语" : myLan.arabic;
        break;
    case LANG_FARSIE:
        text = local ? "波斯语" : myLan.farsi;
        break;
    case LANG_BENGALIE:
        text = local ? "孟加拉语" : myLan.bengali;
        break;
    case LANG_INDONESIA:
        text = local ? "印度尼西亚语" : myLan.indonesia;
        break;
    case LANG_POLAND:
        text = local ? "波兰语" : myLan.poland;
        break;
    default:
        text = local ? "未定义" : "Unknow";
        break;
    }

    return text;
}

/*!
 * \brief GlobalFlow::getLanguageListIcon
 * 获取语言列表图标
 * \param index
 * \return
 */
QIcon GlobalFlow::getLanguageListIcon(int index)
{
    QIcon icon;
    switch (index)
    {
    case LANG_CHS:
    case LANG_CHT:
    case LANG_UYGHUR:
        icon = myIcon.Flag_CN;
        break;
    case LANG_ENG:
        icon = myIcon.Flag_GB;
        break;
    case LANG_VIET:
        icon = myIcon.Flag_VN;
        break;
    case LANG_THAILAND:
        icon = myIcon.Flag_TH;
        break;
    case LANG_MYANMAR:
        icon = myIcon.Flag_MM;
        break;
    case LANG_KOREANE:
        icon = myIcon.Flag_KR;
        break;
    case LANG_TURKEY:
        icon = myIcon.Flag_TR;
        break;
    case LANG_RUS:
        icon = myIcon.Flag_RU;
        break;
    case LANG_BULGARIANE:
        icon = myIcon.Flag_BG;
        break;
    case LANG_SLOVAKE:
        icon = myIcon.Flag_SK;
        break;
    case LANG_FRENCHE:
        icon = myIcon.Flag_FR;
        break;
    case LANG_SPAN:
        icon = myIcon.Flag_ES;
        break;
    case LANG_PORTUGUESEE:
        icon = myIcon.Flag_PT;
        break;
    case LANG_ARABICE:
        icon = myIcon.Flag_SA;
        break;
    case LANG_FARSIE:
        icon = myIcon.Flag_IR;
        break;
    case LANG_BENGALIE:
        icon = myIcon.Flag_BD;
        break;
    case LANG_INDONESIA:
        icon = myIcon.Flag_ID;
        break;
    case LANG_POLAND:
        icon = myIcon.Flag_PL;
        break;
    default:
        break;
    }

    return icon;
}

/*!
 * \brief GlobalFlow::getRunModeList
 * 获取屏运行模式列表
 * \return
 */
QList<int> GlobalFlow::getRunModeList()
{
    QList<int> list;

    list << MODE_RUN_STANDALONE
        << MODE_RUN_MASTER
        << MODE_RUN_SLAVE;

    return list;
}

/*!
 * \brief GlobalFlow::getRunModeName
 * 获取屏运行模式文本
 * \param mode
 * \return
 */
QString GlobalFlow::getRunModeName(int mode)
{
    QString text = "";
    switch (mode)
    {
    case MODE_RUN_STANDALONE:
        text = myLan.standalone;
        break;
    case MODE_RUN_MASTER:
        text = myLan.master;
        break;
    case MODE_RUN_SLAVE:
        text = myLan.slave;
        break;
    default:
        text = myLan.unknown;
        break;
    }

    return text;
}

/*!
 * \brief GlobalFlow::checkSlaveCommStat
 */
void GlobalFlow::checkSlaveCommStat()
{
    if (struCnfg.nRunMode != MODE_RUN_MASTER)
    {
        struGsh.bAlarmSlaveComm = 0;
        return;
    }

    for (int i = 0; i < struGsh.strSlaveList.count(); i++)
    {
        QProcess m_cmd;
        m_cmd.start(QString("ping -w 1 %1").arg(struGsh.strSlaveList.at(i)));

        if (m_cmd.waitForFinished())
        {
            QString str = m_cmd.readAll();

            //! 通过回包信息判断从设备连接状态(兼容Linux与Windows)
            if (!str.contains("ttl") && !str.contains("TTL"))
            {
                m_cmd.kill();
                struGsh.bAlarmSlaveComm = 1;
                return;
            }
        }
    }

    struGsh.bAlarmSlaveComm = 0;
}

/*!
 * \brief GlobalFlow::getSlaveList
 */
void GlobalFlow::getSlaveList()
{
    struGsh.strSlaveList.clear();

    QFile file(CNF_SLAVE_LIST);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QTextStream out(&file);
    while (!out.atEnd())
    {
        struGsh.strSlaveList.push_back(out.readLine());
    }

    file.close();
}

/* 开机初始化 */
void GlobalFlow::initAll()
{
    getSetting();               // 获取配置文件参数内容

    getParamsFileStaus();       // 获取参数文件状态

    getEngineer();              // 获取全局工程参数文件

    getGlobal();                // 获取全局需要保存的参数

    setLang();                  // 设置语言

    getCamera();                // 获取相机参数

    getProfile();               // 获取方案参数

    getPrefixedName();  	    // 重新获取预制方案父节点和子节点名称

    initGsh();                  // 初始化全局共享参数

    initSerial();               // 初始化串口设备

    initMachineType();			// 初始化机器类型

    initUdpImagPara();

    initPixelImagPara();
}

void GlobalFlow::initUdpImagPara()
{
    if (struCnfg.aiEnable != 1)
    {
        return;
    }
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    args[0] = struCnfg.imgFetchHeight / 256;
    args[1] = struCnfg.imgFetchHeight % 256;
    args[2] = struCnfg.imgInferHeight / 256;
    args[3] = struCnfg.imgInferHeight % 256;
    args[4] = struCnfg.imgPicHeight / 256;
    args[5] = struCnfg.imgPicHeight % 256;
    args[6] = struCnfg.imgVideoHeight / 256;
    args[7] = struCnfg.imgVideoHeight % 256;

    for (int i = 0; i < struGsh.aiDeviceNum; i++)
    {
        MyUpd.writeDatagram(CMD_AI_IMAGE_HEIGHT, i, 8, args, struGsh.addressList.at(i), AI_UDP_SEND_PORT);
        data.nCommandAddress = CMD_AI_IMAGE_HEIGHT;
        ret = MyUpd.readUdpDatagrams(&data, 13);
        if (ret != 0)
        {
            qDebug("aiDevice: %d, img ret: %d", i, ret);
        }
    }
}

void GlobalFlow::initPixelImagPara()
{
    if (struCnfg.aiEnable != 1)
    {
        return;
    }
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    int i, j, nUnitAddr;
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            args.clear();
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            args[0] = AIUNIT;
            //ai相机编号，取余
            args[1] = nUnitAddr % 2;
            args[2] = struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr] / 256;
            args[3] = struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr] % 256;
            args[4] = struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr] / 256;
            args[5] = struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr] % 256;
            MyUpd.writeDatagram(CMD_AI_PIXEL_SEND, nUnitAddr / 2, 6, args, struGsh.addressList.at(nUnitAddr / 2), AI_UDP_SEND_PORT);
            data.nCommandAddress = CMD_AI_PIXEL_SEND;
            ret = MyUpd.readUdpDatagrams(&data, 13);
            if (ret != 0)
            {
                qDebug("nUnitAddr: %d, ret: %d", nUnitAddr, ret);
            }
        }
    }
}

void GlobalFlow::initEjectorDelayPara()
{
    if (struCnfg.aiEnable != 1)
    {
        return;
    }

    int i, j, k;
    int nUnitAddr = 0;
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++)
        {
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getTickGroupAddr(i, j, k);
                //                qDebug()<<"nUnitAddr:"<<nUnitAddr;
                args[0] = struCnfg.nEjectorsPerChute;
                args[1] = struCnfp.struGroupTick[i][j].aiEjectorDelay / 256;
                args[2] = struCnfp.struGroupTick[i][j].aiEjectorDelay % 256;
                args[3] = struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay / 256;
                args[4] = struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay % 256;
                args[5] = struCnfp.struGroupTick[i][j].aiEjectorBlowTime;
                //                args[6] = (struCnfp.struGroupTick[i][j].nEjectDelay/10)/256;
                //                args[7] = (struCnfp.struGroupTick[i][j].nEjectDelay/10)%256;
                MyUpd.writeDatagram(CMD_AI_EJECTOR_PARA, nUnitAddr / 2, 6, args, struGsh.addressList.at(nUnitAddr / 2), AI_UDP_SEND_PORT);
                data.nCommandAddress = CMD_AI_EJECTOR_PARA;
                ret = MyUpd.readUdpDatagrams(&data, 13);
                if (ret != 0)
                {
                    qDebug("nUnitAddr: %d, ret: %d", nUnitAddr, ret);
                }
            }
        }
    }

}


void GlobalFlow::initEjectorModePara()
{
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    int nArithmeticEnable[ARITHMETIC_TOTAL];
    memset(nArithmeticEnable, 0, sizeof(nArithmeticEnable));

    for (int i = 0; i < struCnfe.nArithmeticTotal; i++)
    {
        nArithmeticEnable[i] = struCnfp.nArithmeticEnable[i];
    }
    //ai模式且算法使能
    if (struCnfg.aiEnable == 1 && nArithmeticEnable[ARITH_PISTACHIO] == 1)
    {
        args[0] = 3;
    }
    else
    {
        args[0] = 2;
    }
    for (int i = 0; i < struGsh.aiDeviceNum; i++)
    {
        MyUpd.writeDatagram(CMD_AI_EJECTOR_MODE, i, 1, args, struGsh.addressList.at(i), AI_UDP_SEND_PORT);
        data.nCommandAddress = CMD_AI_EJECTOR_MODE;
        ret = MyUpd.readUdpDatagrams(&data, 13);
        if (ret != 0)
        {
            qDebug("aiDevice: %d, img ret: %d", i, ret);
        }
    }
}

void GlobalFlow::initModelType()
{
    if (struCnfg.aiEnable != 1)
    {
        return;
    }

    int i, j, k;
    int nUnitAddr = 0;
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getIdentifyGroupAddr(i, j, k);
                QString modelId = QString::fromUtf8(struCnfp.struGroupIdentify[i][j].struAi.modelId);
                if (modelId == QString("default"))
                {
                    break;
                }
                //                qDebug()<<modelId;
                args = (modelId + ".dlc").toLatin1();
                if (nUnitAddr % 2 == 0)
                {
                    MyUpd.writeDatagram(CMD_AI_MODEL_UPDATE, nUnitAddr / 2, (modelId + ".dlc").length(), args, struGsh.addressList.at(nUnitAddr / 2), AI_UDP_SEND_PORT);
                    ret = MyUpd.readUdpDatagrams(&data, 13);
                    if (ret != 0)
                    {
                        qDebug("nUnitAddr/2: %d, ret: %d", nUnitAddr / 2, ret);
                    }
                }
            }
        }
    }
}

void GlobalFlow::initModelPara()
{
    if (struCnfg.aiEnable != 1)
    {
        return;
    }
    int i, j, k;
    int nUnitAddr = 0;
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    for (i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            QString modelId = QString::fromUtf8(struCnfp.struGroupIdentify[i][j].struAi.modelId);
            QSqlQuery query;
            int modeParaCount;
            query.prepare("SELECT modelId, id, zhName, enName, threshold, isApply, chgTime FROM modelParaInfo  "
                "where modelId = ?  and levelTotal = ? and identifyGroupTotal = ? order by id asc");

            query.bindValue(0, modelId);
            query.bindValue(1, i);
            query.bindValue(2, j);
            if (!query.exec())
            {
                qDebug() << "查询失败：" << query.lastError();
            }
            else
            {
                while (query.next())
                {
                    QString modelId = query.value(0).toString();
                    QString id = query.value(1).toString();
                    QString zhName = query.value(2).toString();
                    QString enName = query.value(3).toString();
                    QString threshold = query.value(4).toString();
                    QString isApply = query.value(5).toString();

                    modeParaStr[modeParaCount].id = id;
                    modeParaStr[modeParaCount].isApply = isApply;
                    modeParaStr[modeParaCount].name = zhName;
                    modeParaStr[modeParaCount].threshold = threshold;
                    modeParaCount++;
                }
            }

            int applyCount = 0;
            int seq = 0;
            args.clear();

            for (int i = 0; i < modeParaCount; i++)
            {
                if (modeParaStr[i].isApply.toInt() == 1)
                {
                    applyCount++;
                    args[seq++] = modeParaStr[i].id.toInt();
                    //阈值=100-灵敏度
                    args[seq++] = 100 - modeParaStr[i].threshold.toInt();
                }
            }
            if (applyCount == 0)
            {
                continue;
            }

            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++)
            {
                nUnitAddr = getIdentifyGroupAddr(i, j, k);
                if (nUnitAddr % 2 == 0)
                {
                    MyUpd.writeDatagram(CMD_AI_MODEL_PARA, nUnitAddr / 2, applyCount * 2, args, struGsh.addressList.at(nUnitAddr / 2), AI_UDP_SEND_PORT);
                    data.nCommandAddress = CMD_AI_MODEL_PARA;
                    ret = MyUpd.readUdpDatagrams(&data, 13);
                    if (ret != 0)
                    {
                        qDebug("nUnitAddr/2: %d, ret: %d", nUnitAddr / 2, ret);
                    }
                }
            }
        }
    }
}

void GlobalFlow::imageInferOnOff(bool onOff)
{
    if (struCnfg.aiEnable != 1)
    {
        return;
    }
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    //开始推向推理
    if (onOff)
    {
        args[0] = 1;
        //相机板发送ai协议
        for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
        {
            if (i % 2 == 0)
            {
                MyUpd.writeDatagram(CMD_AI_IMAGE_INFER, i / 2, 1, args, struGsh.addressList.at(i / 2), AI_UDP_SEND_PORT);
                data.nCommandAddress = CMD_AI_IMAGE_INFER;
                ret = MyUpd.readUdpDatagrams(&data, 13);
                if (ret != 0)
                {
                    qDebug("aiDevice: %d, img ret: %d", i / 2, ret);
                }
            }
            MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 1, 3);
        }
    }
    else
    {
        args[0] = 0;
        //相机板发送ai协议
        for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
        {
            MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 0, 3);
            myFlow.msleep(500);
            if (i % 2 == 0)
            {
                MyUpd.writeDatagram(CMD_AI_IMAGE_INFER, i / 2, 1, args, struGsh.addressList.at(i / 2), AI_UDP_SEND_PORT);
                data.nCommandAddress = CMD_AI_IMAGE_INFER;
                ret = MyUpd.readUdpDatagrams(&data, 13);
                if (ret != 0)
                {
                    qDebug("aiDevice: %d, img ret: %d", i / 2, ret);
                }
            }
        }
    }
}


void GlobalFlow::imageSendOnOff(bool onOff)
{
    if (struCnfg.aiEnable != 1)
    {
        return;
    }
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    //开始推向推理
    if (onOff)
    {
        args[0] = 1;
        //相机板发送ai协议
        MyUpd.writeDatagram(CMD_AI_IMAGE_SEND, 0, 1, args, struGsh.addressList.at(0), AI_UDP_SEND_PORT);
        data.nCommandAddress = CMD_AI_IMAGE_SEND;
        ret = MyUpd.readUdpDatagrams(&data, 13);
        if (ret != 0)
        {
            qDebug("aiDevice: %d, img ret: %d", 0, ret);
        }
        //相机板发送ai协议
        for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
        {
            MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 1, 3);
        }
    }
    else
    {
        args[0] = 0;
        //相机板发送ai协议
        for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
        {
            MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 0, 3);
        }
        myFlow.msleep(500);

        MyUpd.writeDatagram(CMD_AI_IMAGE_INFER, 0, 1, args, struGsh.addressList.at(0), AI_UDP_SEND_PORT);
        data.nCommandAddress = CMD_AI_IMAGE_INFER;
        ret = MyUpd.readUdpDatagrams(&data, 13);
        if (ret != 0)
        {
            qDebug("aiDevice: %d, img ret: %d", 0, ret);
        }

    }
}

int  GlobalFlow::initAiCommunication()
{
    if (struCnfg.aiEnable != 1)
    {
        return 0;
    }
    int i, nAddr = 0;
    int err = 0;
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    for (i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
    {
        nAddr = struCnfg.struLevelInfo[0].nUnitId[i];
        if (nAddr % 2 == 0)
        {
            MyUpd.writeDatagram(CMD_AI_VERSION_FETCH, i / 2, 0, args, struGsh.addressList.at(i / 2), AI_UDP_SEND_PORT);
            ret = MyUpd.readUdpDatagrams(&data, 18);
            if (ret != 0)
            {
                qDebug("aiDevice: %d, img ret: %d", i / 2, ret);
                err = 1;
                break;
            }
            else
            {
                struGsh.aiResult[i / 2] = QString(data.nCommandData);
                //              qDebug()<<struGsh.aiResult[i/2];
            }
        }
    }
    return err;
}

/* 开机发送参数 */
void GlobalFlow::initSendAllParams()
{
    sendAllParamsRgb();
}

void GlobalFlow::sendAllParamsRgb()
{
    initInterfaceTransMode(0);  // 切换接口板到串口模式

    initCamera();               // 重置相机参数

    resetSortParams();          // 重置色选参数

    resetControl();             // 重置控制板参数
}
