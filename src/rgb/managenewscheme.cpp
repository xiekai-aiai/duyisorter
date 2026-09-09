/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        managenewscheme.cpp
 * \brief       彩色机型新建界面源文件
 * \date        2015.01.14
 */
#include "managenewscheme.h"
#include "unilog.h"

ManageNewScheme::ManageNewScheme(QWidget* parent) :
    QWidget(parent)
{
    listWidget = new myListWidget();
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setFixedWidth(BTN_WIDTH);
    listWidget->setMovement(QListView::Static);
    listWidget->setIconSize(QSize(ICON_WID, ICON_HEI));

    /*页面堆栈*/
    stackedWidget = new QStackedWidget(this);

    //预置方案内容
    createPrefixedProfile();
    updatePrefixedProfile();

    //确定
    okBtn = new myPushButton(myLan.ok, myIcon.Action_Apply, false, this);
    //取消
    cancelBtn = new myPushButton(myLan.cancel, myIcon.Action_Cancel, this);
    /*尺寸限制*/
    okBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    cancelBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    /*整体页面布局*/
    newSchemePageHBLayout = new QHBoxLayout();
    newSchemePageHBLayout->addWidget(okBtn);
    horizontalSpacer = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    newSchemePageHBLayout->addItem(horizontalSpacer);
    newSchemePageHBLayout->addWidget(cancelBtn);

    newSchemePageGridLayout = new QGridLayout(this);
    newSchemePageGridLayout->addWidget(listWidget, 0, 0, 1, 1);
    newSchemePageGridLayout->addWidget(stackedWidget, 0, 1, 1, 1);
    newSchemePageGridLayout->addLayout(newSchemePageHBLayout, 1, 0, 1, 2);

    /*槽函数*/
    connect(listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentIndex(int)));
    connect(okBtn, SIGNAL(pressed()), this, SLOT(onOkBtnClickedSlt()));
    connect(cancelBtn, SIGNAL(pressed()), this, SLOT(onCancelBtnClickedSlt()));
}

/* 提取预制方案 */
void ManageNewScheme::getPrefixedProfile(QString fileName)
{
    QString currentFile = QString().sprintf("%s", struCnfe.struParentPrefixedProfile[currentCategory].struSonProfile[currentIndex].sProfileName);
    QString path;
    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        path = "./preset/general";
        break;
    default:
        path = "./preset/common";
        break;
    }

    QDir dir = QDir::current();
    if (dir.cd(path))
    {
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

        QStringList fileList = dir.entryList();
        foreach(QString list, fileList)
        {
            if (list == currentFile)
            {
                QString cmd = QString().sprintf("cp %s/%s userdata/cnf/%s",
                    qPrintable(path), qPrintable(currentFile), qPrintable(fileName));
                system(qPrintable(cmd));
                break;
            }
        }
    }
}

/**
 * @brief ManageNewScheme::updateSchemeList
 */
void  ManageNewScheme::setCurrentIndex(int index)
{
    stackedWidget->setCurrentIndex(parentPrefixedProfileSeq[index]);
}

/**
 * @brief ManageNewScheme::onCancelBtnClickedSlt
 */
void ManageNewScheme::onCancelBtnClickedSlt()
{
    sonPrefixedProfileCbx[currentCategory][currentIndex]->setChecked(false);
    okBtn->setEnabled(false);
    emit goToManageSchemeSig();
}

/**
 * @brief ManageNewScheme::onOkBtnClickedSlt
 */
void ManageNewScheme::onOkBtnClickedSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_select_current_scheme);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        memcpy(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sMaterialName,
            struCnfe.struParentPrefixedProfile[currentCategory].struSonProfile[currentIndex].sMaterialName,
            sizeof(struCnfe.struParentPrefixedProfile[currentCategory].struSonProfile[currentIndex].sMaterialName));

        QDateTime time = QDateTime::currentDateTime();
        QString tmpTime = time.toString("yyyyMMddhhmmss");
        const char* strName = tmpTime.toLatin1();
        memset(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sProfileName, 0, sizeof(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sProfileName));
        memcpy(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sProfileName, strName, strlen(strName));

        /* 提取预置方案文件 */
        getPrefixedProfile(tmpTime);

        /* 保存当前方案参数 */
        if (struCnfg.nProfileTotal)
        {
            myFlow.saveProfile();
        }

        //! 记录上一方案的启动模式
        int nIntMode = struCnfp.nInterfaceBoardMode;
        int nColorMode = struCnfp.nColorBoardMode;

        /* 改变当前方案索引号 */
        struCnfg.nProfile = struCnfg.nProfileTotal;
        struCnfg.nProfileTotal++;
        myFlow.saveGlobal();

        /* 读取预置方案参数 */
        myFlow.getProfile();
        myFlow.saveProfile();

        //! 检查启动模式
        myFlow.checkAndResetFPGAMode(nIntMode, nColorMode);

        myFlow.initSendAllParams();

        LOG_INFO_STM("Create a new profile:" << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName) << "(in file:"
            << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName) << ")");

        infoWidget->hide();

        emit goToManageSchemeSig();
    }

    /* 重置页面勾选状态 */
    sonPrefixedProfileCbx[currentCategory][currentIndex]->setChecked(false);
    okBtn->setEnabled(false);
}

/**
 * @brief ManageNewScheme::createPrefixedProfile
 */
void ManageNewScheme::createPrefixedProfile()
{
    QString strName;

    /* 重新初始化 */
    parentPrefixedProfile.clear();
    sonPrefixedProfile.clear();
    sonPrefixedProfileLayout.clear();
    sonPrefixedProfileSigMapper.clear();

    for (int i = 0; i < struCnfe.nParentPrefixedProfileTotal; i++)
    {
        /* 初始化父预置方案类列表 */
        strName = QString("%1").arg(struCnfe.struParentPrefixedProfile[i].sMaterialName);
        parentPrefixedProfile.append(new myListWidgetItem(strName, myIcon.Edit_List, QSize(BTN_WIDTH - 20, BTN_HEIGHT * 2)));

        /* 初始化对应父预置方案类的子预置方案 */
        sonPrefixedProfile.append(new QWidget(this));
        sonPrefixedProfileLayout.append(new QGridLayout(sonPrefixedProfile[i]));
        sonPrefixedProfileSigMapper.append(new QSignalMapper(sonPrefixedProfile[i]));

        for (int j = 0; j < struCnfe.struParentPrefixedProfile[i].nSonProfileCount; j++)
        {
            strName = QString("%1").arg(struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sMaterialName);
            sonPrefixedProfileCbx[i][j] = new myCustomCheckBox(strName, false, CB_STYLE_CIRCLE, sonPrefixedProfile[i]);

            if (j % 2 == 0)
                sonPrefixedProfileLayout[i]->addWidget(sonPrefixedProfileCbx[i][j], j / 2, 0);
            else
                sonPrefixedProfileLayout[i]->addWidget(sonPrefixedProfileCbx[i][j], j / 2, 1);

            sonPrefixedProfileSigMapper[i]->setMapping(sonPrefixedProfileCbx[i][j], j);
            connect(sonPrefixedProfileCbx[i][j], SIGNAL(pressed()), sonPrefixedProfileSigMapper[i], SLOT(map()));
        }
        connect(sonPrefixedProfileSigMapper[i], SIGNAL(mapped(int)), this, SLOT(getPrefixedProfileCbxSlt(int)));

        /* 将子预置方案页面添加到栈管理器 */
        stackedWidget->addWidget(sonPrefixedProfile[i]);
    }
}

/**
 * @brief ManageNewScheme::updatePrefixedProfile
 */
void ManageNewScheme::updatePrefixedProfile()
{
    currentCategory = 0;
    currentIndex = 0;

    listWidget->clear();
    parentPrefixedProfileSeq.clear();

    for (int i = 0; i < struCnfe.nParentPrefixedProfileTotal; i++)
    {
        /* 列表添加已使能的父预置方案类 */
        if (struCnfe.struParentPrefixedProfile[i].nIsAble == 0)
            continue;

        parentPrefixedProfileSeq.append(i);
        listWidget->addItem(parentPrefixedProfile[i]);

        /* 根据对应父预置方案类的子方案使能状态更新显示 */
        for (int j = 0; j < struCnfe.struParentPrefixedProfile[i].nSonProfileCount; j++)
        {
            if (struCnfe.struParentPrefixedProfile[i].struSonProfile[j].nIsAble == 0)
                sonPrefixedProfileCbx[i][j]->hide();
            else
                sonPrefixedProfileCbx[i][j]->show();
        }
    }

    /* 默认设置第一列 */
    listWidget->setCurrentRow(0);
}

/**
 * @brief ManageNewScheme::getPrefixedProfileCbxSlt
 */
void ManageNewScheme::getPrefixedProfileCbxSlt(int index)
{
    currentCategory = parentPrefixedProfileSeq[listWidget->currentRow()];
    currentIndex = index;
    bFlagChecked[currentCategory][currentIndex] = sonPrefixedProfileCbx[currentCategory][index]->getChecked();

    /* 更新当前子预置方案页面勾选状态 */
    for (int i = 0; i < struCnfe.nParentPrefixedProfileTotal; i++)
    {
        for (int j = 0; j < struCnfe.struParentPrefixedProfile[i].nSonProfileCount; j++)
        {
            if (i == currentCategory && j == currentIndex)
            {
                if (bFlagChecked[i][j])
                    sonPrefixedProfileCbx[i][j]->setChecked(true);
                else
                    sonPrefixedProfileCbx[i][j]->setChecked(false);
            }
            else
            {
                sonPrefixedProfileCbx[i][j]->setChecked(false);
            }
        }
    }

    /* 根据当前选择勾选状态更新确认按钮使能 */
    if (bFlagChecked[currentCategory][currentIndex])
        okBtn->setEnabled(true);
    else
        okBtn->setEnabled(false);
}
