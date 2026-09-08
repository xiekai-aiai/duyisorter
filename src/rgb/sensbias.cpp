/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        sensbias.cpp
 * \brief       彩色机型灵敏度偏置界面源文件
 * \date        2015.01.14
 */
#include "sensbias.h"

SensBias::SensBias(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT-10);
    m_config = g_Config::getInstance();

    createPage();
    connectSigAndSlt();

    for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
        m_nArithIndex[i] = 0;
    }
}

/* 创建灵敏度偏置灵敏度偏置布局页面 */
void SensBias::createPageForGeneral()
{
    for (int i = 0; i < MAX_UNIT; i++) {
        m_lineEdits[i] = new myLineEdit;
        m_lineEdits[i]->setFont(m_config->getFont());
            if(i%2 == 0) {
                m_labels[i] = new myLabel(QString("%1%2").arg(myLan.front_view).arg(i/2+1));
                upRightLay->addWidget(m_labels[i], i/2, 0);
                upRightLay->addWidget(m_lineEdits[i], i/2, 1);
            } else {
                m_labels[i] = new myLabel(QString("%1%2").arg(myLan.rear_view).arg(i/2+1));
                upRightLay->addWidget(m_labels[i], i/2, 2);
                upRightLay->addWidget(m_lineEdits[i], i/2, 3);
            }

        m_labels[i]->setFont(m_config->getFont());
        m_labels[i]->setAlignment(Qt::AlignCenter);
        m_labels[i]->setMaximumWidth(BTN_WIDTH);
        m_lineEdits[i]->setMaximumWidth(BTN_WIDTH-20);
        m_lineEdits[i]->setMinimumHeight(BTN_HEIGHT);
     }
}

/* 创建灵敏度偏置灵敏度偏置布局页面 */
void SensBias::createPageForMazie()
{
    int channelNumPerLevel = (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal + 2) /4;

    /* 通道  前下  后下  前上  后上 */
    m_labels[0] = new myLabel(QString("%1").arg(myLan.chute));
    m_labels[1] = new myLabel(QString("%1%2").arg(myLan.front).arg(myLan.lower));
    m_labels[2] = new myLabel(QString("%1%2").arg(myLan.rear).arg(myLan.lower));
    m_labels[3] = new myLabel(QString("%1%2").arg(myLan.front).arg(myLan.upper));
    m_labels[4] = new myLabel(QString("%1%2").arg(myLan.rear).arg(myLan.upper));
    for (int i = 0; i < 5; i++) {
        m_labels[i]->setMinimumWidth(SMALL_BTN_WIDTH + 20);
        upRightLay->addWidget(m_labels[i],0,i);
    }
    for (int i = 0; i < (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal + 2) /4; i++) {//首列通道的标签总数
        m_labels[i + 5] = new myLabel(QString("  %1").arg(i + 1));
        upRightLay->addWidget(m_labels[i + 5], i + 1, 0);
    }

    for(int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        m_lineEdits[i] = new myLineEdit;
        m_lineEdits[i]->setFont(m_config->getFont());
        m_lineEdits[i]->setMaximumWidth(BTN_WIDTH-20);
        m_lineEdits[i]->show();
    }

    for(int i = 0; i < channelNumPerLevel; i++) {//前下视列
        upRightLay->addWidget(m_lineEdits[i*2], i + 1, 1);
    }
    for(int i = channelNumPerLevel; i < channelNumPerLevel * 2; i++) {//后下视列
        upRightLay->addWidget(m_lineEdits[(i-channelNumPerLevel)*2+1], i + 1 - channelNumPerLevel, 2);
    }
    for(int i = channelNumPerLevel * 2; i < channelNumPerLevel * 3 - 1; i++) {//前上视列
        upRightLay->addWidget(m_lineEdits[(i- channelNumPerLevel*2)*2+channelNumPerLevel*2], i + 1 - channelNumPerLevel * 2, 3);
    }
    for(int i = channelNumPerLevel * 3 - 1; i < channelNumPerLevel * 4 - 2; i++) {//后上视列
        upRightLay->addWidget(m_lineEdits[(i- channelNumPerLevel*3+1)*2+channelNumPerLevel*2+1], i + 2 - channelNumPerLevel * 3, 4);
    }
}

/* 创建灵敏度偏置灵敏度偏置布局页面 */
void SensBias::createPageForLD4()
{
    int channelNumPerLevel = (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal + 2) /4;

    /* 通道  上正  下正  上侧  下侧 */
    m_labels[0] = new myLabel(QString("%1").arg(myLan.chute));
    m_labels[1] = new myLabel(QString("%1%2").arg(myLan.upper).arg(myLan.master_view));
    m_labels[2] = new myLabel(QString("%1%2").arg(myLan.lower).arg(myLan.master_view));
    m_labels[3] = new myLabel(QString("%1%2").arg(myLan.upper).arg(myLan.slave_view));
    m_labels[4] = new myLabel(QString("%1%2").arg(myLan.lower).arg(myLan.slave_view));
    for (int i = 0; i < 5; i++) {   //共4列
        m_labels[i]->setMinimumWidth(SMALL_BTN_WIDTH + 20);
        upRightLay->addWidget(m_labels[i],0,i);
    }

    for (int i = 0; i < (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal + 2) /4; i++) {//首列通道的标签总数
        m_labels[i + 5] = new myLabel(QString("  %1").arg(i + 1));
        upRightLay->addWidget(m_labels[i + 5], i + 1, 0);
    }

    for(int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        m_lineEdits[i] = new myLineEdit;
        m_lineEdits[i]->setFont(m_config->getFont());
        m_lineEdits[i]->setMaximumWidth(BTN_WIDTH-20);
        m_lineEdits[i]->show();
    }

    for(int i = 0; i < channelNumPerLevel; i++) {//上正视列
        upRightLay->addWidget(m_lineEdits[i*2], i + 1, 1);
    }
    for(int i = channelNumPerLevel; i < channelNumPerLevel * 2; i++) {//下正视列
        upRightLay->addWidget(m_lineEdits[(i- channelNumPerLevel)*2+1], i + 1 - channelNumPerLevel, 2);
    }
    for(int i = channelNumPerLevel * 2; i < channelNumPerLevel * 3 - 1; i++) {//上侧视列
        upRightLay->addWidget(m_lineEdits[(i- channelNumPerLevel*2)*2+channelNumPerLevel*2], i + 1 - channelNumPerLevel * 2, 3);
    }
    for(int i = channelNumPerLevel * 3 - 1; i < channelNumPerLevel * 4 - 2; i++) {//下侧视列
        upRightLay->addWidget(m_lineEdits[(i- channelNumPerLevel*3+1)*2+channelNumPerLevel*2+1], i + 2 - channelNumPerLevel * 3, 4);
    }
}

/* 创建灵敏度偏置页面 */
void SensBias::createPage()
{
    /* 定义页面中使用的布局管理器 */
    upRightLay = new QGridLayout();
    QHBoxLayout *upLay = new QHBoxLayout();
    QHBoxLayout *downLay = new QHBoxLayout();
    QVBoxLayout *mainLay = new QVBoxLayout(this);

    /* 定义页面上半部分的控件 */
    m_list = new QListWidget;
    m_list->setFixedSize(width()/6, height()-BTN_HEIGHT-50);
    m_list->setFont(m_config->getFont());
    for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
        m_items[i] = new QListWidgetItem;
        m_list->addItem(m_items[i]);
        m_items[i]->setTextAlignment(Qt::AlignCenter);
    }
    m_list->setCurrentRow(0);

    createPageForGeneral();

    /* 定义页面下半部分的控件 */
    m_backBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_backBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_nMaizeSensIndex = 0;

    m_sensGroup = new myMutex(Qt::Horizontal,0);
    m_sensGroup->setNum(3);

    m_levelBtn = new myPushButton(myLan.back, QIcon());
    m_levelBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    struGsh.nLevel = 0;
    m_levelBtn->setText(myLan.upper_layer);
    m_levelBtn->hide();

    m_sensGroup->setLabelText(myLan.sensitivity + "1",myLan.sensitivity + "2",myLan.proportion + " " + myLan.sensitivity);
    if (LCD_WIDTH == 1024) {
        m_sensGroup->setFixedSize(BTN_WIDTH * 4,BTN_HEIGHT + 15);
    } else {
        m_sensGroup->setFixedSize(BTN_WIDTH * 3 + 30,BTN_HEIGHT + 15);
    }

    if (struCnfe.nEnableMaizeDoubleView == 1) {
        m_sensGroup->show();
    } else {
        m_sensGroup->hide();
    }
    /* 对页面控件进行布局 */
    upLay->addWidget(m_list,Qt::AlignLeft);
    upLay->addLayout(upRightLay);
    downLay->addWidget(m_sensGroup,0,Qt::AlignLeft);
    downLay->addWidget(m_levelBtn,0,Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(m_backBtn);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);
}

/* 刷新灵敏度偏置页面 */
void SensBias::updatePage()
{
    updateList();
    updateSensBias();
}

/* 连接页面中的信号与槽 */
void SensBias::connectSigAndSlt()
{
    m_sigMap = new QSignalMapper(this);
    for(int i=0; i<struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        m_sigMap->setMapping(m_lineEdits[i], i);
        connect(m_lineEdits[i], SIGNAL(pressed()), m_sigMap, SLOT(map()));
    }
    connect(m_sigMap, SIGNAL(mapped(int)), this, SLOT(onLineEditsPressed(int)));
    connect(m_list, SIGNAL(currentRowChanged(int)), this, SLOT(onListIndexChangedSlt()));
    connect(m_backBtn, SIGNAL(pressed()), this, SLOT(onBackBtnPressed()));
    connect(m_sensGroup, SIGNAL(indexChanged(int)), this, SLOT(onSensGroupIndexChange(int)));
    connect(m_levelBtn, SIGNAL(pressed()), this, SLOT(onLevelBtnPressed()));
}

/* 判断该算法是否需要灵敏度偏置功能 */
bool SensBias::isSensBiasEnable(int nArithNo)
{
    switch(nArithNo) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
    case ARITH_INTEL_A:
    case ARITH_INTEL_B:
    case ARITH_MAIZE:
        return true;
    default:
        return false;
    }
}

/* 刷新算法列表 */
void SensBias::updateList()
{
    int nArithNum = 0;
    int headLine = -1;

    /* 刷新算法列表某行对应的算法代号 */
    for(int i=0; i<ARITHMETIC_TOTAL; i++) {
        if(struCnfp.nArithmeticEnable[i] == 1 && isSensBiasEnable(i)) {
            m_nArithIndex[i] = i;
            m_items[i]->setText(QString("\n%1\n").arg(getArithNameFromId(i)));
            m_items[i]->setHidden(false);
            if (headLine == -1) {
                headLine = i;
            }
            nArithNum++;
        } else {
            m_items[i]->setHidden(true);
        }
    }

    if (nArithNum > 0) {
        m_list->setCurrentRow(headLine);
    }
}

/* 刷新灵敏度偏置 */
void SensBias::updateSensBias()
{
    int tmpIndexBias = 0; //LDS2下层的灵敏度偏置存在nSensBias数组的后一半，当前层为下层时此处需要地址的偏移

    /* 显示与通道数对应的灵敏度偏置 */
    for(int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        m_labels[i]->show();
        m_lineEdits[i]->show();
    }

    for (int i = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i < MAX_UNIT; i++) {
        m_labels[i]->hide();
        m_lineEdits[i]->hide();
    }

    /* 刷新灵敏度偏置的显示 */
    int nArithIndex = m_nArithIndex[m_list->currentRow()];
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        int nTmpBias = 0;
        m_lineEdits[i]->setText(QString("%1%").arg((double)(nTmpBias/10.0)));
        if (nArithIndex == ARITH_MAIZE) {
            nTmpBias = struCnfp.nSensBiasMaize[i][m_nMaizeSensIndex];
        } else {
            nTmpBias = struCnfp.nSensBias[i+tmpIndexBias][nArithIndex];
        }
        m_lineEdits[i]->setText(QString("%1%").arg((double)(nTmpBias/10.0)));
    }

    if (struCnfe.nEnableMaizeDoubleView == 1) {
        if (nArithIndex == ARITH_MAIZE) {
            m_sensGroup->show();
        } else {
            m_sensGroup->hide();
        }
    } else {
        m_sensGroup->hide();
    }

    m_levelBtn->hide();
}

/* 根据算法索引号获取算法名称 */
QString SensBias::getArithNameFromId(int nArithIndex)
{
    QString str;
    switch(nArithIndex) {
    case ARITH_GREY_A:
        str = myString.sArithmeticName[ARITH_GREY_A];
        break;
    case ARITH_GREY_B:
        str = myString.sArithmeticName[ARITH_GREY_B];
        break;
    case ARITH_DISCOLOR_A:
        str = myLan.material_defect_discolor_a;
        break;
    case ARITH_DISCOLOR_B:
        str = myLan.material_defect_discolor_b;
        break;
    case ARITH_INTEL_A:
        str = myString.sArithmeticName[ARITH_INTEL_A];
        break;
    case ARITH_INTEL_B:
        str = myString.sArithmeticName[ARITH_INTEL_B];
        break;
    case ARITH_MAIZE:
        str = myString.sArithmeticName[ARITH_MAIZE];
        break;
    default:
        break;
    }

    return str;
}

/* 根据相机号获取该相机所在识别组ID */
int SensBias::getIdentifyGroupId(int nUnit)
{
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[i].nUnitCount; j++) {
            if (nUnit == struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[i].nUnitId[j]) {
                return i;
            }
        }
    }
    return 0;
}

/* 响应编辑框按下事件 */
void SensBias::onLineEditsPressed(int nIndex)
{
    int nArithIndex = 0;
    double fValue = 0.0;
    inputType type;
    int nMin, nMax;
    int nGroupId = 0;
    int tmpIndexBias = 0; //LDS2下层的灵敏度偏置存在nSensBias数组的后一半，当前层为下层时此处需要地址的偏移

    qDebug("bias nIndex:%d",nIndex);

    nArithIndex = m_nArithIndex[m_list->currentRow()];
    fValue = (double)struCnfp.nSensBias[nIndex+tmpIndexBias][nArithIndex]/10.0;

    /* 智能算法输入为整型,其它算法输入为浮点型 */
    if(nArithIndex == ARITH_INTEL_A || nArithIndex == ARITH_INTEL_B) {
        type = intType;
        nMin = -100;
        nMax = 100;
    } else {
        type = floatType;
        nMin = -30;
        nMax = 30;
    }

    if (struCnfe.nEnableMaizeDoubleView == 1) {
        fValue = struCnfp.nSensBiasMaize[nIndex][m_nMaizeSensIndex]/10.0;
    }

    myInputPanel *inputPanel = new myInputPanel(type, nMin, nMax, fValue);
    inputPanel->setInputNegEn(true);
    int nRet = inputPanel->exec();

    if(nRet == QDialog::Accepted) {
        fValue = inputPanel->getValue();
        if(fValue > 0.000001) {
            if (nArithIndex == ARITH_MAIZE) {
                struCnfp.nSensBiasMaize[nIndex][m_nMaizeSensIndex] = fValue*10+0.5;
            } else {
                struCnfp.nSensBias[nIndex+tmpIndexBias][nArithIndex] = fValue*10+0.5;
            }
        } else if(fValue < -0.000001) {
            if (nArithIndex == ARITH_MAIZE) {
                struCnfp.nSensBiasMaize[nIndex][m_nMaizeSensIndex] = fValue*10-0.5;
            } else {
                struCnfp.nSensBias[nIndex+tmpIndexBias][nArithIndex] = fValue*10-0.5;
            }
        } else {
            if (nArithIndex == ARITH_MAIZE) {
                struCnfp.nSensBiasMaize[nIndex][m_nMaizeSensIndex] = 0;
            } else {
                struCnfp.nSensBias[nIndex+tmpIndexBias][nArithIndex] = 0;
            }
        }
        m_lineEdits[nIndex]->setText(QString("%1%").arg((double)struCnfp.nSensBias[nIndex+tmpIndexBias][nArithIndex]/10.0));
        if (nArithIndex == ARITH_MAIZE) {
            m_lineEdits[nIndex]->setText(QString("%1%").arg((double)struCnfp.nSensBiasMaize[nIndex][m_nMaizeSensIndex]/10.0));
        } else {
            m_lineEdits[nIndex]->setText(QString("%1%").arg((double)struCnfp.nSensBias[nIndex+tmpIndexBias][nArithIndex]/10.0));
        }
        /* 发送算法参数 */
        nGroupId = getIdentifyGroupId(nIndex);
        myFlow.resetMaterial(struGsh.nLevel, nGroupId, 0, nArithIndex, 0);
    }
}

/* 响应算法列表索引号变化事件 */
void SensBias::onListIndexChangedSlt()
{
    updateSensBias();
}

/* 响应返回按钮按下事件 */
void SensBias::onBackBtnPressed()
{
    emit goToSetSensPageSig();
}

void SensBias::onSensGroupIndexChange(int index)
{
    m_nMaizeSensIndex = index;
    updateSensBias();
}

void SensBias::onLevelBtnPressed()
{
    updateSensBias();
}
