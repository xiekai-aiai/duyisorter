#include "globalflow.h"

/* 获取全局参数中的层和组信息-TD */
void GlobalFlow::getLevelInfo_TD()
{
    int i, j;

    struCnfg.nLevelTotal = 3;               // 3 层

    for (i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nViewTotal = 1;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 1;  // 剔除组数量.
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 2; //识别组数量
    }
    for (i = 0; i < struCnfg.nLevelTotal; i++) {
        if (struCnfg.struLevelInfo[i].nViewTotal == 2){
            struCnfg.struLevelInfo[i].nUnitLevelTotal = 12;
            for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
                struCnfg.struLevelInfo[i].nUnitId[j] = j;
            }
        } else {
            struCnfg.struLevelInfo[i].nUnitLevelTotal = 6;
            for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
                struCnfg.struLevelInfo[i].nUnitId[j] = j*2;
            }
        }

        if (struCnfg.struLevelInfo[i].nViewTotal == 2) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FRONT_UNIT;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = REAR_UNIT;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal/2;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal/2;
            for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount; j++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[j] = j*2;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitSplice[j] = 0;
            }
            for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount; j++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[j] = j*2+1;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitSplice[j] = 0;
            }
        } else {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = UNIT_NULL;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = UNIT_NULL;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 0;
            for (int j = 0; j < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount; j++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[j] = 2*j;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitSplice[j] = 0;
            }
            for (int j = 0; j < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount; j++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[j] = 2*j;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitSplice[j] = 0;
            }
        }

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = UNIT_NULL;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nLevelIndex = i;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal;
        if (struCnfg.struLevelInfo[i].nViewTotal == 2) {
            for(int j=0;j<struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount;j++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[j] = j;
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[j] = 0;
            }
        } else {
            for(int j=0;j<struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount;j++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[j] = j*2;
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[j] = 0;
            }
        }
    }
}

/* 获取全局参数中的层和组信息-T1 */
void GlobalFlow::getLevelInfo_T1()
{
    int i, j, k;

    struCnfg.nLevelTotal = 1;               // 单层单视

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal           = 1;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j*2+1;
        }
        struCnfg.struLevelInfo[i].nViewTotal                = 1;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal       = 1;
        struCnfg.struLevelInfo[i].nTickGroupTotal           = 1;

        /* 识别组 */
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = UNIT_NULL;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 1;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = UNIT_NULL;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 1;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 1;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 获取每层内组、通道信息  T2 */
void GlobalFlow::getLevelInfo_T2()
{
    int i, j, k;
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;               // 只有单层

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 4;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 2;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 2;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FRONT_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = REAR_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[1] = 3;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 获取每层内组、通道信息  RS */
void GlobalFlow::getLevelInfo_RS()
{
    int i, j, k;
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;
    struCnfg.nAutoFeederSet = 0;
    struCnfg.nAutoReferChute = 2;
    struCnfg.nAutoFlagDark = 0;
    struCnfg.nTickDirect = 0;
    struCnfg.nParamSetAlone = 0;
    struCnfg.nFeederCtrlMode = 0;

    for (i = 0; i < MAX_UNIT; i++) {
        struCnfg.nAssistCamEn[i] = 0;
    }

    for (i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 20;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 6;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 3;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nNameIndex = SECOND_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nNameIndex = SECOND_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nNameIndex = THIRD_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nNameIndex = THIRD_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitCount = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitCount = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[4] = 8;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[2] = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[3] = 7;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[4] = 9;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[0] = 10;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[1] = 12;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[2] = 14;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[0] = 11;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[1] = 13;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[2] = 15;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitId[0] = 16;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitId[1] = 18;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitId[0] = 17;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitId[1] = 19;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = SECOND_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nNameIndex = THIRD_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitCount = 4;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3] = 3;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[4] = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[5] = 5;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[6] = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[7] = 7;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[8] = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[9] = 9;

        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[0] = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[1] = 11;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[2] = 12;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[3] = 13;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[4] = 14;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[5] = 15;

        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[0] = 16;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[1] = 17;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[2] = 18;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[3] = 19;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 获取每层内组、通道信息  RS_MC */
void GlobalFlow::getLevelInfo_MC()
{
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;
    struCnfg.nAutoFeederSet = 0;
    struCnfg.nAutoReferChute = 2;
    struCnfg.nAutoFlagDark = 0;
    struCnfg.nTickDirect = 0;
    struCnfg.nFeederCtrlMode = 0;

    for (int i = 0; i < MAX_UNIT; i++) {
        struCnfg.nAssistCamEn[i] = 0;
    }

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 14;
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 4;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 2;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nNameIndex = SECOND_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nNameIndex = SECOND_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nNameIndex = THIRD_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nNameIndex = THIRD_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitCount = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitCount = 0;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[4] = 8;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[2] = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[3] = 7;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[4] = 9;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[0] = 10;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[1] = 12;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[0] = 11;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[1] = 13;

        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = SECOND_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nNameIndex = THIRD_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitCount = 0;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3] = 3;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[4] = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[5] = 5;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[6] = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[7] = 7;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[8] = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[9] = 9;

        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[0] = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[1] = 11;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[2] = 12;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[3] = 13;

        for (int j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 获取全局参数中的层和组信息-CF7 */
void GlobalFlow::getLevelInfo_CF()
{
    int i, j, k;
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;
    struCnfg.nParamSetAlone = 1;

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 14;
        for (j = 0; j < MAX_UNIT; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;

        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 6;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 3;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nNameIndex = SECOND_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nNameIndex = SECOND_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nNameIndex = THIRD_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nNameIndex = THIRD_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitCount = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitCount = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[2] = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[3] = 7;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[0] = 8;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[1] = 10;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[0] = 9;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[1] = 11;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitId[0] = 12;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitId[0] = 13;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = SECOND_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nNameIndex = THIRD_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3] = 3;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[4] = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[5] = 5;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[6] = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[7] = 7;

        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[0] = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[1] = 9;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[2] = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[3] = 11;

        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[0] = 12;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[1] = 13;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/*黄曲霉毒素机型*/
void GlobalFlow::getLevelInfo_AFLATOXIN()
{
    int i, j, k;
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;               // 只有单层

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 2;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 2;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;


        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 1;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}
/* 获取全局参数中的层和组信息-RSC10 */
void GlobalFlow::getLevelInfo_RSC()
{
    int i, j, k;
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;               // 只有单层

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 20;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 6;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 3;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nNameIndex = SECOND_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nNameIndex = SECOND_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nNameIndex = THIRD_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nNameIndex = THIRD_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitCount = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitCount = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[4] = 8;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[2] = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[3] = 7;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[4] = 9;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[0] = 10;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[1] = 12;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[2] = 14;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[0] = 11;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[1] = 13;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[2] = 15;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitId[0] = 16;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitId[1] = 18;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitId[0] = 17;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitId[1] = 19;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = SECOND_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nNameIndex = THIRD_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitCount = 4;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3] = 3;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[4] = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[5] = 5;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[6] = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[7] = 7;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[8] = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[9] = 9;

        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[0] = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[1] = 11;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[2] = 12;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[3] = 13;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[4] = 14;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[5] = 15;

        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[0] = 16;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[1] = 17;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[2] = 18;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[3] = 19;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/*!
 * \brief 获取每层内组、通道信息  RSC_MC
 */
void GlobalFlow::getLevelInfo_MCC()
{
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 14;
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 6;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 3;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nNameIndex = SECOND_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nNameIndex = SECOND_REAR;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nNameIndex = THIRD_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nNameIndex = THIRD_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitCount = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitCount = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[2] = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[3] = 7;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[0] = 8;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitId[1] = 10;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[0] = 9;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitId[1] = 11;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitId[0] = 12;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitId[0] = 13;

        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = SECOND_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nNameIndex = THIRD_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3] = 3;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[4] = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[5] = 5;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[6] = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[7] = 7;

        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[0] = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[1] = 9;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[2] = 10;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[3] = 11;

        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[0] = 12;
        struCnfg.struLevelInfo[i].struTickGroupInfo[2].nUnitId[1] = 13;

        for (int j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 获取全局参数中的层和组信息-LD2 */
void GlobalFlow::getLevelInfo_LD2()
{
    int i, j, k;
    struCnfg.nLevelTotal = 1;   // 只有单层

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal           = 8;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;//双视
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 4;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_UPPER;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_DOWN;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nNameIndex = SECOND_UPPER;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nNameIndex = SECOND_DOWN;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitCount = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitCount = 0;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[2] = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[3] = 7;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = SECOND_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 0;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3] = 3;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[4] = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[5] = 5;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[6] = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[7] = 7;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

void GlobalFlow::getLevelInfo_LD4()
{
    int i, j, k;

    /*此机型相当于CF5+CF4*/
    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nLevelTotal = 1;               // 只有单层

    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nNameIndex = FIRST_UP_MASTER;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nNameIndex = FIRST_DOWN_MASTER;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nNameIndex = FIRST_UP_SLAVE;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nNameIndex = FIRST_DOWN_SLAVE;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[4].nNameIndex = SECOND_UP_MASTER;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[5].nNameIndex = SECOND_DOWN_MASTER;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[6].nNameIndex = SECOND_UP_SLAVE;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[7].nNameIndex = SECOND_DOWN_SLAVE;

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 18;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }

        struCnfg.struLevelInfo[i].nViewTotal                = QUADRUPLE_VIEW;
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal       = 4;
        struCnfg.struLevelInfo[i].nTickGroupTotal           = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[2].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[3].nUnitCount = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[4].nUnitCount = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[5].nUnitCount = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[6].nUnitCount = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[7].nUnitCount = 0;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[4] = 8;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[2] = 5;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[3] = 7;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[4] = 9;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FIRST_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = SECOND_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount  = 18;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount  = 0;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0]  = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1]  = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2]  = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3]  = 3;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[4]  = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[5]  = 5;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[6]  = 6;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[7]  = 7;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[8]  = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[9]  = 9;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 获取全局参数中的层和组信息-LDS */
void GlobalFlow::getLevelInfo_LDS()
{
    int i, j, k;
    struCnfg.nLevelTotal = 2;       // 双层

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 4;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j*2;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 1;//单视
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 1;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = UNIT_NULL;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 4;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[3] = 6;

        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = UNIT_NULL;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 8;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[2] = 4;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[3] = 6;

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 获取全局参数中的层和组信息-LDS2 */
void GlobalFlow::getLevelInfo_LDS2()
{
    int i, j, k;
    struCnfg.nLevelTotal = 2;       // 双层
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nNameIndex = FIRST_UP_FRONT_ML;  //一次上前
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nNameIndex = FIRST_UP_REAR_ML;   //一次上后
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nNameIndex = SECOND_UP_FRONT_ML; //二次上前
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nNameIndex = SECOND_UP_REAR_ML;  //二次上后
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[0].nNameIndex = FIRST_DOWN_FRONT_ML;//一次下前
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[1].nNameIndex = FIRST_DOWN_REAR_ML; //一次下后
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[2].nNameIndex = SECOND_DOWN_FRONT_ML;   //二次下前
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[3].nNameIndex = SECOND_DOWN_REAR_ML;//二次下后

    struCnfg.struLevelInfo[0].struTickGroupInfo[0].nNameIndex = FIRST_UP_UNIT;
    struCnfg.struLevelInfo[0].struTickGroupInfo[1].nNameIndex = SECOND_UP_UNIT;
    struCnfg.struLevelInfo[1].struTickGroupInfo[0].nNameIndex = FIRST_DOWN_UNIT;
    struCnfg.struLevelInfo[1].struTickGroupInfo[1].nNameIndex = SECOND_DOWN_UNIT;

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 12;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2;//双视
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = 2;
        struCnfg.struLevelInfo[i].nTickGroupTotal = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 6;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 6;

        for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount; k++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[k] = k*2;
        }
        for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount; k++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[k] = k*2+1;
        }
        for(j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 12;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 0;
        for(j = 0; j < struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[j] = j;
        }

        for(j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/* 从全局配置文件获取全局参数 */
bool GlobalFlow::getGlobalSetting(const QString & fileName)
{
    /* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);

    int sizeTmp   = 0;          //配置文件中参数的长度,该变量主要用于memcpy
    const char *strTmp  = NULL;       //配置文件中参数的字符串,该变量主要用于memcpy

    /* get parameters from setting file */
    /* language */
    struCnfg.nLang = setting.value(QString("%1-%2").arg(1).arg(0), struCnfg.nLang).toInt();

    /* system run time */
    struCnfg.nCounter = setting.value(QString("%1-%2").arg(1).arg(1), struCnfg.nCounter).toInt();

    /* profile index */
    struCnfg.nProfile = setting.value(QString("%1-%2").arg(1).arg(2), struCnfg.nProfile).toInt();

    /* profile total */
    struCnfg.nProfileTotal = setting.value(QString("%1-%2").arg(1).arg(3), struCnfg.nProfileTotal).toInt();

    //    sizeTmp = strlen(strTmp);
    //    if (sizeTmp > sizeof(struCnfg.sBacklightPassword)-1) {
    //        sizeTmp = sizeof(struCnfg.sBacklightPassword)-1;    //以'\0'结尾
    //    }
    //    memcpy(struCnfg.sBacklightPassword,strTmp,sizeTmp);

    /* profile details */
    for (int i = 0; i < struCnfg.nProfileTotal; i++) {
        //sProfileName
        strTmp = setting.value(QString("%1-%2-%3-%4").arg(1).arg(4).arg(i).arg(0), struCnfg.struProfileIndex[i].sProfileName).toString().toLocal8Bit().constData();
        sizeTmp = strlen(strTmp);
        if (sizeTmp > sizeof(struCnfg.struProfileIndex[i].sProfileName)-1) {
            sizeTmp = sizeof(struCnfg.struProfileIndex[i].sProfileName)-1;    //以'\0'结尾
        }
        memcpy(struCnfg.struProfileIndex[i].sProfileName,strTmp,sizeTmp);
//        strcpy(struCnfg.struProfileIndex[i].sProfileName, strTmp);

        //sMaterialName
        strTmp = setting.value(QString("%1-%2-%3-%4").arg(1).arg(4).arg(i).arg(1), struCnfg.struProfileIndex[i].sMaterialName).toString().toLocal8Bit().constData();
        sizeTmp = strlen(strTmp);
        if (sizeTmp > sizeof(struCnfg.struProfileIndex[i].sMaterialName)-1) {
            sizeTmp = sizeof(struCnfg.struProfileIndex[i].sMaterialName)-1;    //以'\0'结尾
        }
        memcpy(struCnfg.struProfileIndex[i].sProfileName,strTmp,sizeTmp);

//        strcpy(struCnfg.struProfileIndex[i].sMaterialName, strTmp);
    }

    /* level total */
    struCnfg.nLevelTotal = setting.value(QString("%1-%2").arg(1).arg(5), struCnfg.nLevelTotal).toInt();

    /* level details */
    for (int i = 0; i < MAX_LEVEL; i++) {
        /* total unit per level */
        struCnfg.struLevelInfo[i].nUnitLevelTotal = setting.value(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(0), struCnfg.struLevelInfo[i].nUnitLevelTotal).toInt();

        /* unit id */
        for (int j = 0; j < MAX_UNIT; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(1).arg(6).arg(i).arg(1).arg(j), struCnfg.struLevelInfo[i].nUnitId[j]).toInt();
        }

        /* view total */
        struCnfg.struLevelInfo[i].nViewTotal = setting.value(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(2), struCnfg.struLevelInfo[i].nViewTotal).toInt();
        /* identify group total */
        struCnfg.struLevelInfo[i].nIdentifyGroupTotal = setting.value(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(4), struCnfg.struLevelInfo[i].nIdentifyGroupTotal).toInt();

        /* tick group total */
        struCnfg.struLevelInfo[i].nTickGroupTotal = setting.value(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(5), struCnfg.struLevelInfo[i].nTickGroupTotal).toInt();

        /* identify group details */
        for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++) {
            /* level index */
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(6).arg(j).arg(0), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex).toInt();
            /* name index */
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nNameIndex  = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(6).arg(j).arg(1), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nNameIndex).toInt();
            /* unit count */
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount  = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(6).arg(j).arg(2), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount).toInt();

            /* unit id */
            for (int k = 0; k < MAX_UNIT; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[k] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(6).arg(j).arg(3).arg(k), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[k]).toInt();
            }

            /* unit splice */
            for (int k = 0; k < MAX_UNIT; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(6).arg(j).arg(4).arg(k), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k]).toInt();
            }
        }

        /* tick group details */
        for (int j = 0; j < MAX_GROUP_TICK; j++) {
            /* level index */
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(7).arg(j).arg(0), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex).toInt();
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nNameIndex  = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(7).arg(j).arg(1), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nNameIndex).toInt();
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount  = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(7).arg(j).arg(2), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount).toInt();

            for (int k = 0; k < MAX_UNIT; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(7).arg(j).arg(3).arg(k), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k]).toInt();
            }

            for (int k = 0; k < MAX_UNIT; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(7).arg(j).arg(4).arg(k), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k]).toInt();
            }
        }
    }

    /* interface board */
    struCnfg.nInterfaceBoardTotal = setting.value(QString("%1-%2").arg(1).arg(7), struCnfg.nInterfaceBoardTotal).toInt();

    /* control board */
    struCnfg.nCtrlBoardTotal = setting.value(QString("%1-%2").arg(1).arg(8), struCnfg.nCtrlBoardTotal).toInt();

    /* ejector per chute */
    struCnfg.nEjectorsPerChute = setting.value(QString("%1-%2").arg(1).arg(9), struCnfg.nEjectorsPerChute).toInt();

    /* ejector counts protect */
    struCnfg.nEjectCountProtect = setting.value(QString("%1-%2").arg(1).arg(10), struCnfg.nEjectCountProtect).toInt();

    /* lamp total */
    struCnfg.nLampTotal = setting.value(QString("%1-%2").arg(1).arg(11), struCnfg.nLampTotal).toInt();

    /* delay code */
    //sDelayCode
    strTmp = setting.value(QString("%1-%2").arg(1).arg(12), struCnfg.sDelayCode).toString().toLocal8Bit().constData();
    sizeTmp = strlen(strTmp);
    if (sizeTmp > sizeof(struCnfg.sDelayCode)-1) {
        sizeTmp = sizeof(struCnfg.sDelayCode)-1;    //以'\0'结尾
    }
    memcpy(struCnfg.sDelayCode,strTmp,sizeTmp);
    //sDelayCode


    /* enable backlight password */
    struCnfg.nEnableBacklightPassword = setting.value(QString("%1-%2").arg(1).arg(13), struCnfg.nEnableBacklightPassword).toInt();

    /* backlight password */
    //sBacklightPassword
    strTmp = setting.value(QString("%1-%2").arg(1).arg(14), struCnfg.sBacklightPassword).toString().toLocal8Bit().constData();
    sizeTmp = strlen(strTmp);
    if (sizeTmp > sizeof(struCnfg.sBacklightPassword)-1) {
        sizeTmp = sizeof(struCnfg.sBacklightPassword)-1;    //以'\0'结尾
    }
    memcpy(struCnfg.sBacklightPassword,strTmp,sizeTmp);
    //sBacklightPassword

    /* 辅助相机使能 */
    for(int i=0; i<MAX_UNIT; i++) {
        struCnfg.nAssistCamEn[i] = setting.value(QString("%1-%2-%3").arg(1).arg(15).arg(i),
                                                        struCnfg.nAssistCamEn[i]).toInt();
    }

    /* 一键自动设置 */
    struCnfg.nAutoFeederSet = setting.value(QString("%1-%2").arg(1).arg(16), struCnfg.nAutoFeederSet).toInt();
    struCnfg.nAutoReferChute = setting.value(QString("%1-%2").arg(1).arg(17), struCnfg.nAutoReferChute).toInt();
    struCnfg.nAutoFlagDark = setting.value(QString("%1-%2").arg(1).arg(18), struCnfg.nAutoFlagDark).toInt();

    /* 剔除与识别组显示的次序 */
    struCnfg.nTickDirect = setting.value(QString("%1-%2").arg(1).arg(22), struCnfg.nTickDirect).toInt();

    /* 参数设置 */
    struCnfg.nParamSetAlone = setting.value(QString("%1-%2").arg(1).arg(23), struCnfg.nParamSetAlone).toInt();

    /* 方案模式 */
    for(int i=0; i<MAX_PROFILE; i++) {
        struCnfg.nProfileMode[i] = setting.value(QString("%1-%2-%3").arg(1).arg(24).arg(i),
                                                    struCnfg.nProfileMode[i]).toInt();
    }
    /* 料位监控使能 */
    struCnfg.nFeederCtrlEn = setting.value(QString("%1-%2").arg(1).arg(25),
                                                    struCnfg.nFeederCtrlEn).toInt();

    /* 设置振动器高低电压 */
    struCnfg.nFeederVoltage = setting.value(QString("%1-%2").arg(1).arg(26),
                                                    struCnfg.nFeederVoltage).toInt();
    /* 料位监控模式 */
    struCnfg.nFeederCtrlMode = setting.value(QString("%1-%2").arg(1).arg(27),
                                                    struCnfg.nFeederCtrlMode).toInt();

    /* 串口波特率设置 */
    for(int i = 0; i < MAX_UART; i++) {
        struCnfg.uartBaud[i] = setting.value(QString("%1-%2-%3").arg(1).arg(28).arg(i),
                                                    struCnfg.uartBaud[i]).toInt();
    }

    /* PMT相关参数设置 */
    struCnfg.nPMTRun = setting.value(QString("%1-%2").arg(1).arg(29),
                                                    struCnfg.nPMTRun).toInt();
    struCnfg.nEnablePMTR1 = setting.value(QString("%1-%2").arg(1).arg(30),
                                                    struCnfg.nEnablePMTR1).toInt();
    struCnfg.nEnablePMTR2 = setting.value(QString("%1-%2").arg(1).arg(31),
                                                    struCnfg.nEnablePMTR2).toInt();
    struCnfg.nEnablePMTG = setting.value(QString("%1-%2").arg(1).arg(32),
                                                    struCnfg.nEnablePMTG).toInt();
    struCnfg.nEnablePMTB = setting.value(QString("%1-%2").arg(1).arg(33),
                                                    struCnfg.nEnablePMTB).toInt();
    struCnfg.nPMTNum = setting.value(QString("%1-%2").arg(1).arg(34),
                                                    struCnfg.nPMTNum).toInt();

    /* 振动器外围控制 */
    struCnfg.nFeederRemoteCtrlMode = setting.value(QString("%1-%2").arg(1).arg(35),
                                                    struCnfg.nFeederRemoteCtrlMode).toInt();

    /* 料位检测等待时间 */
    struCnfg.nFeederCtrlWaitTime = setting.value(QString("%1-%2").arg(1).arg(36),
                                                    struCnfg.nFeederCtrlWaitTime).toInt();

    /* 加密提醒使能标志 */
    struCnfg.nEnableDelayCodeInfo = setting.value(QString("%1-%2").arg(1).arg(37),
                                                  struCnfg.nEnableDelayCodeInfo).toInt();

    /* 智能灯控使能标志 */
    struCnfg.nEnableLampAI = setting.value(QString("%1-%2").arg(1).arg(38),
                                                  struCnfg.nEnableLampAI).toInt();
    //! 一键自动使能
    struCnfg.nAutoSigEnable = setting.value(QString("%1-%2").arg(1).arg(39),
                                                  struCnfg.nAutoSigEnable).toInt();

    // 使能大数据
    struCnfg.nStatisticEnable = setting.value(QString("%1-%2").arg(1).arg(40),
                                                  struCnfg.nStatisticEnable).toInt();

    /* machineinfo */
    //MachineInfo
    strTmp = setting.value(QString("%1-%2").arg(1).arg(41),struCnfg.MachineInfo).toString().toLocal8Bit().constData();
    sizeTmp = strlen(strTmp);
    if (sizeTmp > sizeof(struCnfg.MachineInfo)-1) {
        sizeTmp = sizeof(struCnfg.MachineInfo)-1;    //以'\0'结尾
    }
    memcpy(struCnfg.MachineInfo,strTmp,sizeTmp);
    //MachineInfo

    /* engineerinfo */
    //EngineerInfo
    strTmp = setting.value(QString("%1-%2").arg(1).arg(42),struCnfg.EngineerInfo).toString().toLocal8Bit().constData();
    sizeTmp = strlen(strTmp);
    if (sizeTmp > sizeof(struCnfg.EngineerInfo)-1) {
        sizeTmp = sizeof(struCnfg.EngineerInfo)-1;    //以'\0'结尾
    }
    memcpy(struCnfg.EngineerInfo,strTmp,sizeTmp);
    //EngineerInfo

    /* machine_IDinfo */
    //MachineIDInfo
    strTmp = setting.value(QString("%1-%2").arg(1).arg(43),struCnfg.MachineIDInfo).toString().toLocal8Bit().constData();
    sizeTmp = strlen(strTmp);
    if (sizeTmp > sizeof(struCnfg.MachineIDInfo)-1) {
        sizeTmp = sizeof(struCnfg.MachineIDInfo)-1;    //以'\0'结尾
    }
    memcpy(struCnfg.MachineIDInfo,strTmp,sizeTmp);
    //MachineIDInfo

    //! 清灰模式
    struCnfg.nWipeMode = setting.value(QString("%1-%2").arg(1).arg(44), struCnfg.nWipeMode).toInt();

    //! 累计加工时间
    struCnfg.nFeedCounter = setting.value(QString("%1-%2").arg(1).arg(45), struCnfg.nFeedCounter).toInt();

    //! 料位监控分次使能
    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        struCnfg.nFeederCtrlTickEn[i] = setting.value(QString("%1-%2-%3").arg(1).arg(46).arg(i), struCnfg.nFeederCtrlTickEn[i]).toInt();
    }

    //! 温度临界阈值
    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        struCnfg.nTemperatureThreshold = setting.value(QString("%1-%2").arg(1).arg(47), struCnfg.nTemperatureThreshold).toInt();
    }

    //! 报警项使能
    for(int i = 0; i < MAX_ALARM; i++) {
        struCnfg.nAlarmEnable[i] = setting.value(QString("%1-%2-%3").arg(1).arg(48).arg(i),struCnfg.nAlarmEnable[i]).toInt();
    }

    //! 智能设置
    struCnfg.nEnableAI = setting.value(QString("%1-%2").arg(1).arg(49), struCnfg.nEnableAI).toInt();
    struCnfg.nEnableTradition = setting.value(QString("%1-%2").arg(1).arg(50), struCnfg.nEnableTradition).toInt();
    struCnfg.nMaterPer1 = setting.value(QString("%1-%2").arg(1).arg(51),struCnfg.nMaterPer1).toInt();
    struCnfg.nMaterPer2 = setting.value(QString("%1-%2").arg(1).arg(52),struCnfg.nMaterPer2).toInt();
    struCnfg.nMaterPer3 = setting.value(QString("%1-%2").arg(1).arg(53),struCnfg.nMaterPer3).toInt();
    struCnfg.nReservedSens = setting.value(QString("%1-%2").arg(1).arg(54),struCnfg.nReservedSens).toInt();

	//! 大数据采集对象备份
    struCnfg.nCurCapLevel = setting.value(QString("%1-%2").arg(1).arg(55),struCnfg.nCurCapLevel).toInt();
    struCnfg.nCurCapUnit = setting.value(QString("%1-%2").arg(1).arg(56),struCnfg.nCurCapUnit).toInt();

    struCnfg.nLedMode = setting.value(QString("%1-%2").arg(1).arg(57),struCnfg.nLedMode).toInt();
    struCnfg.nRunMode = setting.value(QString("%1-%2").arg(1).arg(58),struCnfg.nRunMode).toInt();

    struCnfg.bAssitCameraEnable = setting.value(QString("%1-%2").arg(1).arg(59),struCnfg.bAssitCameraEnable).toInt();

    //!产量计算拟合公式中系数a、b
    struCnfg.aFactor = setting.value(QString("%1-%2").arg(1).arg(60),struCnfg.aFactor).toDouble();
    struCnfg.bFactor = setting.value(QString("%1-%2").arg(1).arg(61),struCnfg.bFactor).toDouble();

    //累计气压报警次数
    struCnfg.nPressureAlarmCount = setting.value(QString("%1-%2").arg(1).arg(62),struCnfg.nPressureAlarmCount).toInt();

    //累计产量
    for(int i=0; i<MAX_UNIT/2; i++) {
        struCnfg.nThroughtPut[i] = setting.value(QString("%1-%2-%3").arg(1).arg(68).arg(i),
                                                        struCnfg.nThroughtPut[i]).toULongLong();
    }

    struCnfg.aiEnable = setting.value(QString("%1-%2").arg(1).arg(63), struCnfg.aiEnable).toInt();
   struCnfg.imgFetchHeight = setting.value(QString("%1-%2").arg(1).arg(64), struCnfg.imgFetchHeight).toInt();
   struCnfg.imgInferHeight = setting.value(QString("%1-%2").arg(1).arg(65), struCnfg.imgInferHeight).toInt();
   struCnfg.imgPicHeight = setting.value(QString("%1-%2").arg(1).arg(66), struCnfg.imgPicHeight).toInt();
   struCnfg.imgVideoHeight = setting.value(QString("%1-%2").arg(1).arg(67), struCnfg.imgVideoHeight).toInt();

    /* return value */
    if (setting.status() == QSettings::NoError)
        return true;
    else
        return false;

}

/* 保存全局参数至全局配置文件 */
bool GlobalFlow::saveGlobalSetting(const QString & fileName)
{
    /* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);

/* save parameters */
    /* language */
    if(struCnfg.nLang != _t_struCnfg.nLang) {
        setting.setValue(QString("%1-%2").arg(1).arg(0), struCnfg.nLang);
    }

    /* system total run */
    if(struCnfg.nCounter != _t_struCnfg.nCounter) {
        setting.setValue(QString("%1-%2").arg(1).arg(1), struCnfg.nCounter);
    }

    /* profile index */
    if(struCnfg.nProfile != _t_struCnfg.nProfile) {
        setting.setValue(QString("%1-%2").arg(1).arg(2), struCnfg.nProfile);
    }

    /* profile total */
    if(struCnfg.nProfileTotal != _t_struCnfg.nProfileTotal) {
        setting.setValue(QString("%1-%2").arg(1).arg(3), struCnfg.nProfileTotal);
    }

    /* profile details */
    for (int i = 0; i < struCnfg.nProfileTotal; i++) {
        if (memcmp(struCnfg.struProfileIndex[i].sProfileName, _t_struCnfg.struProfileIndex[i].sProfileName, sizeof(struCnfg.struProfileIndex[i].sProfileName)) != 0) {
            setting.setValue(QString("%1-%2-%3-%4").arg(1).arg(4).arg(i).arg(0), struCnfg.struProfileIndex[i].sProfileName);
        }
        if (memcmp(struCnfg.struProfileIndex[i].sMaterialName, _t_struCnfg.struProfileIndex[i].sMaterialName, sizeof(struCnfg.struProfileIndex[i].sMaterialName)) != 0) {
            setting.setValue(QString("%1-%2-%3-%4").arg(1).arg(4).arg(i).arg(1), struCnfg.struProfileIndex[i].sMaterialName);
        }

    }

    /* level total */
    if(struCnfg.nLevelTotal != _t_struCnfg.nLevelTotal) {
        setting.setValue(QString("%1-%2").arg(1).arg(5), struCnfg.nLevelTotal);
    }

    /* level details */
    for (int i = 0; i < MAX_LEVEL; i++) {
        /* total unit per level */
        if(struCnfg.struLevelInfo[i].nUnitLevelTotal != _t_struCnfg.struLevelInfo[i].nUnitLevelTotal) {
            setting.setValue(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(0), struCnfg.struLevelInfo[i].nUnitLevelTotal);
        }

        /* unit id */
        for (int j = 0; j < MAX_UNIT; j++) {
                if(struCnfg.struLevelInfo[i].nUnitId[j] != _t_struCnfg.struLevelInfo[i].nUnitId[j]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5").arg(1).arg(6).arg(i).arg(1).arg(j), struCnfg.struLevelInfo[i].nUnitId[j]);
                }
        }

        /* view total */
            if(struCnfg.struLevelInfo[i].nViewTotal != _t_struCnfg.struLevelInfo[i].nViewTotal) {
                setting.setValue(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(2), struCnfg.struLevelInfo[i].nViewTotal);
            }

        /* identify group total */
            if(struCnfg.struLevelInfo[i].nIdentifyGroupTotal != _t_struCnfg.struLevelInfo[i].nIdentifyGroupTotal) {
                setting.setValue(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(4), struCnfg.struLevelInfo[i].nIdentifyGroupTotal);
            }

        /* tick group total */
            if(struCnfg.struLevelInfo[i].nTickGroupTotal != _t_struCnfg.struLevelInfo[i].nTickGroupTotal) {
                setting.setValue(QString("%1-%2-%3-%4").arg(1).arg(6).arg(i).arg(5), struCnfg.struLevelInfo[i].nTickGroupTotal);
            }

        /* identify group details */
        for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++) {
            if(struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex != _t_struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex) {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(6).arg(j).arg(0), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex);
            }
            if(struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nNameIndex != _t_struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nNameIndex) {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(6).arg(j).arg(1),  struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nNameIndex);
            }
            if(struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount != _t_struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount) {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(6).arg(j).arg(2),  struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount);
            }

            for (int k = 0; k < MAX_UNIT; k++) {
                if(struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[k] != _t_struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(6).arg(j).arg(3).arg(k), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[k]);
                }
            }

            for (int k = 0; k < MAX_UNIT; k++) {
                if(struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] != _t_struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(6).arg(j).arg(4).arg(k), struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k]);
                }
            }
        }

        /* tick group details */
        for (int j = 0; j < MAX_GROUP_TICK; j++) {
            if(struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex != _t_struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex) {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(7).arg(j).arg(0), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex);
            }
            if(struCnfg.struLevelInfo[i].struTickGroupInfo[j].nNameIndex != _t_struCnfg.struLevelInfo[i].struTickGroupInfo[j].nNameIndex) {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(7).arg(j).arg(1),  struCnfg.struLevelInfo[i].struTickGroupInfo[j].nNameIndex);
            }
            if(struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount != _t_struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount) {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(1).arg(6).arg(i).arg(7).arg(j).arg(2),  struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount);
            }

            for (int k = 0; k < MAX_UNIT; k++) {
                if(struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k] != _t_struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(7).arg(j).arg(3).arg(k), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k]);
                }
            }

            for (int k = 0; k < MAX_UNIT; k++) {
                if(struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] != _t_struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(1).arg(6).arg(i).arg(7).arg(j).arg(4).arg(k), struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k]);
                }
            }
        }
    }

    /* interface borad */
    if(struCnfg.nInterfaceBoardTotal != _t_struCnfg.nInterfaceBoardTotal) {
        setting.setValue(QString("%1-%2").arg(1).arg(7), struCnfg.nInterfaceBoardTotal);
    }

    /* control board */
    if(struCnfg.nCtrlBoardTotal != _t_struCnfg.nCtrlBoardTotal) {
        setting.setValue(QString("%1-%2").arg(1).arg(8), struCnfg.nCtrlBoardTotal);
    }

    /* ejector per chute */
    if(struCnfg.nEjectorsPerChute != _t_struCnfg.nEjectorsPerChute) {
        setting.setValue(QString("%1-%2").arg(1).arg(9), struCnfg.nEjectorsPerChute);
    }

    /* ejector counts protect */
    if(struCnfg.nEjectCountProtect != _t_struCnfg.nEjectCountProtect) {
        setting.setValue(QString("%1-%2").arg(1).arg(10), struCnfg.nEjectCountProtect);
    }

    /* lamp total */
    if(struCnfg.nLampTotal != _t_struCnfg.nLampTotal) {
        setting.setValue(QString("%1-%2").arg(1).arg(11), struCnfg.nLampTotal);
    }

    /* delay code */
    if (memcmp(struCnfg.sDelayCode, _t_struCnfg.sDelayCode, sizeof(struCnfg.sDelayCode)) != 0) {
        setting.setValue(QString("%1-%2").arg(1).arg(12), struCnfg.sDelayCode);
    }

    /* enable backlight password */
    if(struCnfg.nEnableBacklightPassword != _t_struCnfg.nEnableBacklightPassword) {
        setting.setValue(QString("%1-%2").arg(1).arg(13), struCnfg.nEnableBacklightPassword);
    }

    /* backlight password */
    if (memcmp(struCnfg.sBacklightPassword, _t_struCnfg.sBacklightPassword, sizeof(struCnfg.sBacklightPassword)) != 0) {
        setting.setValue(QString("%1-%2").arg(1).arg(14), struCnfg.sBacklightPassword);
    }

    /* 辅助相机使能 */
    for(int i=0; i<MAX_UNIT; i++) {
        if(struCnfg.nAssistCamEn[i] != _t_struCnfg.nAssistCamEn[i]) {
            setting.setValue(QString("%1-%2-%3").arg(1).arg(15).arg(i), struCnfg.nAssistCamEn[i]);
        }
    }

    /* 一键自动设置 */
    if(struCnfg.nAutoFeederSet != _t_struCnfg.nAutoFeederSet) {
        setting.setValue(QString("%1-%2").arg(1).arg(16), struCnfg.nAutoFeederSet);
    }
    if(struCnfg.nAutoReferChute != _t_struCnfg.nAutoReferChute) {
        setting.setValue(QString("%1-%2").arg(1).arg(17), struCnfg.nAutoReferChute);
    }
    if(struCnfg.nAutoFlagDark != _t_struCnfg.nAutoFlagDark) {
        setting.setValue(QString("%1-%2").arg(1).arg(18), struCnfg.nAutoFlagDark);
    }

    /* 剔除与识别组显示的次序 */
    if(struCnfg.nTickDirect != _t_struCnfg.nTickDirect) {
        setting.setValue(QString("%1-%2").arg(1).arg(22), struCnfg.nTickDirect);
    }

    /* 参数设置 */
    if(struCnfg.nParamSetAlone != _t_struCnfg.nParamSetAlone) {
        setting.setValue(QString("%1-%2").arg(1).arg(23), struCnfg.nParamSetAlone);
    }

    /* 方案模式 */
    for(int i=0; i<MAX_PROFILE; i++) {
        if(struCnfg.nProfileMode[i] != _t_struCnfg.nProfileMode[i]) {
            setting.setValue(QString("%1-%2-%3").arg(1).arg(24).arg(i), struCnfg.nProfileMode[i]);
        }
    }

    /* 料位监控使能 */
    if(struCnfg.nFeederCtrlEn != _t_struCnfg.nFeederCtrlEn) {
        setting.setValue(QString("%1-%2").arg(1).arg(25), struCnfg.nFeederCtrlEn);
        myLog->info(LOG_FEED,"feeder control enable status: %d",struCnfg.nFeederCtrlEn);
    }

    /* 设置振动器高低电压 */
    if(struCnfg.nFeederVoltage != _t_struCnfg.nFeederVoltage) {
        setting.setValue(QString("%1-%2").arg(1).arg(26), struCnfg.nFeederVoltage);
    }

    /* 料位监控模式 */
    if(struCnfg.nFeederCtrlMode != _t_struCnfg.nFeederCtrlMode) {
        setting.setValue(QString("%1-%2").arg(1).arg(27), struCnfg.nFeederCtrlMode);
        myLog->info(LOG_FEED,"feeder control mode: %d",struCnfg.nFeederCtrlEn);
    }

    /* 串口波特率设置 */
    for(int i = 0; i<MAX_UART; i++) {
        if(struCnfg.uartBaud[i] != _t_struCnfg.uartBaud[i]) {
            setting.setValue(QString("%1-%2-%3").arg(1).arg(28).arg(i), struCnfg.uartBaud[i]);
        }
    }

    /**PMT相关*/
    if(struCnfg.nPMTRun != _t_struCnfg.nPMTRun) {
        setting.setValue(QString("%1-%2").arg(1).arg(29), struCnfg.nPMTRun);
    }
    if(struCnfg.nEnablePMTR1 != _t_struCnfg.nEnablePMTR1) {
        setting.setValue(QString("%1-%2").arg(1).arg(30), struCnfg.nEnablePMTR1);
    }
    if(struCnfg.nEnablePMTR2 != _t_struCnfg.nEnablePMTR2) {
        setting.setValue(QString("%1-%2").arg(1).arg(31), struCnfg.nEnablePMTR2);
    }
    if(struCnfg.nEnablePMTG != _t_struCnfg.nEnablePMTG) {
        setting.setValue(QString("%1-%2").arg(1).arg(32), struCnfg.nEnablePMTG);
    }
    if(struCnfg.nEnablePMTB != _t_struCnfg.nEnablePMTB) {
        setting.setValue(QString("%1-%2").arg(1).arg(33), struCnfg.nEnablePMTB);
    }
    if(struCnfg.nPMTNum != _t_struCnfg.nPMTNum) {
        setting.setValue(QString("%1-%2").arg(1).arg(34), struCnfg.nPMTNum);
    }

    /* 料位检测等待时间 */
    if(struCnfg.nFeederRemoteCtrlMode != _t_struCnfg.nFeederRemoteCtrlMode) {
        setting.setValue(QString("%1-%2").arg(1).arg(35), struCnfg.nFeederRemoteCtrlMode);
    }

    /* 料位检测等待时间 */
    if(struCnfg.nFeederCtrlWaitTime != _t_struCnfg.nFeederCtrlWaitTime) {
        setting.setValue(QString("%1-%2").arg(1).arg(36), struCnfg.nFeederCtrlWaitTime);
    }

    /* 加密提醒使能标志 */
    if(struCnfg.nEnableDelayCodeInfo != _t_struCnfg.nEnableDelayCodeInfo) {
        setting.setValue(QString("%1-%2").arg(1).arg(37), struCnfg.nEnableDelayCodeInfo);
    }

    /* 使能智能灯控 */
    if(struCnfg.nEnableLampAI != _t_struCnfg.nEnableLampAI) {
        setting.setValue(QString("%1-%2").arg(1).arg(38), struCnfg.nEnableLampAI);
    }

    //! 一键自动使能
    if (struCnfg.nAutoSigEnable != _t_struCnfg.nAutoSigEnable) {
      setting.setValue(QString("%1-%2").arg(1).arg(39), struCnfg.nAutoSigEnable);
    }

    // 使能大数据
    if (struCnfg.nStatisticEnable != _t_struCnfg.nStatisticEnable) {
      setting.setValue(QString("%1-%2").arg(1).arg(40), struCnfg.nStatisticEnable);
    }

    /* machineinfo */
    setting.setValue(QString("%1-%2").arg(1).arg(41), struCnfg.MachineInfo);

    /* engineerinfo */
    setting.setValue(QString("%1-%2").arg(1).arg(42), struCnfg.EngineerInfo);

    /* machine_IDinfo */
    setting.setValue(QString("%1-%2").arg(1).arg(43), struCnfg.MachineIDInfo);

    //! 清灰模式
    if (struCnfg.nWipeMode != _t_struCnfg.nWipeMode) {
        setting.setValue(QString("%1-%2").arg(1).arg(44), struCnfg.nWipeMode);
    }

    //! 累计加工时间
    if (struCnfg.nFeedCounter != _t_struCnfg.nFeedCounter) {
        setting.setValue(QString("%1-%2").arg(1).arg(45), struCnfg.nFeedCounter);
    }

    //! 料位监控分次使能
    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        if (struCnfg.nFeederCtrlTickEn[i] != _t_struCnfg.nFeederCtrlTickEn[i]) {
            setting.setValue(QString("%1-%2-%3").arg(1).arg(46).arg(i), struCnfg.nFeederCtrlTickEn[i]);
        }
    }

    //! 温度临界阈值
    if (struCnfg.nTemperatureThreshold != _t_struCnfg.nTemperatureThreshold) {
        setting.setValue(QString("%1-%2").arg(1).arg(47), struCnfg.nTemperatureThreshold);
    }

    //! 报警项使能
    for (int i = 0; i < MAX_ALARM; i++) {
        if (struCnfg.nAlarmEnable[i] != _t_struCnfg.nAlarmEnable[i]) {
            setting.setValue(QString("%1-%2-%3").arg(1).arg(48).arg(i),struCnfg.nAlarmEnable[i]);
        }
    }

    //! 智能设置
    if (struCnfg.nEnableAI != _t_struCnfg.nEnableAI) {
        setting.setValue(QString("%1-%2").arg(1).arg(49),struCnfg.nEnableAI);
    }
    if (struCnfg.nEnableTradition != _t_struCnfg.nEnableTradition) {
        setting.setValue(QString("%1-%2").arg(1).arg(50),struCnfg.nEnableTradition);
    }
    if (struCnfg.nMaterPer1 != _t_struCnfg.nMaterPer1){
        setting.setValue(QString("%1-%2").arg(1).arg(51),struCnfg.nMaterPer1);
    }
    if (struCnfg.nMaterPer2 != _t_struCnfg.nMaterPer2) {
        setting.setValue(QString("%1-%2").arg(1).arg(52),struCnfg.nMaterPer2);
    }
    if (struCnfg.nMaterPer3 != _t_struCnfg.nMaterPer3) {
        setting.setValue(QString("%1-%2").arg(1).arg(53),struCnfg.nMaterPer3);
    }
    if (struCnfg.nReservedSens != _t_struCnfg.nReservedSens){
        setting.setValue(QString("%1-%2").arg(1).arg(54),struCnfg.nReservedSens);
    }
    if (struCnfg.nCurCapLevel != _t_struCnfg.nCurCapLevel){
        setting.setValue(QString("%1-%2").arg(1).arg(55),struCnfg.nCurCapLevel);
    }
    if (struCnfg.nCurCapUnit != _t_struCnfg.nCurCapUnit){
        setting.setValue(QString("%1-%2").arg(1).arg(56),struCnfg.nCurCapUnit);
    }
    if (struCnfg.nLedMode != _t_struCnfg.nLedMode){
        setting.setValue(QString("%1-%2").arg(1).arg(57),struCnfg.nLedMode);
    }
    if (struCnfg.nRunMode != _t_struCnfg.nRunMode){
        setting.setValue(QString("%1-%2").arg(1).arg(58),struCnfg.nRunMode);
    }

    if(struCnfg.bAssitCameraEnable != _t_struCnfg.bAssitCameraEnable) {
        setting.setValue(QString("%1-%2").arg(1).arg(59),struCnfg.bAssitCameraEnable);
    }

    if(struCnfg.aFactor != _t_struCnfg.aFactor) {
        setting.setValue(QString("%1-%2").arg(1).arg(60),struCnfg.aFactor);
    }

    if(struCnfg.bFactor != _t_struCnfg.bFactor) {
        setting.setValue(QString("%1-%2").arg(1).arg(61),struCnfg.bFactor);
    }

    if (struCnfg.nPressureAlarmCount != _t_struCnfg.nPressureAlarmCount){
        setting.setValue(QString("%1-%2").arg(1).arg(62),struCnfg.nPressureAlarmCount);
    }

    /* enable ai */
    if(struCnfg.aiEnable != _t_struCnfg.aiEnable) {
        setting.setValue(QString("%1-%2").arg(1).arg(63), struCnfg.aiEnable);
    }
    if(struCnfg.imgFetchHeight != _t_struCnfg.imgFetchHeight) {
        setting.setValue(QString("%1-%2").arg(1).arg(64), struCnfg.imgFetchHeight);
    }
    if(struCnfg.imgInferHeight != _t_struCnfg.imgInferHeight) {
        setting.setValue(QString("%1-%2").arg(1).arg(65), struCnfg.imgInferHeight);
    }
    if(struCnfg.imgPicHeight != _t_struCnfg.imgPicHeight) {
        setting.setValue(QString("%1-%2").arg(1).arg(66), struCnfg.imgPicHeight);
    }
    if(struCnfg.imgVideoHeight != _t_struCnfg.imgVideoHeight) {
        setting.setValue(QString("%1-%2").arg(1).arg(67), struCnfg.imgVideoHeight);
    }

    //总产量
    for(int i=0; i<MAX_UNIT/2; i++) {
        if(struCnfg.nThroughtPut[i] != _t_struCnfg.nThroughtPut[i]) {
            setting.setValue(QString("%1-%2-%3").arg(1).arg(68).arg(i), struCnfg.nThroughtPut[i]);
        }
    }

    /* return value */
    if (setting.status() == QSettings::NoError) {
        return true;
    }
    return false;
}

/* 获取全局文件参数 */
void GlobalFlow::getGlobal()
{
    int i;
    QFile file;

    strNetInfo.selfNetControlPort = DEFAULT_PORT;
#ifdef CONFIG_MULTICAST
    strNetInfo.groupAddress = QHostAddress(DEFAULT_ADDRESS);
#else
    strNetInfo.groupAddress = QHostAddress(QHostAddress::Broadcast);
#endif
    memset(&struCnfg, 0, sizeof(struCnfGlobal));
    memset(&_t_struCnfg, 0, sizeof(struCnfGlobal));

    //! MC定制机型初始语言为英文
    if (struGsh.bIsMC) {
        struCnfg.nLang = LANG_ENG;
    } else {
        struCnfg.nLang = LANG_CHS;
    }

    struCnfg.nCounter = 0;
    struCnfg.nFeedCounter = 0;
    struCnfg.nPressureAlarmCount = 0;
    for(int i=0; i<MAX_UNIT/2; i++){
        struCnfg.nThroughtPut[i] = 0;
    }

    struCnfg.nProfile = 0;
    struCnfg.nProfileTotal = 1;

    for (i = 0; i < struCnfg.nProfileTotal; i++) {
        memcpy(struCnfg.struProfileIndex[i].sMaterialName, "material", sizeof("material"));
        char pFileName[64];
        sprintf(pFileName, "2013081012003%d", i);
        memcpy(struCnfg.struProfileIndex[i].sProfileName,  pFileName, sizeof(pFileName));
    }

    struCnfg.nEjectCountProtect = 400;

    memset(struCnfg.sDelayCode, 0, sizeof(struCnfg.sDelayCode));
    struCnfg.nEnableBacklightPassword = 0;
    sprintf(struCnfg.sBacklightPassword, "%s", "123456");
    struCnfg.nFeederVoltage = 1;

    for (int i = 0; i < 3; i++) {
        struCnfg.uartBaud[i] = MY_UARTBAUD_115200;
    }

    struCnfg.nAutoFeederSet = 1;
    struCnfg.nAutoReferChute = 2;
    struCnfg.nAutoFlagDark = 0;
    struCnfg.nAutoSigEnable = 0;
    struCnfg.nStatisticEnable = 0;
    struCnfg.nWipeMode = 0;
    struCnfg.nFeederCtrlEn = 0;
    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        struCnfg.nFeederCtrlTickEn[i] = 1;
    }

    /* 获取各个层的组信息 */
    struCnfg.nCtrlBoardTotal = 1;
    //修改机器通道数量
    struCnfg.nEjectorsPerChute = 64;
    struCnfg.nEnableLampAI = 1;
    struCnfg.nLampTotal = 10;       // 灯数量10
    getLevelInfo_CF();
    struCnfg.nPMTRun        = 0;
    struCnfg.nEnablePMTR1   = 0;
    struCnfg.nEnablePMTR2   = 1;
    struCnfg.nEnablePMTG    = 1;
    struCnfg.nEnablePMTB    = 1;
    struCnfg.nPMTNum        = 1;
    struCnfg.nFeederRemoteCtrlMode = 0;
    struCnfg.nFeederCtrlWaitTime = 5;

    struCnfg.nEnableDelayCodeInfo = 0;

    // 报警使能
    for (int i = 0; i < MAX_ALARM; i++) {
        struCnfg.nAlarmEnable[i] = 0;
    }

    struCnfg.nAlarmEnable[ALARM_ID_PRESSURE] = 1;

    // 温度检测
    struCnfg.nTemperatureThreshold = 60;

    struCnfg.nEnableAI = 0;
    struCnfg.nEnableTradition = 0;
    struCnfg.nMaterPer1 = 5;
    struCnfg.nMaterPer2 = 15;
    struCnfg.nMaterPer3 = 10;
    struCnfg.nReservedSens = 180;

	// 大数据采集对象备份
	struCnfg.nCurCapLevel = 0;
	struCnfg.nCurCapUnit = 0;

    struCnfg.nLedMode = 0;
    struCnfg.nRunMode = MODE_RUN_STANDALONE;
    struCnfg.aFactor = 0.0000001082933625755753;
    struCnfg.bFactor = 0.219758440421461;

    struCnfg.bAssitCameraEnable = 0;

    struCnfg.aiEnable = 0;
    struCnfg.imgFetchHeight = 512;
    struCnfg.imgInferHeight = 64;
    struCnfg.imgPicHeight = 500;
    struCnfg.imgVideoHeight = 500;


    /* 获取预制方案父节点和子节点名称 */
    getPrefixedName();

    memcpy(&_t_struCnfg, &struCnfg, sizeof(struCnfGlobal));

    if (struCnfs.value != GLOBAL_FILE_ERROR) {
        if (QFileInfo(CNF_GLOBAL).exists() && QFileInfo(CNF_GLOBAL).size() > 0)  {
            if (!getGlobalSetting(CNF_GLOBAL)) {
                qDebug("getGlobal(): can not get global parameters, using default");
                return;
            }
            file.remove(CNF_GLOBAL_B);
            file.copy(CNF_GLOBAL, CNF_GLOBAL_B);
        }
        memcpy(&_t_struCnfg, &struCnfg, sizeof(struCnfGlobal));
    } else {
        if (QFileInfo(CNF_GLOBAL_B).exists() && QFileInfo(CNF_GLOBAL_B).size() > 0) {
            if (!getGlobalSetting(CNF_GLOBAL_B)) {
                qDebug("getGlobal(): can not get global parameters, using default");
                return;
            }
            file.remove(CNF_GLOBAL);
            file.copy(CNF_GLOBAL_B, CNF_GLOBAL);
        }
        memcpy(&_t_struCnfg, &struCnfg, sizeof(struCnfGlobal));
    }
    return;
}


/* 保存全局文件参数 */
bool GlobalFlow::saveGlobal()
{
    int i;
    QFile file;

    saveParamsFileStaus(GLOBAL_FILE_ERROR);
    for (i = 0; i < 3; i++) {
        if (saveGlobalSetting(CNF_GLOBAL)) {
            saveParamsFileStaus(ALL_PARAMS_FILE_OK);
            break;
        }
    }

    if (i == 3)
        return false;

    if (QFileInfo(CNF_GLOBAL).exists() && QFileInfo(CNF_GLOBAL).size() > 0) {
        file.remove(CNF_GLOBAL_B);
        file.copy(CNF_GLOBAL, CNF_GLOBAL_B);
    }

    memcpy(&_t_struCnfg, &struCnfg, sizeof(struCnfGlobal)); // 更新_t_struCnfg
    return true;
}
