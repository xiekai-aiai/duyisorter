#include "globalflow.h"

/* 获取预置方案名称 */
void GlobalFlow::getPrefixedName()
{
    /* 预制方案文件名，即子节点文件名，用中文的汉语拼音表示*/
    memcpy(struCnfe.struParentPrefixedProfile[0].struSonProfile[0].sProfileName, "tongyong", sizeof("tongyong"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[0].sProfileName, "baihe", sizeof("baihe"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[1].sProfileName, "hetao", sizeof("hetao"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[2].sProfileName, "gouqi", sizeof("gouqi"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[3].sProfileName, "yimiren", sizeof("yimiren"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[4].sProfileName, "mianzi", sizeof("mianzi"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[5].sProfileName, "lianzi", sizeof("lianzi"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[6].sProfileName, "putaogan", sizeof("putaogan"));
    memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[7].sProfileName, "yumi", sizeof("yumi"));

    memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[0].sProfileName, "baiyundou", sizeof("baiyundou"));
    memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[1].sProfileName, "hongxiaodou", sizeof("hongxiaodou"));
    memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[2].sProfileName, "candou", sizeof("candou"));
    memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[3].sProfileName, "heiyundou", sizeof("heiyundou"));
    memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[4].sProfileName, "naihuayundou", sizeof("naihuayundou"));
    memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[5].sProfileName, "yingguohong", sizeof("yingguohong"));
    memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[6].sProfileName, "lvdou", sizeof("lvdou"));

    memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[0].sProfileName, "tuopihuasheng-sheng", sizeof("tuopihuasheng-sheng"));
    memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[1].sProfileName, "tuopihuasheng-shu", sizeof("tuopihuasheng-shu"));
    memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[2].sProfileName, "huashengguo", sizeof("huashengguo"));
    memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[3].sProfileName, "hongpihuasheng", sizeof("hongpihuasheng"));
    memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[4].sProfileName, "baishahuasheng", sizeof("baishahuasheng"));

    memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[0].sProfileName, "baiguazi", sizeof("baiguazi"));
    memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[1].sProfileName, "hongguazi", sizeof("hongguazi"));
    memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[2].sProfileName, "youkuiren", sizeof("youkuiren"));
    memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[3].sProfileName, "sandaomeikuiren", sizeof("sandaomeikuiren"));
    memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[4].sProfileName, "jiaoguazi", sizeof("jiaoguazi"));
    memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[5].sProfileName, "jiaoguaziren", sizeof("jiaoguaziren"));
    memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[6].sProfileName, "nanguaziren", sizeof("nanguaziren"));

    /* 预制方案父节点和子节点名称，只有中英文*/
    if(struCnfg.nLang == LANG_CHS || struCnfg.nLang == LANG_CHT) {
        memcpy(struCnfe.struParentPrefixedProfile[0].sMaterialName, "通用类", sizeof("通用类"));
        memcpy(struCnfe.struParentPrefixedProfile[0].struSonProfile[0].sMaterialName, "通用", sizeof("通用"));

        memcpy(struCnfe.struParentPrefixedProfile[1].sMaterialName, "综合类", sizeof("综合类"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[0].sMaterialName, "百合", sizeof("百合"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[1].sMaterialName, "核桃", sizeof("核桃"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[2].sMaterialName, "枸杞", sizeof("枸杞"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[3].sMaterialName, "薏米", sizeof("薏米"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[4].sMaterialName, "棉籽", sizeof("棉籽"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[5].sMaterialName, "莲子", sizeof("莲子"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[6].sMaterialName, "葡萄干", sizeof("葡萄干"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[7].sMaterialName, "玉米", sizeof("玉米"));

        memcpy(struCnfe.struParentPrefixedProfile[2].sMaterialName, "豆类", sizeof("豆类"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[0].sMaterialName, "白芸豆", sizeof("白芸豆"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[1].sMaterialName, "红小豆", sizeof("红小豆"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[2].sMaterialName, "蚕豆", sizeof("蚕豆"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[3].sMaterialName, "黑芸豆", sizeof("黑芸豆"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[4].sMaterialName, "奶花芸豆", sizeof("奶花芸豆"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[5].sMaterialName, "英国红", sizeof("英国红"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[6].sMaterialName, "绿豆", sizeof("绿豆"));

        memcpy(struCnfe.struParentPrefixedProfile[3].sMaterialName, "花生", sizeof("花生"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[0].sMaterialName, "脱皮花生-生", sizeof("脱皮花生-生"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[1].sMaterialName, "脱皮花生-熟", sizeof("脱皮花生-熟"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[2].sMaterialName, "花生果", sizeof("花生果"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[3].sMaterialName, "红皮花生", sizeof("红皮花生"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[4].sMaterialName, "白沙花生", sizeof("白沙花生"));

        memcpy(struCnfe.struParentPrefixedProfile[4].sMaterialName, "籽仁", sizeof("籽仁"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[0].sMaterialName, "白瓜子", sizeof("白瓜子"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[1].sMaterialName, "红瓜子", sizeof("红瓜子"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[2].sMaterialName, "油葵仁", sizeof("油葵仁"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[3].sMaterialName, "三道梅葵仁", sizeof("三道梅葵仁"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[4].sMaterialName, "茭瓜子", sizeof("茭瓜子"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[5].sMaterialName, "茭瓜子仁", sizeof("茭瓜子仁"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[6].sMaterialName, "南瓜子仁", sizeof("南瓜子仁"));
    }
    else {
        memcpy(struCnfe.struParentPrefixedProfile[0].sMaterialName, "General", sizeof("General"));
        memcpy(struCnfe.struParentPrefixedProfile[0].struSonProfile[0].sMaterialName, "General", sizeof("General"));

        memcpy(struCnfe.struParentPrefixedProfile[1].sMaterialName, "Misc", sizeof("Misc"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[0].sMaterialName, "Lily", sizeof("Lily"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[1].sMaterialName, "Walnut", sizeof("Walnut"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[2].sMaterialName, "Wolfberry", sizeof("Wolfberry"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[3].sMaterialName, "Pearl Barley Kernel", sizeof("Pearl Barley Kernel"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[4].sMaterialName, "Cottonseed", sizeof("Cottonseed"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[5].sMaterialName, "Lotus Nut", sizeof("Lotus Nut"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[6].sMaterialName, "Raisin", sizeof("Raisin"));
        memcpy(struCnfe.struParentPrefixedProfile[1].struSonProfile[7].sMaterialName, "Corn", sizeof("Corn"));

        memcpy(struCnfe.struParentPrefixedProfile[2].sMaterialName, "Bean", sizeof("Bean"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[0].sMaterialName, "White Kidney Bean", sizeof("White Kidney Bean"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[1].sMaterialName, "Small Red Bean", sizeof("Small Red Bean"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[2].sMaterialName, "Board Bean", sizeof("Board Bean"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[3].sMaterialName, "Black Kidney Bean", sizeof("Black Kidney Bean"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[4].sMaterialName, "Light Speckled Kidney Beans", sizeof("Light Speckled Kidney Beans"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[5].sMaterialName, "English Red", sizeof("English Red"));
        memcpy(struCnfe.struParentPrefixedProfile[2].struSonProfile[6].sMaterialName, "Mung Bean", sizeof("Mung Bean"));

        memcpy(struCnfe.struParentPrefixedProfile[3].sMaterialName, "Peanut", sizeof("Peanut"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[0].sMaterialName, "Peeled Peanut - Raw", sizeof("Peeled Peanut - Raw"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[1].sMaterialName, "Peeled Peanut - cooked", sizeof("Peeled Peanut - cooked"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[2].sMaterialName, "Peanut in Shell", sizeof("Peanut in Shell"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[3].sMaterialName, "Red-peel Peanut", sizeof("Red-peel Peanut"));
        memcpy(struCnfe.struParentPrefixedProfile[3].struSonProfile[4].sMaterialName, "White Peel Peanut", sizeof("White Peel Peanut"));

        memcpy(struCnfe.struParentPrefixedProfile[4].sMaterialName, "Seeds", sizeof("Seeds"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[0].sMaterialName, "White Seed", sizeof("White Seed"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[1].sMaterialName, "Red Seed", sizeof("Red Seed"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[2].sMaterialName, "Oil Seeds Kernel", sizeof("Oil Seeds Kernel"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[3].sMaterialName, "Sandaomei Kernel", sizeof("Sandaomei Kernel"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[4].sMaterialName, "Jiao Melon Seeds", sizeof("Jiao Melon Seeds"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[5].sMaterialName, "Jiao Melon Seed Kernel", sizeof("Jiao Melon Seed Kernel"));
        memcpy(struCnfe.struParentPrefixedProfile[4].struSonProfile[6].sMaterialName, "Pumpkin Seed Kernel", sizeof("Pumpkin Seed Kernel"));
    }
}

/* 获取工程模式下算法使能设置 */
void GlobalFlow::getArithmeticMachineEnable()
{
    for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
        struCnfe.nArithmeticSeq[i] = 0;
    }

    // 通用机型算法列表
    struCnfe.nArithmeticSeq[ARITH_GREY_A]       = 1;
    struCnfe.nArithmeticSeq[ARITH_GREY_B]       = 1;
    struCnfe.nArithmeticSeq[ARITH_DISCOLOR_A]   = 1;
    struCnfe.nArithmeticSeq[ARITH_DISCOLOR_B]   = 1;
    struCnfe.nArithmeticSeq[ARITH_CROSS]        = 1;
    struCnfe.nArithmeticSeq[ARITH_SHAPE]        = 1;
    struCnfe.nArithmeticSeq[ARITH_SCALE]        = 1;
    struCnfe.nArithmeticSeq[ARITH_SHAPE_LENGTH] = 1;
    struCnfe.nArithmeticSeq[ARITH_BUD_1]        = 1;
    struCnfe.nArithmeticSeq[ARITH_BUD_2]        = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_A]      = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_B]      = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_C]      = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_D]      = 1;
    struCnfe.nArithmeticSeq[ARITH_SCALE_B]      = 1;
    struCnfe.nArithmeticSeq[ARITH_PISTACHIO]    = 1;
}

/*!
 * \brief 重置算法列表
 */
/*!
 * \brief 重置算法使能
 */
/* 获取工程模式中默认工程文件保存的参数  */
void GlobalFlow::getEngineerDefault()
{
    //! 判断CG机型
    if (struCnfe.nMachine == MACHINE_CG) {
        struCnfe.nMachine = MACHINE_CF;
        struCnfe.nEnableCGMachine = 1;
    } else {
        struCnfe.nEnableCGMachine = 0;
    }
    struCnfe.nEnableHGQMachine = 0;

    struCnfe.nEnableBeanMachine = 0;
    struCnfe.nEnableMaizeDoubleView = 0;
    struCnfe.nDerivedDevType = 0x103;

    struCnfe.nLangList[0] = 1;
    struCnfe.nLangList[1] = LANG_CHS;
    struCnfe.nLangList[2] = LANG_ENG;

    struCnfe.nArithmeticTotal = ARITHMETIC_TOTAL;
    getArithmeticMachineEnable();
    memcpy(struCnfe.sTitle, "", sizeof(""));



    /* 预制方案相关 */
    struCnfe.nParentPrefixedProfileTotal = 1;                       // 预制方案种类数量
    struCnfe.struParentPrefixedProfile[0].nSonProfileCount = 1;     // 通用类 预制方案数量
    struCnfe.struParentPrefixedProfile[1].nSonProfileCount = 8;     // 综合类 预制方案数量
    struCnfe.struParentPrefixedProfile[2].nSonProfileCount = 7;     // 豆类  预制方案数量
    struCnfe.struParentPrefixedProfile[3].nSonProfileCount = 5;     // 花生类 预制方案数量
    struCnfe.struParentPrefixedProfile[4].nSonProfileCount = 7;     // 籽仁类 预制方案数量

    for (int i = 0; i < struCnfe.nParentPrefixedProfileTotal; i++) {
        struCnfe.struParentPrefixedProfile[i].nIsAble = 1;
        for (int j = 0; j < struCnfe.struParentPrefixedProfile[i].nSonProfileCount; j++) {
            struCnfe.struParentPrefixedProfile[i].struSonProfile[j].nIsAble = 1;
        }
    }
}

/* 从工程配置文件获取工程模式参数 */
bool GlobalFlow::getEngineerSetting(const QString & fileName)
{
    int sizeTmp   = 0;          //配置文件中参数的长度,该变量主要用于memcpy
    const char *strTmp  = NULL;       //配置文件中参数的字符串,该变量主要用于memcpy

    /* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);

    /* product */
    struCnfe.nProduct = setting.value(QString("%1-%2").arg(0).arg(0), struCnfe.nProduct).toInt();

    /* machine */
    struCnfe.nMachine = setting.value(QString("%1-%2").arg(0).arg(1), struCnfe.nMachine).toInt();

    /* arithmetic total */
    struCnfe.nArithmeticTotal = ARITHMETIC_TOTAL;   //默认算法总数不需要从文件中获取

    for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
        struCnfe.nArithmeticSeq[i] = setting.value(QString("%1-%2-%3").arg(0).arg(3).arg(i), struCnfe.nArithmeticSeq[i]).toInt();
    }
    /* language list */
    for (int i = 0; i < 64; i++) {
        struCnfe.nLangList[i] = setting.value(QString("%1-%2-%3").arg(0).arg(4).arg(i), struCnfe.nLangList[i]).toInt();
    }

    /* prefixed profile total */
    struCnfe.nParentPrefixedProfileTotal = setting.value(QString("%1-%2").arg(0).arg(5), struCnfe.nParentPrefixedProfileTotal).toInt();

    /* parent profile */
    for (int i = 0; i < MAX_PARENT_PREFIXED_PROFILE_COUNT; i++) {
        struCnfe.struParentPrefixedProfile[i].nIsAble = setting.value(QString("%1-%2-%3-%4").arg(0).arg(6).arg(i).arg(0), struCnfe.struParentPrefixedProfile[i].nIsAble).toInt();
        struCnfe.struParentPrefixedProfile[i].nSonProfileCount = setting.value(QString("%1-%2-%3-%4").arg(0).arg(6).arg(i).arg(1), struCnfe.struParentPrefixedProfile[i].nSonProfileCount).toInt();
        strTmp = setting.value(QString("%1-%2-%3-%4").arg(0).arg(6).arg(i).arg(2), struCnfe.struParentPrefixedProfile[i].sMaterialName).toString().toLocal8Bit().constData();
        sizeTmp = strlen(strTmp);
        if (sizeTmp > sizeof(struCnfe.struParentPrefixedProfile[i].sMaterialName)-1) {
            sizeTmp = sizeof(struCnfe.struParentPrefixedProfile[i].sMaterialName)-1;    //以'\0'结尾
        }
        memcpy(struCnfe.struParentPrefixedProfile[i].sMaterialName,strTmp,sizeTmp);

        /* son profile */
        for (int j = 0; j < MAX_SON_PREFIXED_PROFILE_COUNT; j++) {
            struCnfe.struParentPrefixedProfile[i].struSonProfile[j].nIsAble = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(0).arg(6).arg(i).arg(3).arg(j).arg(0),
                                                                                            struCnfe.struParentPrefixedProfile[i].struSonProfile[j].nIsAble).toInt();
            //sMaterialName
            strTmp = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(0).arg(6).arg(i).arg(3).arg(j).arg(1), struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sMaterialName).toString().toLocal8Bit().constData();
            sizeTmp = strlen(strTmp);
            if (sizeTmp > sizeof(struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sMaterialName)-1) {
                sizeTmp = sizeof(struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sMaterialName)-1;    //以'\0'结尾
            }
            memcpy(struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sMaterialName,strTmp,sizeTmp);
            //sMaterialName

            //sProfileName
            strTmp = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(0).arg(6).arg(i).arg(3).arg(j).arg(2), struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sProfileName).toString().toLocal8Bit().constData();
            sizeTmp = strlen(strTmp);
            if (sizeTmp > sizeof(struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sProfileName)-1) {
                sizeTmp = sizeof(struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sProfileName)-1;    //以'\0'结尾
            }
            memcpy(struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sProfileName,strTmp,sizeTmp);
            //sProfileName
        }
    }

    /* title */
    //sTitle
    strTmp = setting.value(QString("%1-%2").arg(0).arg(7), struCnfe.sTitle).toString().toLocal8Bit().constData();
    sizeTmp = strlen(strTmp);
    if (sizeTmp > sizeof(struCnfe.sTitle)-1) {
        sizeTmp = sizeof(struCnfe.sTitle)-1;
    }
    memcpy(struCnfe.sTitle,strTmp,sizeTmp);
    //sTitle

    struCnfe.nEnableBeanMachine  = setting.value(QString("%1-%2").arg(0).arg(8), struCnfe.nEnableBeanMachine).toInt();
    struCnfe.nEnableMaizeDoubleView  = setting.value(QString("%1-%2").arg(0).arg(9), struCnfe.nEnableMaizeDoubleView).toInt();
    struCnfe.nDerivedDevType  = setting.value(QString("%1-%2").arg(0).arg(10), struCnfe.nDerivedDevType).toInt();
    struCnfe.nEnableCGMachine  = setting.value(QString("%1-%2").arg(0).arg(11), struCnfe.nEnableCGMachine).toInt();
    struCnfe.nEnableHGQMachine  = setting.value(QString("%1-%2").arg(0).arg(12), struCnfe.nEnableHGQMachine).toInt();

     /* return value */
    if (setting.status() == QSettings::NoError)
        return true;
    else
        return false;
}

/* 保存工程模式参数至配置文件 */
bool GlobalFlow::saveEngineerSetting(const QString & fileName)
{
    /* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);

    /* product */
    setting.setValue(QString("%1-%2").arg(0).arg(0), struCnfe.nProduct);

    /* machine */
    setting.setValue(QString("%1-%2").arg(0).arg(1), struCnfe.nMachine);

    /* arithmetic total */
    setting.setValue(QString("%1-%2").arg(0).arg(2), struCnfe.nArithmeticTotal);
    if (struCnfe.nArithmeticTotal != _t_struCnfe.nArithmeticTotal) {
        myLog->info(LOG_ARITH,"arith total: %d",struCnfe.nArithmeticTotal);
    }

    /* arithmetic sequence */
    for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
        setting.setValue(QString("%1-%2-%3").arg(0).arg(3).arg(i), struCnfe.nArithmeticSeq[i]);
        if (struCnfe.nArithmeticSeq[i] != _t_struCnfe.nArithmeticSeq[i]) {
        myLog->info(LOG_ARITH,"arith list(%d,%d)",i,struCnfe.nArithmeticSeq[i]);
        }
    }
    /* language list */
    for (int i = 0; i < 64; i++) {
        setting.setValue(QString("%1-%2-%3").arg(0).arg(4).arg(i), struCnfe.nLangList[i]);
    }

    /* prefixed profile total */
    setting.setValue(QString("%1-%2").arg(0).arg(5), struCnfe.nParentPrefixedProfileTotal);

    /* parent prefixed profile */
    for (int i = 0; i < MAX_PARENT_PREFIXED_PROFILE_COUNT; i++) {
        setting.setValue(QString("%1-%2-%3-%4").arg(0).arg(6).arg(i).arg(0), struCnfe.struParentPrefixedProfile[i].nIsAble);
        setting.setValue(QString("%1-%2-%3-%4").arg(0).arg(6).arg(i).arg(1), struCnfe.struParentPrefixedProfile[i].nSonProfileCount);
        setting.setValue(QString("%1-%2-%3-%4").arg(0).arg(6).arg(i).arg(2), struCnfe.struParentPrefixedProfile[i].sMaterialName);
        /* son prefixed profile */
        for (int j = 0; j < MAX_SON_PREFIXED_PROFILE_COUNT; j++) {
            setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(0).arg(6).arg(i).arg(3).arg(j).arg(0), struCnfe.struParentPrefixedProfile[i].struSonProfile[j].nIsAble);
            setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(0).arg(6).arg(i).arg(3).arg(j).arg(1), struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sMaterialName);
            setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(0).arg(6).arg(i).arg(3).arg(j).arg(2), struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sProfileName);
        }
    }

    /* title */
    setting.setValue(QString("%1-%2").arg(0).arg(7), struCnfe.sTitle);

    /* 豆类定制机型使能 */
    setting.setValue(QString("%1-%2").arg(0).arg(8), struCnfe.nEnableBeanMachine);

    /* 玉米双视定制机型使能 */
    setting.setValue(QString("%1-%2").arg(0).arg(9), struCnfe.nEnableMaizeDoubleView);

    /* 衍生机型 - I */
    setting.setValue(QString("%1-%2").arg(0).arg(10), struCnfe.nDerivedDevType);

    /* 衍生机型 - CG */
    setting.setValue(QString("%1-%2").arg(0).arg(11), struCnfe.nEnableCGMachine);

    /* 黑枸杞定制机型使能 */
    setting.setValue(QString("%1-%2").arg(0).arg(12), struCnfe.nEnableHGQMachine);

    /* return value */
    if (setting.status() == QSettings::NoError) {
        return true;
    }
    return false;
}

/* 获取工程文件参数 */
void GlobalFlow::getEngineer()
{
    QFile file;

#ifdef Q_OS_UNIX
    struCnfe.nProduct = myString.sProduct.toInt();
    struCnfe.nMachine = myString.sMachine.toInt();
    struGsh.nCustomReq = myString.sCustomReq.toInt();
#else
    struCnfe.nProduct = OPT_PRODUCT_ANCOO_GENERAL;
    struCnfe.nMachine = MACHINE_CG;
    struGsh.nCustomReq = 0;
#endif

    //! 判断是否为MC机型
    myMonoFlow.checkIsCustom();

    //! 获取默认的工程参数
    getEngineerDefault();

    //! MC系列只存在英文且不可切换
    if (struGsh.bIsMC) {
        struCnfe.nLangList[1] = LANG_ENG;
    }

    memcpy(&_t_struCnfe, &struCnfe, sizeof(struCnfEngineer));
    if (struCnfs.value != ENGINEER_FILE_ERROR) {
        if (QFileInfo(CNF_ENGINEER).exists() && QFileInfo(CNF_ENGINEER).size() > 0) {
            if (!getEngineerSetting(CNF_ENGINEER)) {
                qDebug("getEngineer(): can not get engineer parameters, using default");
                return;
            }
            file.remove(CNF_ENGINEER_B);
            file.copy(CNF_ENGINEER, CNF_ENGINEER_B);
        }
        memcpy(&_t_struCnfe, &struCnfe, sizeof(struCnfEngineer));
    } else {
        if (QFileInfo(CNF_ENGINEER_B).exists() && QFileInfo(CNF_ENGINEER_B).size() > 0) {
            if (!getEngineerSetting(CNF_ENGINEER_B)) {
                qDebug("getEngineer(): can not get engineer parameters, using default");
                return;
            }
            file.remove(CNF_ENGINEER);
            file.copy(CNF_ENGINEER_B, CNF_ENGINEER);
        }
        memcpy(&_t_struCnfe, &struCnfe, sizeof(struCnfEngineer));
    }

    //! 玉米机型重置算法列表(与前置板统一)

    return;
}

/* 保存工程文件参数 */
bool GlobalFlow::saveEngineer()
{
    int i;
    QFile file;

    saveParamsFileStaus(ENGINEER_FILE_ERROR);
    for (i = 0; i < 3; i++) {
        if (saveEngineerSetting(CNF_ENGINEER)) {
            saveParamsFileStaus(ALL_PARAMS_FILE_OK);
            break;
        }
    }

    if (i == 3) {
        return false;
    }

    if (QFileInfo(CNF_ENGINEER).exists() && QFileInfo(CNF_ENGINEER).size() > 0) {
        file.remove(CNF_ENGINEER_B);
        file.copy(CNF_ENGINEER, CNF_ENGINEER_B);
    }

    memcpy(&_t_struCnfe, &struCnfe, sizeof(struCnfEngineer));   // 更新_t_struCnfe
    return true;
}
