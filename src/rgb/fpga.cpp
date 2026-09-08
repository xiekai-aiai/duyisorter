/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        fpga.cpp
 * \brief       彩色机型下位机远程升级界面源文件
 * \date        2015.01.14
 */
#include "fpga.h"

fpga::fpga(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT);
    
    margin = 25;
    config = g_Config::getInstance();

    stackedWidget = new QStackedWidget(this);
    stackedWidget->setGeometry(0,0,width(),height());

    initUpdateParams();
    bTransByUSB = false;

    createMountUdiskPage();
    createFileListPage();
    createNoticePage();
    createBoardUpdatePage();
    createWholeUpdatePage();

    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetIndexChangedSlt(int)));
}

fpga::~fpga()
{
}

/* 初始化下位机升级相关参数 */
void fpga::initUpdateParams()
{
    defIntMode = struCnfp.nInterfaceBoardMode;
    defColorMode = struCnfp.nColorBoardMode;
    intUserMode = 0;
    colorUserMode = 0;
    infUserMode = 0;
    ctrlUserMode = 0;

    for(int i=0; i<MAX_LEVEL; i++){
        bColorFileIsOk[i] = false;
        bInfFileIsOk[i] = false;
    }

    intAddr = 0x01;
    colorAddr = 0x00;
    infAddr = 0x00;

    ctrlAddr = 0x00;
    bIntIsFactory = false;
    bColorIsFactory = false;
    curUpdateStep = 0;
    curLevel = 0;
}

/* 创建挂载U盘界面 */
void fpga::createMountUdiskPage()
{
    mountUdiskWidget = new QWidget;
    mountUdiskWidget->setGeometry(0,0,width(),height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *midLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(mountUdiskWidget);

    mountUdiskLabel = new QLabel(myLan.msg_insert_udisk);
    mountUdiskLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    mountUdiskLabel->setFixedHeight(LCD_HEIGHT/3);
    mountUdiskLabel->setAlignment(Qt::AlignCenter);
    warningLabel = new QLabel;
    pe.setColor(QPalette::WindowText, Qt::red);
    warningLabel->setPalette(pe);
    warningLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    warningLabel->setFixedHeight(LCD_HEIGHT/3);
    warningLabel->setAlignment(Qt::AlignCenter);

    QSize btnSize(BTN_WIDTH, BTN_HEIGHT);
    mountUdiskBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    mountUdiskBackBtn->setFixedSize(btnSize);
    mountUdiskGoBtn = new myPushButton(myLan.next,myIcon.Arrow_Right);
    mountUdiskGoBtn->setFixedSize(btnSize);

    upLay->addStretch();
    upLay->addWidget(mountUdiskLabel);
    upLay->addStretch();
    upLay->setContentsMargins(0, 0, 0, 0);
    midLay->addStretch();
    midLay->addWidget(warningLabel);
    midLay->addStretch();
    midLay->setContentsMargins(0, 0, 0, 0);
    downLay->addWidget(mountUdiskBackBtn);
    downLay->addStretch();
    downLay->addWidget(mountUdiskGoBtn);
    downLay->setContentsMargins(0, 0, 0, 0);
    mainLay->addLayout(upLay);
    mainLay->addLayout(midLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(mountUdiskGoBtn,SIGNAL(clicked()),this,SLOT(onMountUdiskGoBtnClicked()));
    connect(mountUdiskBackBtn,SIGNAL(pressed()),this,SLOT(onMountUdiskBackBtnClicked()));

    stackedWidget->addWidget(mountUdiskWidget);
}

/* 创建文件列表界面 */
void fpga::createFileListPage()
{
    fileListWidget = new QWidget;
    fileListWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(fileListWidget);

    fileList = new QListWidget;
    fileList->setSelectionMode(QAbstractItemView::NoSelection);
    fileList->setMinimumHeight(LCD_HEIGHT/2);
    for(int i=0; i<MAX_FILE_NUM; i++){
        fileItem[i] = new QListWidgetItem;
        fileItem[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        fileItem[i]->setSizeHint(QSize(200, 50));
        fileList->addItem(fileItem[i]);
    }

    QSize btnSize(BTN_WIDTH, BTN_HEIGHT);
    lastBtn = new myPushButton(myLan.last, myIcon.Arrow_Left);
    lastBtn->setFixedSize(btnSize);
    nextBtn = new myPushButton(myLan.next, myIcon.Arrow_Right);
    nextBtn->setFixedSize(btnSize);
    transModeBtn = new myPushButton("", QIcon());
    transModeBtn->setFixedSize(btnSize);

    //新增升级方式选择功能
    myMutex *updateModeMutex = new myMutex(Qt::Horizontal,0);
    updateModeMutex->setTitle(myLan.upgrade_fpga+"-"+myLan.profile);
    updateModeMutex->setNum(2);
    updateModeMutex->setLabelText(myLan.profile+"1",myLan.profile+"2");
    updateModeMutex->setMinimumSize(BTN_WIDTH*2,BTN_HEIGHT*2);

    upLay->addWidget(fileList);
    upLay->setContentsMargins(LCD_WIDTH/5, LCD_HEIGHT/10, LCD_WIDTH/5, 20);
    downLay->addWidget(lastBtn);
    downLay->addStretch();
    downLay->addWidget(transModeBtn);
    downLay->addStretch();
    downLay->addWidget(nextBtn);
    mainLay->addLayout(upLay);
    mainLay->addWidget(updateModeMutex,1,Qt::AlignCenter);
    mainLay->addLayout(downLay);

    connect(fileList, SIGNAL(currentRowChanged(int)), this, SLOT(onFileListChangedSlt(int)));
    connect(lastBtn, SIGNAL(pressed()), this, SLOT(onFileListLastBtnClicked()));
    connect(nextBtn, SIGNAL(pressed()), this, SLOT(onFileListNextBtnClicked()));
    connect(transModeBtn, SIGNAL(pressed()), this, SLOT(onTransModeBtnClicked()));
    connect(updateModeMutex, SIGNAL(indexChanged(int)), this, SLOT(onModeMutexIndexChanged(int)));

    stackedWidget->addWidget(fileListWidget);
}

/* 创建提示界面 */
void fpga::createNoticePage()
{
    noticeWidget = new QWidget;
    noticeWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(noticeWidget);

    noticeLabel = new myLabel(myLan.msg_insert_data_wire);
    noticeLabel->setAlignment(Qt::AlignCenter);
    noticeLabel->setFixedHeight(100);

    QSize btnSize(BTN_WIDTH, BTN_HEIGHT);
    noticeLastBtn = new myPushButton(myLan.last, myIcon.Arrow_Left);
    noticeLastBtn->setFixedSize(btnSize);
    noticeNextBtn = new myPushButton(myLan.next, myIcon.Arrow_Right);
    noticeNextBtn->setFixedSize(btnSize);

    downLay->addWidget(noticeLastBtn);
    downLay->addStretch();
    downLay->addWidget(noticeNextBtn);
    mainLay->addStretch();
    mainLay->addWidget(noticeLabel);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(noticeLastBtn, SIGNAL(pressed()), this, SLOT(onNoticeLastBtnClicked()));
    connect(noticeNextBtn, SIGNAL(pressed()), this, SLOT(onNoticeNextBtnClicked()));

    stackedWidget->addWidget(noticeWidget);
}

/* 创建板级升级界面 */
void fpga::createBoardUpdatePage()
{
    boardUpdateWidget = new QWidget;
    boardUpdateWidget->setGeometry(0,0,width(),height());

    QVBoxLayout *upLeftLay = new QVBoxLayout;
    QVBoxLayout *upRightLay = new QVBoxLayout;
    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(boardUpdateWidget);

    layerGroup = new myMutex(Qt::Vertical, 0);
    layerGroup->setMaximumWidth(180);
    layerMaizeAddGroup = new myMutex(Qt::Vertical, 0);
    layerMaizeAddGroup->setMaximumWidth(180);
    layerMaizeAddGroup->hide();
    userModeGroup = new myMutex(Qt::Horizontal);
    userModeGroup->setTitle(myLan.user_mode);
    userModeGroup->setLabelText(myLan.mode_1, myLan.mode_2, myLan.mode_3);
    userModeGroup->setFixedHeight(2*config->getBtnSize(SMALL_BTN_SIZE).height());

    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    boardNumGroup = new QGroupBox(myLan.board_num);
    boardNumGroup->setFont(config->getFont());
    boardNumGroup->setMaximumWidth(180);
    QVBoxLayout *layout = new QVBoxLayout(boardNumGroup);

    boardNumPlusBtn = new myPushButton(myIcon.Action_Plus, btnSize);
    boardUpdateLcdNum = new QLCDNumber(2);
    boardUpdateLcdNum->display(1);
    boardUpdateLcdNum->setFixedSize(btnSize);
    boardNumMinusBtn = new myPushButton(myIcon.Action_Minus, btnSize);

    layout->addWidget(boardNumPlusBtn,0,Qt::AlignHCenter);
    layout->addWidget(boardUpdateLcdNum,0,Qt::AlignHCenter);
    layout->addWidget(boardNumMinusBtn,0,Qt::AlignHCenter);

    messagesGroup = new QGroupBox;
    messagesGroup->setFixedHeight(LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-180);
    QGridLayout *messagesLayout = new QGridLayout(messagesGroup);
    for(int i=0; i<MES_NUM; i++){
        updateMesLabels[i] = new QLabel;
        updateMesLabels[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        updateMesLabels[i]->setAlignment(Qt::AlignLeft);
        updateMesLabels[i]->setMinimumHeight(30);
        messagesLayout->addWidget(updateMesLabels[i], i, 0, 1, 2, Qt::AlignCenter);
        updateStatusLabels[i] = new QLabel;
        updateStatusLabels[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        updateStatusLabels[i]->setAlignment(Qt::AlignCenter);
        messagesLayout->addWidget(updateStatusLabels[i], i, 2, Qt::AlignCenter);
    }

    btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    boardUpdateBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    boardUpdateBackBtn->setFixedSize(btnSize);
    boardUpdateGoBtn = new myPushButton(myLan.upgrade,myIcon.Arrow_Up);
    boardUpdateGoBtn->setFixedSize(btnSize);
    boardUpdateRetryBtn = new myPushButton(myLan.upgrade_retry, myIcon.Action_Refresh);
    boardUpdateRetryBtn->setFixedSize(btnSize);
    boardUpdateWholeBtn = new myPushButton(myLan.upgrade_whole, QIcon());
    boardUpdateWholeBtn->setFixedSize(btnSize);

    upLeftLay->addWidget(layerGroup);
    upLeftLay->addSpacing(10);
    upLeftLay->addWidget(layerMaizeAddGroup);
    upLeftLay->addSpacing(10);
    upLeftLay->addWidget(boardNumGroup);
    upLeftLay->setContentsMargins(0, 0, 0, 0);
    upRightLay->addWidget(userModeGroup);
    upRightLay->addSpacing(30);
    upRightLay->addWidget(messagesGroup);
    upRightLay->setContentsMargins(0, 0, 10, 0);
    upLay->addLayout(upLeftLay);
    upLay->addSpacing(30);
    upLay->addLayout(upRightLay);
    upLay->setContentsMargins(0 ,0, 0, 0);
    downLay->addWidget(boardUpdateWholeBtn);
    downLay->addWidget(boardUpdateGoBtn);
    downLay->addStretch();
    downLay->addWidget(boardUpdateRetryBtn);
    downLay->addStretch();
    downLay->addWidget(boardUpdateBackBtn);
    downLay->setContentsMargins(0, 0, 0, 0);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);

    connect(boardUpdateBackBtn,SIGNAL(pressed()),this,SLOT(onBoardUpdateBackBtnClicked()));
    connect(boardUpdateGoBtn,SIGNAL(pressed()),this,SLOT(onBoardUpdateGoBtnClicked()));
    connect(boardUpdateRetryBtn,SIGNAL(pressed()),this,SLOT(onBoardUpdateRetryBtnClicked()));
    connect(boardUpdateWholeBtn,SIGNAL(pressed()),this,SLOT(onBoardUpdateWholeBtnClicked()));
    connect(boardNumPlusBtn,SIGNAL(pressed()),this,SLOT(onBoardNumPlusBtnClicked()));
    connect(boardNumMinusBtn,SIGNAL(pressed()),this,SLOT(onBoardNumMinusBtnClicked()));
    connect(userModeGroup, SIGNAL(indexChanged(int)), this, SLOT(onUserModeChangedSlt(int)));
    connect(layerGroup, SIGNAL(indexChanged(int)), this, SLOT(onLayerChangedSlt(int)));
    connect(layerMaizeAddGroup, SIGNAL(indexChanged(int)), this, SLOT(onlayerAddChangedSlt(int)));

    stackedWidget->addWidget(boardUpdateWidget);
}

/* 创建整机升级界面 */
void fpga::createWholeUpdatePage()
{
    updateWholeWidget = new QWidget;
    updateWholeWidget->setGeometry(0,0,width(),height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(updateWholeWidget);

    QGroupBox *group = new QGroupBox;
    group->setFixedHeight(LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-80);
    QGridLayout *layout = new QGridLayout(group);

    for(int i=0; i<MAX_VIEW_LOCATION; i++){
        for(int j=0; j<MAX_BOARD_PER_LEVEL; j++){
            boardNumLabels[i][j] = new QLabel;
            boardNumLabels[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            boardNumLabels[i][j]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
            layout->addWidget(boardNumLabels[i][j], j, 2*i, 1, 1);
            statusLabels[i][j] = new QLabel;
            statusLabels[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            statusLabels[i][j]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
            layout->addWidget(statusLabels[i][j], j, 2*i+1, 1, 1);
        }
    }

    QSize btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    updateWholeRebootBtn = new myPushButton(myLan.reboot, QIcon());
    updateWholeRebootBtn->setFixedSize(btnSize);
    updateWholeBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    updateWholeBackBtn->setFixedSize(btnSize);

    upLay->addWidget(group);
    upLay->setContentsMargins(0, 0, 0, 0);
    downLay->addWidget(updateWholeRebootBtn);
    downLay->addStretch();
    downLay->addWidget(updateWholeBackBtn);
    downLay->setContentsMargins(0, 0, 0, 0);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);
    mainLay->setContentsMargins(10, 10, 20, 10);

    connect(updateWholeBackBtn,SIGNAL(pressed()),this,SLOT(onWholeUpdateBackBtnClicked()));
    connect(updateWholeRebootBtn,SIGNAL(pressed()),this,SLOT(onWholeUpdateReturnBtnClicked()));

    stackedWidget->addWidget(updateWholeWidget);
}

/* 根据当前升级板的不同设置显示内容 */
void fpga::setUpdateDisplay()
{
    int num;
    pe.setColor(QPalette::WindowText, Qt::blue);
    for(int i=0; i<MES_NUM; i++){
        updateMesLabels[i]->show();
        updateStatusLabels[i]->show();
        updateStatusLabels[i]->setPalette(pe);
    }
    userModeGroup->setNum(3);
    layerGroup->setNum(2);

    switch(curBoardMode){
    case INT_BOARD:
    case INT_CTRL_BOARD:
        layerMaizeAddGroup->hide();
        boardNumGroup->hide();
        boardUpdateWholeBtn->hide();
        layerGroup->hide();
        if (struCnfg.nInterfaceBoardTotal == 2) {
            layerGroup->show();
            layerGroup->setNum(2);
            layerGroup->setLabelText(myLan.interface_board+"1", myLan.interface_board+"2");
            layerGroup->setTitle(myLan.interface_board);
        }

        num = 4;
        updateMesLabels[0]->setText(QString("1.%1").arg(myLan.msg_int_to_factory));
        updateMesLabels[1]->setText(QString("2.%1").arg(myLan.msg_int_download_file));
        updateMesLabels[2]->setText(QString("3.%1").arg(myLan.msg_int_burn_file));
        updateMesLabels[3]->setText(QString("4.%1").arg(myLan.msg_int_to_user));

        for(int i=0; i<num; i++){
            updateStatusLabels[i]->setText(myLan.upgrade_untreated);
        }
        for(int i=num; i<MES_NUM; i++){
            updateStatusLabels[i]->hide();
            updateMesLabels[i]->hide();
        }

        switch(intUserMode){
        case 0:
            userModeGroup->clearBtnsIcon();
            break;
        case 1:
            userModeGroup->setCurrentIndex(0);
            break;
        case 2:
            userModeGroup->setCurrentIndex(1);
            break;
        case 4:
            userModeGroup->setCurrentIndex(2);
        default:
            break;
        }
        break;
    case COLOR_BOARD:      
        layerGroup->show();
        layerMaizeAddGroup->hide();
        layerGroup->setNum(2);
        layerGroup->setLabelText(myLan.front_view, myLan.rear_view);
        layerGroup->setCurrentIndex(0);

        boardNumGroup->show();
        boardUpdateWholeBtn->show();

        num = 5;
        updateMesLabels[0]->setText(QString("1.%1").arg(myLan.msg_int_to_factory));
        updateMesLabels[1]->setText(QString("2.%1").arg(myLan.msg_color_download_file));
        updateMesLabels[2]->setText(QString("3.%1").arg(myLan.msg_color_to_factory));
        updateMesLabels[3]->setText(QString("4.%1").arg(myLan.msg_color_burn_file));
        updateMesLabels[4]->setText(QString("5.%1").arg(myLan.msg_color_to_user));
        for(int i=0; i<num; i++){
            updateStatusLabels[i]->setText(myLan.upgrade_untreated);
        }
        for(int i=num; i<MES_NUM; i++){
            updateStatusLabels[i]->hide();
            updateMesLabels[i]->hide();
        }
        switch(colorUserMode){
        case 0:
            userModeGroup->clearBtnsIcon();
            break;
        case 1:
            userModeGroup->setCurrentIndex(0);
            break;
        case 2:
            userModeGroup->setCurrentIndex(1);
            break;
        case 4:
            userModeGroup->setCurrentIndex(2);
        default:
            break;
        }
        break;

    case INF_BOARD:
        layerMaizeAddGroup->hide();
        layerGroup->setCurrentIndex(0);
        layerGroup->hide();

        boardNumGroup->show();
        boardUpdateWholeBtn->show();

        num = 7;
        updateMesLabels[0]->setText(QString("1.%1").arg(myLan.msg_int_to_factory));
        updateMesLabels[1]->setText(QString("2.%1").arg(myLan.msg_inf_download_file));
        updateMesLabels[2]->setText(QString("3.%1").arg(myLan.msg_color_to_factory));
        updateMesLabels[3]->setText(QString("4.%1").arg(myLan.msg_inf_to_factory));
        updateMesLabels[4]->setText(QString("5.%1").arg(myLan.msg_inf_burn_file));
        updateMesLabels[5]->setText(QString("6.%1").arg(myLan.msg_inf_to_user));
        updateMesLabels[6]->setText(QString("7.%1").arg(myLan.msg_color_to_user));
        for(int i=0; i<num; i++){
            updateStatusLabels[i]->setText(myLan.upgrade_untreated);
        }

        switch(infUserMode){
        case 0:
            userModeGroup->clearBtnsIcon();
            break;
        case 1:
            userModeGroup->setCurrentIndex(0);
            break;
        case 2:
            userModeGroup->setCurrentIndex(1);
            break;
        case 4:
            userModeGroup->setCurrentIndex(2);
        default:
            break;
        }
        break;

    case CTRL_BOARD:
        userModeGroup->setIndexEnabled(1, false);
        userModeGroup->setIndexEnabled(2, false);
        layerGroup->hide();
        boardNumGroup->hide();
        boardUpdateWholeBtn->hide();
        layerMaizeAddGroup->hide();

        num = 4;
        updateMesLabels[0]->setText(myLan.msg_ctrl_to_factory);
        updateMesLabels[1]->setText(myLan.msg_ctrl_erase);
        updateMesLabels[2]->setText(myLan.msg_ctrl_burn_file);
        updateMesLabels[3]->setText(myLan.msg_ctrl_to_user);

        for(int i=0; i<num; i++){
            updateStatusLabels[i]->setText(myLan.upgrade_untreated);
        }
        for(int i=num; i<MES_NUM; i++){
            updateMesLabels[i]->hide();
            updateStatusLabels[i]->hide();
        }
        if(ctrlUserMode == 1)
            userModeGroup->setCurrentIndex(0);
        else
            userModeGroup->clearBtnsIcon();
        break;

    default:
        break;
    }

    boardUpdateRetryBtn->setEnabled(false);
}

/* 根据当前状态设置整机升级界面的显示 */
void fpga::setUpdateWholeDisplay()
{
    int chutePerViewLocation = struCnfg.struLevelInfo[0].nUnitLevelTotal / 2;//通道数

    for(int i=0; i<chutePerViewLocation; i++){
        if(curBoardMode == COLOR_BOARD){
            boardNumLabels[0][i]->setText(QString("%1 %2 %3").arg(myLan.front_view).arg(i+1).arg(myLan.upgrade));
            boardNumLabels[1][i]->setText(QString("%1 %2 %3").arg(myLan.rear_view).arg(i+1).arg(myLan.upgrade));
            for (int j = 2; j < MAX_VIEW_LOCATION; j++) {
                boardNumLabels[j][i]->show();
                statusLabels[j][i]->show();
                setWholeStatusShow(j, i, UPDATE_NO_START);
            }
            for (int j = 2; j < MAX_VIEW_LOCATION; j++) {
                boardNumLabels[j][i]->hide();
                statusLabels[j][i]->hide();
            }
        } else if(curBoardMode == INF_BOARD){
            boardNumLabels[0][i]->setText(QString("%1 %2 %3").arg(myLan.infra+"-"+myLan.front).arg(i+1).arg(myLan.upgrade));
            boardNumLabels[0][i]->show();

            boardNumLabels[1][i]->setText(QString("%1 %2 %3").arg(myLan.infra+"-"+myLan.rear).arg(i+1).arg(myLan.upgrade));
            boardNumLabels[1][i]->show();

            setWholeStatusShow(0, i, UPDATE_NO_START);
            statusLabels[0][i]->show();
            setWholeStatusShow(1, i, UPDATE_NO_START);
            statusLabels[1][i]->show();

            for (int j = 2; j < MAX_VIEW_LOCATION; j++) {
                boardNumLabels[j][i]->hide();
                statusLabels[j][i]->hide();
            }
        }
    }
    for(int i=0; i<MAX_VIEW_LOCATION; i++){
        for(int j=chutePerViewLocation; j<MAX_BOARD_PER_LEVEL; j++){
            boardNumLabels[i][j]->hide();
            statusLabels[i][j]->hide();
        }
    }
}

/*!
 * \brief 清空文件的下载状态
 */
void fpga::clearFileStatus()
{
    for (int i = 0; i < MAX_LEVEL; i++) {
        bColorFileIsOk[i] = false;
        bInfFileIsOk[i] = false;
    }
}

/* 切换下位机至工厂模式 */
bool fpga::stepSwitchToFactory(boardMode module)
{
    int ret;
    int loopTimes = 10;

    switch(module){
    case INT_BOARD:
    case INT_CTRL_BOARD:
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_FACTORY, intAddr, colorAddr, 0, 0, 0, 0, 0, 3);
		myFlow.setCom1Baud();
        break;
    case COLOR_BOARD:
        MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_FACTORY, 2, colorAddr, 0, 0, 0, 1, 0, 3);
        break;
    case INF_BOARD:
        MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_FACTORY, 2, infAddr, 0, 0, 0, 2, 0, 3);
        break;
    case CTRL_BOARD:
        break;
    default:
        break;
    }

    myFlow.sleep(4);

    for(int i=0; i<loopTimes; i++){
        switch(module){
        case INT_BOARD:
        case INT_CTRL_BOARD:
            MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 1);
            break;
        case COLOR_BOARD:
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, colorAddr, 0, 0, 0, 1, 0, 1);
            break;
        case INF_BOARD:
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, infAddr, 0, 0, 0, 2, 0, 1);
            break;
        case CTRL_BOARD:
            break;
        default:
            break;
        }
	
        myFlow.msleep(3);
        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);

        if (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                && packetBuf[3]==0xAA && (packetBuf[15]&MASK_LOAD_FACTORY_MODE)) {
            return true;
        }

        myFlow.sleep(1);
    }

    return false;
}

/* 向下位机发送升级文件 */
bool fpga::stepSendFile()
{
    int ret;
    int writeLen;
    int curIsOK = 0;
    int loopTimes = 3;
    int per;

    if(curBoardMode == INF_BOARD || curBoardMode == INT_CTRL_BOARD) {
        writeLen = PACKET_COUNT_SMALL;
    } else if (curBoardMode == COLOR_BOARD && m_bIs115) {
        writeLen = PACKET_COUNT_BIG;
    } else {
        writeLen = PACKET_COUNT;
    }

    /* 切换接口板至串口模式 */
	int tmpIntAddr = intAddr;	
    if (colorAddr > (MAX_UNIT_PER_INT-1)) {
        tmpIntAddr = 0x11;
    }
	MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, tmpIntAddr, 0, 0, 0, 0, 0, 0, 0, 3);

    qDebug("Step send file");

    for (int i = 0; i < writeLen; i++) {
        if (stackedWidget->currentIndex() == UPDATE_PAGE) {
            per = (100*i)/writeLen;
            QString str = QString("%1 %2%").arg(myLan.upgrade_doing).arg(per);
            updateStatusLabels[1]->setText(str);
        }

        for(int j=0; j<loopTimes; j++){
			MySerial.fpgaUpdateWrite(COM_SEND_FILE, intAddr, colorAddr, 0, 0, 0, i/256, i%256, 1, fileBuf);
            myFlow.msleep(30);
            MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 1);
            myFlow.msleep(3);
            ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
            if(ret < PACKET_SIZE)
                continue;

            if(packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                    && packetBuf[3]==0xAA && (packetBuf[15]&MASK_BURN_SDRAM_PAGE_FILE)) {
                curIsOK = 1;
                break;
            }
        }//end for

        if(curIsOK)
            curIsOK = 0;
        else
            return false;
    }

    return true;
}

/* 通过高速串口发送升级文件 */
bool fpga::stepSendFileByHighUART()
{
    int ret;
    int writeLen;
    int nPacketCount;   //! 升级文件页的数量(每页256Bytes)
    int curIsOK = 0;
    int loopTimes = 3;
    int per;
	int colorAddrTmp = 0;
    if (struCnfe.nMachine == MACHINE_CF) {
		colorAddrTmp = colorAddr;
	}

    /* 根据当前升级文件的类型确定升级包的个数 */
    if(curBoardMode==INT_CTRL_BOARD) {
        writeLen = PACKET_COUNT_SMALL_MAIZE;
        nPacketCount = PACKET_COUNT_SMALL - 1;
    } else if (curBoardMode == COLOR_BOARD && m_bIs115) {
        writeLen = PACKET_COUNT_BIG_MAIZE;
        nPacketCount = PACKET_COUNT_BIG - 1;
    } else {
        writeLen = PACKET_COUNT_MAIZE;
        nPacketCount = PACKET_COUNT - 1;
    }

    /* 将升级文件拷贝至文件缓冲区中 */
    uchar *sFileBuf = new uchar[PACKET_COUNT_BIG_MAIZE*FILE_PACKET_SIZE_MAIZE];
    for(int i=0; i<PACKET_COUNT_BIG; i++) {
        for(int j=0; j<FILE_PACKET_SIZE; j++) {
            sFileBuf[i*FILE_PACKET_SIZE+j] = fileBuf[i][j];
        }
    }

    /* 切换接口板至串口模式 */
    int tmpIntAddr = intAddr;
    if (colorAddr > (MAX_UNIT_PER_INT-1)) {
        tmpIntAddr = 0x11;
    }
    printf("UART IntAddr:%x\n",tmpIntAddr);
	MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, tmpIntAddr, 0, 0, 0, 0, 0, 0, 0, 3);

    if (updateModeIndex == 1) {
        MySerial.fpgaUpdateWrite(COM_INT_FILE_LEN, intAddr, colorAddrTmp, 0, 0, 0, nPacketCount/256, nPacketCount%256, 3);
    	myFlow.sleep(1);
        loopTimes = 1;
	}

    for (int i = 0; i < writeLen; i++) {
        if(stackedWidget->currentIndex() == UPDATE_PAGE){
            per = (100*i)/writeLen;
            QString str = QString("%1 %2%").arg(myLan.upgrade_doing).arg(per);
            updateStatusLabels[1]->setText(str);
        }

        for (int j = 0; j < loopTimes; j++) {
           	myFlow.msleep(100);
            if (updateModeIndex == 0) {  //旧版每次发送都查询一次
                MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 1);
                myFlow.msleep(3);
                ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
                if(ret < PACKET_SIZE)
                    continue;

                if(packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                        && packetBuf[3]==0xAA && (packetBuf[15]&MASK_BURN_SDRAM_PAGE_FILE)){
                    curIsOK = 1;
                    break;
                }
            }
        }//end for

        if (updateModeIndex == 0) {  //旧版每次发送都查询一次
            if(curIsOK) {
                curIsOK = 0;
            } else {
                delete []sFileBuf;
                return false;
            }
        }
    }

    if (updateModeIndex == 1) {  //新版在发送完之后查询一次
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 1);
        myFlow.msleep(3);
        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
        if(ret < PACKET_SIZE)
            curIsOK = 0;

        if(packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                && packetBuf[3]==0xAA && (packetBuf[15]&MASK_BURN_SDRAM_ALL_FILE)) {
            curIsOK = 1;
        }

        printf("curIsOK=%d\n",curIsOK);
        if(curIsOK == 0) {
            delete []sFileBuf;
            return false;
        }
    }

    delete []sFileBuf;
    return true;
}

/* 通过USB向下位机发送升级文件 */
bool fpga::stepSendFileByUSB()
{
#ifdef Q_OS_UNIX
    int ret;
    int writeLen;
    int loopTimes = 10;
	int colorAddrTmp = 0;
    if (struCnfe.nMachine == MACHINE_CF) {
		colorAddrTmp = colorAddr;
	}

    /* 红外相机板及接口控制板升级文件大小为512K,其余板子为2M */
    if(curBoardMode == INF_BOARD || curBoardMode == INT_CTRL_BOARD) {
        writeLen = PACKET_COUNT_SMALL;
    } else if (curBoardMode == COLOR_BOARD && m_bIs115) {
        writeLen = PACKET_COUNT_BIG;
    } else {
        writeLen = PACKET_COUNT;
    }

    if(myUsbDev != NULL){
        usb.myusbFree();
    }
    usb.myusbInit();
    if(myUsbDev == NULL){
        printf("usb init error");
		fflush(0);
        return false;
    }

    /* 考虑到USB初始化向下位机发送升级文件数据时，会漏发前64个字节的数据，
     * 因此USB初始化结束后向下位机写128个字节的废数据 */
    char buf[128] = {0};
    ret = usb_bulk_write(myUsbDev, USB_EP_OUT_AI, buf, 128, 1000);
    printf("MSG : write_ret1=%d", ret);
	fflush(0);

    if (updateModeIndex == 1) {
        int writeLenTotal = writeLen - 1;
        MySerial.fpgaUpdateWrite(COM_INT_FILE_LEN, intAddr, colorAddrTmp, 0, 0, 0, writeLenTotal/256, writeLenTotal%256, 3);
    	myFlow.sleep(1);
	}

    /* 切换接口板至USB模式 */
	int tmpIntAddr = intAddr;	
    if (colorAddr > (MAX_UNIT_PER_INT-1)) {
        tmpIntAddr = 0x11;
    }
	MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, tmpIntAddr, 0, 0, 0, 0, 0, 0, 1, 3);
    USBSendFile(COM_SEND_FILE, tmpIntAddr, writeLen);
    myFlow.sleep(1);

    if (updateModeIndex == 0) {  //旧版每次发送
    	for(int j=0; j<loopTimes; j++){
            MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 1);
        	myFlow.msleep(3);
        	ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);

        	if(ret < PACKET_SIZE){
                myFlow.sleep(1);
                continue;
        	}

        	if(packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
            	    && packetBuf[3]==0xAA && (packetBuf[15]&0x40)==0x40){
                return true;
        	}
        	myFlow.sleep(1);
    	}//end for
    }

    if (updateModeIndex == 1) {  //新版在发送完之后查询一次
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 1);
        myFlow.msleep(3);
        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
        if(ret == PACKET_SIZE) {
            if(packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                && packetBuf[3]==0xAA && (packetBuf[15]&MASK_BURN_SDRAM_ALL_FILE)) {
            	return true;
            }
        }
    }

    /* 发送文件失败后复位下位机固件 */
    MySerial.com1Write(CMD_INT_IMAGE_ERROR_REVISE, tmpIntAddr, 0, 0, 0, 0, 0, 0, 1, 3);
    myFlow.sleep(1);
#endif

    return false;
}

/* 下位机烧写升级文件 */
bool fpga::stepBurnFile()
{
    int ret;
    int writeLen;
    int loopTimes = 10;
	int colorAddrTmp = 0;
    int nWaitSecs = 3;
    if (struCnfe.nMachine == MACHINE_CF) {
		colorAddrTmp = colorAddr;
	}

    /* 红外相机板及接口控制板升级文件大小为512K,其余板子为2M */
    if(curBoardMode == INF_BOARD || curBoardMode == INT_CTRL_BOARD) {
        writeLen = PACKET_COUNT_SMALL - 1;
    } else if (curBoardMode == COLOR_BOARD && m_bIs115) {
        writeLen = PACKET_COUNT_BIG - 1;
    } else {
        writeLen = PACKET_COUNT - 1;
    }
	
	if (updateModeIndex == 0) {
        MySerial.fpgaUpdateWrite(COM_INT_FILE_LEN, intAddr, colorAddrTmp, 0, 0, 0, writeLen/256, writeLen%256, 3);
	}
    myFlow.sleep(1);

    switch(curBoardMode){
    case INT_BOARD:
    case INT_CTRL_BOARD:
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_BURN, intAddr, colorAddr, 0, 0, 0, 0, intUserMode, 3);
        break;
    case COLOR_BOARD:
        if (updateModeIndex == 0) {  //  旧版
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_BURN, 2, colorAddr, 0, 0, 0, 1, colorUserMode, 3);
        } else {                     //  新版
            //! 重置包数量
            int nPacketCount = PACKET_COUNT - 1;
            if (m_bIs115) {
                nPacketCount = PACKET_COUNT_BIG - 1;
                nWaitSecs = 6;
            }

            //! 新版升级功能在此处添加转发命令COM_TRANSFER
            //! 1.相机板准备写入SDRAM
            //! 2.向接口板指出待升级相机板
            //! 3.查询相机板是否写入完成
            MySerial.fpgaUpdateWrite(COM_UNIT_FILE_LEN, 2, colorAddr , 0, 0, 0, nPacketCount/256, nPacketCount%256, 3);
            MySerial.fpgaUpdateWrite(COM_TRANSFER, intAddr, colorAddr, 0, 0, 0, 0xff ,0xff, 3);//转发命令
            myFlow.sleep(nWaitSecs);

            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, colorAddr, 0, 0, 0, 1, 0, 3);//3秒后查询
            myFlow.msleep(50);
            ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
            if (ret < PACKET_SIZE) {
                return false;
            }
            if (packetBuf[0]!=0xAA || packetBuf[1]!=0xAA || packetBuf[2]!=0xAA
                    || packetBuf[3]!=0xAA || !(packetBuf[15]&MASK_BURN_SDRAM_ALL_FILE)) { //查询SDRAM是否完成文件写
                return false;
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_BURN, 2, colorAddr, 0, 0, 0, 1, colorUserMode, 3);//烧写FLASH命令
        }
        break;
    case INF_BOARD:
        if (updateModeIndex == 0) {  //  旧版
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_BURN, 2, infAddr, 0, 0, 0, 2, infUserMode, 3);
        } else {                    //  新版
		   /* 先主相机 */
           MySerial.fpgaUpdateWrite(COM_UNIT_FILE_LEN, 2, infAddr-struGsh.nDevTypeAddrBias , 0, 0, 0, (PACKET_COUNT_SMALL-1)/256, (PACKET_COUNT_SMALL-1)%256, 3);
           MySerial.fpgaUpdateWrite(COM_TRANSFER, intAddr, infAddr-struGsh.nDevTypeAddrBias, 0, 0, 0, 0xff ,0xff, 3);//向接口板发转发命令,转到从板对应的主板
           myFlow.sleep(3);

           MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, infAddr-struGsh.nDevTypeAddrBias, 0, 0, 0, 1, 0, 3);//3秒后查询
           myFlow.msleep(100);

           ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
           if ((ret < PACKET_SIZE)||(packetBuf[0]!=0xAA || packetBuf[1]!=0xAA || packetBuf[2]!=0xAA
                   || packetBuf[3]!=0xAA || !(packetBuf[15]&MASK_BURN_SDRAM_ALL_FILE))) {//查询SDRAM是否完成文件写
               return false;
           }

           /* 再从相机 */
		   MySerial.fpgaUpdateWrite(COM_UNIT_FILE_LEN, 2, infAddr, 0, 0, 0, (PACKET_COUNT_SMALL-1)/256, (PACKET_COUNT_SMALL-1)%256, 3);
           MySerial.fpgaUpdateWrite(COM_TRANSFER, 2, infAddr-struGsh.nDevTypeAddrBias, 0, 0, 0, 0xff ,0xff, 3);//向主板发转发命令
           myFlow.sleep(3);

           MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, infAddr, 0, 0, 0, 2, 0, 3);//3秒后查询
           myFlow.msleep(100);
           ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
           if((ret < PACKET_SIZE) || packetBuf[0]!=0xAA || packetBuf[1]!=0xAA || packetBuf[2]!=0xAA
                   || packetBuf[3]!=0xAA || !(packetBuf[15]&MASK_BURN_SDRAM_ALL_FILE)) {//查询从板SDRAM是否完成文件写
               return false;
           }

           MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_BURN, 2, infAddr, 0, 0, 0, 2, infUserMode, 3);//烧写FLASH命令
        }
        break;
    case CTRL_BOARD:
        break;
    default:
        break;
    }

    myFlow.sleep(15);

    for(int i=0; i<loopTimes; i++){
        switch(curBoardMode){
        case INT_BOARD:
        case INT_CTRL_BOARD:
            MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 3);
            break;
        case COLOR_BOARD:
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, colorAddr, 0, 0, 0, 1, 0, 3);
            break;
        case INF_BOARD:
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, infAddr, 0, 0, 0, 2, 0, 3);
            break;
        case CTRL_BOARD:
            break;
        default:
            break;
        }

        myFlow.msleep(3);
        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);

        if(packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                && packetBuf[3]==0xAA && (packetBuf[15]&MASK_BURN_FLASH_FILE)){
            return true;
        }

        myFlow.sleep(1);
    }

    return false;
}

/* 切换下位机至用户模式 */
bool fpga::stepSwitchToUser(boardMode module)
{
    int ret;
    int loopTimes = 10;

	if (updateModeIndex == 1) {
		loopTimes = 1;
	}

    switch(module){
    case INT_BOARD:
    case INT_CTRL_BOARD:
        if(intUserMode == 0) {
            MySerial.fpgaUpdateWrite(COM_INT_UPDATE_USER, intAddr, colorAddr, 0, 0, 0, 0, defIntMode, 3);
        } else{
            MySerial.fpgaUpdateWrite(COM_INT_UPDATE_USER, intAddr, colorAddr, 0, 0, 0, 0, intUserMode, 3);
        }
        break;
    case COLOR_BOARD:
        if(colorUserMode == 0) {
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_USER, 2, colorAddr, 0, 0, 0, 1, defColorMode, 3);
        } else{
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_USER, 2, colorAddr, 0, 0, 0, 1, colorUserMode, 3);
        }
        break;
    case INF_BOARD:
        MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_USER, 2, infAddr, 0, 0, 0, 2, infUserMode, 3);
		break;
    case CTRL_BOARD:
        break;
    default:
        break;
    }

    myFlow.sleep(3);

    for(int i=0; i<loopTimes; i++){
        switch(module){
        case INT_BOARD:
        case INT_CTRL_BOARD:
            MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, intAddr, colorAddr, 0, 0, 0, 0, 0, 3);
            break;
        case COLOR_BOARD:
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, colorAddr, 0, 0, 0, 1, 0, 3);
            break;
        case INF_BOARD:
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, infAddr, 0, 0, 0, 2, 0, 3);
            break;
        case CTRL_BOARD:
            break;
        default:
            break;
        }

        myFlow.msleep(3);

		if (updateModeIndex == 1) {
        	myFlow.msleep(100);
		}
        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);

#if 0
        qDebug("ret = %d", ret);
        printf("com << ");
        for(int i=0; i<PACKET_SIZE; i++){
            printf("%02x ", packetBuf[i]);
        }
	printf("\n");
#endif
	if (ret == PACKET_SIZE) {
	    if (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
		&& packetBuf[3]==0xAA  && (packetBuf[15]&MASK_LOAD_CUSTOM_MODE)) {
            return true;
	    }
	}

        myFlow.sleep(1);
    }

    return false;
}

/* 切换控制板至工厂模式 */
bool fpga::ctrlSwitchToFactory()
{
    int ret;
    int loopTimes = 10;

    MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_FACTORY, ctrlAddr, 0, 0, 0, 0, 0, 0, 1);
    myFlow.sleep(2);

    for(int i=0; i<loopTimes; i++){
        MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_STATUS, ctrlAddr, 0, 0, 0, 0, 0, 0, 1);
        myFlow.msleep(3);

        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE, true);
        if(ret < PACKET_SIZE){
            myFlow.sleep(1);
            continue;
        }
	
        /*
        qDebug("ret = %d", ret);
        for(int i=0; i<PACKET_SIZE; i++){
            printf("%02x ", packetBuf[i]);
        }
        printf("\n");
        */

        if (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                && packetBuf[3]==0xAA  && (packetBuf[15]&MASK_LOAD_FACTORY_MODE)) {
                return true;
        }

        myFlow.sleep(1);
    }

    return false;
}

/* 擦除控制板的用户空间 */
bool fpga::ctrlEraseFlash()
{
    int ret;
    int loopTimes = 10;

    MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_ERASE, ctrlAddr, 0, 0, 0, 0, 0, ctrlUserMode, 1);
    myFlow.sleep(2);

    for(int i=0; i<loopTimes; i++){
        MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_STATUS, ctrlAddr, 0, 0, 0, 0, 0, 0, 1);
        myFlow.msleep(3);
        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE, true);
        if(ret < PACKET_SIZE){
            myFlow.sleep(1);
            continue;
        }

        /*
        qDebug("ret = %d", ret);
        for(int i=0; i<PACKET_SIZE; i++){
            printf("%02x ", packetBuf[i]);
        }
        printf("\n");
        */

        if (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                && packetBuf[3]==0xAA  && (packetBuf[15]&MASK_BOARD_ADDRESS_ERROR)) {
            return true;
        }

        myFlow.sleep(1);
    }

    return false;
}

/* 向控制板发送文件 */
bool fpga::ctrlSendFile()
{
    int ret;
    int per;
    int loopTimes = 3;
    int writeLen = PACKET_COUNT;
    bool curPacketIsOk = false;

    for(int i=0; i<writeLen; i++){
        if(stackedWidget->currentIndex() == UPDATE_PAGE){
            pe.setColor(QPalette::WindowText, Qt::darkBlue);
            updateStatusLabels[2]->setPalette(pe);
            per = (100*i)/writeLen;
            QString str = QString("%1 %2%").arg(myLan.upgrade_doing).arg(per);
            updateStatusLabels[2]->setText(str);
        }

        MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_SEND_FILE, ctrlAddr, 0, 0, 0, 0, i/256, i%256, 1, fileBuf);
        myFlow.msleep(30);

        for(int j=0; j<loopTimes; j++){
            MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_STATUS, ctrlAddr, 0, 0, 0, 0, 0, 0, 1);
            myFlow.msleep(3);

            ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE, true);

#if DEBUG_OUTPUT == 1
            printf("ret = %d\n", ret);
            for(int k=0; k<PACKET_SIZE; k++){
                printf("%02x ", packetBuf[k]);
            }
            printf("\n");
#endif

            if(ret < PACKET_SIZE)
                continue;

            if (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                    && packetBuf[3]==0xAA  && (packetBuf[15]&MASK_BURN_SDRAM_PAGE_FILE
			)) {
                curPacketIsOk = true;
                break;
            }
        }

        if(curPacketIsOk)
            curPacketIsOk = false;
        else
            return false;
    }

    return true;
}

/* 向控制板发送文件（改进版） */
bool fpga::ctrlSendFileImprove()
{
    int ret;
    int per;
    int loopTimes = 20;
    int writeLen = PACKET_COUNT;
    int bulkSize = 128;     //每发送128页的数据查询一次状态
    int bulkLen = 0;        //当前页数据在块数据中的位置
    bool curPacketIsOk = false;

    for(int i=0; i<writeLen; i++){
        if(stackedWidget->currentIndex() == UPDATE_PAGE){
            pe.setColor(QPalette::WindowText, Qt::darkBlue);
            updateStatusLabels[2]->setPalette(pe);
            per = (100*i)/writeLen;
            QString str = QString("%1 %2%").arg(myLan.upgrade_doing).arg(per);
            updateStatusLabels[2]->setText(str);
        }

        MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_SEND_FILE, ctrlAddr, 0, 0, 0, 0, i/256, i%256, 1, fileBuf);
        ++bulkLen;
        if(bulkLen != bulkSize){
            myFlow.msleep(1);
            continue;
        }

        bulkLen = 0;
        myFlow.msleep(30);

        for(int j=0; j<loopTimes; j++){
            MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_STATUS, ctrlAddr, 0, 0, 0, 0, 0, 0, 1);
            myFlow.msleep(3);

            ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE, true);

#if DEBUG_OUTPUT == 1
            printf("ret = %d\n", ret);
            for(int k=0; k<PACKET_SIZE; k++){
                printf("%02x ", packetBuf[k]);
            }
            printf("\n");
#endif

            if(ret < PACKET_SIZE){
                myFlow.msleep(50);
                continue;
            }

            if (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                    && packetBuf[3]==0xAA  && (packetBuf[15]&MASK_BURN_SDRAM_PAGE_FILE)) {
                curPacketIsOk = true;
                break;
            }

            myFlow.msleep(50);
        }

        if(curPacketIsOk)
            curPacketIsOk = false;
        else
            return false;
    }

    return true;
}

/* 切换控制板至用户模式 */
bool fpga::ctrlSwitchToUser()
{
    int ret;
    int loopTimes = 10;
    bool bIsOk = false;

    myFlow.sleep(2);
    MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_USER, ctrlAddr, 0, 0, 0, 0, 0, ctrlUserMode, 1);
    myFlow.sleep(5);

    for(int i=0; i<loopTimes; i++){
        MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_STATUS, ctrlAddr, 0, 0, 0, 0, 0, 0, 1);
        myFlow.msleep(3);

        ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE, true);
        if(ret < PACKET_SIZE){
            myFlow.sleep(1);
            continue;
        }

        if (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                && packetBuf[3]==0xAA  && (packetBuf[15]&MASK_LOAD_CUSTOM_MODE)) {
            bIsOk = true;
        }

        myFlow.sleep(1);
    }

    return bIsOk;

    if(bIsOk){
        MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_IS_OK, ctrlAddr, 0, 0, 0, 0, 0x5A, 0x5A, 1);
        return true;
    }
    else{
        MySerial.fpgaUpdateWrite(COM_CTRL_UPDATE_IS_OK, ctrlAddr, 0, 0, 0, 0, 0xA5, 0xA5, 1);
        return false;
    }
}

/* 接口板升级流程 */
int fpga::intBoardUpdate(int step)
{
    qDebug("intAddr = %d,intUserMode = %d,curBoardMode=%d", intAddr,intUserMode,curBoardMode);
    int status = 0;
    bool tmpStatus;

    switch(step){
        case 1:
            goto step1;
            break;
        case 2:
            goto step2;
            break;
        case 3:
            goto step3;
            break;
        case 4:
            goto step4;
            break;
        default:
            break;
    }

step1:  //切换到工厂模式
    setStatusLabelShow(0, UPDATE_DOING);
    tmpStatus = stepSwitchToFactory(INT_BOARD);

    if(!tmpStatus){
        status = 1;
        setStatusLabelShow(0, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(0, UPDATE_OK);

step2:  //发送升级文件
    if(bTransByUSB){
        setStatusLabelShow(1, UPDATE_DOING);
        myFlow.sleep(4);
        tmpStatus = stepSendFileByUSB();
    } else {
        if((struGsh.nIntBaud[0] == MY_UARTBAUD_921600) || (struGsh.nIntBaud[1] == MY_UARTBAUD_921600) ) {
			tmpStatus = stepSendFileByHighUART();
        } else {
            tmpStatus = stepSendFile();
        }
    }

    if (!tmpStatus){
        status = 2;
        setStatusLabelShow(1, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(1, UPDATE_OK);

step3:  //  烧写flash
    setStatusLabelShow(2, UPDATE_DOING);
    tmpStatus = stepBurnFile();

    if (!tmpStatus){
        status = 3;
        setStatusLabelShow(2, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(2, UPDATE_OK);

step4:  // 接口板切换到用户模式
    setStatusLabelShow(3, UPDATE_DOING);
    tmpStatus = stepSwitchToUser(INT_BOARD);

    if (!tmpStatus){
        status = 4;
        setStatusLabelShow(3, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(3, UPDATE_OK);

    return status;
}

/* 彩色相机板升级流程 */
int fpga::colorBoardUpdate(int step)
{
    int status = 0;
    bool tmpStatus;

    qDebug("[colorBoardUpdate]colorAddr = 0x%02x", colorAddr);

    switch(step){
    case 1:
        switch(struCnfg.nLevelTotal){
        case 1:
            //! 整机升级时，当进行到第二块接口板时，需重新下载升级文件
            if(bColorFileIsOk[0] && (colorAddr != MAX_UNIT_PER_INT))
                goto step3;
            else
                goto step1;
            break;
        case 2:
            if((bColorFileIsOk[0] && intAddr==0x01) || (bColorFileIsOk[1] && intAddr==0x11))
                goto step3;
            else
                goto step1;
            break;
        case 3:
            if((bColorFileIsOk[0] && intAddr==0x01) || (bColorFileIsOk[1] && intAddr==0x11)
                    || (bColorFileIsOk[2] && intAddr==0x21))
                goto step3;
            else
                goto step1;
            break;
        default:
            break;
        }
        break;
    case 2:
        goto step2;
        break;
    case 3:
        goto step3;
        break;
    case 4:
        goto step4;
        break;
    case 5:
        goto step5;
        break;
    default:
        break;
    }

step1:  //接口板切到工厂模式
    setStatusLabelShow(0, UPDATE_DOING);
    tmpStatus = stepSwitchToFactory(INT_BOARD);

    if(!tmpStatus){
        status = 1;
        setStatusLabelShow(0, UPDATE_FAIL);
        return status;
    }
    else{
        bIntIsFactory = true;
        setStatusLabelShow(0, UPDATE_OK);
    }

step2:  //传送文件至接口板
    if (bTransByUSB) {
        setStatusLabelShow(1, UPDATE_DOING);
        myFlow.sleep(4);
        tmpStatus = stepSendFileByUSB();
    } else {
        if (struGsh.nIntBaud[0] == MY_UARTBAUD_921600
                || struGsh.nIntBaud[1] == MY_UARTBAUD_921600) {
            tmpStatus = stepSendFileByHighUART();
        } else {
            tmpStatus = stepSendFile();
        }
    }

    if (!tmpStatus){
        status = 2;
        setStatusLabelShow(1, UPDATE_FAIL);
        return status;
    }
    else{
        if(intAddr == 0x01)
            bColorFileIsOk[0] = true;
        else if(intAddr == 0x11)
            bColorFileIsOk[1] = true;
        else if(intAddr == 0x21)
            bColorFileIsOk[2] = true;

        setStatusLabelShow(1, UPDATE_OK);
    }

step3:  //切换相机板到工厂模式
    setStatusLabelShow(2, UPDATE_DOING);
    tmpStatus = stepSwitchToFactory(COLOR_BOARD);
    if(!tmpStatus){
        status = 3;
        setStatusLabelShow(2, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(2, UPDATE_OK);

step4:  //烧写FLASH
    setStatusLabelShow(3, UPDATE_DOING);
    tmpStatus = stepBurnFile();
    if (!tmpStatus){
        status = 4;
        setStatusLabelShow(3, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(3, UPDATE_OK);

step5:
    setStatusLabelShow(4, UPDATE_DOING);
    tmpStatus = stepSwitchToUser(COLOR_BOARD);
    if (!tmpStatus){
        status = 5;
        setStatusLabelShow(4, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(4, UPDATE_OK);


    return status;
}

/* 红外相机板升级流程 */
int fpga::infBoardUpdate(int step)
{

    int status = 0;
    int tmpStatus;

    printf("[infBoardUpdate]infAddr = 0x%02x,colorAddr = 0x%02x,", infAddr,colorAddr);
    fflush(0);

    switch(step){
    case 1:
        switch(struCnfg.nLevelTotal){
        case 1:
            if(bInfFileIsOk[0] && (colorAddr != MAX_UNIT_PER_INT))
                goto step3;
            else
                goto step1;
            break;
        case 2:
            if((bInfFileIsOk[0] && intAddr==0x01) || (bInfFileIsOk[1] && intAddr==0x11))
                goto step3;
            else
                goto step1;
            break;
        case 3:
            if((bInfFileIsOk[0] && intAddr==0x01) || (bInfFileIsOk[1] && intAddr==0x11)
                    || (bInfFileIsOk[2] && intAddr==0x21))
                goto step3;
            else
                goto step1;
            break;
        default:
            break;
        }
        break;
    case 2:
        goto step2;
        break;
    case 3:
        goto step3;
        break;
    case 4:
        goto step4;
        break;
    case 5:
        goto step5;
        break;
    case 6:
        goto step6;
        break;
    case 7:
        goto step7;
        break;
    default:
        break;
    }

step1:  //  接口板切换到工厂模式
    setStatusLabelShow(0, UPDATE_DOING);
    tmpStatus = stepSwitchToFactory(INT_BOARD);

    if(!tmpStatus){
        status = 1;
        setStatusLabelShow(0, UPDATE_FAIL);
        return status;
    }
    else{
        bIntIsFactory = true;
        setStatusLabelShow(0, UPDATE_OK);
    }

step2:  // 发送红外升级文件到接口板
    if(bTransByUSB){
        setStatusLabelShow(1, UPDATE_DOING);
        myFlow.sleep(4);
        tmpStatus = stepSendFileByUSB();
    } else {
        if((struGsh.nIntBaud[0] == MY_UARTBAUD_921600) || (struGsh.nIntBaud[1] == MY_UARTBAUD_921600) ) {
            tmpStatus = stepSendFileByHighUART();
        } else {
            tmpStatus = stepSendFile();
        }
    }

    if(!tmpStatus){
        status = 2;
        setStatusLabelShow(1, UPDATE_FAIL);
        return status;
    }
    else{
        if(intAddr == 0x01)
            bInfFileIsOk[0] = true;
        else if(intAddr == 0x11)
            bInfFileIsOk[1] = true;
        else if(intAddr == 0x21)
            bInfFileIsOk[2] = true;

        setStatusLabelShow(1, UPDATE_OK);
    }

step3:  //    切换主板到工厂模式

    setStatusLabelShow(2, UPDATE_DOING);
    tmpStatus = stepSwitchToFactory(COLOR_BOARD);

    if(!tmpStatus){
        status = 3;
        setStatusLabelShow(2, UPDATE_FAIL);
        return status;
    }
    else{
        bColorIsFactory = true;
        setStatusLabelShow(2, UPDATE_OK);
    }

step4:  //切换红外板到工厂模式
    setStatusLabelShow(3, UPDATE_DOING);
    tmpStatus = stepSwitchToFactory(INF_BOARD);

    if(!tmpStatus){
        status = 4;
        setStatusLabelShow(3, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(3, UPDATE_OK);

step5:  //  红外板烧写FLASH
    setStatusLabelShow(4, UPDATE_DOING);
    tmpStatus = stepBurnFile();

    if(!tmpStatus){
        status = 5;
        setStatusLabelShow(4, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(4, UPDATE_OK);

step6:  //  红外板切到用户模式
    setStatusLabelShow(5, UPDATE_DOING);
    tmpStatus = stepSwitchToUser(INF_BOARD);

    if(!tmpStatus){
        status = 6;
        setStatusLabelShow(5, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(5, UPDATE_OK);

step7:  //  主板切到用户模式
    setStatusLabelShow(6, UPDATE_DOING);
    tmpStatus = stepSwitchToUser(COLOR_BOARD);

    if (!tmpStatus){
        status = 7;
        setStatusLabelShow(6, UPDATE_FAIL);
        return status;
    }
    else{
        bColorIsFactory = false;
        setStatusLabelShow(6, UPDATE_OK);
    }

step8:
    return status;
}

/* 控制板升级流程 */
int fpga::ctrlBoardUpdate(int step)
{
    qDebug("ctrlAddr = %d", ctrlAddr);
    int status = 0;
    bool tmpStatus;

    switch(step){
        case 1:
            goto step1;
            break;
        case 2:
            goto step2;
            break;
        case 3:
            goto step3;
            break;
        case 4:
            goto step4;
            break;
        default:
            break;
    }

step1:
    setStatusLabelShow(0, UPDATE_DOING);
    tmpStatus = ctrlSwitchToFactory();

    if(!tmpStatus){
        status = 1;
        setStatusLabelShow(0, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(0, UPDATE_OK);

step2:
    setStatusLabelShow(1, UPDATE_DOING);
    tmpStatus = ctrlEraseFlash();

    if (!tmpStatus){
        status = 2;
        setStatusLabelShow(1, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(1, UPDATE_OK);

step3:
    tmpStatus = ctrlSendFileImprove();

    if (!tmpStatus){
        status = 3;
        setStatusLabelShow(2, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(2, UPDATE_OK);

step4:
    setStatusLabelShow(3, UPDATE_DOING);
    tmpStatus = ctrlSwitchToUser();

    if (!tmpStatus){
        status = 4;
        setStatusLabelShow(3, UPDATE_FAIL);
        return status;
    }
    else
        setStatusLabelShow(3, UPDATE_OK);

    return status;
}

/*!
 * \brief 检查当前机型是否存在彩色相机板
 * \return false:无 true:有
 */
bool fpga::checkHasColor()
{
    return true;
}

/*!
 * \brief 检查升级文件是否针对115芯片
 * \param fileName 升级文件名
 */
void fpga::checkIs115(QString fileName)
{
    if (fileName.contains("T3QZ")
            && fileName.contains("115")) {
        m_bIs115 = true;
    } else {
        m_bIs115 = false;
    }
}

void fpga::setBoardConfig(UpdateConfig  &config)
{
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++){
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++){

            config.IntConfig[i/16].nIntAddr = 1 + ((i/16)<<4);
            config.IntConfig[i/16].intBoradMask = 0;

            config.ColorConfig[i].nUnitAddr = struCnfg.struLevelInfo[intBoardNum].nUnitId[i];
            config.ColorConfig[i].masterBoardMask = 0;
            config.ColorConfig[i].slaveBoardMask = 0;
            config.ColorConfig[i].failedMask = 0;
        }
    }
    if (curBoardMode == INF_BOARD || curBoardMode ==  INT_CTRL_BOARD) {
        config.dateLen = PACKET_COUNT_SMALL_MAIZE;
    } else {
        config.dateLen = PACKET_COUNT_MAIZE;
    }
}

void fpga::checkBoardStatus(UpdateConfig &config, char packetMask, int step)
{
    bool tmpStatus;
    int boardAddr = 0;
    int loopTimes = 5;
    int mask = (packetMask == MASK_BURN_SDRAM_ALL_FILE) ? MASK_BURN_SDRAM_ALL_FILE | MASK_CRC_ERROR: packetMask;

    for (int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++) {    //查询切换命令执行结果
        for (int i = 0; i < struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++) {
            //! 若待检测相机板上一步骤已经失败，则直接跳过后续操作
            if (config.ColorConfig[i].masterBoardMask) {
                continue;
            }
            //! 获取相机板地址
            boardAddr = config.ColorConfig[i].nUnitAddr;
            //! 设置默认反馈状态
            config.ColorConfig[i].masterBoardMask = 1;
            //! 重复检测反馈状态
            for (int j = 0; j < loopTimes; j++) {
                MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, boardAddr, 0, 0, 0, 1, 0, 1);
                myFlow.msleep(3);
                int ret = MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);
                if (((ret >= 16) && (packetBuf[0]==0xAA && packetBuf[1]==0xAA && packetBuf[2]==0xAA
                        && packetBuf[3]==0xAA && (packetBuf[15]&mask) == packetMask))) {
                    config.ColorConfig[i].masterBoardMask = 0;
                    break;
                }
            }
        }
    }

    /* 打印进度 */
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++) {
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++) {
            tmpStatus = config.ColorConfig[i].masterBoardMask;
            if(tmpStatus == 0){
                setWholeStatusShow(i%2, i/2, UPDATE_DOING,step+1);
            } else {
                if(!config.ColorConfig[i].failedMask) {
                    config.ColorConfig[i].failedMask = 1;
                    setWholeStatusShow(i%2, i/2, UPDATE_FAIL,step);
                }
            }
        }
    }
}

/* 新版整机升级,并行升级方式 ,该函数只对应相机编码为0~20*/
void fpga::wholeUpdateMasterParallel()
{
    /* 共有7步 */
    bool tmpStatus;
    int boardAddr = 0;

    UpdateConfig updateConfig;
    setBoardConfig(updateConfig);

    //初始化状态显示
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++) {
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++) {
            setWholeStatusShow(i%2, i/2, UPDATE_DOING,1);
        }
    }

    // step1.切接口板到工厂模式,记下未成功的接口板
    for(int intBoardNum = 0; intBoardNum < struCnfg.nInterfaceBoardTotal; intBoardNum++){//逐个切换接口板
        intAddr = updateConfig.IntConfig[intBoardNum].nIntAddr;

        tmpStatus = stepSwitchToFactory(INT_BOARD); //逐个切换接口板
        if(!tmpStatus){
            updateConfig.IntConfig[intBoardNum].intBoradMask = 1;
        }
    }
    myFlow.sleep(4);	//	等4s，下位机复位

    /* 打印进度1 */
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++) {
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++) {
            tmpStatus = updateConfig.IntConfig[i/MAX_UNIT_PER_INT].intBoradMask;
            if(tmpStatus == 0){
                setWholeStatusShow(i%2, i/2, UPDATE_DOING,2);
            } else {
                if(!updateConfig.ColorConfig[i].failedMask) {
                    updateConfig.ColorConfig[i].failedMask = 1;
                    setWholeStatusShow(i%2, i/2, UPDATE_FAIL,1);
                }
            }
        }
    }

    // step2.下载文件至接口板,记下未成功的接口板
    for(int intBoardNum = 0; intBoardNum < struCnfg.nInterfaceBoardTotal; intBoardNum++) {
        intAddr = updateConfig.IntConfig[intBoardNum].nIntAddr;
        if (updateConfig.IntConfig[intBoardNum].intBoradMask) {   //该接口板切工厂模式失败,跳到下一块接口板
            continue;
        }
        if(bTransByUSB) {
            tmpStatus = stepSendFileByUSB();
        } else {
            if(struGsh.nIntBaud[intBoardNum] == MY_UARTBAUD_921600) {
                tmpStatus = stepSendFileByHighUART();
            }
        }

        if(!tmpStatus) {
            updateConfig.IntConfig[intBoardNum].intBoradMask = 1;
        }
    }

    /* 打印进度2 */
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++) {
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++) {
            tmpStatus = updateConfig.IntConfig[i/MAX_UNIT_PER_INT].intBoradMask;
            if(tmpStatus == 0){
                setWholeStatusShow(i%2, i/2, UPDATE_DOING,3);
            } else {
                updateConfig.ColorConfig[i].masterBoardMask = 1;
                if(!updateConfig.ColorConfig[i].failedMask) {
                    updateConfig.ColorConfig[i].failedMask = 1;
                    setWholeStatusShow(i%2, i/2, UPDATE_FAIL,2);
                }
            }
        }
    }

    // step3.切主相机板到工厂模式,记下未成功的主相机板
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++){    //切换命令先全部下发
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++){
            if (updateConfig.ColorConfig[i].masterBoardMask) {   //若板子上一步骤已经失败，则直接跳过即可
                continue;
            }

            boardAddr = updateConfig.ColorConfig[i].nUnitAddr;
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_FACTORY, 2, boardAddr, 0, 0, 0, 1, 0, 3);//切换命令
        }
    }

    myFlow.sleep(3);    //等3s再查
    checkBoardStatus(updateConfig, MASK_LOAD_FACTORY_MODE, 3); //查询切换命令执行结果,并在界面上打印

    // step4.下载文件至主相机板（写SDRAM）,记下未成功的主相机板
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++){
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++){
            if (updateConfig.ColorConfig[i].masterBoardMask) {
                continue;
            }
            boardAddr = struCnfg.struLevelInfo[intBoardNum].nUnitId[i];
			/* 根据当前升级文件的类型确定升级包的个数 */
            MySerial.fpgaUpdateWrite(COM_UNIT_FILE_LEN, 2, boardAddr, 0, 0, 0, (updateConfig.dateLen*32-1)/256, (updateConfig.dateLen*32-1)%256, 3);
        }
    }
    /* 向接口板发转发命令 */
    for(int intBoardNum = 0; intBoardNum < struCnfg.nInterfaceBoardTotal; intBoardNum++) {
        intAddr = updateConfig.IntConfig[intBoardNum].nIntAddr;
        MySerial.fpgaUpdateWrite(COM_TRANSFER, intAddr, 0, 0, 0, 0, 0xff, 0xff, 3);
    }

    myFlow.sleep(3);    //等3s再查
    checkBoardStatus(updateConfig, MASK_BURN_SDRAM_ALL_FILE, 4); //查询切换命令执行结果,并在界面上打印

    // step5.烧写FLASH，记录下未成功的相机板
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++) {
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++) {  //向每个相机板烧写FLASH命令
            if (updateConfig.ColorConfig[i].masterBoardMask) {
                continue;
            }
            boardAddr = updateConfig.ColorConfig[i].nUnitAddr;
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_BURN, 2, boardAddr, 0, 0, 0, (curBoardMode == INF_BOARD)?2:1, (curBoardMode == INF_BOARD)?infUserMode:colorUserMode, 3);
        }
    }

    myFlow.sleep(15);   //留15s的时间给下位机烧写
    checkBoardStatus(updateConfig, MASK_BURN_FLASH_FILE, 5); //查询切换命令执行结果,并在界面上打印


    // step6.相机板切换用户模式，记录下未成功的相机板
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++){    //切换命令先全部下发
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++){
            if (updateConfig.ColorConfig[i].masterBoardMask) {
                continue;
            }
            boardAddr = updateConfig.ColorConfig[i].nUnitAddr;
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_USER, 2, boardAddr, 0, 0, 0, (curBoardMode == INF_BOARD)?2:1, (curBoardMode == INF_BOARD)?infUserMode:colorUserMode, 3);//切换命令
        }
    }
    myFlow.sleep(3);    //等3s再查
    checkBoardStatus(updateConfig, MASK_LOAD_CUSTOM_MODE, 6); //查询切换命令执行结果,并在界面上打印

    // step7 红外与彩色相机板升级完成后将接口板切至用户模式
    switchAllIntBoardToUser();

    //显示查询升级结果
    for(int intBoardNum = 0; intBoardNum < struCnfg.nLevelTotal; intBoardNum++) {
        for(int i=0; i<struCnfg.struLevelInfo[intBoardNum].nUnitLevelTotal; i++) {
            tmpStatus = updateConfig.ColorConfig[i].masterBoardMask;
            if(tmpStatus == 0){
                setWholeStatusShow(i%2, i/2, UPDATE_OK);
            }
        }
    }

    switch(curBoardMode){
    case COLOR_BOARD:
        struCnfp.nColorBoardMode = colorUserMode;
        break;
    default:
        break;
    }
}

/* 整机升级 */
void fpga::wholeUpdate()
{
    int tmpStatus;
    bool bChangeBoardMode = false;
    int tmpIntAddr = 0;
    int tmpColorAddr = 0;
    int tmpInfAddr = 0;
    int step = 1;

    if (updateModeIndex == 1) {
        if (struCnfe.nMachine == MACHINE_CF && struCnfe.nEnableCGMachine) {
            wholeUpdateMasterParallel();
        }
        return;
    }

    tmpIntAddr = intAddr;
    tmpColorAddr = colorAddr;
    tmpInfAddr = infAddr;

    intAddr = 0x01;
    colorAddr = 0x00;
    infAddr = 0x00;

    if(curBoardMode == COLOR_BOARD){
        for(int i=0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal; i++){
            colorAddr = struCnfg.struLevelInfo[0].nUnitId[i];
            if(colorAddr%2 == 0){
                // CF机型彩色相机板地址为16时，进行高速升级，需要切换接口板
                if(colorAddr == MAX_UNIT_PER_INT && bTransByUSB) {
                    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.msg_remove_data_wire);
                    int ret = mesBox->exec();
                    if( ret != QDialog::Accepted) {
                        break;
                    } else {
                        setWholeStatusShow(0, i/2, UPDATE_DOING);
                        tmpStatus = colorBoardUpdate(step);
                        if(tmpStatus == 0){
                            bChangeBoardMode = true;
                            setWholeStatusShow(0, i/2, UPDATE_OK);
                        } else {
                            setWholeStatusShow(0, i/2, UPDATE_FAIL);
                        }
                    }
                }
                else {
                    setWholeStatusShow(0, i/2, UPDATE_DOING);
                    tmpStatus = colorBoardUpdate(step);
                    if(tmpStatus == 0){
                        bChangeBoardMode = true;
                        setWholeStatusShow(0, i/2, UPDATE_OK);
                    } else {
                        setWholeStatusShow(0, i/2, UPDATE_FAIL);
                    }
                }

            } else {
                setWholeStatusShow(1, i/2, UPDATE_DOING);
                tmpStatus = colorBoardUpdate(step);
                if(tmpStatus == 0){
                    bChangeBoardMode = true;
                    setWholeStatusShow(1, i/2, UPDATE_OK);
                } else {
                    setWholeStatusShow(1, i/2, UPDATE_FAIL);
                }
            }
            qDebug("[wholeUpdat]colorAddr = %d,infAddr=%d,end", colorAddr,infAddr);
        }
    } else if(curBoardMode == INF_BOARD){
        for(int i=0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal; i++){
            colorAddr = struCnfg.struLevelInfo[0].nUnitId[i];
            if(colorAddr%2 == 0){
                setWholeStatusShow(0, i/2, UPDATE_DOING);
                tmpStatus = infBoardUpdate(step);
                if(tmpStatus == 0){
                    bChangeBoardMode = true;
                    setWholeStatusShow(0, i/2, UPDATE_OK);
                } else {
                    setWholeStatusShow(0, i/2, UPDATE_FAIL);
                }
            } else {
                continue;
            }
            qDebug("[wholeUpdat]colorAddr = %d,infAddr=%d,finish", colorAddr,infAddr);
        }
    }

    intAddr = tmpIntAddr;
    colorAddr = tmpColorAddr;
    infAddr = tmpInfAddr;

    if(bChangeBoardMode){
        switch(curBoardMode){
        case COLOR_BOARD:
            struCnfp.nColorBoardMode = colorUserMode;
            break;
        default:
            break;
        }
    }
    switchAllIntBoardToUser();
}

/* 整机升级完成后将接口板切换至用户模式 */
void fpga::switchAllIntBoardToUser()
{
    int tmp = colorAddr;

    for(int k = 0; k < struCnfg.nLevelTotal; k++) {
        for(int i=0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal; i++){
            colorAddr =  struCnfg.struLevelInfo[0].nUnitId[i];
            if ((colorAddr == 0) || (colorAddr == MAX_UNIT_PER_INT)) {
                stepSwitchToUser(INT_BOARD);
            }
        }
    }

    colorAddr = tmp;
}

/* 根据某一步升级状态显示不同内容 */
void fpga::setStatusLabelShow(int mesIndex, fpga::updateStatus status)
{
    if(stackedWidget->currentIndex() != UPDATE_PAGE)
        return;

    QString suffix;
    suffix = QString();

    switch(status){
    case UPDATE_NO_START:
        pe.setColor(QPalette::WindowText, Qt::blue);
        updateStatusLabels[mesIndex]->setPalette(pe);
        updateStatusLabels[mesIndex]->setText(myLan.upgrade_untreated);
        break;
    case UPDATE_DOING:
        pe.setColor(QPalette::WindowText, Qt::darkBlue);
        updateStatusLabels[mesIndex]->setPalette(pe);
        updateStatusLabels[mesIndex]->setText(myLan.upgrade_doing + suffix);
        break;
    case UPDATE_OK:
        pe.setColor(QPalette::WindowText, Qt::green);
        updateStatusLabels[mesIndex]->setPalette(pe);
        updateStatusLabels[mesIndex]->setText(myLan.upgrade_ok + suffix);
        break;
    case UPDATE_FAIL:
        pe.setColor(QPalette::WindowText, Qt::red);
        updateStatusLabels[mesIndex]->setPalette(pe);
        updateStatusLabels[mesIndex]->setText(myLan.upgrade_fail + suffix);
        break;
    case UPDATE_NO_EXIST:
        pe.setColor(QPalette::WindowText, Qt::green);
        updateStatusLabels[mesIndex]->setPalette(pe);
        updateStatusLabels[mesIndex]->setText(" - -  - -");
        break;
    default:
        break;
    }
}

/* 根据单板的升级状态显示不同内容 */
void fpga::setWholeStatusShow(int level, int mesIndex, fpga::updateStatus status, int guage)
{
    switch(status){
    case UPDATE_NO_START:
        pe.setColor(QPalette::WindowText, Qt::blue);
        statusLabels[level][mesIndex]->setPalette(pe);
        statusLabels[level][mesIndex]->setText(myLan.upgrade_untreated);
        break;
    case UPDATE_DOING:
        pe.setColor(QPalette::WindowText, Qt::darkBlue);
        statusLabels[level][mesIndex]->setPalette(pe);
        if (guage == 0) {
            statusLabels[level][mesIndex]->setText(myLan.upgrade_doing);
        } else {
            statusLabels[level][mesIndex]->setText(myLan.upgrade_doing+QString("(%1)").arg(guage));
        }
        break;
    case UPDATE_OK:
        pe.setColor(QPalette::WindowText, Qt::green);
        statusLabels[level][mesIndex]->setPalette(pe);
        statusLabels[level][mesIndex]->setText(myLan.upgrade_ok);
        break;
    case UPDATE_FAIL:
        pe.setColor(QPalette::WindowText, Qt::red);
        statusLabels[level][mesIndex]->setPalette(pe);
        if (guage == 0) {
            statusLabels[level][mesIndex]->setText(myLan.upgrade_fail);
        } else {
            statusLabels[level][mesIndex]->setText(myLan.upgrade_fail+QString("(%1)").arg(guage));
        }
        break;
    case UPDATE_NO_EXIST:
        pe.setColor(QPalette::WindowText, Qt::blue);
        statusLabels[level][mesIndex]->setPalette(pe);
        statusLabels[level][mesIndex]->setText(" - -  - -");
        break;
    default:
        break;
    }
}

/* 响应挂载U盘界面的返回按钮 */
void fpga::onMountUdiskBackBtnClicked()
{
#ifndef Q_OS_WIN
    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();

    myFlow.saveProfile();
    myFlow.sleep(1);
    myFlow.resetGroupParams();
    myFlow.initCamera();
    myFlow.resetSortParams();
    infoWidget->delayHide();
#endif
    emit sBackToFactorySetPage();
}

/* 响应挂载U盘界面的下一步按钮 */
void fpga::onMountUdiskGoBtnClicked()
{   
#ifdef Q_OS_UNIX
    if (!myFlow.mountUdisk()) {
        infoWidget->setLabelText(myLan.msg_insert_udisk);
        infoWidget->showSecs();
        return;
    }
#endif

    QFileInfoList fileInfo;
    QString fileName;
    QDir dir(UPDATE_FILE_PATH);

    fileInfo = dir.entryInfoList();
    updateFileList.clear();

    for(int i=0; i<fileInfo.size(); i++){
        fileName = fileInfo.at(i).fileName();
        if (fileName.contains(".rpd")) {
            updateFileList.push_back(fileName);
        }
    }

    int fileNum = (updateFileList.size()>MAX_FILE_NUM) ? MAX_FILE_NUM:updateFileList.size();
    for(int i=0; i<fileNum; i++){
        fileItem[i]->setText(QString("%1. %2").arg(i+1).arg(updateFileList.at(i)));
        fileItem[i]->setHidden(false);
    }
    for(int i=fileNum; i<MAX_FILE_NUM;i++){
        fileItem[i]->setHidden(true);
    }

    if(bTransByUSB)
        transModeBtn->setText(myLan.ai_high_speed);
    else
        transModeBtn->setText(myLan.ai_low_speed);

    stackedWidget->setCurrentIndex(FILE_LIST_PAGE);
}

/* 响应文件列表界面文件索引发生改变的事件 */
void fpga::onFileListChangedSlt(int index)
{
    int fileNum = (updateFileList.size()>MAX_FILE_NUM) ? MAX_FILE_NUM:updateFileList.size();
    for(int i=0; i<fileNum; i++){
        fileList->item(i)->setBackground(QBrush(QColor(190,190,190)));
    }
    fileList->item(index)->setBackground(QBrush(QColor(172,144,75)));
}

/* 响应文件列表界面下一步按钮 */
void fpga::onFileListNextBtnClicked()
{
    if(updateFileList.size() == 0)
        return;

    QString fileName = QString("%1%2").arg(UPDATE_FILE_PATH).arg(updateFileList.at(fileList->currentRow()));
    qDebug("fileName = %s", fileName.toLatin1().data());

    FILE *fp;
    if ((fp = fopen(fileName.toLatin1().data(), "r")) == NULL){
        infoWidget->setLabelText(myLan.msg_file_open_error);
        infoWidget->showSecs();
        return;
    }

    /* 1.根据文件名判断升级板子的类型 */
    if (fileName.contains("JKKZ")) {
        curBoardMode = INT_CTRL_BOARD;
    } else if (fileName.contains("JK")) {
        curBoardMode = INT_BOARD;
    } else if (fileName.contains("SXCG")
               || fileName.contains("T2QZ")
               || fileName.contains("T3QZ")) {
        //! 针对I机型检查是否存在彩色相机
        if (!checkHasColor()) {
            infoWidget->setLabelText(myLan.msg_no_color);
            infoWidget->showSecs();
            return;
        }

        curBoardMode = COLOR_BOARD;
    } else if(fileName.contains("KZ")){
        curBoardMode = CTRL_BOARD;
    } else if(fileName != "") {
        infoWidget->setLabelText(myLan.msg_unknown_file);
        infoWidget->showSecs();
        return;
    }

    //! 检查是否为115芯片
    checkIs115(fileName);

    /* 2.设置升级页面的显示 */
    setUpdateDisplay();

    /* 3.将升级文件拷贝至本地缓冲区 */
    memset(fileBuf, 0, PACKET_COUNT_BIG*FILE_PACKET_SIZE);
    if (curBoardMode == INF_BOARD
            || curBoardMode == INT_CTRL_BOARD) {
        fread(fileBuf, 1, PACKET_COUNT_SMALL*FILE_PACKET_SIZE, fp);
    } else if (curBoardMode == COLOR_BOARD && m_bIs115) {
        fread(fileBuf, 1, PACKET_COUNT_BIG*FILE_PACKET_SIZE, fp);
    } else {
        fread(fileBuf, 1, PACKET_COUNT*FILE_PACKET_SIZE, fp);
    }
    fclose(fp);

    if (bTransByUSB) {
        stackedWidget->setCurrentIndex(NOTICE_PAGE);
    } else {
        stackedWidget->setCurrentIndex(UPDATE_PAGE);
    }

    /* 4.重置远程升级相关参数,并更新界面显示 */
    userModeGroup->clearBtnsIcon();
    layerGroup->setCurrentIndex(0);
    initUpdateParams();

    boardUpdateLcdNum->display(1);  
}

/* 响应文件列表界面上一步按钮 */
void fpga::onFileListLastBtnClicked()
{
    stackedWidget->setCurrentIndex(MOUNT_PAGE);
}

/* 响应文件列表界面中传输模式按钮 */
void fpga::onTransModeBtnClicked()
{
    bTransByUSB = !bTransByUSB;
    if(bTransByUSB)
        transModeBtn->setText(myLan.ai_high_speed);
    else
        transModeBtn->setText(myLan.ai_low_speed);
}

/* 选择升级模式按钮 */
void fpga::onModeMutexIndexChanged(int index)
{
    updateModeIndex = index;
}

/* 响应提示界面中的"上一步"按钮 */
void fpga::onNoticeLastBtnClicked()
{
    stackedWidget->setCurrentIndex(FILE_LIST_PAGE);
}

/* 响应提示界面中的"下一步"按钮 */
void fpga::onNoticeNextBtnClicked()
{
#ifdef Q_OS_UNIX
    if(myUsbDev != NULL){
        usb.myusbFree();
    }
    usb.myusbInit();
    if(myUsbDev == NULL){
        qDebug("usb init error");
        infoWidget->setLabelText(myLan.cfm_insert_datawire);
        infoWidget->showSecs();
        return;
    }
#endif

    stackedWidget->setCurrentIndex(UPDATE_PAGE);
}

/* 响应板级升级界面的返回按钮 */
void fpga::onBoardUpdateBackBtnClicked()
{
    if(bTransByUSB)
        stackedWidget->setCurrentIndex(MOUNT_PAGE);
    else
        stackedWidget->setCurrentIndex(FILE_LIST_PAGE);
}

/* 响应板级升级界面的升级按钮 */
void fpga::onBoardUpdateGoBtnClicked()
{
    if((curBoardMode==INT_BOARD && intUserMode==0)
            || (curBoardMode==COLOR_BOARD && colorUserMode==0)
            || (curBoardMode==INF_BOARD && infUserMode==0)
            || (curBoardMode==CTRL_BOARD && ctrlUserMode==0)){
        infoWidget->setLabelText(myLan.msg_select_user_mode);
        infoWidget->showSecs();
        return;
    }

    infoWidget->hideCentralWidget();
    switch(curBoardMode){
    case INT_BOARD:
    case INT_CTRL_BOARD:
        for(int i=0; i<4; i++){
            setStatusLabelShow(i, UPDATE_NO_START);
        }
        infoWidget->delayShow();
        curUpdateStep = intBoardUpdate(1);
        infoWidget->hide();
        break;

    case COLOR_BOARD:
        if((intAddr==0x01 && bColorFileIsOk[0]) || (intAddr==0x11 && bColorFileIsOk[1])
                || (intAddr==0x21 && bColorFileIsOk[2])){
            setStatusLabelShow(0, UPDATE_OK);
            setStatusLabelShow(1, UPDATE_OK);
            for(int i=2; i<5; i++){
                setStatusLabelShow(i, UPDATE_NO_START);
            }
        }
        else{
            for(int i=0; i<5; i++){
                setStatusLabelShow(i, UPDATE_NO_START);
            }
        }
        infoWidget->delayShow();
        curUpdateStep = colorBoardUpdate(1);
        infoWidget->hide();
        break;

    case INF_BOARD:
        if((intAddr==0x01 && bInfFileIsOk[0]) ||(intAddr==0x11 && bInfFileIsOk[1])
                || (intAddr==0x21 && bInfFileIsOk[2])){
            setStatusLabelShow(0, UPDATE_OK);
            setStatusLabelShow(1, UPDATE_OK);
            for(int i=2; i<7; i++){
                setStatusLabelShow(i, UPDATE_NO_START);
            }
        }
        else{
            for(int i=0; i<7; i++){
                setStatusLabelShow(i, UPDATE_NO_START);
            }
        }
        infoWidget->delayShow();
        curUpdateStep = infBoardUpdate(1);
        infoWidget->hide();
        break;

    case CTRL_BOARD:
        for(int i=0; i<4; i++){
            setStatusLabelShow(i, UPDATE_NO_START);
        }
        infoWidget->delayShow();
        curUpdateStep = ctrlBoardUpdate(1);
        infoWidget->hide();
        break;

    default:
        break;
    }

    if(curUpdateStep != 0) {
        boardUpdateRetryBtn->setEnabled(true);
	} else {
        // 升级成功后设置对应的全局参数
        switch(curBoardMode){
        case INT_BOARD:
            struCnfp.nInterfaceBoardMode = intUserMode;
            break;
        }
    }
}

/* 响应板级升级界面的整机升级按钮 */
void fpga::onBoardUpdateWholeBtnClicked()
{
    if((curBoardMode==COLOR_BOARD && colorUserMode==0)
            || (curBoardMode==INF_BOARD && infUserMode==0)){
        infoWidget->setLabelText(myLan.msg_select_user_mode);
        infoWidget->showSecs();
        return;
    }

    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_upgrade_whole);
    int ret = mesBox->exec();

    if(ret == QDialog::Accepted){
        setUpdateWholeDisplay();
        clearFileStatus();
        stackedWidget->setCurrentIndex(WHOLE_UPDATE_PAGE);
        infoWidget->hideCentralWidget();
        infoWidget->delayShow();
        wholeUpdate();
        infoWidget->hide();
    }
}

/* 响应板级升级界面的重试按钮 */
void fpga::onBoardUpdateRetryBtnClicked()
{
    switch(curBoardMode){
    case INT_BOARD:
    case INT_CTRL_BOARD:
        curUpdateStep = intBoardUpdate(curUpdateStep);
        break;
    case COLOR_BOARD:
        curUpdateStep = colorBoardUpdate(curUpdateStep);
        break;
    case INF_BOARD:
        curUpdateStep = infBoardUpdate(curUpdateStep);
        break;
    case CTRL_BOARD:
        curUpdateStep = ctrlBoardUpdate(curUpdateStep);
        break;
    default:
        break;
    }

    if(curUpdateStep == 0){
        boardUpdateRetryBtn->setEnabled(false);

        // 升级成功后设置对应的全局参数
		if (updateModeIndex != 1){
        	switch(curBoardMode){
        	case INT_BOARD:
            	struCnfp.nInterfaceBoardMode = intUserMode;
            	break;
        	case COLOR_BOARD:
            	struCnfp.nColorBoardMode = colorUserMode;
            	break;
        	default:
            	break;
			}
        }
    }
}

/* 响应用户模式改变的事件 */
void fpga::onUserModeChangedSlt(int index)
{
    int tmpMode;
    if(index == 0)
        tmpMode = 1;
    else if(index == 1)
        tmpMode = 2;
    else if(index == 2)
        tmpMode = 4;

    switch(curBoardMode){
    case INT_BOARD:
    case INT_CTRL_BOARD:
        intUserMode = tmpMode;
        break;
    case COLOR_BOARD:
        colorUserMode = tmpMode;
        break;
    case INF_BOARD:
        infUserMode = tmpMode;
        break;
    case CTRL_BOARD:
        ctrlUserMode = tmpMode;
        break;
    default:
        break;
    }
}

/* 响应主从发生改变的事件 */
void fpga::onlayerAddChangedSlt(int index)
{
    qDebug("colorAddr: %d",colorAddr);
}

/* 响应层（前后视）发生改变的事件 */
void fpga::onLayerChangedSlt(int index)
{
    if (curBoardMode == INT_BOARD || curBoardMode == INT_CTRL_BOARD) {
        if(index == 0){
            intAddr = 0x01;
        } else{
            intAddr = 0x11;
        }
    } else {
        if(index == 0){
            if (colorAddr%2==1 && infAddr%2==1) {
                colorAddr -= 1;
                infAddr -= 1;
            }
        } else{
            if (colorAddr%2==0 && infAddr%2==0) {
                colorAddr += 1;
                infAddr += 1;
            }
        }
    }
    printf("colorAddr: %d, intAddr: %d",colorAddr,intAddr);
	fflush(0);
}

/* 响应板级升级界面的增加板号按钮 */
void fpga::onBoardNumPlusBtnClicked()
{
    int boardNum = colorAddr+1-intAddr;

    if(struCnfg.struLevelInfo[curLevel].nViewTotal == 2){   //双视
        if(boardNum < struCnfg.struLevelInfo[curLevel].nUnitLevelTotal-2){
            colorAddr += 2;
            infAddr += 2;
            boardNum += 2;
            boardUpdateLcdNum->display(boardNum/2+1);
        }
    } else if(struCnfg.struLevelInfo[curLevel].nViewTotal == 1){  //单视
        if(boardNum < struCnfg.struLevelInfo[curLevel].nUnitLevelTotal*2-2){
            colorAddr += 2;
            infAddr += 2;
            boardNum += 2;
            boardUpdateLcdNum->display(boardNum/2+1);
        }
    }

	if ((int)boardUpdateLcdNum->value() == MAX_UNIT_PER_INT/2 + 1) {
		bColorFileIsOk[0] = false;
	}

    qDebug("[onBoardNumPlusBtnClicked]colorAddr = %d,infAddr = %d", colorAddr,infAddr);
}

/* 响应板级升级界面的减小板号按钮 */
void fpga::onBoardNumMinusBtnClicked()
{
    int boardNum = colorAddr+1-intAddr;

    if(struCnfg.struLevelInfo[curLevel].nViewTotal == 2){   //双视
        if(boardNum >= 2){
            colorAddr -= 2;
            infAddr -= 2;
            boardNum -= 2;
            boardUpdateLcdNum->display(boardNum/2+1);
        }
    } else if(struCnfg.struLevelInfo[curLevel].nViewTotal == 1){  //单视
        if(boardNum >= 2){
            colorAddr -= 2;
            infAddr -= 2;
            boardNum -= 2;
            boardUpdateLcdNum->display(boardNum/2+1);
        }
    }

	if ((int)boardUpdateLcdNum->value() == MAX_UNIT_PER_INT/2) {
		bColorFileIsOk[0] = false;
	}

    printf("[onBoardNumMinusBtnClicked]colorAddr = %d", colorAddr);
	fflush(0);
}

/* 响应整机升级界面的返回按钮 */
void fpga::onWholeUpdateBackBtnClicked()
{
    clearFileStatus();
    stackedWidget->setCurrentIndex(UPDATE_PAGE);
}

/* 响应整机升级界面的退出按钮 */
void fpga::onWholeUpdateReturnBtnClicked()
{
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_reboot);
    if (mesBox->exec() == QDialog::Accepted) {
#ifdef Q_OS_WIN
        emit sBackToFactorySetPage();
        stackedWidget->setCurrentIndex(MOUNT_PAGE);
#else
        myFlow.saveProfile();
        myFlow.sleep(1);
        system("reboot");
#endif
    }
}

/* USB发送文件 */
void fpga::USBSendFile(int cmd, unsigned char module, int num)
{
#ifdef Q_OS_UNIX
    char packet[PACKET_COUNT_BIG*FRAME_LEN+16];
    char sBuf[FRAME_LEN_MAIZE];
    unsigned int crcitt;
    int i, j;
    int ret = 0;

    /* pack & add checksum */
    bzero(packet, PACKET_COUNT_BIG*FRAME_LEN+16);

    packet[0]  = 0xF0;
    packet[1]  = 0xA5;
    packet[2]  = module;
    packet[3]  = 0;
    packet[4]   = cmd/256;
    packet[5]   = cmd%256;
    packet[6]   = struGsh.nSerialCount1++;
    packet[7]   = 0;
    packet[8]   = 0;
    packet[9]   = 0;
    packet[10]  = (num-1)/256;
    packet[11]  = (num-1)%256;

    for(i=0; i<num; i++){
        for(j=0; j<FRAME_LEN; j++){
            packet[12+i*FRAME_LEN+j] = fileBuf[i][j];
        }
    }

    crcitt = MySerial.crcFpga(packet + 2, num*FRAME_LEN + 10);
    packet[num*FRAME_LEN+12] = crcitt/256;
    packet[num*FRAME_LEN+13] = crcitt%256;
    packet[num*FRAME_LEN+14] = 0xff;
    packet[num*FRAME_LEN+15] = 0xff;

#if 0
    printf("\n");
    printf("usb >> ");
    for(i=0; i<500; i++){
        if (i%20 == 0) {
            printf("\n");
        }
        printf("%02x ", packet[i]);
    }
    printf("\n");
#endif

    if (struGsh.nSerialCount1 > 250) {
        struGsh.nSerialCount1 = 0;
    }

    ret = usb_bulk_read(myUsbDev, USB_EP_IN_AI, sBuf, FRAME_LEN_MAIZE, 1000);
    qDebug("MSG : read_ret=%d", ret);
    ret = usb_bulk_write(myUsbDev, USB_EP_OUT_AI, packet, num*FRAME_LEN+16, 1000);
    qDebug("MSG : write_ret = %d", ret);
#endif
}

/* 返回第一页 */
void fpga::goToFirstPageSlt()
{
    stackedWidget->setCurrentIndex(MOUNT_PAGE);
}

/* 响应当前页索引发生变化的情况 */
void fpga::onStackedWidgetIndexChangedSlt(int index)
{

    switch(index){
    case MOUNT_PAGE:
        titleLabel->setText(QString("%1 > %2").arg(myLan.factory_set).arg(myLan.upgrade_fpga));
        break;
    case FILE_LIST_PAGE:
        titleLabel->setText(QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.upgrade_fpga)
                            .arg(myLan.upgrade_file_list));
        break;
    case NOTICE_PAGE:
        titleLabel->setText(QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.upgrade_fpga)
                            .arg(myLan.upgrade_usb_communication));
        break;
    case UPDATE_PAGE:
        titleLabel->setText(QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.upgrade_fpga)
                            .arg(myLan.upgrade_board));
        break;
    case WHOLE_UPDATE_PAGE:
        titleLabel->setText(QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.upgrade_fpga)
                            .arg(myLan.upgrade_whole));
        break;
    default:
        break;
    }
}
