/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        systeminitialization.cpp
 * \brief       系统初始化界面源文件
 *              子页面包括通信自检、自检列表、系统初始化及系统预热。
 * \date        2015.01.14
 */
#include "systeminitialization.h"
#include "bootstage.h"
#include "configmgr.h"
#include "unilog.h"
#include <QtMath>
#include <QFontMetrics>
#include "common/globalconfig.h"

/*!
 * \brief 汇总自检失败的硬件设备名称（只读 struGsh，不改变自检逻辑）
 */
static QString collectHwErrorDetail()
{
    QStringList errors;

    /* 接口板：波特率既不是 115200 也不是 921600 判为异常 */
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal && i < MAX_INT; i++)
    {
        if (struGsh.nIntBaud[i] != MY_UARTBAUD_115200
            && struGsh.nIntBaud[i] != MY_UARTBAUD_921600)
        {
            errors << QString("接口板%1").arg(i + 1);
        }
    }

    /* 控制板 */
    for (int i = 0; i < struCnfg.nCtrlBoardTotal && i < MAX_CTRL; i++)
    {
        if (struGsh.nCtrlBaud[i] != MY_UARTBAUD_115200
            && struGsh.nCtrlBaud[i] != MY_UARTBAUD_921600)
        {
            errors << QString("控制板%1").arg(i + 1);
        }
    }

    /* 相机板：版本号任一字节为 255 判为异常 */
    for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.nLevelTotal; j++)
        {
            if (struGsh.struVer.sUnit[j][i][0] == 255
                || struGsh.struVer.sUnit[j][i][1] == 255)
            {
                errors << QString("相机板%1")
                              .arg(struCnfg.struLevelInfo[j].nUnitId[i] + 1);
            }
        }
    }

    return errors.join("、");
}

 /**
  * @brief communication::通信自检控件
  * @param parent
  */
communication::communication(QWidget* parent) :
    QWidget(parent)
{
    waitCommLabel = new myLabel(myLan.msg_communicating, this);
    waitCommLabel->setStyleSheet("color: white;");  

    /*页面布局*/
    commVBLayout = new QVBoxLayout(this);
    commVBLayout->addWidget(waitCommLabel);
    //    setFixedHeight(LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);
}
/**
 *   自动通信检测功能
 */
void communication::startComSlt()
{
    int ret = 0;
    int ret1 = 0;

    // ---------------- 组 3：界面与数据表 / 加密授权 ----------------
    myFlow.bootStage(BOOT_STAGE_UI, BOOT_STATUS_ACTIVE, "正在校验加密授权…");

    bool encyOk = true;
    if (myMachineInfo.macEncyCheck())
    {
        setDccryt dlg;
        //! 启动页为置顶窗口，授权对话框需同样置顶才能显示在其上方
        dlg.setWindowFlag(Qt::WindowStaysOnTopHint, true);
        ret = dlg.exec();
        encyOk = (ret == QDialog::Accepted);
        //! 行为保持不变：授权未通过也继续启动，仅在启动页红色提示
    }

    if (encyOk)
    {
        myFlow.bootStage(BOOT_STAGE_UI, BOOT_STATUS_OK, "UI加密正常");
    }
    else
    {
        myFlow.bootStage(BOOT_STAGE_UI, BOOT_STATUS_ERROR, "加密授权未通过");
    }

    // initInterfaceTransMode(0) = AI采集模式 (简化版 arg5=0)
    // 注意: 全链路都是 FPGA eth0 UDP 推图给 AI板, 没有串口收图!
    // mode=0 是 AI采集模式, mode=1 是传统采集模式 (简化版, AIMainWidget 拍照时自己发完整版)
    // FPGA 上电默认也是 eth0 UDP 推图, 这里发一次 initInterfaceTransMode(0) 双重保险
    myFlow.initInterfaceTransMode(0);  // ← 开机走 AI采集模式

    // ---------------- 组 4：设备自检 / AI 板自检 ----------------
    myFlow.bootStage(BOOT_STAGE_HW, BOOT_STATUS_ACTIVE, "正在执行设备自检…");

    ret = myFlow.initCommunication();

    QStringList aiFailed;
    if (myFlow.isAiCheckEnabled())
    {
        ret1 = myFlow.initAiCommunication(&aiFailed);
    }

    QString detail = collectHwErrorDetail();
    if (!aiFailed.isEmpty())
    {
        if (!detail.isEmpty())
        {
            detail += "、";
        }
        detail += aiFailed.join("、");
    }

    if (ret == 0 && ret1 == 0)
    {
        myFlow.bootStage(BOOT_STAGE_HW, BOOT_STATUS_OK, "硬件工作正常");
    }
    else
    {
        if (detail.isEmpty())
        {
            detail = "设备通信异常（详见自检列表）";
        }
        myFlow.bootStage(BOOT_STAGE_HW, BOOT_STATUS_ERROR, "硬件异常：" + detail);
    }

    //#endif
    if (ret == 1 || ret1 == 1)
    {
        emit updateCommunicationSig();
        emit goToCommListPageSig();     // 自检失败，进入自检列表页面
    }
    else
    {
        emit goToInitPageSig();         // 自检成功，进入系统初始化页面
    }
}
/*************************************************************************************/
/***
 *   通信列表控件
 */
void communicationList::commuUpdateInterface()
{
    int i, j;

    for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        //        interfaceListLabel[i][0]->setText(QString("%1").arg("接口板"));
        interfaceListLabel[i][0]->setText(QString("%1").arg(myLan.interface_board));

        //        interfaceListLabel[i][0]->setText(QString("%1").arg(i+1));
        if (struGsh.nIntBaud[i] == MY_UARTBAUD_115200)
        {
            interfaceListLabel[i][1]->setText(myLan.normal);
            interfaceListLabel[i][1]->setStyleSheet("color:white");
        }
        else if (struGsh.nIntBaud[i] == MY_UARTBAUD_921600)
        {
            //            interfaceListLabel[i][1]->setText(myLan.normal+"1");
            interfaceListLabel[i][1]->setText(myLan.normal);

            interfaceListLabel[i][1]->setStyleSheet("color:white");
        }
        else
        {
            interfaceListLabel[i][1]->setText(myLan.abnormal);
            interfaceListLabel[i][1]->setStyleSheet("color:red");
        }
    }

    for (i = struCnfg.nInterfaceBoardTotal; i < MAX_INT; i++)
    {
        for (j = 0; j < 2; j++)
        {
            interfaceListLabel[i][j]->hide();
        }
    }

    /** 黑白机型下不显示固件版本 */
        //todo
    interfaceListLabel[MAX_INT][0]->setText("USB");
    //        interfaceListLabel[MAX_INT][0]->setText("USB接口");
    if (struGsh.struVer.nUsbFirmware == 255)
    {
        interfaceListLabel[MAX_INT][1]->setText(myLan.abnormal);
        interfaceListLabel[MAX_INT][1]->setStyleSheet("color:red");
    }
    else
    {
        interfaceListLabel[MAX_INT][1]->setText(myLan.normal);
        interfaceListLabel[MAX_INT][1]->setStyleSheet("color:black");
    }
    interfaceListLabel[MAX_INT][0]->hide();
    interfaceListLabel[MAX_INT][1]->hide();
}

void communicationList::commuUpdateCtrl()
{
    int i, j;

    for (i = 0; i < struCnfg.nCtrlBoardTotal; i++)
    {
        controlListLabel[i][0]->setText(QString("%1").arg(myLan.control_board));
        //    controlListLabel[i][0]->setText(QString("%1").arg("主控板"));

        //        controlListLabel[i][0]->setText(QString("%1").arg(i+1));
        if (struGsh.nCtrlBaud[i] == MY_UARTBAUD_115200)
        {
            controlListLabel[i][1]->setText(myLan.normal);
            controlListLabel[i][1]->setStyleSheet("color:white");
        }
        else if (struGsh.nCtrlBaud[i] == MY_UARTBAUD_921600)
        {
            //            controlListLabel[i][1]->setText(myLan.normal+"1");
            controlListLabel[i][1]->setText(myLan.normal);
            controlListLabel[i][1]->setStyleSheet("color:white");
        }
        else
        {
            controlListLabel[i][1]->setText(myLan.abnormal);
            controlListLabel[i][1]->setStyleSheet("color:red");
        }
    }
    for (i = struCnfg.nCtrlBoardTotal; i < MAX_CTRL; i++)
    {
        for (j = 0; j < 2; j++)
        {
            controlListLabel[i][j]->hide();
        }
    }
}

void communicationList::commuUpdateCamera()
{
    /*
    *******************************************************
    0:   通道  前视       后视        AI前视  AI后视
    1:   1     V1.00_1    V1.00_2
    2:   2     V1.00_1    V1.00_2
    *******************************************************
    */

    for (int i = AIDEV_BEGIN_COL_NO; i < SUM_SHOW_COLUMN_NUM; i++)
    {
        hideColumn(i);
    }

    cameraListLabel[0][0]->setText(myLan.chute);
    cameraListLabel[0][1]->setText(myLan.front_view);
    cameraListLabel[0][2]->setText(myLan.rear_view);
    if (struCnfp.enableAi)
    {
        cameraListLabel[0][3]->setText("AI" + myLan.front_view);
        cameraListLabel[0][4]->setText("AI" + myLan.rear_view);
    }

    // 行号
    int row_no = 0;
    for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
    {
        // 一行显示前后视相机 
        row_no = i / 2 + 1;
        // 通道列
        cameraListLabel[row_no][0]->setText(QString("%1").arg(row_no));
        for (int j = 0; j < struCnfg.nLevelTotal; j++)
        {
            int cam_no = struCnfg.struLevelInfo[j].nUnitId[i];
            // 相机前视对应列号1， 后视对应列号2 
            int col_no = ((cam_no % 2) == 0) ? 1 : 2;
            if ((struGsh.struVer.sUnit[j][i][0] != 255) && (struGsh.struVer.sUnit[j][i][1] != 255))
            {
                cameraListLabel[row_no][col_no]->setText(myLan.normal);
                cameraListLabel[row_no][col_no]->setStyleSheet("color:white");
            }
            else
            {
                cameraListLabel[row_no][col_no]->setText(myLan.abnormal);
                cameraListLabel[row_no][col_no]->setStyleSheet("color:red");
            }

            if (struCnfp.enableAi)
            {
                // AI前视对应列号3， AI后视对应列号4
                col_no = ((cam_no % 2) == 0) ? 3 : 4;
                if (struGsh.aiResult[j] != QString(""))
                {
                    cameraListLabel[row_no][col_no]->setText(myLan.normal);
                    cameraListLabel[row_no][col_no]->setStyleSheet("color:white");
                }
                else
                {
                    cameraListLabel[row_no][col_no]->setText(myLan.abnormal);
                    cameraListLabel[row_no][col_no]->setStyleSheet("color:red");
                }
            }
        }
    }

    row_no += 1;
    for (int i = row_no; i < SUM_SHOW_ROW_NUM; i++)
    {
        for (int j = 0; j < SUM_SHOW_COLUMN_NUM; j++)
        {
            cameraListLabel[i][j]->hide();
        }
    }
}

/* 更新恒流源板版本信息 */
void communicationList::commuUpdateLightSrc()
{
    for (int i = 0; i < struCnfg.nLampAINum; i++)
    {
        m_lightSrcLabel[i][0]->setText(QString("%1").arg(i + 1));

        /** 若恒流源板未使用则显示"--" */
        if (struCnfg.nCurSrcBoradLampNum[i] == 0)
        {
            m_lightSrcLabel[i][1]->setText("--");
            m_lightSrcLabel[i][1]->setStyleSheet("color:white");
        }
        else if (struGsh.struVer.sLightSrc[i][0] != 255 && struGsh.struVer.sLightSrc[i][1] != 255)
        {
            m_lightSrcLabel[i][1]->setText(myLan.normal);
            m_lightSrcLabel[i][1]->setStyleSheet("color:white");
        }
        else
        {
            m_lightSrcLabel[i][1]->setText(myLan.abnormal);
            m_lightSrcLabel[i][1]->setStyleSheet("color:red");
        }
    }

    for (int i = struCnfg.nLampAINum; i < MAX_LIGHT_SOURCE; i++)
    {
        m_lightSrcLabel[i][0]->hide();
        m_lightSrcLabel[i][1]->hide();
    }
}

/* 隐藏某一列 */
void communicationList::hideColumn(int index)
{
    for (int i = 0; i < SUM_SHOW_ROW_NUM; i++)
    {
        cameraListLabel[i][index]->hide();
    }
}

/* 显示某一列 */
void communicationList::showColumn(int index)
{
    for (int i = 0; i < SUM_SHOW_ROW_NUM; i++)
    {
        cameraListLabel[i][index]->show();
    }
}

void communicationList::updateCommunicationResultSlt()
{
    commuUpdateInterface();
    commuUpdateCtrl();
    commuUpdateCamera();
    /*恒流源板状态检测*/
    commuUpdateLightSrc();
}

communicationList::communicationList(QWidget* parent) :
    QWidget(parent)
{
    int i, j;
    config = g_Config::getInstance();

    /*布局管理*/
    /* 接口板 */
    commListGridLayout = new QGridLayout();
    commListGridLayout->setSpacing(2);
    commListGridLayout->setContentsMargins(50, 0, 50, 10);

    interfaceBoardListGroup = new myGroupBox("");
    interfaceBoardListGroup->setObjectName("interfaceBoardListGroup");
    //    interfaceBoardListGroup->setStyleSheet("QGroupBox#interfaceBoardListGroup { border: 2px solid black; }");
    interfaceBoardListGroup->setStyleSheet("QGroupBox#interfaceBoardListGroup { border: none; }");

    //    interfaceListLayout = new QGridLayout(interfaceBoardListGroup);
    interfaceListLayout = new QGridLayout;
    interfaceListLayout->setVerticalSpacing(50);


    newInterfaceBoardListGroup = new myGroupBox("");
    newInterfaceListLayout = new QGridLayout(newInterfaceBoardListGroup);
    newInterfaceBoardListGroup->setObjectName("newInterfaceBoardListGroup");
    //    newInterfaceBoardListGroup->setStyleSheet("QGroupBox#newInterfaceBoardListGroup title: { padding:0 1px;}");
    //    newInterfaceBoardListGroup->setStyleSheet("QGroupBox#newInterfaceBoardListGroup::title { px 0 3 px }");
    newInterfaceBoardListGroup->setStyleSheet("QGroupBox#newInterfaceBoardListGroup { border: 2px solid rgb(209, 243, 249); }");

    //    newInterfaceBoardListGroup->setAlignment(Qt::AlignHCenter);
    newInterfaceListLayout->setSpacing(5);
    newInterfaceListLayout->setContentsMargins(5, 0, 15, 15);

    /* 控制板 */
    ctrlBoardListGroup = new myGroupBox(myLan.control_board);
    controlListLayout = new QGridLayout(ctrlBoardListGroup);

    /* 相机板  */
//    cameraBoardListGroup = new myGroupBox("");
//    cameraBoardListGroup->setAlignment(Qt::AlignHCenter);
//    cameraBoardListGroup->setObjectName("cameraBoardListGroup");
//    cameraBoardListGroup->setStyleSheet("QGroupBox#cameraBoardListGroup { border: none; }");
//    cameraListLayout = new QGridLayout(cameraBoardListGroup);
    cameraListLayout = new QGridLayout;

    /* 横流源板 */
    m_lightSrcListGroup = new myGroupBox(myLan.light_src_board);
    m_lightSrcLay = new QGridLayout(m_lightSrcListGroup);

    for (i = 0; i < MAX_LIGHT_SOURCE; i++)
    {
        for (j = 0; j < 2; j++)
        {
            m_lightSrcLabel[i][j] = new QLabel;
            m_lightSrcLabel[i][j]->setStyleSheet("color:white");
            m_lightSrcLabel[i][j]->setFont(config->getFont());
            m_lightSrcLabel[i][j]->setAlignment(Qt::AlignCenter);
            m_lightSrcLay->addWidget(m_lightSrcLabel[i][j], i, j);
        }
    }

    for (i = 0; i < MAX_CTRL; i++)
    {
        for (j = 0; j < 2; j++)
        {
            controlListLabel[i][j] = new QLabel;
            controlListLabel[i][j]->setStyleSheet("color:white");
            controlListLabel[i][j]->setAlignment(Qt::AlignCenter);
            controlListLabel[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            interfaceListLayout->addWidget(controlListLabel[i][j], j, i);
        }
    }

    for (i = 0; i < MAX_INT + 1; i++)
    {
        for (j = 0; j < 2; j++)
        {
            interfaceListLabel[i][j] = new QLabel;
            interfaceListLabel[i][j]->setStyleSheet("color:white");
            interfaceListLabel[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            interfaceListLabel[i][j]->setAlignment(Qt::AlignCenter);
            interfaceListLayout->addWidget(interfaceListLabel[i][j], j, i + MAX_CTRL);
        }
    }

    for (i = 0; i < SUM_SHOW_ROW_NUM; i++)
    {
        for (j = 0; j < SUM_SHOW_COLUMN_NUM; j++)
        {
            cameraListLabel[i][j] = new QLabel;
            cameraListLabel[i][j]->setStyleSheet("color:white");
            cameraListLabel[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            cameraListLabel[i][j]->setAlignment(Qt::AlignHCenter);
            cameraListLayout->addWidget(cameraListLabel[i][j], i, j);
        }
    }

    cameraStateLbl = new QLabel;
    cameraStateLbl->setText(myLan.cameraList);
    cameraStateLbl->setStyleSheet("color:white");
    //    cameraStateLbl->setText("相机状态");

    cameraStateLbl->setFont(config->getFont(DEFAULT_FONT_SIZE));
    cameraStateLbl->setAlignment(Qt::AlignHCenter);

    testStateLbl = new QLabel;
    testStateLbl->setText(myLan.state);
    testStateLbl->setStyleSheet("color:white");
    testStateLbl->setFont(config->getFont(DEFAULT_FONT_SIZE));
    testStateLbl->setAlignment(Qt::AlignHCenter);

    /* 所有机型下增加恒流源板版本查询 */
//    if(struCnfg.nEnableLampAI == 1){
//        commListGridLayout->addWidget(interfaceBoardListGroup, 0, 0, 3, 1);
//        commListGridLayout->addWidget(ctrlBoardListGroup, 3, 0, 2, 1);
//        commListGridLayout->addWidget(m_lightSrcListGroup, 5, 0, 3, 1);
//        commListGridLayout->addWidget(cameraBoardListGroup, 0, 1, 8, 2);
//    }else{
//        commListGridLayout->addWidget(interfaceBoardListGroup, 0, 0, 4, 1);
//        commListGridLayout->addWidget(ctrlBoardListGroup, 4, 0, 4, 1);
//        commListGridLayout->addWidget(cameraBoardListGroup, 0, 1, 8, 2);
    newInterfaceListLayout->addWidget(cameraStateLbl, 0, 3, 1, 2, Qt::AlignCenter);
    //        newInterfaceListLayout->addWidget(cameraBoardListGroup, 1, 0, 7, 6);
    //        newInterfaceListLayout->addWidget(interfaceBoardListGroup, 1, 6, 7, 3);
    newInterfaceListLayout->addLayout(cameraListLayout, 1, 0, 7, 6);
    newInterfaceListLayout->addLayout(interfaceListLayout, 1, 6, 1, 3);

    //        newInterfaceListLayout->addWidget(ctrlBoardListGroup, 4, 4, 4, 1);
    commListGridLayout->addWidget(testStateLbl, 0, 0, 1, 1);
    commListGridLayout->addWidget(newInterfaceBoardListGroup, 1, 0, 15, 1);

    //    }
        /* 按钮 */
    rechekBtn = new myPushButton(myLan.recheck, QIcon(), true, true, this);
    nextBtn = new myPushButton(myLan.enter_system, QIcon(), true, true, this);

    if (struCnfg.nLang == LANG_TURKEY)
    {
        rechekBtn->setButtonFont(config->getFont(FONT_SIZE_16));
        nextBtn->setButtonFont(config->getFont(FONT_SIZE_16));
    }

    backHBLayout = new QVBoxLayout();
    backHBLayout->addStretch();
    backHBLayout->addWidget(rechekBtn);
    QSpacerItem* horizontalSpacer = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    backHBLayout->addItem(horizontalSpacer);
    backHBLayout->addStretch();
    backHBLayout->addWidget(nextBtn);

    /*尺寸限制*/
    rechekBtn->setFixedSize(QSize(BTN_WIDTH - 40, BTN_HEIGHT));
    nextBtn->setFixedSize(QSize(BTN_WIDTH - 40, BTN_HEIGHT));

    /* 与方案管理页按钮统一的点击动效样式（9-slice 三态） */
    QString strBtnStyle = QString(
        "QPushButton {"
        "  text-align: center;"
        "  outline: none;"
        "  border-width: 8px 16px 8px 16px;"
        "  border-style: solid;"
        "  border-color: transparent;"
        "  border-image: url(:/res/png2/button/btn_normal.png) 8 16 8 16 stretch stretch;"
        "  color: #E8F6FF;"
        "  font-size: 14px;"
        "  padding: 0px;"
        "}"
        "QPushButton:hover {"
        "  border-image: url(:/res/png2/button/btn_pressed.png) 8 16 8 16 stretch stretch;"
        "  color: #FFFFFF;"
        "}"
        "QPushButton:pressed {"
        "  border-image: url(:/res/png2/button/btn_pressed.png) 8 16 8 16 stretch stretch;"
        "  color: #FFFFFF;"
        "}"
        "QPushButton:disabled {"
        "  border-image: url(:/res/png2/button/btn_disabled.png) 8 16 8 16 stretch stretch;"
        "  color: #5A7686;"
        "}"
    );
    rechekBtn->setStyleSheet(strBtnStyle);
    nextBtn->setStyleSheet(strBtnStyle);

    communicationLayout = new QHBoxLayout(this);
    //    communicationLayout->addStretch();
    communicationLayout->addLayout(commListGridLayout, 9);
    //    communicationLayout->addStretch();
    communicationLayout->addLayout(backHBLayout, 1);

    /*槽函数*/
    connect(rechekBtn, SIGNAL(clicked()), this, SLOT(onRecheckBtnClickedSlt()));
    connect(nextBtn, SIGNAL(clicked()), this, SLOT(onNextBtnClickedSlt()));
}
/**
 * 重新检测
 */
void communicationList::onRecheckBtnClickedSlt()
{
    emit goToCommPageSig();
}
/**
 * 进入初始化页面
 */
void communicationList::onNextBtnClickedSlt()
{
    emit goToInitPageSig();
}

/************************************************************************************/
/***
 *   初始化页面
 */
namespace {
const int INIT_PAGE_W = LCD_WIDTH - 10;                       // 页面宽度
const int INIT_PAGE_H = LCD_HEIGHT - LCD_TITLE_HEIGHT - 20;   // 页面高度

const int RING_CX    = 236;     // 圆环中心
const int RING_CY    = 336;
const int RING_OUTER = 178;     // 外圈刻度半径
const int RING_DASH  = 140;     // 中圈虚线半径
const int RING_TRACK = 112;     // 进度轨道半径
const int RING_INNER = 100;     // 内圈实线半径

const int STEPS_X     = 486;    // 执行链路列表
const int STEPS_W     = 504;
const int STEP_ROW_Y0 = 96;
const int STEP_ROW_H  = 62;

const int FOOT_LINE_Y = INIT_PAGE_H - 52;   // 底部进度线
}

initialization::initialization(QWidget* parent) :
    QWidget(parent)
    , m_currentStep(0)
    , m_doneCount(0)
    , m_percent(0)
    , m_targetPercent(0)
    , m_pulseTick(0)
    , m_finishDelay(0)
    , m_finished(false)
{
    setFixedSize(INIT_PAGE_W, INIT_PAGE_H);

    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(33);
    connect(m_animTimer, SIGNAL(timeout()), this, SLOT(onAnimTickSlt()));
}

/***
 *   自动初始化，完成后跳到预热界面
 */
void initialization::startInitSlt()
{
    m_currentStep = 0;
    m_doneCount = 0;
    m_percent = 0;
    m_targetPercent = 0;
    m_pulseTick = 0;
    m_finishDelay = 0;
    m_finished = false;

    m_animTimer->start();
    update();

    /* 先显示 0% 再开始下发，避免一进页面就卡住 */
    QTimer::singleShot(200, this, SLOT(runNextStepSlt()));
}

/*!
 * \brief 顺序执行 5 个下发步骤，每步之间留出动画时间
 */
void initialization::runNextStepSlt()
{
    if (m_currentStep >= INIT_STEP_COUNT)
    {
        return;
    }

    const int idx = m_currentStep;
    switch (idx)
    {
    case 0:
        myFlow.initInterfaceTransMode(0);   // 下发图像传输模式
        break;
    case 1:
        myFlow.initCamera();                // 重置/下发全部相机参数
        break;
    case 2:
        myFlow.resetSortParams();           // 重置色选参数
        break;
    case 3:
        myFlow.resetControl();              // 重置控制板参数
        break;
    default:
        myFlow.resetAiModel();              // 重置AI模型参数
        break;
    }

    m_doneCount = idx + 1;
    m_currentStep = idx + 1;
    m_targetPercent = m_doneCount * 100.0 / INIT_STEP_COUNT;

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    update();

    if (m_currentStep < INIT_STEP_COUNT)
    {
        QTimer::singleShot(220, this, SLOT(runNextStepSlt()));
    }
}

/*!
 * \brief 动画节拍：百分比平滑逼近目标值、执行中图标脉动
 */
void initialization::onAnimTickSlt()
{
    ++m_pulseTick;

    if (m_percent < m_targetPercent)
    {
        m_percent += qMax<qreal>(0.7, (m_targetPercent - m_percent) * 0.12);
        if (m_percent > m_targetPercent)
        {
            m_percent = m_targetPercent;
        }

        update(RING_CX - RING_OUTER - 12, RING_CY - RING_OUTER - 12,
               (RING_OUTER + 12) * 2, (RING_OUTER + 12) * 2);
        update(0, FOOT_LINE_Y - 40, INIT_PAGE_W, INIT_PAGE_H - FOOT_LINE_Y + 40);
    }

    /* 执行中图标脉动，只刷新当前行 */
    if (m_currentStep < INIT_STEP_COUNT)
    {
        update(STEPS_X, STEP_ROW_Y0 + m_currentStep * STEP_ROW_H,
               STEPS_W, STEP_ROW_H);
    }

    /* 全部完成并等到 100% 后跳转预热页 */
    if (!m_finished && m_doneCount >= INIT_STEP_COUNT && m_percent >= 99.9)
    {
        if (++m_finishDelay > 12)
        {
            m_finished = true;
            m_animTimer->stop();
            emit goToPreheatPageSig();
        }
    }
}

QString initialization::stepText(int index)
{
    switch (index)
    {
    case 0:
        return myLan.init_step_trans;
    case 1:
        return myLan.init_step_camera;
    case 2:
        return myLan.init_step_sort;
    case 3:
        return myLan.init_step_control;
    case 4:
        return myLan.init_step_ai;
    default:
        return QString();
    }
}

void initialization::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    drawTitle(p);
    drawRing(p);
    drawSteps(p);
    drawFooter(p);
}

void initialization::drawTitle(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    p.fillRect(QRectF(24, 26, 3, 24), QColor(0x22, 0xD3, 0xEE));

    QFont font("Noto Sans SC");
    font.setPixelSize(22);
    font.setWeight(QFont::Medium);
    p.setFont(font);
    p.setPen(QColor(0xE8, 0xFA, 0xFF));
    p.drawText(QPointF(40, 46), myLan.msg_system_init);
}

void initialization::drawRing(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    const QPointF c(RING_CX, RING_CY);

    /* 外圈刻度：60 根，每 5 根加长 */
    p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 46), 1));
    for (int i = 0; i < 60; ++i)
    {
        const double a = i * 6.0 * M_PI / 180.0;
        const double r1 = RING_OUTER - ((i % 5 == 0) ? 10 : 5);
        p.drawLine(QPointF(c.x() + r1 * qCos(a), c.y() + r1 * qSin(a)),
                   QPointF(c.x() + RING_OUTER * qCos(a), c.y() + RING_OUTER * qSin(a)));
    }

    /* 中圈虚线：28 段 */
    const QRectF dashRc(c.x() - RING_DASH, c.y() - RING_DASH,
                        RING_DASH * 2, RING_DASH * 2);
    p.setPen(QPen(QColor(0x2A, 0x7E, 0x93, 140), 1.5));
    for (int i = 0; i < 28; ++i)
    {
        const double start = i * (360.0 / 28.0) + 1.5;
        p.drawArc(dashRc, int(start * 16), int((360.0 / 28.0 - 7.0) * 16));
    }

    /* 内圈实线 */
    p.setPen(QPen(QColor(0x1B, 0x41, 0x52), 1));
    p.drawEllipse(c, RING_INNER, RING_INNER);

    /* 进度轨道 */
    p.setPen(QPen(QColor(0x12, 0x30, 0x3C), 4, Qt::SolidLine, Qt::FlatCap));
    p.drawEllipse(c, RING_TRACK, RING_TRACK);

    /* 进度弧：从 6 点钟方向顺时针 */
    if (m_percent > 0.01)
    {
        const QRectF trackRc(c.x() - RING_TRACK, c.y() - RING_TRACK,
                             RING_TRACK * 2, RING_TRACK * 2);
        p.setPen(QPen(QColor(0x22, 0xD3, 0xEE), 5, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(trackRc, 270 * 16, int(-m_percent * 360.0 * 16.0));
    }

    /* 中心百分比 */
    QFont pctFont("Noto Sans SC");
    pctFont.setPixelSize(42);
    pctFont.setBold(true);
    p.setFont(pctFont);
    p.setPen(QColor(0xE8, 0xFA, 0xFF));

    const QString pct = QString("%1%").arg(int(m_percent + 0.5));
    QFontMetrics pctFm(pctFont);
    p.drawText(QPointF(c.x() - pctFm.horizontalAdvance(pct) / 2.0, c.y() + 8), pct);

    /* 中心状态词（跟随语言，不与英文同时出现） */
    QFont statusFont("Noto Sans SC");
    statusFont.setPixelSize(12);
    p.setFont(statusFont);
    p.setPen(QColor(0x4E, 0x7A, 0x8C));

    QFontMetrics statusFm(statusFont);
    p.drawText(QPointF(c.x() - statusFm.horizontalAdvance(myLan.init_doing) / 2.0,
                       c.y() + 44), myLan.init_doing);

    p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 90), 1));
    p.drawLine(QPointF(c.x() - 15, c.y() + 58), QPointF(c.x() + 15, c.y() + 58));
}

void initialization::drawSteps(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    /* 标题 */
    QFont headFont("Noto Sans SC");
    headFont.setPixelSize(14);
    p.setFont(headFont);
    p.setPen(QColor(0x4E, 0x7A, 0x8C));
    p.drawText(QPointF(STEPS_X, 66), myLan.init_link);

    p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 40), 1));
    p.drawLine(QPointF(STEPS_X, 78), QPointF(STEPS_X + STEPS_W, 78));

    for (int i = 0; i < INIT_STEP_COUNT; ++i)
    {
        const QRectF rowRc(STEPS_X, STEP_ROW_Y0 + i * STEP_ROW_H,
                           STEPS_W, STEP_ROW_H - 8);

        /* 0待执行 1执行中 2已完成 */
        const int state = (i < m_doneCount) ? 2 : ((i == m_currentStep) ? 1 : 0);

        if (state == 1)
        {
            p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 90), 1));
            p.setBrush(QColor(0x22, 0xD3, 0xEE, 20));
            p.drawRect(rowRc);
            p.fillRect(QRectF(rowRc.left(), rowRc.top(), 3, rowRc.height()),
                       QColor(0x22, 0xD3, 0xEE));
        }
        else
        {
            p.setPen(QPen(QColor(255, 255, 255, 16), 1));
            p.drawLine(QPointF(rowRc.left(), rowRc.bottom() + 4),
                       QPointF(rowRc.right(), rowRc.bottom() + 4));
        }

        /* 步骤编号 */
        QFont numFont("Noto Sans SC");
        numFont.setPixelSize(14);
        numFont.setBold(state == 1);
        p.setFont(numFont);
        p.setPen(state == 1 ? QColor(0x22, 0xD3, 0xEE) : QColor(0x3E, 0x62, 0x72));
        p.drawText(QPointF(rowRc.left() + 14, rowRc.center().y() + 5),
                   QString("%1").arg(i + 1, 2, 10, QChar('0')));

        /* 步骤名称 */
        QFont nameFont("Noto Sans SC");
        nameFont.setPixelSize(16);
        p.setFont(nameFont);
        if (state == 1)
        {
            p.setPen(QColor(0xE8, 0xFA, 0xFF));
        }
        else if (state == 2)
        {
            p.setPen(QColor(0x5E, 0x86, 0x96));
        }
        else
        {
            p.setPen(QColor(0x6B, 0x8A, 0x99));
        }
        p.drawText(QPointF(rowRc.left() + 52, rowRc.center().y() + 6),
                   stepText(i));

        /* 右侧状态图标 */
        drawStateIcon(p, int(rowRc.right()) - 18, int(rowRc.center().y()), state);
    }
}

void initialization::drawStateIcon(QPainter& p, int cx, int cy, int state)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    if (state == 2)
    {
        /* 完成：绿色圆形打勾 */
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x3D, 0xDC, 0x97));
        p.drawEllipse(QPointF(cx, cy), 8, 8);

        p.setPen(QPen(QColor(0x0B, 0x1A, 0x24), 2, Qt::SolidLine,
                      Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(QPointF(cx - 3.5, cy + 0.5), QPointF(cx - 1, cy + 3));
        p.drawLine(QPointF(cx - 1, cy + 3), QPointF(cx + 4, cy - 3));
    }
    else if (state == 1)
    {
        /* 执行中：圆环 + 中心点，按节拍脉动 */
        const int alpha = 130 + int(120 * qAbs(qSin(m_pulseTick * 0.09)));
        p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, alpha), 1.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(cx, cy), 8, 8);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x22, 0xD3, 0xEE));
        p.drawEllipse(QPointF(cx, cy), 3, 3);
    }
    else
    {
        /* 待执行：空心圆 */
        p.setPen(QPen(QColor(0x2A, 0x4E, 0x5E), 1.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(cx, cy), 7, 7);
    }
}

void initialization::drawFooter(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    QFont font("Noto Sans SC");
    font.setPixelSize(14);
    p.setFont(font);
    p.setPen(QColor(0x4E, 0x7A, 0x8C));

    /* 当前执行项（只在初始化过程中显示说明文字） */
    if (m_doneCount > 0 && m_doneCount <= INIT_STEP_COUNT)
    {
        const QString running = myLan.init_running + ": "
                + stepText(m_doneCount - 1);
        p.drawText(QPointF(24, FOOT_LINE_Y - 22), running);
    }

    /* 进度线 */
    const qreal lineX0 = 24;
    const qreal lineX1 = INIT_PAGE_W - 24;
    p.setPen(QPen(QColor(0x12, 0x30, 0x3C), 2));
    p.drawLine(QPointF(lineX0, FOOT_LINE_Y), QPointF(lineX1, FOOT_LINE_Y));

    if (m_percent > 0.01)
    {
        p.setPen(QPen(QColor(0x22, 0xD3, 0xEE), 2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(lineX0, FOOT_LINE_Y),
                   QPointF(lineX0 + (lineX1 - lineX0) * m_percent / 100.0,
                           FOOT_LINE_Y));
    }

    /* 左下角说明 + 右下角百分比 */
    QFont smallFont("Noto Sans SC");
    smallFont.setPixelSize(12);
    p.setFont(smallFont);
    p.setPen(QColor(0x3E, 0x62, 0x72));
    p.drawText(QPointF(lineX0, FOOT_LINE_Y + 30), myLan.system_initial);

    const QString pct = QString("%1%").arg(int(m_percent + 0.5));
    QFontMetrics fm(smallFont);
    p.drawText(QPointF(lineX1 - fm.horizontalAdvance(pct), FOOT_LINE_Y + 30), pct);
}

/**************************************************************************************/
/***
 *   预热页面控件
 */
preheat::preheat(QWidget* parent) :
    QWidget(parent)
{
    bFlagSkip = false;
    waitCommLabel = new myLabel(myLan.preheat, this);
    waitCommLabel->setGeometry(QRect(20, 200, 500, 40));
    skipBtn = new myPushButton(myLan.skip_preheat, myIcon.Edit_Redo, this);
    skipBtn->setGeometry(QRect(LCD_WIDTH - BTN_WIDTH - 20, LCD_CONTENT_HEIGTH - 50, BTN_WIDTH, 40));
    progressBar = new QProgressBar(this);
    progressBar->setGeometry(20, 260, LCD_WIDTH - 40, 40);
    progressBar->setMaximum(900);
    progressBar->setValue(-1);
    progressBar->setMinimumHeight(BTN_HEIGHT);

    /*尺寸限制*/
    skipBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    /*布局管理*/
    preheatGridLayout = new QGridLayout(this);
    preheatGridLayout->setVerticalSpacing((LCD_CONTENT_HEIGTH - 3 * BTN_HEIGHT) / 2);
    preheatGridLayout->addWidget(waitCommLabel, 0, 0, 1, 1);
    preheatGridLayout->addWidget(progressBar, 1, 0, 1, 1);

    preheatHBLayout = new QHBoxLayout();
    horizontalSpacer = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    preheatHBLayout->addItem(horizontalSpacer);
    preheatHBLayout->addWidget(skipBtn);
    preheatGridLayout->addLayout(preheatHBLayout, 2, 0, 1, 1);

    /*槽函数*/
    connect(skipBtn, SIGNAL(pressed()), this, SLOT(onSkipBtnClickSlt()));
}

/***
 *   直接跳到预热页面
 */
void preheat::onSkipBtnClickSlt()
{
    bFlagSkip = true;
}

/***
 *   自动预热页面
 */
void preheat::startPreheatSlt()
{
    progressBar->setRange(0, 900);
    for (int i = 1; i <= 1; i++)
    {
        progressBar->setValue(i);
        if (bFlagSkip)
        {
            break;
        }
        QTime dieTime = QTime::currentTime().addSecs(1);
        while (QTime::currentTime() < dieTime)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }

        if (bFlagSkip)
        {
            break;
        }
    }
    emit goToHomePageSig();
}
