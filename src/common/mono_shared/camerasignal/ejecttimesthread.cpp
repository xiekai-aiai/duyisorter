#include "ejecttimesthread.h"

EjectTimesThread *g_ejectTimesThread;
int pEjectTimesPerSec[MAX_EJECTOR_SIXTY*4 + 1];

/**********
 *
 * 读取吹气次数线程类
 *
 *****************************************************/
EjectTimesThread::EjectTimesThread()
{
    m_bIsEmit = false;
    m_nChuteNum = 0;
    m_nErrorTimes = 0;
    m_nUnitAddr = 0;

    memset(pEjectTimesPerSec, 0, sizeof(pEjectTimesPerSec));

    typeMachine = 1;
}

EjectTimesThread::~EjectTimesThread()
{
}

/* 读取吹气次数线程运行函数 */
void EjectTimesThread::run()
{
    int feederTotal = 0;    //默认振动器个数

    if (typeMachine == 1) {
        feederTotal = struCnfg.struLevelInfo[0].nUnitLevelTotal/2;
    }

    m_nRunFlag = true;
    while(m_nRunFlag) {
        if(struGsh.bStatFeed==1 && struGsh.isQueryingCom3==false) {
            if (typeMachine == 1) { //杂粮的智能供料
                if (struCnfp.isFeederAIEnable == 1 && struCnfp.autoFeedEnablePageFlag == 1) {
                    for (int i = 0; i < feederTotal; i++) {
                        if (!m_nRunFlag) {
                            return;
                        }
                        if(!(struGsh.bStatFeed==1))  {
                            break;
                        }
                        struGsh.nUnitFeederAI = i*2;
                        readSerialData();
                        emit readDataFinished();
                        msleep(800);
                    }
                } else {
                    sleep(1);
                }
            } else {
                readSerialData();
                checkEjectTimesLimit();
                if(m_bIsEmit) {
                    emit readDataFinished();
                }
                msleep(800);
            }
        } else {
            sleep(1);
        }
        /* 若不在吹气次数页面且没有超过吹气次数限制，则更新当前滑槽 */
        if (!m_bIsEmit && m_nErrorTimes == 0) {
            int maxChute = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2-1;
            m_nChuteNum = (m_nChuteNum < maxChute) ? (m_nChuteNum+1) : 0;
            m_nUnitAddr = (m_nUnitAddr < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal-2)?(m_nUnitAddr+2):0;
        }
    }
}

void EjectTimesThread::stopStatRun()
{
    m_nRunFlag = false;
}

/* 开始发送信号 */
void EjectTimesThread::startEmitSignal()
{
    m_bIsEmit = true;
}

/* 停止发送信号 */
void EjectTimesThread::stopEmitSignal()
{
    m_bIsEmit = false;
}

/* 检查吹气次数限制 */
void EjectTimesThread::checkEjectTimesLimit()
{
    /* 1.获取当前滑槽的通道组 */
    int nTickGroup;
    if (m_bIsEmit) {    // 吹气次数页面
        nTickGroup = getTickGroup(struGsh.nUnit/2);
    } else {            // 非吹气次数页面
        nTickGroup = getTickGroup(m_nChuteNum);
    }

    /* 2.统计前视与后视的吹气次数 */
    int nFrontTimes = getEjectTimes(SM_EJECT_MAJOR_FRONT);
    int nRearTimes = getEjectTimes(SM_EJECT_MAJOR_REAR);

    /* 3.若超过吹气次数限制，则将出错次数加一
         超过最大出错次数时，清灰处理 */
    if(nFrontTimes > struCnfp.struGroupTick[0][nTickGroup].nEjectTimesLimit*struCnfg.nEjectorsPerChute
            || nRearTimes > struCnfp.struGroupTick[0][nTickGroup].nEjectTimesLimit*struCnfg.nEjectorsPerChute) {
        m_nErrorTimes++;
        if (m_nErrorTimes >= MAX_ERROR_TIMES) {
            m_nErrorTimes = 0;
            emit startWipeSig();
            sleep(struCnfp.struGroupCtrl[0].nWipeDuration+10);
        }
    }
}

/* 根据滑槽获取当前剔除组 */
int EjectTimesThread::getTickGroup(int chute)
{
    for(int i=0; i<struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
        for(int j=0; j<struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitCount; j++) {
            if(struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitId[j] == chute*2) {
                return i;
            }
        }
    }

    return 0;
}

/* 前后视的吹气次数统计 */
int EjectTimesThread::getEjectTimes(int flag)
{
    int *p;
    switch(flag) {
    case SM_EJECT_MAJOR_FRONT:
        p = pEjectTimesPerSec;
        break;
    case SM_EJECT_MAJOR_REAR:
        p = pEjectTimesPerSec+1;
        break;
    case SM_EJECT_ASSIST_FRONT:
        p = pEjectTimesPerSec+2;
        break;
    case SM_EJECT_ASSIST_REAR:
        p = pEjectTimesPerSec+3;
        break;
    default:
        break;
    }

    int nSum = 0;
    for (int i = 0; i < MAX_EJECTOR_SIXTY; i++) {
        nSum += *(p+i*4);
    }

    return nSum;
}

/* 从串口读取吹气次数 */
void EjectTimesThread::readSerialData()
{
    /* 回包498字节=包头10+回包数据482+包尾6 */
    int nPacketSize = 496;
    int nChuteNum;

    int typeMachine = 0;//0:米，1：杂粮，2：茶叶

    /* 若在吹气次数页面，则固定当前滑槽 */
    if(m_bIsEmit) {
        m_nChuteNum = struGsh.nUnit/2;
        m_nUnitAddr = (struGsh.nUnit%2 == 0)?struGsh.nUnit : struGsh.nUnit-1;
    }

    /* 将获取的每秒吹气次数增加到全局参数中 */
    if (typeMachine == 1) {
        /* 回包258字节=包头10+回包数据242+包尾6 */
        nPacketSize = 258;
        MySerial.com1Write(CMD_INT_EJECTOR_COUNT,
                           INT,
                           struGsh.nLevel,
                           struGsh.nUnitFeederAI,
                           0,0,0,0,0,1);

    } else {
        /* 根据滑槽数设置接口板号 */
        if(m_nChuteNum < MAX_UNIT_PER_INT/2) {
            nChuteNum = m_nChuteNum;
            MySerial.com1Write(CMD_INT_EJECTOR_COUNT, INT, 0, 0, 0, 0, 0, nChuteNum, 0, 1);
        } else {
            nChuteNum = m_nChuteNum-MAX_UNIT_PER_INT/2;
            MySerial.com1Write(CMD_INT_EJECTOR_COUNT, INT, 1, 0, 0, 0, 0, nChuteNum, 0, 1);
        }
    }
    char p[nPacketSize];
    memset(p, 0, nPacketSize);

    myFlow.msleep(100);
    int nRet = MySerial.com3Read(p, nPacketSize);

    if (typeMachine == 1) {
        if (nRet == nPacketSize && p[0] == 0xAA
                && p[1] == 0xAA && p[2] == 0xAA
                && p[3] == 0xAA) {
            for(int i=0; i<MAX_EJECTOR_SIXTY*2; i++) {
                pEjectTimesPerSec[i] = p[i*2+10]*256+p[i*2+10+1];
            }
            pEjectTimesPerSec[MAX_EJECTOR_SIXTY*4] = p[nPacketSize-8]*256+p[nPacketSize-7];
        }
    } else {
        /* RS机型 */
        if (nRet == nPacketSize && p[0] == 0xAA
                && p[1] == 0xAA && p[2] == 0xAA
                && p[3] == 0xAA && p[4] == 0x01
                && p[5] == nChuteNum) {
            for (int i = 0; i < MAX_EJECTOR_SIXTY*4+1; i++) {
                pEjectTimesPerSec[i] = p[i*2+10]*256+p[i*2+10+1];
            }
        }
    }
}
