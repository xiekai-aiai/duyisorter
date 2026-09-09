/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        manageschemepage.cpp
 * \brief       方案管理界面源文件
 *              子页面包括新建方案、更改方案模式及复制重要参数。
 * \date        2015.01.14
 */
#include "manageschemepage.h"
#include "dirent.h"
#include "unilog.h"

ManageSchemePage::ManageSchemePage(QWidget* parent) :
    QWidget(parent)
{
    /* create scheme list widget */
    m_listWidget = new myListWidget(QSize(ICON_WID, ICON_HEI), this);
    m_listWidget->setFixedWidth(800);
    setFixedSize(LCD_WIDTH - 10, LCD_HEIGHT - 135);

    //    QString strStyle = "QListWidget::item::selected{ color:#4f7d8b; background:#edf2f3;}";
    //    m_listWidget->setStyleSheet(strStyle);
        /* create button */
    m_newSchemeBtn = new myPushButton(QString(myLan.new_scheme), QIcon(""));
    m_copySchemeBtn = new myPushButton(QString(myLan.copy_scheme), QIcon(""));
    m_deleteSchemeBtn = new myPushButton(QString(myLan.delte_scheme), QIcon(""));
    m_renameSchemeBtn = new myPushButton(QString(myLan.rename_scheme), QIcon(""));
    m_importSchemeBtn = new myPushButton(QString(myLan.recover_scheme), QIcon(""));
    m_exportSchemeBtn = new myPushButton(QString(myLan.backup_scheme), QIcon(""));
    m_changeModeBtn = new myPushButton(myLan.scheme_change_mode, QIcon(""));
    m_copyMajorBtn = new myPushButton(myLan.scheme_copy_major, QIcon(""));

    m_okBtn = new myPushButton(myLan.apply, QIcon(""));
    m_cancelBtn = new myPushButton(myLan.back, QIcon(""));


    //    QPalette palette1;
    //    QPixmap pixmap1(":/res/png/materialButton.png");
    //    pixmap1 = pixmap1.scaled(843,648, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    //    palette1.setBrush(backgroundRole(),QBrush(pixmap1));

    QString strStyle = QString("QPushButton {border-image: url(:/res/png/materialButton.png);}"
        "QToolTip {border-image: url(:/res/png/materialButton.png);}");

    m_newSchemeBtn->setStyleSheet(strStyle);
    m_copySchemeBtn->setStyleSheet(strStyle);
    m_deleteSchemeBtn->setStyleSheet(strStyle);
    m_renameSchemeBtn->setStyleSheet(strStyle);
    m_importSchemeBtn->setStyleSheet(strStyle);
    m_exportSchemeBtn->setStyleSheet(strStyle);
    m_changeModeBtn->setStyleSheet(strStyle);
    m_copyMajorBtn->setStyleSheet(strStyle);
    m_okBtn->setStyleSheet(strStyle);
    /* update scheme list widget and button status */
    updateListWidget();

    /* the restriction on the size of button was used for layout */
    m_newSchemeBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_copySchemeBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_deleteSchemeBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_renameSchemeBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_importSchemeBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_exportSchemeBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_changeModeBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_copyMajorBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_okBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));
    m_cancelBtn->setMaximumSize(QSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT + 10));

    /* page layout */
    /* sub layout */
    m_schemePageVBLayout = new QVBoxLayout();
    m_schemePageVBLayout->addWidget(m_okBtn);

    m_schemePageVBLayout->addWidget(m_newSchemeBtn);
    m_schemePageVBLayout->addWidget(m_copySchemeBtn);
    m_schemePageVBLayout->addWidget(m_deleteSchemeBtn);
    m_schemePageVBLayout->addWidget(m_renameSchemeBtn);
    m_schemePageVBLayout->addWidget(m_importSchemeBtn);
    m_schemePageVBLayout->addWidget(m_exportSchemeBtn);
    /* sub layout */
    m_schemePageHBLayout = new QHBoxLayout();
    m_schemePageHBLayout->addWidget(m_cancelBtn);
    m_cancelBtn->setHidden(true);
    m_horizontalSpacer = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_schemePageHBLayout->addItem(m_horizontalSpacer);
    m_schemePageHBLayout->addWidget(m_okBtn);

    /* main layout */
    m_schemePageGridLayout = new QGridLayout(this);
    m_schemePageGridLayout->addWidget(m_listWidget, 0, 0, 1, 6);
    m_schemePageGridLayout->addLayout(m_schemePageVBLayout, 0, 6, 1, 1, Qt::AlignCenter);
    //    m_schemePageGridLayout->addLayout(m_schemePageHBLayout,2,0,1,2);

        /* slots */
    connect(m_cancelBtn, SIGNAL(pressed()), this, SLOT(onCancelBtnClickedSlt()));
    connect(m_okBtn, SIGNAL(pressed()), this, SLOT(onOkBtnClickedSlt()));
    connect(m_newSchemeBtn, SIGNAL(pressed()), this, SLOT(onNewSchemeBtnClickedSlt()));
    connect(m_copySchemeBtn, SIGNAL(pressed()), this, SLOT(onCopySchemeBtnClickedSlt()));
    connect(m_deleteSchemeBtn, SIGNAL(pressed()), this, SLOT(onDeleteSchemeBtnClickedSlt()));
    connect(m_renameSchemeBtn, SIGNAL(pressed()), this, SLOT(onRenameSchemeBtnClickedSlt()));
    connect(m_importSchemeBtn, SIGNAL(pressed()), this, SLOT(onImportSchemeBtnClickedSlt()));
    connect(m_exportSchemeBtn, SIGNAL(pressed()), this, SLOT(onExportSchemeBtnClickedSlt()));
    connect(m_changeModeBtn, SIGNAL(pressed()), this, SLOT(onChangeModeBtnPressed()));
    connect(m_copyMajorBtn, SIGNAL(pressed()), this, SLOT(onCopyMajorBtnPressed()));

    QPalette palette;
    QPixmap pixmap(":/res/png/schemeBack.png");
    pixmap = pixmap.scaled(1000, 660, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    palette.setBrush(backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    m_listWidget->setStyleSheet("background-color:transparent");
    m_listWidget->setFrameStyle(QFrame::Panel);

}

/*!
 * \brief ManageSchemePage::resetProfileMode
 */
void ManageSchemePage::resetProfileMode()
{
}

/* 根据权限刷新方案管理页面按钮状态*/
void ManageSchemePage::refreshBtnStauts()
{
    if (struGsh.nAuthenticationLevel == AUTHENTICATION_LEVEL_OPERATOR)
    {
        m_newSchemeBtn->setEnabled(false);
        m_copySchemeBtn->setEnabled(false);
        m_deleteSchemeBtn->setEnabled(false);
        m_renameSchemeBtn->setEnabled(false);
        m_importSchemeBtn->setEnabled(false);
        m_exportSchemeBtn->setEnabled(false);
        m_changeModeBtn->setEnabled(false);
        m_copyMajorBtn->setEnabled(false);
        m_newSchemeBtn->setHidden(true);
        m_copySchemeBtn->setHidden(true);
        m_deleteSchemeBtn->setHidden(true);
        m_renameSchemeBtn->setHidden(true);
        m_importSchemeBtn->setHidden(true);
        m_exportSchemeBtn->setHidden(true);
    }
    else
    {
        m_newSchemeBtn->setEnabled(true);
        m_copySchemeBtn->setEnabled(true);
        m_deleteSchemeBtn->setEnabled(true);
        m_renameSchemeBtn->setEnabled(true);
        m_importSchemeBtn->setEnabled(true);
        m_exportSchemeBtn->setEnabled(true);
        m_newSchemeBtn->setHidden(false);
        m_copySchemeBtn->setHidden(false);
        m_deleteSchemeBtn->setHidden(false);
        m_renameSchemeBtn->setHidden(false);
        m_importSchemeBtn->setHidden(false);
        m_exportSchemeBtn->setHidden(false);
    }
}

/*!
 * \brief ManageSchemePage::updateListWidget \n
 * update scheme list widget and button status
 */
void ManageSchemePage::updateListWidget()
{
    QString strScheme;
    /* clear m_listWidget item */
    m_listWidget->clear();

    /* scheme list widget adds items */
    for (int i = 0; i < struCnfg.nProfileTotal; i++)
    {
        QString srcFilename = QString("%1/userdata/cnf/%2%3").arg(APP_PATH).arg(struCnfg.struProfileIndex[i].sProfileName).arg(".bak");
        QFileInfo fi(srcFilename);
        QString time = fi.lastModified().toString("yyyy-MM-dd hh:mm");
        QByteArray ctime = time.toLatin1();
        if (QFile(srcFilename).exists())
        {
        }
        else
        {
            strScheme.sprintf("%d %s (%s%s)", i + 1, struCnfg.struProfileIndex[i].sMaterialName, "bak@", ctime.data());
            strScheme.sprintf("%d %s", i + 1, struCnfg.struProfileIndex[i].sMaterialName);
        }

        myListWidgetItem* schemeListItem = new myListWidgetItem(strScheme, QIcon(), QSize(760, 80));
        schemeListItem->setTextAlignment(Qt::AlignCenter);
        QPixmap pixmap(":/res/png/material.png");
        pixmap = pixmap.scaled(QSize(797, 80), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        schemeListItem->setBackground(QBrush(pixmap));

        m_listWidget->addItem(schemeListItem);
        m_listWidget->setSpacing(1);
    }

    /* set selected row */
    m_listWidget->setCurrentRow(struCnfg.nProfile);

    /* update btn stauts*/
    refreshBtnStauts();
}

/*!
 * \brief ManageSchemePage::onCancelBtnClickedSlt \n
 * Cancel your select and go back mainpage
 */
void ManageSchemePage::onCancelBtnClickedSlt()
{
    emit goToHomePageSig();
}

/*!
 * \brief ManageSchemePage::onOkBtnClickedSlt \n
 * Confirm your select and go back mainpage
 */
void ManageSchemePage::onOkBtnClickedSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_select_current_scheme);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();

        myFlow.saveProfile();

        //! 记录上一方案的启动模式
        int nIntMode = struCnfp.nInterfaceBoardMode;
        int nColorMode = struCnfp.nColorBoardMode;

        struCnfg.nProfile = m_listWidget->currentRow();
        myFlow.getProfile();

        LOG_INFO_STM("select a profile:" << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName)
            << " (in file: " << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName) << ")");

        //! 检查启动模式
        myFlow.checkAndResetFPGAMode(nIntMode, nColorMode);

        myFlow.initSendAllParams();
        infoWidget->hide();
        emit goToHomePageSig();
    }
}

/*!
 * \brief ManageSchemePage::onNewSchemeBtnClickedSlt \n
 * create new scheme,those schemes were the best!
 * \see managenewscheme.cpp
 */
void ManageSchemePage::onNewSchemeBtnClickedSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_scheme_create);
    int ret = msgBox.exec();

    //! 判断方案数量限制条件
    if (struCnfg.nProfileTotal >= MAX_PROFILE)
    {
        ret = QDialog::Rejected;
        infoWidget->setLabelText("Reject!");
        infoWidget->delayShow();
        myFlow.sleep(1);
        infoWidget->hide();
    }

    if (ret == QDialog::Accepted)
    {
        emit goToNewSchemePageSig();
    }
}

/*!
 * \brief ManageSchemePage::onCopySchemeBtnClickedSlt \n
 * copy the same scheme
 */
void ManageSchemePage::onCopySchemeBtnClickedSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_scheme_copy);
    int ret = msgBox.exec();

    //! 新建方案有数量限制
    if (struCnfg.nProfileTotal >= MAX_PROFILE)
    {
        ret = QDialog::Rejected;
        infoWidget->setLabelText("Reject!");
        infoWidget->delayShow();
        myFlow.sleep(1);
        infoWidget->hide();
    }

    if (ret == QDialog::Accepted)
    {
        if (m_listWidget->currentItem())
        {
            memcpy(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sMaterialName,
                struCnfg.struProfileIndex[m_listWidget->currentRow()].sMaterialName,
                sizeof(struCnfg.struProfileIndex[m_listWidget->currentRow()].sMaterialName));

            infoWidget->setLabelText(myLan.msg_applying);
            infoWidget->delayShow();

            /* save current profile */
            myFlow.saveProfile();

            //! 记录上一方案的启动模式
            int nIntMode = struCnfp.nInterfaceBoardMode;
            int nColorMode = struCnfp.nColorBoardMode;

            /* change to selected profile */
            struCnfg.nProfile = m_listWidget->currentRow();
            QString dir = QString("%1/userdata/cnf/").arg(APP_PATH);
            QString oriFileName = QString(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
            myFlow.getProfile();

            //! 检查启动模式
            myFlow.checkAndResetFPGAMode(nIntMode, nColorMode);

            myFlow.initSendAllParams();

            QString oriFile = QString(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);

            /* copy selected profile */
            struCnfg.nProfileMode[struCnfg.nProfileTotal] = struCnfg.nProfileMode[struCnfg.nProfile];
            struCnfg.nProfile = struCnfg.nProfileTotal;
            QDateTime time = QDateTime::currentDateTime();
            QString tmpTime = time.toString("yyyyMMddhhmmss");
            const char* strName = tmpTime.toLatin1();
            memset(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sProfileName, 0,
                sizeof(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sProfileName));
            memcpy(struCnfg.struProfileIndex[struCnfg.nProfileTotal].sProfileName, strName, strlen(strName));
            QString desFileName = QString(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
            if (!QFile::copy(dir + oriFileName, dir + desFileName))
            {
                qDebug("ERROR : Copy profile error!");
            }

            QString desFile = QString(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
            QFile::copy(oriFile, desFile);
            struCnfg.nProfileTotal++;
            myFlow.saveGlobal();

            LOG_INFO_STM("Copy a new profile:" << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName)
                << " (in file: " << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName) << ")");

            infoWidget->hide();
            updateListWidget();
        }
    }
}

/*!
 * \brief ManageSchemePage::onDeleteSchemeBtnClickedSlt \n
 * delete the scheme which you select
 * \note Do not delete the scheme when only one scheme.
 */
void ManageSchemePage::onDeleteSchemeBtnClickedSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_scheme_delete);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        if (m_listWidget->currentItem())
        {
            if (m_listWidget->count() > 1)
            {
                /* delete scheme file */
                char str[256];
                if (strlen(struCnfg.struProfileIndex[m_listWidget->currentRow()].sProfileName) > 0)
                {
                    sprintf(str, "rm -rf userdata/cnf/%s*", struCnfg.struProfileIndex[m_listWidget->currentRow()].sProfileName);
                    system(str);
                }

                /* update scheme m_listWidget status*/
                for (int i = m_listWidget->currentRow(); i < m_listWidget->count() - 1; i++)
                {
                    memcpy(struCnfg.struProfileIndex[i].sMaterialName,
                        struCnfg.struProfileIndex[i + 1].sMaterialName,
                        sizeof(struCnfg.struProfileIndex[i + 1].sMaterialName));
                    memcpy(struCnfg.struProfileIndex[i].sProfileName,
                        struCnfg.struProfileIndex[i + 1].sProfileName,
                        sizeof(struCnfg.struProfileIndex[i + 1].sProfileName));
                }

                //! RS及RS-C需重置方案模式
                resetProfileMode();

                //! 记录上一方案的启动模式
                int nIntMode = struCnfp.nInterfaceBoardMode;
                int nColorMode = struCnfp.nColorBoardMode;

                /* set current scheme */
                struCnfg.nProfile = 0;
                struCnfg.nProfileTotal--;
                myFlow.saveGlobal();

                infoWidget->setLabelText(myLan.msg_applying);
                infoWidget->delayShow();
                myFlow.getProfile();

                //! 检查启动模式
                myFlow.checkAndResetFPGAMode(nIntMode, nColorMode);

                myFlow.initSendAllParams();

                LOG_INFO_STM("Delete a profile, current profile:" << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName)
                    << " (in file: " << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName) << ")");

                infoWidget->hide();
                updateListWidget();
            }
            else
            {
                infoWidget->setLabelText(myLan.msg_delete_forbidden);
                infoWidget->delayShow();
                myFlow.sleep(3);
                infoWidget->hide();
            }
        }
    }
}

/*!
 * \brief ManageSchemePage::onRenameSchemeBtnClickedSlt \n
 * rename scheme'name
 * \note the scheme'name must be chinese's name or english's name.
 */
void ManageSchemePage::onRenameSchemeBtnClickedSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_scheme_rename);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        if (m_listWidget->currentItem())
        {
            QString strInput = QString("%1").arg(struCnfg.struProfileIndex[m_listWidget->currentRow()].sMaterialName);
            myInputMethod inputPanel(myLan.msg_name_input, strInput);
            ret = inputPanel.exec();
            if (ret == QDialog::Accepted)
            {
                const char* strName = inputPanel.getText().toLatin1();

                memset(struCnfg.struProfileIndex[m_listWidget->currentRow()].sMaterialName, 0,
                    sizeof(struCnfg.struProfileIndex[m_listWidget->currentRow()].sMaterialName));
                memcpy(struCnfg.struProfileIndex[m_listWidget->currentRow()].sMaterialName,
                    strName, sizeof(struCnfg.struProfileIndex[m_listWidget->currentRow()].sMaterialName));
                updateListWidget();

                LOG_INFO_STM("Rename a profile and new profile name:" << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName)
                    << " (in file: " << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName) << ")");
            }
        }
    }
}

/**
 * \brief ManageSchemePage::onImportSchemeBtnClickedSlt \n
 * when checking usb ,this function import scheme ,or this function restore scheme
 */
void ManageSchemePage::onImportSchemeBtnClickedSlt()
{
    char sConfFile[64];
    char sConfFileBak[64];

    //! 记录上一方案的启动模式
    int nIntMode = struCnfp.nInterfaceBoardMode;
    int nColorMode = struCnfp.nColorBoardMode;

    QDir dir("/proc/scsi/usb-storage");

    if (!dir.exists())
    {    // 未检测到USB设备
        myMessageBox msgBox(MSG_QUES, myLan.cfm_restore_from_local);
        int ret = msgBox.exec();

        if (ret == QDialog::Accepted)
        {
            infoWidget->setLabelText(myLan.msg_applying);
            infoWidget->delayShow();
            sprintf(sConfFile, "userdata/cnf/%s", struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
            sprintf(sConfFileBak, "userdata/cnf/%s.bak", struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
            QString cmd = QString("cp -rf %1 %2").arg(sConfFileBak).arg(sConfFile);
            system(qPrintable(cmd));
            myFlow.sleep(1);
            system("sync");
            myFlow.sleep(1);

            myFlow.getProfile();

            //! 检查启动模式
            myFlow.checkAndResetFPGAMode(nIntMode, nColorMode);

            myFlow.initSendAllParams();
            infoWidget->hide();
        }
    }
    else
    {
        myMessageBox msgBox(MSG_QUES, myLan.cfm_restore_from_usb);
        int ret = msgBox.exec();
        if (ret == QDialog::Accepted)
        {
            if (!myFlow.mountUdisk())
            {
                infoWidget->setLabelText(myLan.msg_insert_udisk);
                infoWidget->delayShow();
                myFlow.sleep(1);
                infoWidget->hide();
                return;
            }

            QDir paramsDir("/udisk/cnf/");
            if (paramsDir.exists())
            {
                infoWidget->setLabelText(myLan.msg_applying);
                infoWidget->delayShow();

                system("rm -rf userdata/cnf/*");
                system("cp -rf userdata/cnf/* userdata/cnf/");

                myFlow.sleep(1);
                system("sync");
                myFlow.sleep(1);
                char cmd[64];
                sprintf(cmd, "umount /udisk");
                system(cmd);
                myFlow.sleep(1);

                myFlow.initAll();

                //! 检查启动模式
                myFlow.checkAndResetFPGAMode(nIntMode, nColorMode);

                myFlow.initSendAllParams();
                updateListWidget();
                infoWidget->hide();
            }
            else
            {
                infoWidget->setLabelText(myLan.msg_restore_from_usb_error);
                infoWidget->delayShow();
                myFlow.sleep(1);
                infoWidget->hide();
            }
        }
    }
}

/**
 * \brief ManageSchemePage::onExportSchemeBtnClickedSlt \n
 * when checking usb ,this function export scheme ,or this function backup scheme
 */
void ManageSchemePage::onExportSchemeBtnClickedSlt()
{
    char sConfFile[64];
    char sConfFileBak[64];

    QDir dir("/proc/scsi/usb-storage");

    if (!dir.exists())
    {      // 未检测到USB设备
        myMessageBox msgBox(MSG_QUES, myLan.cfm_backup_to_local);
        int ret = msgBox.exec();

        if (ret == QDialog::Accepted)
        {
            infoWidget->setLabelText(myLan.msg_applying);
            infoWidget->delayShow();
            myFlow.saveProfile();
            system("sync");
            myFlow.sleep(1);

            sprintf(sConfFile, "userdata/cnf/%s", struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
            sprintf(sConfFileBak, "userdata/cnf/%s.bak", struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName);
            QString cmd = QString("cp -rf %1 %2").arg(sConfFile).arg(sConfFileBak);
            system(qPrintable(cmd));

            myFlow.sleep(1);
            system("sync");
            myFlow.sleep(1);
            infoWidget->hide();
        }
    }
    else
    {
        myMessageBox msgBox(MSG_QUES, myLan.cfm_backup_to_usb);
        int ret = msgBox.exec();

        if (ret == QDialog::Accepted)
        {
            if (!myFlow.mountUdisk())
            {
                infoWidget->setLabelText(myLan.msg_insert_udisk);
                infoWidget->delayShow();
                myFlow.sleep(1);
                infoWidget->hide();
                return;
            }
            infoWidget->setLabelText(myLan.msg_applying);
            infoWidget->delayShow();
            myFlow.saveProfile();
            system("sync");
            myFlow.sleep(1);

            system("cp -rf userdata/cnf /udisk");

            myFlow.sleep(1);
            system("sync");
            myFlow.sleep(1);
            char cmd[64];
            sprintf(cmd, "umount /udisk");
            system(cmd);
            myFlow.sleep(1);
            infoWidget->hide();
        }
    }
    updateListWidget();
}

/* 响应更改模式按钮按下事件 */
void ManageSchemePage::onChangeModeBtnPressed()
{
    int nProfile = m_listWidget->currentRow();
    emit goToNewSchemePageSig();
    emit goToChangeModePageSig(nProfile);
}

/* 响应复制重要参数按钮按下事件 */
void ManageSchemePage::onCopyMajorBtnPressed()
{
    emit goToCopyMajorPageSig();
}
