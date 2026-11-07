/*!
 * \file        test_main.cpp
 * \brief       DuySorter 无UI ARM 测试程序
 * \author      AI
 *
 *  用法:  ./test_arm [mode]
 *    mode = 1  阀门顺序测试 (1~10号阀依次喷)
 *    mode = 2  单色1 配置 (蓝色100%精度, 行数8, 斑点2) + 供料速度50%
 *              + 开料持续10秒后自动关料 (全程打印 com1/com2 下发帧)
 *    mode = 3  先测阀门再单色1
 *    mode = 4  只读诊断: 打印方案加载后的算法使能态/单色参数, 不碰硬件 (默认)
 *    无参数    默认 mode=4
 */
#include <QCoreApplication>
#include <QtGlobal>
#include <QHostAddress>
#include <cstdio>

#include "globalflow.h"
#include "globalparams.h"
#include "myqextserialport.h"
#include "sortertypes.h"
#include "cmdworker.h"
#include "cmdudpmanager.h"
#include "aihelper.h"

/* ------------------------------------------------------------------ */
/*  基础宏                                                             */
/* ------------------------------------------------------------------ */
/* 同时输出到 stderr(控制台/重定向) 和 qDebug; 兼容 release 的 QT_NO_DEBUG_OUTPUT */
#define LOG_TAG(x)  do { QString _m = QStringLiteral("[TEST] ") + (x); \
                         fprintf(stderr, "%s\n", _m.toLocal8Bit().constData()); \
                         fflush(stderr); qDebug().noquote() << _m; } while (0)

/* ------------------------------------------------------------------ */
/*  Test 1 - 阀门顺序测试 (UDP → AI 板子, 模仿上位机测阀模式)         */
/*    循环喷 1~10 号阀, 每个喷一下停一下, 低速模式 (interval=800ms)   */
/*    不再直接写串口 CMD_INT_EJTEST, 完全走 AI 板子 UDP 链路:         */
/*      FixedCheckParam { valve=0, injector, interval }               */
/*      → StartFixCheckRequest (UDP 9193)                              */
/*      → msleep(interval/2)                                          */
/*      → StopFixCheckRequest  (UDP 9193)                              */
/*      → msleep(interval/2)                                          */
/* ------------------------------------------------------------------ */
static int test_valve_sequence()
{
    LOG_TAG("=== Test 1: 阀门顺序测试 1~10 (UDP → AI 板子 FixCheck) ===");

    /* 先关供料, 确保安全 */
    if (struGsh.bStatFeed == 1)
    {
        LOG_TAG("先停止供料...");
        myFlow.onOff();
        myFlow.msleep(3000);
    }

    /* 保护: 残缺方案里 nEjectorsPerChute 可能是 0 */
    const int JETS_PER_CHUTE = qMax((int)struCnfg.nEjectorsPerChute, 12);
    LOG_TAG(QString("[DIAG] nEjectorsPerChute (或默认12) = %1").arg(JETS_PER_CHUTE));

    const quint16 INTERVAL_MS = 800;          // 低速模式, 和上位机一致
    const int MAX_VALVE = 10;

    for (int nDisplayNum = 1; nDisplayNum <= MAX_VALVE; nDisplayNum++)
    {
        int dev_no = (nDisplayNum - 1) / JETS_PER_CHUTE;   // 第几个通道/AI板组
        QString ip = ai_helper::GetAiIpByIndex(dev_no * 2); // 前视 AI 板子 index
        quint8 injector_no = (nDisplayNum - 1) % JETS_PER_CHUTE;

        FixedCheckParam info;
        info.valve_no_    = 0;
        info.injector_no_ = injector_no;
        info.interval_    = INTERVAL_MS;

        LOG_TAG(QString("  测阀 #%1: injector=%2 dev_no=%3 ip=%4 interval=%5ms")
            .arg(nDisplayNum).arg(injector_no).arg(dev_no).arg(ip).arg(INTERVAL_MS));

        /* ---- 启动定检 (UDP) ---- */
        QByteArray start_req = cmdworker::StartFixCheckRequest(info);
        QByteArray start_rep;
        bool start_ok = CmdUdpManager::instance().onSendCommand(
            QHostAddress(ip), AI_UPD_CMD_PORT, start_req, start_rep, AI_RESPONSE_TIMEOUT);
        LOG_TAG(QString("    [StartFixCheck] ok=%1 req(%2B)=%3 rep(%4B)=%5")
            .arg(start_ok).arg(start_req.size()).arg(QString(start_req.toHex(' ').toUpper()))
            .arg(start_rep.size()).arg(QString(start_rep.toHex(' ').toUpper())));

        myFlow.msleep(INTERVAL_MS / 2);

        /* ---- 停止定检 (UDP) ---- */
        QByteArray stop_req = cmdworker::StopFixCheckRequest(info);
        QByteArray stop_rep;
        bool stop_ok = CmdUdpManager::instance().onSendCommand(
            QHostAddress(ip), AI_UPD_CMD_PORT, stop_req, stop_rep, AI_RESPONSE_TIMEOUT);
        LOG_TAG(QString("    [StopFixCheck]  ok=%1 req(%2B)=%3 rep(%4B)=%5")
            .arg(stop_ok).arg(stop_req.size()).arg(QString(stop_req.toHex(' ').toUpper()))
            .arg(stop_rep.size()).arg(QString(stop_rep.toHex(' ').toUpper())));

        myFlow.msleep(INTERVAL_MS / 2);
    }

    LOG_TAG("=== 阀门测试完成 ===");
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Test 5 - 阀门顺序测试 (直接写 FPGA 串口 CMD_INT_EJTEST)          */
/*    不走 AI 板子 UDP, 直接 com1Write 给接口板喷阀                   */
/*    对比 mode=1 (AI UDP) 看两条链路哪个在你的板卡上响               */
/* ------------------------------------------------------------------ */
static int test_valve_direct()
{
    LOG_TAG("=== Test 5: 阀门顺序测试 1~10 (direct FPGA 串口 CMD_INT_EJTEST) ===");

    /* 先关供料, 确保安全 */
    if (struGsh.bStatFeed == 1)
    {
        LOG_TAG("先停止供料...");
        myFlow.onOff();
        myFlow.msleep(3000);
    }

    const int MAX_VALVE = 10;

    /* 进入工作态: direct 版只需要接口板 INT_ONOFF (CMD_INT_EJTEST 是 INT 级命令) */
    LOG_TAG("确保接口板进入工作态 (CMD_INT_ONOFF)...");
    {
        for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
            MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, 0, 1, 3);
    }
    myFlow.msleep(200);

    for (int valveIdx = 1; valveIdx <= MAX_VALVE; valveIdx++)
    {
        LOG_TAG(QString("  [DIRECT] 喷阀 #%1 -> injector=%2").arg(valveIdx).arg(valveIdx - 1));

        /* CMD_INT_EJTEST: INT, level, 0, 0x01(开), 0, 0, valve/256, valve%256, 3 */
        MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel, 0, 0x01, 0, 0,
            (valveIdx - 1) / 256, (valveIdx - 1) % 256, 3);
        myFlow.msleep(200);

        MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel, 0, 0x00, 0, 0,
            (valveIdx - 1) / 256, (valveIdx - 1) % 256, 3);
        myFlow.msleep(300);
    }

    LOG_TAG("=== Direct 阀门测试完成 ===");
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Test 2 - 单色1 (ARITH_GREY_A) 配置 + 启动供料                      */
/*    设置: 蓝色 (COLOR_OPT_B=4), 精度=100%(nPercent=100),            */
/*          行数=8 (nRow=8), 斑点=2 (nColumn=2)                       */
/*          灵敏度上限 nSensMax 拉大, 精度上限 nPercent 拉大          */
/*    然后 FPGA 下发 + 启动供料                                        */
/* ------------------------------------------------------------------ */
static int test_grey_a()
{
    LOG_TAG("=== Test 2: 单色1 (GREY_A) 蓝色 + 供料速度50% + 开料10秒 ===");

    /* 安全: 进入时若正在供料, 先关料 */
    if (struGsh.bStatFeed == 1)
    {
        LOG_TAG("  检测到当前已在供料, 先关料 ...");
        myFlow.onOff();
        myFlow.msleep(1000);
    }

    /* ---- 1. 内存设置算法使能 ---- */
    for (int i = 0; i < ARITHMETIC_TOTAL; i++)
        struCnfp.nArithmeticEnable[i] = 0;

    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;   // 单色1 使能
    struCnfp.nArithmeticEnable[ARITH_GREY_B] = 0;
    struCnfp.nArithmeticEnable[ARITH_DISCOLOR_A] = 0;
    struCnfp.nArithmeticEnable[ARITH_DISCOLOR_B] = 0;
    struCnfp.nArithmeticEnable[ARITH_CROSS] = 0;

    LOG_TAG("  nArithmeticEnable[GREY_A] = 1");

    /* ---- 2. 内存设置 GREY_A 参数 ----
     *    用户要求 (参考UI截图):
     *      算法     = 蓝色 (COLOR_OPT_B = 4)
     *      精度     = 100.0% (nPercent = 100)
     *      行数     = 8
     *      斑点     = 2 (UI上"斑点"对应 nColumn)
     *    struGreyColor:
     *      nRow      = 8   行数
     *      nColumn   = 2   斑点(列数)
     *      nPercent  = 100 纯度(%)
     *      nColor    = 4   蓝色通道 (COLOR_OPT_B)
     *      nMode     = 1   选亮 (1=选亮, 0=选暗)
     *      nSensMin  = 0   灵敏度下限
     *      nSensMax  = 590 灵敏度上限 (参考方案文件 3-14 中常见值)
     */
    int level   = 0;  // ONE_LEVEL
    int groupId = 0;  // 第一个识别组

    stu_grey_color& greyA = struCnfp.struGroupIdentify[level][groupId].struGreyColor[ARITH_GREY_A];

    greyA.nRow      = 8;
    greyA.nColumn   = 2;
    greyA.nPercent  = 100;     // 100.0%
    greyA.nColor    = COLOR_OPT_B;   // 4 = 蓝色
    greyA.nMode     = 1;       // 选亮 (蓝高)
    greyA.nSensMin  = 0;
    greyA.nSensMax  = 590;
    greyA.nBurntReservedFlag = 0;
    memset(greyA.sName, 0, sizeof(greyA.sName));

    LOG_TAG(QString("  GREY_A 参数: row=%1 col=%2 percent=%3 color=BLUE(%4) mode=亮 sensMin=%5 sensMax=%6")
        .arg(greyA.nRow).arg(greyA.nColumn).arg(greyA.nPercent)
        .arg(greyA.nColor).arg(greyA.nSensMin).arg(greyA.nSensMax));

    /* ---- 3. 设置背景颜色为蓝色 ---- */
    struCnfp.nBackgroundColor[level][0] = COLOR_OPT_B;
    struCnfp.nBackgroundColor[level][1] = COLOR_OPT_WHITE;   // 后视白背景
    struCnfp.nBackgroundColorValue[level][0] = 200;          // 亮度
    struCnfp.nBackgroundColorValue[level][1] = 240;

    LOG_TAG("  背景颜色: 前视=蓝色 后视=白色");

    /* ---- 4. FPGA 下发 ----
     *    resetSortParams() 内部做了:
     *      resetMaterial()         -> 调 materialParamsSet(CMD_UNIT_GREY_A, ...) 下发灰度参数
     *      resetBackgroundColor()  -> 下发背景颜色
     *      resetTick()             -> 下发剔除参数
     *      resetArithmeticEnable() -> 下发算法使能位图
     */
    LOG_TAG("  下发 FPGA 参数...");
    myFlow.initSendAllParams();
    LOG_TAG("  FPGA 下发完成");

    /* ---- 5. 设置供料速度 = 50% 并使能全部供料器, 然后下发 ----
     *    界面显示值 0~100.0(%), 内部存储 nFeeder = 显示值 * 10
     *    => 50.0% 对应 nFeeder = 500
     */
    for (int f = 0; f < MAX_FEED; f++)
    {
        struCnfp.struGroupCtrl[0].nFeederEnable[f] = 1;
        struCnfp.struGroupCtrl[0].nFeeder[f] = 500;
    }
    myFlow.resetFeeder(0);
    LOG_TAG("  供料速度=50% (nFeeder=500), 全部供料器使能并已下发");

    /* ---- 6. 开料, 持续 10 秒后自动关料 ---- */
    LOG_TAG("  启动供料 (开料)...");
    myFlow.onOff();   // onOff 是 toggle, 当前为关闭则开启
    LOG_TAG(QString("  供料状态: bStatFeed=%1").arg(struGsh.bStatFeed));

    const int FEED_SECONDS = 10;
    for (int s = FEED_SECONDS; s > 0; s--)
    {
        LOG_TAG(QString("  [供料中] 剩余 %1 秒 ...").arg(s));
        myFlow.msleep(1000);   // 内部会 pump Qt 事件循环, 串口收发不中断
    }

    LOG_TAG("  10 秒到, 关闭供料 (关料)...");
    myFlow.onOff();
    LOG_TAG(QString("  供料状态: bStatFeed=%1").arg(struGsh.bStatFeed));

    return 0;
}

/* ------------------------------------------------------------------ */
/*  诊断 - 只读打印方案加载后的真实算法使能态/单色参数 (不碰硬件)        */
/*    用于定位: 勾选单色1为何没有下发 GREY_A 参数包                     */
/* ------------------------------------------------------------------ */
static const char* arithName(int i)
{
    switch (i)
    {
    case ARITH_GREY_A:      return "GREY_A(单色1)";
    case ARITH_GREY_B:      return "GREY_B(单色2)";
    case ARITH_DISCOLOR_A:  return "DISCOLOR_A(色差1)";
    case ARITH_DISCOLOR_B:  return "DISCOLOR_B(色差2)";
    case ARITH_CROSS:       return "CROSS(差分)";
    case ARITH_INTEL_A:     return "INTEL_A(智能A)";
    case ARITH_WATERMELON:  return "WATERMELON(西瓜子)";
    case ARITH_CANDY:       return "CANDY(糖果)";
    case ARITH_PISTACHIO:   return "PISTACHIO(AI开心果)";
    default:                return "";
    }
}

static int diag_dump()
{
    LOG_TAG("================ 方案加载诊断 (只读) ================");

    /* 当前加载的方案文件名 */
    QString prof = QString::fromLatin1(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
    LOG_TAG(QString("方案文件名 sProfileName = [%1]  nProfile=%2")
        .arg(prof).arg(struCnfg.nProfile));
    LOG_TAG(QString("nArithmeticTotal=%1  nLevelTotal=%2")
        .arg(struCnfe.nArithmeticTotal).arg(struCnfg.nLevelTotal));

    /* 全部算法使能位 + 本机是否显示该算法 */
    LOG_TAG("---- struCnfp.nArithmeticEnable[0..29] ----");
    for (int i = 0; i < ARITHMETIC_TOTAL; i++)
    {
        int en = struCnfp.nArithmeticEnable[i];
        int seq = struCnfe.nArithmeticSeq[i];
        const char* nm = arithName(i);
        if (en == 1 || seq == 1 || nm[0] != '\0')
        {
            LOG_TAG(QString("  [%1] enable=%2 nArithmeticSeq=%3 %4")
                .arg(i, 2).arg(en).arg(seq).arg(nm));
        }
    }

    LOG_TAG(QString("==> 关键: GREY_A(0) enable=%1  PISTACHIO(29) enable=%2")
        .arg(struCnfp.nArithmeticEnable[ARITH_GREY_A])
        .arg(struCnfp.nArithmeticEnable[ARITH_PISTACHIO]));

    /* 单色1 物料参数 */
    const stu_grey_color& g = struCnfp.struGroupIdentify[0][0].struGreyColor[ARITH_GREY_A];
    LOG_TAG(QString("GreyColor[0][0][GREY_A]: row=%1 column=%2 percent=%3 color=%4 mode=%5 sensMin=%6 sensMax=%7")
        .arg(g.nRow).arg(g.nColumn).arg(g.nPercent)
        .arg(g.nColor).arg(g.nMode).arg(g.nSensMin).arg(g.nSensMax));

    LOG_TAG("============ 诊断结束 (未对硬件做任何操作) ============");
    return 0;
}

/* ================================================================ */
/* TEST_ONLY_CAM0 宏: 设为 1 时仅给 camera0 (unitAddr=0) 发指令       */
/*                   设为 0 时恢复正常 (给所有相机发)                 */
/* 用途: 验证相机0 能否独立工作 → 排除 FPGA 或协议本身的问题          */
/* ================================================================ */
#define TEST_ONLY_CAM0  0   // 0=给所有相机发(正常)  1=只给相机0发(调试)

/* ------------------------------------------------------------------ */
/*  Test 6 - 前后视不同 GREY_A (蓝低) + 供料 40% + 开料 10 秒           */
/*    前视 (group 0, first_front): 蓝低 mode=0 sensMin=1000 (100%)      */
/*    后视 (group 1, first_rear): 蓝低 mode=0 sensMin=0     (0%)        */
/*    nMatAssembleMode 强制 PARAMS_ALL_SEPARATE → 前后各自独立下发       */
/*    initSendAllParams 后重建喷阀寄存器 (确保接口板工作态 + relation=0) */
/*    直接开料 10 秒 → 关料退出 (无测阀)                                */
/* ------------------------------------------------------------------ */
static int test_grey_a_front_rear()
{
    /* === TEST_ONLY_CAM0 开关: 保存原始值, 临时改, 用完恢复 === */
    int _save_nUnitLevelTotal = struCnfg.struLevelInfo[0].nUnitLevelTotal;
    int _save_nIdentifyGroupTotal = struCnfg.struLevelInfo[0].nIdentifyGroupTotal;
#if TEST_ONLY_CAM0
    LOG_TAG("  🔴 TEST_ONLY_CAM0=1: 只给相机0 (unitAddr=0) 发指令！");
    LOG_TAG(QString("     原始 nUnitLevelTotal=%1 nIdentifyGroupTotal=%2")
        .arg(_save_nUnitLevelTotal).arg(_save_nIdentifyGroupTotal));
    struCnfg.struLevelInfo[0].nUnitLevelTotal = 1;      // 只遍历 1 个 unit
    struCnfg.struLevelInfo[0].nIdentifyGroupTotal = 1;  // 只遍历 1 个 group
    // unitId[0] 已经是 0，不用改
    LOG_TAG(QString("     临时 nUnitLevelTotal=1 nIdentifyGroupTotal=1"));
#else
    LOG_TAG("  🟢 TEST_ONLY_CAM0=0: 给所有相机发指令 (正常模式)");
#endif
    /* ============================================================
     * G6: GREY_A enable=1 + 给料器 40% (sensMin / 前后视独立继续默认)
     * G5 回归了给料器设置导致供料器都不转; G6 补回给料器,
     * 这就和主程序 sorter.log 13:48 完全对齐:
     *   用户勾选 GREY_A enable + 设 Feeder=500 + Feed on
     * 目标: 验证只要 GREY_A=1 + Feeder>0, FPGA 就会跑检测 → 喷阀动.
     * ============================================================ */
    LOG_TAG("=== Test 6 G6: GREY_A=1 + 给料40% (其他全默认) ===");

    /* 安全: 进入时若正在供料, 先关料 */
    if (struGsh.bStatFeed == 1) {
        LOG_TAG("  检测到当前已在供料, 先关料 ...");
        myFlow.onOff();
        myFlow.msleep(1000);
    }

    /* ---- 1. 开 GREY_A 算法使能 ---- */
    LOG_TAG("  [G6] GREY_A enable=1...");
    struCnfp.nArithmeticEnable[ARITH_GREY_A] = 1;
    for (int lv = 0; lv < MAX_LEVEL; lv++)
        for (int g = 0; g < MAX_GROUP_IDTNTIFY; g++)
            struCnfp.nArithmeticEnableGroup[lv][g][ARITH_GREY_A] = 1;

    /* ---- 2. 设给料器速度 = 40% ---- */
    LOG_TAG("  [G6] Feeder=400 (40%) + enable=1 ...");
    for (int f = 0; f < MAX_FEED; f++) {
        struCnfp.struGroupCtrl[0].nFeederEnable[f] = 1;
        struCnfp.struGroupCtrl[0].nFeeder[f] = 400;   // 40.0% → 400
    }

    /* ---- G10: 强制独立模式 + G0 sensMin=1000, G1 sensMin=0 ---- */
    LOG_TAG("  [G10] 强制 nMatAssembleMode=PARAMS_ALL_SEPARATE (1)...");
    struCnfp.nMatAssembleMode = PARAMS_ALL_SEPARATE;

    LOG_TAG("  [G10] 覆盖 GREY_A 参数 (G0 sensMin=1000, G1 sensMin=0)...");
    {
        stu_grey_color& g0a = struCnfp.struGroupIdentify[0][0].struGreyColor[ARITH_GREY_A];
        stu_grey_color& g0b = struCnfp.struGroupIdentify[0][0].struGreyColor[ARITH_GREY_B];
        stu_grey_color& g1a = struCnfp.struGroupIdentify[0][1].struGreyColor[ARITH_GREY_A];
        stu_grey_color& g1b = struCnfp.struGroupIdentify[0][1].struGreyColor[ARITH_GREY_B];

        g0a.nSensMin = 1000; g0a.nSensMax = 0;
        g0a.nMode = 0; g0a.nColor = 4;
        g0a.nRow = 4; g0a.nColumn = 4; g0a.nPercent = 16;

        g1a.nSensMin = 0;    g1a.nSensMax = 0;    // ← G1=0, 和 G0=1000 最大差异!
        g1a.nMode = 0; g1a.nColor = 4;
        g1a.nRow = 4; g1a.nColumn = 4; g1a.nPercent = 16;

        g0b.nSensMin = 0; g0b.nSensMax = 0;
        g1b.nSensMin = 0; g1b.nSensMax = 0;
    }

    /* 打印关键参数 */
    LOG_TAG(QString("  [G6] nMatAssembleMode=%1 GREY_A=%2 enableAi=%3")
        .arg(struCnfp.nMatAssembleMode)
        .arg(struCnfp.nArithmeticEnable[ARITH_GREY_A])
        .arg(struCnfp.enableAi));
    {
        stu_grey_color& g0 = struCnfp.struGroupIdentify[0][0].struGreyColor[ARITH_GREY_A];
        stu_grey_color& g1 = struCnfp.struGroupIdentify[0][1].struGreyColor[ARITH_GREY_A];
        LOG_TAG(QString("  [STEP0-设完] G0.sensMin=%1 G1.sensMin=%2 (期望 1000 vs 0)")
            .arg(g0.nSensMin).arg(g1.nSensMin));
    }

    /* === dump_grey: 每步打印 G0/G1 sensMin === */
    auto dump_grey = [](const char* tag) {
        stu_grey_color& g0 = struCnfp.struGroupIdentify[0][0].struGreyColor[ARITH_GREY_A];
        stu_grey_color& g1 = struCnfp.struGroupIdentify[0][1].struGreyColor[ARITH_GREY_A];
        LOG_TAG(QString("  [%1] G0.sensMin=%2 G1.sensMin=%3  |  G0.color=%4 G1.color=%5  |  G0.mode=%6 G1.mode=%7")
            .arg(tag).arg(g0.nSensMin).arg(g1.nSensMin)
            .arg(g0.nColor).arg(g1.nColor)
            .arg(g0.nMode).arg(g1.nMode));
    };

    /* ---- 下发前: 验证 getArithEnable 能读到我们设的 GREY_A=1 ---- */
    {
        int ae_before = myFlow.getArithEnable(0, 0, ARITH_GREY_A);
        int ae_group1 = myFlow.getArithEnable(0, 1, ARITH_GREY_A);
        LOG_TAG(QString("  [G7-DBG] getArithEnable(0,0,GREY_A)=%1  getArithEnable(0,1,GREY_A)=%2 (期望两个都是1)")
            .arg(ae_before).arg(ae_group1));
        LOG_TAG(QString("  [G7-DBG] 内存 nArithmeticEnableGroup[0][0][GREY_A]=%1  [0][1][GREY_A]=%2")
            .arg(struCnfp.nArithmeticEnableGroup[0][0][ARITH_GREY_A])
            .arg(struCnfp.nArithmeticEnableGroup[0][1][ARITH_GREY_A]));
    }

    /* ---- 下发 + 开料 ---- */
    LOG_TAG("  initSendAllParams...");
    myFlow.initSendAllParams();
    dump_grey("STEP1-initSendAllParams后");

    LOG_TAG("  initMachineType...");
    myFlow.initMachineType();
    myFlow.msleep(100);
    myFlow.resetFeeder(0);
    dump_grey("STEP2-resetFeeder后");

    LOG_TAG(QString("  开料前 bStatFeed=%1").arg(struGsh.bStatFeed));
    myFlow.onOff();
    LOG_TAG(QString("  开料后 bStatFeed=%1").arg(struGsh.bStatFeed));
    dump_grey("STEP3-onOff后");

    /* === G8 修复: onOff 里 resetTick() 发了 CMD_UNIT_EJECT_ONOFF arg8=bStatEjectFpga
     * 但 onOff 本身不调 ejectorOn(). 手动补 ejectorOn() 确保喷阀通道开启 === */
    LOG_TAG(QString("  [G8-FIX] ejectorOn() 前 bStatEjectFpga=%1").arg(struGsh.bStatEjectFpga));
    struGsh.bStatEjectFpga = 1;   // 强制开
    myFlow.ejectorOn();            // 显式发 CMD_UNIT_EJECT_ONOFF arg8=1 给所有 unit
    myFlow.msleep(200);
    LOG_TAG("  [G8-FIX] ejectorOn() 完成, 等喷阀动...");

    const int FEED_SECONDS = 10;
    for (int s = FEED_SECONDS; s > 0; s--) {
        if (s == FEED_SECONDS || s % 2 == 0)
            LOG_TAG(QString("  [运行中] 剩余 %1 秒  bStatFeed=%2").arg(s).arg(struGsh.bStatFeed));
        myFlow.msleep(1000);
    }

    LOG_TAG(QString("  关料前 bStatFeed=%1").arg(struGsh.bStatFeed));
    myFlow.onOff();
    LOG_TAG(QString("  关料后 bStatFeed=%1").arg(struGsh.bStatFeed));
    LOG_TAG("  === G6 测试结束 (GREY_A=1 + Feeder=40%) ===");

#if TEST_ONLY_CAM0
    /* 恢复原始值, 不影响其他模式 */
    struCnfg.struLevelInfo[0].nUnitLevelTotal = _save_nUnitLevelTotal;
    struCnfg.struLevelInfo[0].nIdentifyGroupTotal = _save_nIdentifyGroupTotal;
    LOG_TAG(QString("  TEST_ONLY_CAM0 恢复: nUnitLevelTotal=%1 nIdentifyGroupTotal=%2")
        .arg(_save_nUnitLevelTotal).arg(_save_nIdentifyGroupTotal));
#endif

    return 0;
}

/* ------------------------------------------------------------------ */
/*  main                                                               */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    /* QCoreApplication 依赖 Qt 事件循环 (串口通信需要) */
    QCoreApplication app(argc, argv);
    app.setApplicationName("DuySorterTest");

    /* 纯 stdio 打印, 不依赖 unilog 日志库 */
    fprintf(stderr, "===== DuySorter ARM Test Start =====\n");
    fprintf(stderr, "Build: %s %s\n", __DATE__, __TIME__);
    fflush(stderr);

    /* ---- 1. 初始化 (正常启动流程，跳过 UI) ---- */
    LOG_TAG("调用 myFlow.initAll() 加载配置 + 打开串口...");
    myFlow.initAll();
    LOG_TAG(QString("initAll 完成, nLevelTotal=%1, nProfile=%2")
        .arg(struCnfg.nLevelTotal).arg(struCnfg.nProfile));

    /* ---- 2. 补主程序启动必经但 test_main 跳过的两步 ---- */
    LOG_TAG("调用 myFlow.initCommunication() 版本查询 + 波特率 + 用户模式检查...");
    {
        int ret = myFlow.initCommunication();
        LOG_TAG(QString("initCommunication ret=%1 (0=ok, 1=有通信错误)").arg(ret));
    }
    LOG_TAG("调用 myFlow.initAiCommunication() AI 板版本查询 (即使 enableAi=false 也无害)...");
    {
        int ret = myFlow.initAiCommunication();
        LOG_TAG(QString("initAiCommunication ret=%1").arg(ret));
    }

    LOG_TAG("串口初始化完成 + 通信自检已执行 (com1/com2/com3 已由 initAll 打开)");

    /* ---- 先解析 mode, 决定是否走 AI 初始化 ---- */
    int mode = 4;   // 默认只做只读诊断
    if (argc > 1)
    {
        bool ok;
        mode = QString(argv[1]).toInt(&ok);
        if (!ok || mode < 1 || mode > 6)
        {
            LOG_TAG(QString("Usage: %1 [1|2|3|4|5|6]\n"
                            "  1 = AI 板子测阀 (UDP FixCheck → 192.168.4.126)\n"
                            "  2 = 强制单色1开料10秒\n"
                            "  3 = 先 AI 测阀再单色1\n"
                            "  4 = 只读诊断, 不碰硬件 (默认)\n"
                            "  5 = Direct 串口测阀 (com1 CMD_INT_EJTEST)\n"
                            "  6 = 前后视独立 GREY_A (前视蓝低100% / 后视蓝低0%)\n"
                            "      + 供料40% + 开料10秒 (无测阀, 直接功能测试)").arg(argv[0]));
            return 1;
        }
    }
    LOG_TAG(QString("测试模式 = %1").arg(mode));

    /* mode 4: 只读诊断, 不碰硬件, 直接结束 */
    if (mode == 4)
    {
        diag_dump();
        LOG_TAG("只读诊断完成, 退出。");
        return 0;
    }

    /* ------------------------------------------------------------------
     * ValveModeParam.mode 语义: 1=AI接管喷阀  2=传统(FPGA接口板)  3=混合
     * mode=5 Direct 串口测阀时, AI 板子必须保持默认"传统模式"(不发 ValveModeParam)
     *   否则 AI 板子会优先接管喷阀硬件, Direct 串口帧被无视 -> 不响!
     *   这是主程序 mainwidget.cpp:3415 的分叉逻辑:
     *     if (!struCnfp.enableAi)  → Direct 串口 (此时 initEjectorModePara 门控不发 ValveModeParam)
     *     else                     → AI 板子 UDP FixCheck
     * ------------------------------------------------------------------ */
    /* mode=5 或 mode=6: Direct 串口路径 (不走 AI 板子 UDP 喷阀) */
    if (mode != 5 && mode != 6)
    {
    /* ---- 关键: 启动 AI 采集 + 让 FPGA/接口板进入工作态 ----
     * 远程 merge 后 startAiInfer / initEjectorModePara / initEjectorDelayPara
     * 的门控条件都从 ConfigMgr::GetAiCfgInfo().enable_ai_ 改成了
     * struCnfp.enableAi (方案文件读取). 板卡上残缺方案 enableAi 默认 0,
     * 加上 ARITH_PISTACHIO=0, 新版门控直接 return 了,
     * AI 采集帧 / ValveModeParam / JetsParam 全部没发!
     * 这里手动强制打开.
     */
    struCnfp.enableAi = true;
    struCnfp.nArithmeticEnable[ARITH_PISTACHIO] = 1;
    struCnfp.nArithmeticEnable[ARITH_GREY_A]   = 1;   // 让 initEjectorModePara 的 model 含 TRADITIONAL 位
    LOG_TAG(QString("强制门控通过: enableAi=%1 PISTACHIO=%2 GREY_A=%3")
        .arg(struCnfp.enableAi).arg(struCnfp.nArithmeticEnable[ARITH_PISTACHIO])
        .arg(struCnfp.nArithmeticEnable[ARITH_GREY_A]));

    LOG_TAG("下发完整方案参数 (initSendAllParams) ...");
    LOG_TAG("  -> 内部会调 initEjectorModePara (UDP ValveModeParam 给 AI 板子)");
    LOG_TAG("  -> 内部会调 initEjectorDelayPara  (UDP JetsParam 给 AI 板子)");
    myFlow.initSendAllParams();
    LOG_TAG("方案参数下发完成 (AI 板子喷阀模式已就绪)");

    /* ---- 关键补丁: 残缺方案导致 initSendAllParams 内部
     *   initEjectorModePara / initEjectorDelayPara 的 for 循环
     *   (依赖 nTickGroupTotal / nIdentifyGroupTotal) 根本没迭代!
     *   = ValveModeParam 和 JetsParam 一条都没发给 AI 板子.
     *   这里手动补发给所有 AI 板子 (126/127, 按 dev_no*2 共 2 块前视).
     *
     *   单位注意: blow_time_ / fixed_delay_ / dynamics_delay_ 都是 0.1 ms 单位!
     */
    {
        ValveModeParam vmp;
        vmp.mode_ = MODEL_VAVLE_AI | MODEL_VALVE_TRADITIONAL;   // 0x03: AI + 传统都开

        JetsParam jp;
        jp.jets_num_       = (quint8)qMax((int)struCnfg.nEjectorsPerChute, 12);
        jp.blow_time_      = 50;    // 5.0 ms 吹气时间
        jp.fixed_delay_    = 10;    // 1.0 ms 固定延时
        jp.dynamics_delay_ = 0;     // 0 ms 动态延时

        int aiBoards = 2;           // dev_no 0 和 1, 前视板子 index = dev_no*2
        for (int d = 0; d < aiBoards; d++)
        {
            QString ip = ai_helper::GetAiIpByIndex(d * 2);
            QHostAddress addr(ip);

            /* ValveModeParam */
            QByteArray vmp_req = cmdworker::ValveModeParamRequest(vmp);
            QByteArray vmp_rep;
            bool vmp_ok = CmdUdpManager::instance().onSendCommand(
                addr, AI_UPD_CMD_PORT, vmp_req, vmp_rep, AI_RESPONSE_TIMEOUT);
            LOG_TAG(QString("  [补] ValveModeParam → %1 ok=%2 mode=0x%3 req=%4 rep=%5")
                .arg(ip).arg(vmp_ok).arg(vmp.mode_, 2, 16, QChar('0'))
                .arg(QString(vmp_req.toHex(' ').toUpper()))
                .arg(QString(vmp_rep.toHex(' ').toUpper())));

            /* JetsParam */
            QByteArray jp_req = cmdworker::JetsParamRequest(jp);
            QByteArray jp_rep;
            bool jp_ok = CmdUdpManager::instance().onSendCommand(
                addr, AI_UPD_CMD_PORT, jp_req, jp_rep, AI_RESPONSE_TIMEOUT);
            LOG_TAG(QString("  [补] JetsParam → %1 ok=%2 jets=%3 blow=5.0ms delay=1.0ms req=%4 rep=%5")
                .arg(ip).arg(jp_ok).arg(jp.jets_num_)
                .arg(QString(jp_req.toHex(' ').toUpper()))
                .arg(QString(jp_rep.toHex(' ').toUpper())));
        }
    }

    /* 诊断: 强制打印 initEjectorModePara / initEjectorDelayPara 依赖的循环计数 */
    {
        const int tgTotal = struCnfg.struLevelInfo[0].nTickGroupTotal;
        const int idTotal = struCnfg.struLevelInfo[0].nIdentifyGroupTotal;
        LOG_TAG(QString("[DIAG] nLevelTotal=%1 nTickGroupTotal=%2 nIdentifyGroupTotal=%3")
            .arg(struCnfg.nLevelTotal).arg(tgTotal).arg(idTotal));
        for (int j = 0; j < qMax(tgTotal, idTotal); j++) {
            int tgUnitCnt = (j < tgTotal) ? struCnfg.struLevelInfo[0].struTickGroupInfo[j].nUnitCount : -1;
            int idUnitCnt = (j < idTotal) ? struCnfg.struLevelInfo[0].struIdentifyGroupInfo[j].nUnitCount : -1;
            LOG_TAG(QString("[DIAG] group[%1]: tgUnitCnt=%2 idUnitCnt=%3")
                .arg(j).arg(tgUnitCnt).arg(idUnitCnt));
        }
    }

    LOG_TAG("启动 AI 采集 (startAiInfer)...");
    myFlow.startAiInfer();
    myFlow.msleep(300);
    LOG_TAG("AI 采集已启动");

    LOG_TAG("下发 CMD_INT_ONOFF/CMD_UNIT_ONOFF 让接口板进入工作态...");
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, 0, 1, 3);
    }
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            int nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, nAddr, 0, 0, 0, 0, 1, 3);
        }
    }
    myFlow.msleep(300);
    LOG_TAG("工作态已下发, 板子现在能响应喷阀/检测");
    }
    else
    {
        /* mode=5 Direct 串口: 让 AI 板子强制切换到"传统模式"(mode=2),
         * 防止上一次 mode=1 跑过 AI+传统混合(mode=0x03)后残留,
         * 导致 AI 板子优先接管喷阀硬件, Direct 串口被无视!
         *
         * ValveModeParam.mode 语义: 1=AI接管喷阀  2=传统(FPGA接口板)  3=混合
         */
        {
            ValveModeParam vmp;
            vmp.mode_ = MODEL_VALVE_TRADITIONAL;   // 0x02: 切回传统模式, 让 FPGA 接管

            for (int d = 0; d < 2; d++)
            {
                QString ip = ai_helper::GetAiIpByIndex(d * 2);
                QHostAddress addr(ip);

                QByteArray vmp_req = cmdworker::ValveModeParamRequest(vmp);
                QByteArray vmp_rep;
                bool vmp_ok = CmdUdpManager::instance().onSendCommand(
                    addr, AI_UPD_CMD_PORT, vmp_req, vmp_rep, AI_RESPONSE_TIMEOUT);
                LOG_TAG(QString("[Direct 模式] 强制 ValveModeParam→传统(0x02) → %1 ok=%2 rep_len=%3")
                    .arg(ip).arg(vmp_ok).arg(vmp_rep.size()));
            }
        }
        myFlow.msleep(100);

        LOG_TAG("[Direct 模式] 发送 CMD_INT_ONOFF 让接口板进入工作态...");
        for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
            MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, 0, 1, 3);
        myFlow.msleep(200);
        LOG_TAG("[Direct 模式] 接口板已就绪, AI 板子已切回传统模式");
    }

    /* 任何非 mode=4 模式都先打印一次方案加载后的真实使能态 */
    diag_dump();

    if (mode == 1 || mode == 3)
    {
        if (test_valve_sequence() != 0)
            return 1;
    }

    if (mode == 5)
    {
        if (test_valve_direct() != 0)
            return 1;
    }

    if (mode == 2 || mode == 3)
    {
        if (test_grey_a() != 0)
            return 1;
    }

    if (mode == 6)
    {
        if (test_grey_a_front_rear() != 0)
            return 1;
    }

    /* mode 1/2/3/5/6: 都是短时测试, 测完自动退出, 不进入常驻循环 */
    if (mode == 1 || mode == 2 || mode == 3 || mode == 5 || mode == 6)
    {
        LOG_TAG(QString("mode=%1 测试结束, 退出。").arg(mode));
        return 0;
    }

    /* ---- 3. 保持运行，让用户抛料测试 ---- */
    LOG_TAG("测试程序运行中，按 Ctrl+C 退出...");
    LOG_TAG(QString("  bStatFeed = %1").arg(struGsh.bStatFeed));
    while (true)
    {
        app.processEvents();
        myFlow.msleep(500);
    }

    return 0;
}
