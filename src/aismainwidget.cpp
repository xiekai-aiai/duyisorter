#include "aismainwidget.h"

/***
 *   主控件
 */
AisMainWidget::AisMainWidget(QWidget *parent)
    : QWidget(parent)
{
    QString text = "";

    setGeometry(0, 0, LCD_WIDTH-20, LCD_HEIGHT-60);
    setFixedSize(LCD_WIDTH-20, LCD_HEIGHT-60);
    config = g_Config::getInstance();
    // 标题栏默认不显示
    setWindowFlags(Qt::FramelessWindowHint);
    downPercentTimer = new QTimer(this);

    //! 初始化图标
    myIcon.init();
    this->setAutoFillBackground(true);

    QPalette palette;
    QPixmap pixmap(":/res/png/back.png");
    pixmap = pixmap.scaled(this->size());

    palette.setBrush(backgroundRole(),QBrush(pixmap));
    this->setPalette(palette);

//    aiCommunicate = new AiCommunicate();
    
    getDefaultPara();
    //获取初始化参数
//    getParaInfo();
//    writeDatagram(quint16 cmd, char sAiIntAddr, quint16 arglength, QByteArray args, QHostAddress address, quint16 port)
//    MyUpd.writeDatagram(CMD_AI_IMAGE_SEND, 32, 1, args, address, AI_UDP_SEND_PORT);

    // 状态列表
    listWidget = new myListWidget();
    listWidget->setViewMode(QListView::ListMode);
    listWidget->setMovement(QListView::Static);
    listWidget->setFixedWidth(BTN_WIDTH);
    listWidget->setStyleSheet("background-color:transparent");
    listWidget->setFixedSize(LCD_WIDTH/5,LCD_HEIGHT-200);


    for(int i = 0; i < 8; i++) {
        statusListItem[i] = new myListWidgetItem(QString(""),QIcon(),QSize());
        statusListItem[i]->setSizeHint(QSize(100,70));
        listWidget->addItem(statusListItem[i]);
    }
    statusListItem[AI_PAGE_PIXEL]->setText(myLan.pixel_info);
    statusListItem[AI_PAGE_EJECTOR]->setText(myLan.ejector_para);
    statusListItem[AI_PAGE_IMG_HEIGHT]->setText(myLan.default_params);
    statusListItem[AI_PAGE_MODEL_PARA]->setText(myLan.model_para);
    statusListItem[AI_PAGE_MODEL_UPDATE]->setText(myLan.model_update);
    statusListItem[AI_PAGE_VERSION]->setText(myLan.ai_version);
    statusListItem[AI_DISK_VALID]->setText(myLan.disk_space);
    statusListItem[AI_DISK_EJECTOR_TEST]->setText(myLan.test_ejector);

    statusListItem[AI_PAGE_PIXEL]->setHidden(true);
    statusListItem[AI_PAGE_EJECTOR]->setHidden(true);
    statusListItem[AI_PAGE_MODEL_PARA]->setHidden(true);
    statusListItem[AI_DISK_EJECTOR_TEST]->setHidden(true);
    statusListItem[AI_PAGE_VERSION]->setHidden(true);


    // 默认选中第一列
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setFixedSize(700,LCD_HEIGHT-150);

    // 像元信息页面
    pagePixel = new QWidget();
    stackedWidget->addWidget(pagePixel);
    CreatePagePixel();

    // 喷阀参数
    pageEjector = new QWidget();
    stackedWidget->addWidget(pageEjector);
    CreatePageEjector();

    // 图像高度
    pageImgHeight = new QWidget();
    stackedWidget->addWidget(pageImgHeight);
    CreatePageImgHeight();

    //模型参数
    pageModePara = new QWidget();
    stackedWidget->addWidget(pageModePara);
    CreatePageModePara();

    //模型升级
    pageModeUpdate = new QWidget();
    stackedWidget->addWidget(pageModeUpdate);
    CreatePageModeUpdate();

    //版本查询
    pageVersion = new QWidget();
    stackedWidget->addWidget(pageVersion);
    CreatePageVersion();

    //磁盘空间
    pageDiskValid = new QWidget();
    stackedWidget->addWidget(pageDiskValid);
    CreatePageDiskValid();

    //喷发检测
    pageEjectTest= new QWidget();
    stackedWidget->addWidget(pageEjectTest);
    CreatePageEjectTest();

    stackedWidget->setCurrentIndex(2);
    imageSendBtn = new myPushButton(myLan.image_acquisition,QIcon(""),
                                   true,true, this);
    imageSendBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    imageSendBtn->setIconSize(QSize(ICON_WID, ICON_WID));
    isImageSend =false;
    isImageInfer = false;

    imageInferBtn = new myPushButton(myLan.image_reasoning,QIcon(""),
                                 true,true, this);
    imageInferBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    imageInferBtn->setIconSize(QSize(ICON_WID, ICON_WID));

    ApplyBtn = new myPushButton(myLan.apply,QIcon(""),
                            true,true, this);
    ApplyBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    ApplyBtn->setIconSize(QSize(ICON_WID, ICON_WID));

    backPushButton= new myPushButton(myLan.back,QIcon(""),
                                    true,true, this);
    backPushButton->setFont(config->getFont(DEFAULT_FONT_SIZE));
    backPushButton->setIconSize(QSize(ICON_WID, ICON_WID));

    saveParaPushButton= new myPushButton(myLan.save_parameterNew,QIcon(""),
                                        true,true, this);
    saveParaPushButton->setFont(config->getFont(DEFAULT_FONT_SIZE));
    saveParaPushButton->setIconSize(QSize(ICON_WID, ICON_WID));

    downLoadImgBtn = new myPushButton(myLan.download_image,QIcon(""),
                            true,true, this);
    downLoadImgBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    downLoadImgBtn->setIconSize(QSize(ICON_WID, ICON_WID));

    imageSendBtn->setFixedSize(BTN_WIDTH+30, BTN_HEIGHT);
    imageInferBtn->setFixedSize(BTN_WIDTH+30, BTN_HEIGHT);
    ApplyBtn->setFixedSize(BTN_WIDTH+30, BTN_HEIGHT);
    backPushButton->setFixedSize(BTN_WIDTH+30, BTN_HEIGHT);
    downLoadImgBtn->setFixedSize(BTN_WIDTH+70, BTN_HEIGHT);
    saveParaPushButton->setFixedSize(BTN_WIDTH+30, BTN_HEIGHT);
    saveParaPushButton->hide();

    mainGridLayout = new QGridLayout(this);
    mainGridLayout->setContentsMargins(20, 0, 20, 20);
    mainGridLayout->setHorizontalSpacing(20);
    mainGridLayout->setRowMinimumHeight(0 ,LCD_TITLE_HEIGHT-10);
    mainGridLayout->setVerticalSpacing(0);

    lineTitle = new QFrame(this);
    lineTitle->setFrameShape(QFrame::HLine);
    lineTitle->setFrameShadow(QFrame::Plain);
    lineTitle->setLineWidth(2);
    lineTitle->setStyleSheet("color: black");
    lineTitle->hide();
    // 警示框
    infoWidget = new myInfoWidget(this);
    infoWidget->raise();
    infoWidget->hide();
    imageSendBtn->hide();
    imageInferBtn->hide();
//    mainGridLayout->addWidget(imageSendBtn, 0, 1, 1, 1, Qt::AlignCenter);
//    mainGridLayout->addWidget(imageInferBtn, 0, 3, 1, 1, Qt::AlignCenter);
//    mainGridLayout->addWidget(lineTitle, 1, 0, 1, 5);
    mainGridLayout->addWidget(listWidget, 0, 0, 1, 1, Qt::AlignCenter);
    mainGridLayout->addWidget(stackedWidget, 0, 1, 1, 4, Qt::AlignCenter);

    mainGridLayout->addWidget(ApplyBtn, 1, 0, 1, 1, Qt::AlignCenter);
    mainGridLayout->addWidget(downLoadImgBtn, 1, 2, 1, 1, Qt::AlignCenter);
//    mainGridLayout->addWidget(saveParaPushButton, 3, 3, 1, 1, Qt::AlignCenter);
    mainGridLayout->addWidget(backPushButton, 1, 4, 1, 1, Qt::AlignCenter);


    connect(listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onListWidgetRowChangedSlt(int)));

    connect(imageSendBtn, SIGNAL(pressed()), this, SLOT(onImageSendBtnPressedSlt()));
    connect(imageInferBtn, SIGNAL(pressed()), this, SLOT(onImageInferBtnPressedSlt()));
    connect(ApplyBtn, SIGNAL(pressed()), this, SLOT(onApplyBtnPressedSlt()));
    connect(downLoadImgBtn, SIGNAL(pressed()), this, SLOT(onDownLoadImgBtnPressedSlt()));
    connect(uploader, SIGNAL(downloadFinished()), this, SLOT(onDownloadFinishedSlt()));
    connect(uploader, SIGNAL(blankDirListFinished()), this, SLOT(onBlankDirListFinishedSlt()));
    connect(uploader, SIGNAL(removeFinished()), this, SLOT(onRemoveFinishedSlt()));
    connect(backPushButton, SIGNAL(pressed()), this, SLOT(onSetBackBtnClicked()));
    connect(downPercentTimer, SIGNAL(timeout()), this, SLOT(onDownPercentTimeout()));
    connect(saveParaPushButton, SIGNAL(pressed()), this, SLOT(onSaveParaBtnPressedSlt()));

}

void AisMainWidget::getParaInfo(){
    listWidget->setCurrentRow(2);
//    myFlow.getDefaultPara();

//    QSqlQuery sql_select;
//    QVariantMap resultMap;
//    sql_select.prepare("select paraName, paraValue from paraInfo;");
//    bool result = sql_select.exec();
//    if(result){
//        qDebug() << "select paraInfo成功";
//    }else{
//        qDebug() << "select paraInfo失败";
//    }
//    while (sql_select.next()) {
//            QString paraName = sql_select.value(0).toString();
//            int paraValue = sql_select.value(1).toInt();
//            resultMap.insert(paraName,paraValue);
//        }
//    ejectorNum = resultMap.value("ejectorNum").toInt();
//    ejectorDelay = resultMap.value("ejectorDelay").toInt();
//    ejectorDynamicDelay = resultMap.value("ejectorDynamicDelay").toInt();
//    ejectorBlowTime = resultMap.value("ejectorBlowTime").toInt();
//    imgFetchHeight = resultMap.value("imgFetchHeight").toInt();
//    imgInferHeight = resultMap.value("imgInferHeight").toInt();

//    ejectorNumEdit->setText(QString("%1").arg(ejectorNum));
//    ejectorDelayEdit->setText(QString("%1").arg(ejectorDelay));
//    ejectorDynamicDelayEdit->setText(QString("%1").arg(ejectorDynamicDelay));
//    ejectorBlowTimeEdit->setText(QString("%1").arg(ejectorBlowTime));
//    imgFetchHeightEdit->setText(QString("%1").arg(imgFetchHeight));
//    imgInferHeightEdit->setText(QString("%1").arg(imgInferHeight));

////     qDebug() << "ejectorNum:" << ejectorNum << "ejectorDelay:" << ejectorDelay
////              << "ejectorDynamicDelay:" << ejectorDynamicDelay<<"ejectorBlowTime:" << ejectorBlowTime
////              << "imgFetchHeight:" << imgFetchHeight<< "imgInferHeight:" << imgInferHeight;
//    QByteArray args;
//    AI_Data_Protocol_D data;
//    args[0] = ejectorNum;
//    args[1] = ejectorDelay/256;
//    args[2] = ejectorDelay%256;
//    args[3] = ejectorDynamicDelay/256;
//    args[4] = ejectorDynamicDelay%256;
//    args[5] = ejectorBlowTime;
//    for(int i=0; i< aiDeviceNum; i++){
//        MyUpd.writeDatagram(CMD_AI_EJECTOR_PARA, i, 6, args, addressList.at(i), AI_UDP_SEND_PORT);
//        MyUpd.readUdpDatagrams(&data,13);
//    }
////    myFlow.msleep(100);
//    args.clear();
//    args[0] = imgFetchHeight/256;
//    args[1] = imgFetchHeight%256;
//    args[2] = imgInferHeight/256;
//    args[3] = imgInferHeight%256;
//    for(int i=0; i< aiDeviceNum; i++){
//        MyUpd.writeDatagram(CMD_AI_IMAGE_HEIGHT, i, 4, args, addressList.at(i), AI_UDP_SEND_PORT);
//    //    myFlow.msleep(100);
//        MyUpd.readUdpDatagrams(&data,13);
//    }
}

void AisMainWidget::saveAiPara(QString paraCol, int paraVal){
    QSqlQuery sql_update;
    QString updateStatement = "update paraInfo set  paraValue = :paraValue where paraName = :paraName ";
    sql_update.prepare(updateStatement);
    sql_update.bindValue(":paraName",paraCol);
    sql_update.bindValue(":paraValue", paraVal);
    if(sql_update.exec()){
        qDebug() << "update paraInfo "+paraCol+" 成功";
    }else{
        qDebug() << "update paraInfo "+paraCol+" 失败";
    }
}


void AisMainWidget::onSaveParaBtnPressedSlt(){
    infoWidget->setLabelText(myLan.msg_saving_params);
    infoWidget->delayShow();
    QSqlQuery sql_delete;
    QSqlQuery sql_insert;
    sql_delete.prepare("delete  from paraInfo;");
    bool result = sql_delete.exec();
    if(result){
        qDebug() << "delete paraInfo成功";
    }else{
        qDebug() << "delete paraInfo失败";
    }

    QString insertStatement = "INSERT INTO paraInfo (paraName, paraValue) VALUES (:paraName, :paraValue) ";
    sql_insert.prepare(insertStatement);
    sql_insert.bindValue(":paraName", "ejectorNum");
    sql_insert.bindValue(":paraValue", ejectorNum);
    if(sql_insert.exec()){
        qDebug() << "insert paraInfo ejectorNum成功";
    }else{
        qDebug() << "insert paraInfo ejectorNum失败";
    }

    sql_insert.bindValue(":paraName", "ejectorDelay");
    sql_insert.bindValue(":paraValue", ejectorDelay);
    if(sql_insert.exec()){
        qDebug() << "insert paraInfo ejectorDelay成功";
    }else{
        qDebug() << "insert paraInfo ejectorDelay失败";
    }

    sql_insert.bindValue(":paraName", "ejectorDynamicDelay");
    sql_insert.bindValue(":paraValue", ejectorDynamicDelay);
    if(sql_insert.exec()){
        qDebug() << "insert paraInfo ejectorDynamicDelay成功";
    }else{
        qDebug() << "insert paraInfo ejectorDynamicDelay失败";
    }

    sql_insert.bindValue(":paraName", "ejectorBlowTime");
    sql_insert.bindValue(":paraValue", ejectorBlowTime);
    if(sql_insert.exec()){
        qDebug() << "insert paraInfo ejectorBlowTime成功";
    }else{
        qDebug() << "insert paraInfo ejectorBlowTime失败";
    }

    sql_insert.bindValue(":paraName", "imgFetchHeight");
    sql_insert.bindValue(":paraValue", imgFetchHeight);
    if(sql_insert.exec()){
        qDebug() << "insert paraInfo imgFetchHeight成功";
    }else{
        qDebug() << "insert paraInfo imgFetchHeight失败";
    }

    sql_insert.bindValue(":paraName", "imgInferHeight");
    sql_insert.bindValue(":paraValue", imgInferHeight);
    if(sql_insert.exec()){
        qDebug() << "insert paraInfo imgInferHeight成功";
    }else{
        qDebug() << "insert paraInfo imgInferHeight失败";
    }

    myFlow.sleep(2);
//    infoWidget->setLabelText(myLan.msg_saving_params);
    infoWidget->hide();
}

void AisMainWidget::getDefaultPara(){
    //ai板子数量初始化
    int idTotal = 0;
    switch(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal) {
    case 1:     // 单视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
        break;
    case 2:     // 双视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        break;
    }
    aiDeviceNum = idTotal/2;
//    qDebug() << "aiDeviceNum"<<aiDeviceNum;

    ftpIpAddressList.clear();
    addressList.clear();

    ftpIpAddressList.append(AI_IP_ADDRESS);
    QString oldIpAddress = AI_IP_ADDRESS;

    for(int i=1; i<aiDeviceNum; i++){
        QString newIpAddress = oldIpAddress;
        newIpAddress = incrementLastOctet(oldIpAddress);
        oldIpAddress = newIpAddress;
        ftpIpAddressList.append(newIpAddress);
    }
    //json文件解析
    QFile file(DEFAULT_PARA_INIT);
    file.open(QFile::ReadOnly);
    QString json=  QString::fromUtf8(file.readAll());
    file.close();
    bool ok;
    QVariantMap resultMap = QJson::parse(json.toUtf8(), ok).toMap();
    if(!ok){
         qDebug() << "json open failed, using default IpAddress";
    }else{
        for(int i=0; i<aiDeviceNum; i++){
            QString key = QString("ftpIp%1").arg(i);
            if(resultMap.keys().contains(key)){
                ftpIpAddress = resultMap[key].toString();
                ftpIpAddressList.replace(i,ftpIpAddress);
            }
        }
    }
    qDebug() << "ftpIpAddressList"<<ftpIpAddressList;
    address = QHostAddress(ftpIpAddressList.at(0));
    addressList.append(address);
    for(int i=1; i<ftpIpAddressList.size(); i++){
        addressList.append(QHostAddress(ftpIpAddressList.at(i)));
    }

}

// IP地址最后一位+1
QString AisMainWidget::incrementLastOctet(const QString & ip) {
    QStringList parts = ip.split(".");
    int lastOctet = parts[3].toInt();

    // 处理最后一位+1的逻辑
    if (lastOctet < 255) {
        lastOctet++;
    } else {
        // 如果最后一位是255，则保持不变或根据需求处理
        qDebug() << "警告: IP地址最后一位已经是255，无法再增加。" << endl;
        return ip;
    }

    // 构建新的IP地址
    return parts[0] + "." + parts[1] + "." + parts[2] + "." +  QString::number(lastOctet);
}

void AisMainWidget::onDownPercentTimeout(){

    QString text = QString("%1%2").arg(myLan.img_downloading+":").arg(uploader->getDownLoadPercent());
    downLoadImgBtn->setText(text);
}

void AisMainWidget::onSetBackBtnClicked()
{
    emit backToHomePageSig();
}

void AisMainWidget::onDownLoadImgBtnPressedSlt(){

    if(!uploader->getFptStatus()){
        infoWidget->setLabelText(myLan.ftp_status_error);
        infoWidget->delayShow();
        myFlow.sleep(1);
        infoWidget->hide();
        return;
    }
    bool ftpStatus;
#ifdef Q_OS_UNIX
//    QDir dir("/proc/scsi/usb-storage");

//    if(!dir.exists()){
//        infoWidget->setLabelText(myLan.msg_insert_udisk);
//        infoWidget->showSecs();
//        return;
//    }
//    char cmd[64];
//    if (!myFlow.mountUdisk()) {
//        infoWidget->setLabelText(myLan.msg_insert_udisk);
//        infoWidget->delayShow();
//        myFlow.sleep(1);
//        infoWidget->hide();
//        return;
//    }
    /* copy png files to usb storage */
    QDateTime time = QDateTime::currentDateTime();
    QString pngDir = QString("%1%2/").arg(LOCAL_IMG_PATH).arg(time.toString("yyyyMMddhhmmss"));

    QString shell;
    shell.sprintf("mkdir -p %s", qPrintable(pngDir));
    qDebug("%s", qPrintable(shell));
    system(qPrintable(shell));
    myFlow.sleep(1);

    infoWidget->setLabelText(myLan.msg_copy_image);
    infoWidget->delayShow();
    myFlow.sleep(1);
    infoWidget->hide();

    ftpStatus = uploader->downloadDirectory(REMOTE_IMG_PATH,pngDir);
#endif
#ifdef Q_OS_WIN
    ftpStatus = uploader->downloadDirectory(REMOTE_IMG_PATH,LOCAL_IMG_PATH);
#endif
    if(!ftpStatus){
        infoWidget->setLabelText(myLan.ftp_status_error);
        infoWidget->delayShow();
        myFlow.sleep(1);
        infoWidget->hide();
        return;
    }
    downLoadImgBtn->setText(myLan.img_downloading);
    downLoadImgBtn->setEnabled(false);
    downPercentTimer->start(3000);
    uploader->start();

}
void AisMainWidget::onDownloadFinishedSlt(){
    infoWidget->setLabelText(myLan.img_download_finished);
    infoWidget->delayShow();
    myFlow.sleep(2);
    infoWidget->hide();
    downLoadImgBtn->setText(myLan.download_image);
    downPercentTimer->stop();
    uploader->stop();

    myMessageBox msgBox(MSG_QUES, myLan.delete_original_image);
    int ret = msgBox.exec();
    if (ret == QDialog::Accepted){
        downLoadImgBtn->setText(myLan.deleting);
        downLoadImgBtn->setEnabled(false);
        uploader->deleteDir();
        return;

    }else{
        myFlow.sleep(1);
        downLoadImgBtn->setEnabled(true);
        return;
    }
}

void AisMainWidget::onBlankDirListFinishedSlt(){
    infoWidget->setLabelText(myLan.img_list_blank);
    infoWidget->delayShow();
    myFlow.sleep(2);
    infoWidget->hide();
    downLoadImgBtn->setText(myLan.download_image);
    downLoadImgBtn->setEnabled(true);
}

void AisMainWidget::onRemoveFinishedSlt(){
    downLoadImgBtn->setText(myLan.download_image);
    downLoadImgBtn->setEnabled(true);
    infoWidget->setLabelText(myLan.delete_original_image_finished);
    infoWidget->delayShow();
    myFlow.sleep(3);
    infoWidget->hide();

}

void AisMainWidget::onApplyBtnPressedSlt(){
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    QString msg = QString("");
    ApplyBtn->setEnabled(false);
    switch(listWidget->currentIndex().row()){
    case AI_PAGE_PIXEL:
//        startPixelNum = startPixelEdit->text().toInt();
//        endPixelNum = endPixelEdit->text().toInt();
        startPixelNum = struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit];
        endPixelNum = struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit];
        args[0] = AIUNIT;
        //ai相机编号，取余
        args[1] = struGsh.nUnit%2;
        args[2] = startPixelNum/256;
        args[3] = startPixelNum%256;
        args[4] = endPixelNum/256;
        args[5] = endPixelNum%256;

        aiInfoWidget->setLabelText(myLan.apply+" ......");
        aiInfoWidget->setMsgHeightRatio(2);
        aiInfoWidget->delayShow();

        MyUpd.writeDatagram(CMD_AI_PIXEL_SEND, struGsh.nUnit/2, 6, args, addressList.at(struGsh.nUnit/2), AI_UDP_SEND_PORT);
//        myFlow.sleep(1);
        ret = MyUpd.readUdpDatagrams(&data,13);
        if(ret == -1){
            msg += QString("aiDevice %1 ").arg(struGsh.nUnit/2+1) + myLan.setting_error + "1";
//            if(i != (aiDeviceNum-1)){
//               msg +=QString("\n");
//            }
        }else if(ret == -2){
            msg +=  QString("aiDevice %1 ").arg(struGsh.nUnit/2+1) + myLan.setting_error + "2";
//            if(i != (aiDeviceNum-1)){
//               msg +=QString("\n");
//            }
        }else{
            quint8 mode = (quint8)data.nCommandData.at(0);
            if (mode == 1){
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    infoWidget->delayShow();
                msg +=  QString("aiDevice %1 ").arg(struGsh.nUnit/2+1) + myLan.setting_succeeded;
//                if(i != (aiDeviceNum-1)){
//                    msg +=QString("\n");
//                }
            }else if(mode == 0){
//                    infoWidget->setLabelText(myLan.setting_error);
                msg +=  QString("aiDevice %1 ").arg(struGsh.nUnit/2+1) + myLan.setting_error;
//                if(i != (aiDeviceNum-1)){
//                    msg +=QString("\n");
//                }
//                    infoWidget->delayShow();
            }else{
//                    infoWidget->setLabelText(myLan.setting_value_error);
//                    infoWidget->delayShow();
                msg +=  QString("aiDevice %1 ").arg(struGsh.nUnit/2+1) + myLan.setting_value_error;
//                if(i != (aiDeviceNum-1)){
//                    msg +=QString("\n");
//                }
            }
        }


//        for(int i=0; i< aiDeviceNum; i++){
//            MyUpd.writeDatagram(CMD_AI_PIXEL_SEND, i, 6, args, addressList.at(i), AI_UDP_SEND_PORT);
//    //        myFlow.sleep(1);
//            ret = MyUpd.readUdpDatagrams(&data,13);
//            if(ret == -1){
//                msg += QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "1";
//                if(i != (aiDeviceNum-1)){
//                   msg +=QString("\n");
//                }
//            }else if(ret == -2){
//                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "2";
//                if(i != (aiDeviceNum-1)){
//                   msg +=QString("\n");
//                }
//            }else{
//                quint8 mode = (quint8)data.nCommandData.at(0);
//                if (mode == 1){
////                    infoWidget->setLabelText(myLan.setting_succeeded);
////                    infoWidget->delayShow();
//                    msg =  QString("aiDevice %1 ").arg(i+1) + myLan.setting_succeeded;
//                    if(i != (aiDeviceNum-1)){
//                        msg +=QString("\n");
//                    }
//                }else if(mode == 0){
////                    infoWidget->setLabelText(myLan.setting_error);
//                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error;
//                    if(i != (aiDeviceNum-1)){
//                        msg +=QString("\n");
//                    }
////                    infoWidget->delayShow();
//                }else{
////                    infoWidget->setLabelText(myLan.setting_value_error);
////                    infoWidget->delayShow();
//                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_value_error;
//                    if(i != (aiDeviceNum-1)){
//                        msg +=QString("\n");
//                    }
//                }
//            }
//        }


//        if(ret == -1){
//            infoWidget->setLabelText(myLan.setting_error + "1");
//            infoWidget->delayShow();
//        }else if(ret == -2){
//            infoWidget->setLabelText(myLan.setting_error + "2");
//            infoWidget->delayShow();
//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<"mode:"<<mode;
//            if (mode == 1){
//                infoWidget->setLabelText(myLan.setting_succeeded);
//                infoWidget->delayShow();
//            }else if(mode == 0){
//                infoWidget->setLabelText(myLan.setting_error);
//                infoWidget->delayShow();
//            }else{
//                infoWidget->setLabelText(myLan.setting_value_error);
//                infoWidget->delayShow();
//            }
//        }
        aiInfoWidget->setLabelText(msg);
//        infoWidget->delayShow();
        myFlow.sleep(4);
        aiInfoWidget->hide();
        break;
    case AI_PAGE_EJECTOR:
        ejectorNum = ejectorNumEdit->text().toInt();
        ejectorDelay = ejectorDelayEdit->text().toInt();
        ejectorDynamicDelay= ejectorDynamicDelayEdit->text().toInt();
        ejectorBlowTime = ejectorBlowTimeEdit->text().toInt();
        args[0] = ejectorNum;
        args[1] = ejectorDelay/256;
        args[2] = ejectorDelay%256;
        args[3] = ejectorDynamicDelay/256;
        args[4] = ejectorDynamicDelay%256;
        args[5] = ejectorBlowTime;

        saveAiPara("ejectorNum",ejectorNum);
        saveAiPara("ejectorDelay",ejectorDelay);
        saveAiPara("ejectorDynamicDelay",ejectorDynamicDelay);
        saveAiPara("ejectorBlowTime",ejectorBlowTime);

        aiInfoWidget->setLabelText(myLan.apply+" ......");
        aiInfoWidget->setMsgHeightRatio(2);
        aiInfoWidget->delayShow();

        for(int i=0; i< aiDeviceNum; i++){
            MyUpd.writeDatagram(CMD_AI_EJECTOR_PARA, i, 6, args, addressList.at(i), AI_UDP_SEND_PORT);
    //        myFlow.sleep(1);

            ret = MyUpd.readUdpDatagrams(&data,13);

            if(ret == -1){
                msg += QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "1";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else if(ret == -2){
                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "2";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else{
                quint8 mode = (quint8)data.nCommandData.at(0);
                if (mode == 1){
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    infoWidget->delayShow();
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_succeeded;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }else if(mode == 0){
//                    infoWidget->setLabelText(myLan.setting_error);
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
//                    infoWidget->delayShow();
                }else{
//                    infoWidget->setLabelText(myLan.setting_value_error);
//                    infoWidget->delayShow();
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_value_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }
            }

//            if(ret == -1){
//                infoWidget->setLabelText(myLan.setting_error + "1");
//                infoWidget->delayShow();
//            }else if(ret == -2){
//                infoWidget->setLabelText(myLan.setting_error + "2");
//                infoWidget->delayShow();
//            }else{
//                quint8 mode = (quint8)data.nCommandData.at(0);
//                qDebug()<<mode;
//                if (mode == 1){
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    saveAiPara("ejectorNum",ejectorNum);
//                    saveAiPara("ejectorDelay",ejectorDelay);
//                    saveAiPara("ejectorDynamicDelay",ejectorDynamicDelay);
//                    saveAiPara("ejectorBlowTime",ejectorBlowTime);
//                    infoWidget->delayShow();
//                }else if(mode == 0){
//                    infoWidget->setLabelText(myLan.setting_error);
//                    infoWidget->delayShow();
//                }else{
//                    infoWidget->setLabelText(myLan.setting_value_error);
//                    infoWidget->delayShow();
//                }
//            }
        }

        aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
        aiInfoWidget->setLabelText(msg);
        myFlow.sleep(4);
        aiInfoWidget->hide();
//        myFlow.sleep(3);
//        infoWidget->hide();
        break;
    case AI_PAGE_IMG_HEIGHT:
        args[0] = imgFetchHeight/256;
        args[1] = imgFetchHeight%256;
        args[2] = imgInferHeight/256;
        args[3] = imgInferHeight%256;
        saveAiPara("imgFetchHeight",imgFetchHeight);
        saveAiPara("imgInferHeight",imgInferHeight);

        aiInfoWidget->setLabelText(myLan.apply+" ......");
        aiInfoWidget->setMsgHeightRatio(2);
        aiInfoWidget->delayShow();

        for(int i=0; i< aiDeviceNum; i++){
            MyUpd.writeDatagram(CMD_AI_IMAGE_HEIGHT, i, 4, args, addressList.at(i), AI_UDP_SEND_PORT);
    //        myFlow.sleep(1);
            ret = MyUpd.readUdpDatagrams(&data,13);
            if(ret == -1){
                msg += QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "1";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else if(ret == -2){
                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "2";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else{
                quint8 mode = (quint8)data.nCommandData.at(0);
                if (mode == 1){
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    infoWidget->delayShow();
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_succeeded;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }else if(mode == 0){
//                    infoWidget->setLabelText(myLan.setting_error);
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
//                    infoWidget->delayShow();
                }else{
//                    infoWidget->setLabelText(myLan.setting_value_error);
//                    infoWidget->delayShow();
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_value_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }
            }
        }

        aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
        aiInfoWidget->setLabelText(msg);
//        infoWidget->delayShow();
        myFlow.sleep(4);
        aiInfoWidget->hide();

//        if(ret == -1){
//            infoWidget->setLabelText(myLan.setting_error + "1");
//            infoWidget->delayShow();
//        }else if(ret == -2){
//            infoWidget->setLabelText(myLan.setting_error + "2");
//            infoWidget->delayShow();
//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<mode;
//            if (mode == 1){
//                infoWidget->setLabelText(myLan.setting_succeeded);
//                saveAiPara("imgFetchHeight",imgFetchHeight);
//                saveAiPara("imgInferHeight",imgInferHeight);
//                infoWidget->delayShow();
//            }else if(mode == 0){
//                infoWidget->setLabelText(myLan.setting_error);
//                infoWidget->delayShow();
//            }else{
//                infoWidget->setLabelText(myLan.setting_value_error);
//                infoWidget->delayShow();
//            }
//        }
//        myFlow.sleep(3);
//        infoWidget->hide();
        break;
    case AI_PAGE_VERSION:

        for(int i=0; i< aiDeviceNum; i++){
            MyUpd.writeDatagram(CMD_AI_VERSION_FETCH, i, 0, args, addressList.at(i), AI_UDP_SEND_PORT);
    //        myFlow.sleep(1);
            ret = MyUpd.readUdpDatagrams(&data,18);
            if(ret == -1){
                msg += QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "1";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            } else if(ret == -2){
                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "2";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else{
                versionInfo.clear();
                versionInfo =QString(data.nCommandData);
                qDebug()<<versionInfo;
                if(versionInfo.isEmpty()){
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }else{
                    versionInfoLbl[i]->setText(versionInfo);
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_succeeded;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }

              }

//            if(ret == -1){
//                infoWidget->setLabelText(myLan.setting_error + "1");
//                infoWidget->delayShow();
//            }else if(ret == -2){
//                infoWidget->setLabelText(myLan.setting_error + "2");
//                infoWidget->delayShow();
//            }else{
//                versionInfo =QString(data.nCommandData);
//                qDebug()<<versionInfo;
//                if(versionInfo.isEmpty()){
//                    infoWidget->setLabelText(myLan.setting_error);
//                    infoWidget->delayShow();
//                }else{
//    //                versionInfoLbl->setText(versionInfo);
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    infoWidget->delayShow();
//                }

//            }
        }

        aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
        aiInfoWidget->setLabelText(msg);
        aiInfoWidget->delayShow();
        myFlow.sleep(3);
        aiInfoWidget->hide();
        break;
    case AI_DISK_VALID:
        for(int i=0; i< aiDeviceNum; i++){
            MyUpd.writeDatagram(CMD_AI_DISK_FETCH, i, 0, args, addressList.at(i), AI_UDP_SEND_PORT);
    //        myFlow.sleep(1);
            ret = MyUpd.readUdpDatagrams(&data,16);

            if(ret == -1){
                msg += QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "1";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            } else if(ret == -2){
                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "2";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else{
                diskValidInfo.clear();
                if(data.nCommandData.size() == 0){
                    diskValidInfo=QString::number(0);
                }else{
                    quint8 mode1 = (quint8)data.nCommandData.at(0);
                    quint8 mode2 = (quint8)data.nCommandData.at(1);
                    quint8 mode3 = (quint8)data.nCommandData.at(2);
                    quint8 mode4 = (quint8)data.nCommandData.at(3);
                    qDebug()<<mode1<<mode2<<mode3<<mode4;
                    diskValidInfo =  QString::number(qulonglong(mode1*16777216+mode2*65536+mode3*256+mode4));
                }
                qDebug()<<diskValidInfo;
                if(diskValidInfo.isEmpty()){
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }else{
                    diskValidLblInfoLbl[i]->setText(diskValidInfo+" M");
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_succeeded;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }
              }



//            if(ret == -1){
//                infoWidget->setLabelText(myLan.setting_error + "1");
//                infoWidget->delayShow();
//            }else if(ret == -2){
//                infoWidget->setLabelText(myLan.setting_error + "2");
//                infoWidget->delayShow();
//            }else{
//                quint8 mode1 = (quint8)data.nCommandData.at(0);
//                quint8 mode2 = (quint8)data.nCommandData.at(1);
//                quint8 mode3 = (quint8)data.nCommandData.at(2);
//                quint8 mode4 = (quint8)data.nCommandData.at(3);
//                qDebug()<<mode1<<mode2<<mode3<<mode4;
//                diskValidInfo =  QString::number(qulonglong(mode1*2304+mode2*1024+mode3*256+mode4));

//                qDebug()<<diskValidInfo;
//                if(diskValidInfo.isEmpty()){
//                    infoWidget->setLabelText(myLan.setting_error);
//                    infoWidget->delayShow();
//                }else{
//    //                diskValidLblInfoLbl->setText(diskValidInfo+" M");
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    infoWidget->delayShow();
//                }
//            }
        }

        aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
        aiInfoWidget->setLabelText(msg);
        aiInfoWidget->delayShow();
        myFlow.sleep(3);
        aiInfoWidget->hide();

//        myFlow.sleep(3);
//        infoWidget->hide();
        break;

    case AI_PAGE_MODEL_PARA:
        //保存modelParaInfo
        if(!modelListWidget->count()){
            infoWidget->setLabelText(myLan.model_list_blank);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
            ApplyBtn->setEnabled(true);
            return;
        }
        QStringList list =  modelListWidget->currentItem()->text().split("-");
        if(list.size() < 2){
            infoWidget->setLabelText(myLan.model_para_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
            ApplyBtn->setEnabled(true);
            return;
        }
        QString  modelId = list.at(1);
        QSqlQuery sql_update;
        sql_update.prepare("update  modelParaInfo  set threshold = ? , isApply = ? WHERE modelId = ? and id =?");

        sql_update.bindValue(2, modelId);
        for(int i=0 ; i<modeParaCount; i++){
           sql_update. bindValue(0, modeParaStr[i].threshold);
           sql_update. bindValue(1, modeParaStr[i].isApply);
           sql_update. bindValue(3, modeParaStr[i].id);
            if(!sql_update.exec()){
                qDebug() << "update modelParaInfo 失败"<<modeParaStr[i].id<<":"<<modeParaStr[i].threshold;
                ApplyBtn->setEnabled(true);
                return;
            }
        }

        int count = 0;
        int seq = 0;
        for(int i=0 ; i<modeParaCount; i++){
            if(modeParaStr[i].isApply.toInt() == 1){
                count++;
                args[seq++] = modeParaStr[i].id.toInt();
                //阈值=100-灵敏度
                args[seq++] = 100-modeParaStr[i].threshold.toInt();
            }
        }
        if(count == 0){
            infoWidget->setLabelText(myLan.model_para_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
            ApplyBtn->setEnabled(true);
            return;
        }

        aiInfoWidget->setLabelText(myLan.apply+" ......");
        aiInfoWidget->setMsgHeightRatio(2);
        aiInfoWidget->delayShow();


        for(int i=0; i< aiDeviceNum; i++){
            MyUpd.writeDatagram(CMD_AI_MODEL_PARA, i, count*2, args, addressList.at(i), AI_UDP_SEND_PORT);
    //        myFlow.sleep(1);
            ret = MyUpd.readUdpDatagrams(&data,13);


            if(ret == -1){
                msg += QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "1";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else if(ret == -2){
                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error + "2";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
            }else{
                quint8 mode = (quint8)data.nCommandData.at(0);
                if (mode == 1){
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    infoWidget->delayShow();
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_succeeded;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }else if(mode == 0){
//                    infoWidget->setLabelText(myLan.setting_error);
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
//                    infoWidget->delayShow();
                }else{
//                    infoWidget->setLabelText(myLan.setting_value_error);
//                    infoWidget->delayShow();
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.setting_value_error;
                    if(i != (aiDeviceNum-1)){
                        msg +=QString("\n");
                    }
                }
            }

//            if(ret == -1){
//                infoWidget->setLabelText(myLan.setting_error + "1");
//                infoWidget->delayShow();
//            }else if(ret == -2){
//                infoWidget->setLabelText(myLan.setting_error + "2");
//                infoWidget->delayShow();
//            }else{
//                quint8 mode = (quint8)data.nCommandData.at(0);
//                qDebug()<<mode;
//                if (mode == 1){
//                    infoWidget->setLabelText(myLan.setting_succeeded);
//                    infoWidget->delayShow();
//                }else if(mode == 0){
//                    infoWidget->setLabelText(myLan.setting_error);
//                    infoWidget->delayShow();
//                }else{
//                    infoWidget->setLabelText(myLan.setting_value_error);
//                    infoWidget->delayShow();
//                }
//            }
        }


        aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
        aiInfoWidget->setLabelText(msg);
        myFlow.sleep(4);
        aiInfoWidget->hide();

//        myFlow.sleep(3);
//        infoWidget->hide();
        break;
    }


    ApplyBtn->setEnabled(true);

}

void AisMainWidget::onImageInferBtnPressedSlt(){

    if(isImageSend){
//        infoWidget->setLabelText("图像采集进行中");
        infoWidget->setLabelText(myLan.image_acquisition+myLan.beginning);

        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    isImageInfer= !isImageInfer;
    if(isImageInfer){
        QString msg = QString("");
        QByteArray args;
        AI_Data_Protocol_D data;
        int ret;
        args[0] = 1;

        aiInfoWidget->setLabelText(myLan.image_reasoning+myLan.begin);
        aiInfoWidget->setMsgHeightRatio(2);
        aiInfoWidget->delayShow();
        int count = 0;

        for(int i=0; i< aiDeviceNum; i++){
            MyUpd.writeDatagram(CMD_AI_IMAGE_INFER, i, 1, args, addressList.at(i), AI_UDP_SEND_PORT);
            ret = MyUpd.readUdpDatagrams(&data,13);
            if(ret == -1){
                msg += QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.begin+myLan.error + "1";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
                isImageInfer = false;
            }else if(ret == -2){
                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.begin+myLan.error + "2";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
                isImageInfer = false;
            }else{
                quint8 mode = (quint8)data.nCommandData.at(0);
                qDebug()<<mode;
                if (mode == 1){
                    count++;
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.begin+myLan.succeed;
                    if(i != (aiDeviceNum-1)){
                       msg +=QString("\n");
                    }
                    //相机板发送ai协议
                    for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++) {
                        MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 1, 3);
                    }
                }else if(mode == 0){
                    msg += QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.begin+myLan.error;
                    if(i != (aiDeviceNum-1)){
                       msg +=QString("\n");
                    }
                    isImageInfer = false;
                }else{
                    msg += QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.begin+myLan.setting_value_error;
                    if(i != (aiDeviceNum-1)){
                       msg +=QString("\n");
                    }
                    isImageInfer = false;
                }
            }

        }

        if(count == aiDeviceNum){
            imageInferBtn->setText(myLan.image_reasoning+myLan.stop);
            QPalette patlette;
            patlette.setColor(QPalette::ButtonText, Qt::red);
            imageInferBtn->setPalette(patlette);
            isImageInfer = true;
            msg.clear();
            msg = myLan.image_reasoning+myLan.begin+myLan.succeed;
            aiInfoWidget->setMsgHeightRatio(1);
            aiInfoWidget->setLabelText(msg);
            myFlow.sleep(4);
            aiInfoWidget->hide();
            return;
        }
//        MyUpd.writeDatagram(CMD_AI_IMAGE_INFER, 0, 1, args, address, AI_UDP_SEND_PORT);
//        infoWidget->setLabelText("图像推理开始");
//        infoWidget->setLabelText(myLan.image_reasoning+myLan.begin);

//        infoWidget->delayShow();
//        myFlow.sleep(1);
//        ret = MyUpd.readUdpDatagrams(&data,13);
//        if(ret == -1){
////            infoWidget->setLabelText("图像推理开始失败1");
//            infoWidget->setLabelText(myLan.image_reasoning+myLan.begin+myLan.error + "1");

//            infoWidget->delayShow();
//            isImageInfer = false;
//        }else if(ret == -2){
////            infoWidget->setLabelText("图像推理开始失败2");
//            infoWidget->setLabelText(myLan.image_reasoning+myLan.begin+myLan.error + "2");

//            infoWidget->delayShow();
//            isImageInfer = false;
//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<mode;
//            if (mode == 1){
////                imageInferBtn->setText("图像推理停止");
//                imageInferBtn->setText(myLan.image_reasoning+myLan.stop);

//                QPalette patlette;
//                patlette.setColor(QPalette::ButtonText, Qt::red);
//                imageInferBtn->setPalette(patlette);
////                infoWidget->setLabelText("图像推理开始成功");
//                infoWidget->setLabelText(myLan.image_reasoning+myLan.begin+myLan.succeed);

//                //相机板发送ai协议
//                for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++) {
//                    MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 1, 3);
//                }
//                infoWidget->delayShow();
//                isImageInfer = true;
//            }else if(mode == 0){
////                infoWidget->setLabelText("图像推理开始失败");
//                infoWidget->setLabelText(myLan.image_reasoning+myLan.begin+myLan.error);

//                infoWidget->delayShow();
//                isImageInfer = false;

//            }else{
////                infoWidget->setLabelText("图像推理开始返回值错误");
//                infoWidget->setLabelText(myLan.image_reasoning+myLan.begin+myLan.setting_value_error);

//                infoWidget->delayShow();
//                isImageInfer = false;
//            }
//        }
//        myFlow.sleep(3);
//        infoWidget->hide();
        isImageInfer = false;
        aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
        aiInfoWidget->setLabelText(msg);
        myFlow.sleep(4);
        aiInfoWidget->hide();

    }else{
        //相机板发送ai协议
        for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++) {
            MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 0, 3);
        }
        myFlow.msleep(100);
        QString msg = QString("");
        QByteArray args;
        AI_Data_Protocol_D data;
        int ret;
        args[0] = 0;
        int count = 0;

        aiInfoWidget->setLabelText(myLan.image_reasoning+myLan.stop);
        aiInfoWidget->setMsgHeightRatio(2);
        aiInfoWidget->delayShow();

        for(int i=0; i< aiDeviceNum; i++){
            MyUpd.writeDatagram(CMD_AI_IMAGE_INFER, i, 1, args, addressList.at(i), AI_UDP_SEND_PORT);
            ret = MyUpd.readUdpDatagrams(&data,13);
            if(ret == -1){
                msg += QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.stop+myLan.error + "1";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
                isImageInfer = true;
            }else if(ret == -2){
                msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.stop+myLan.error + "2";
                if(i != (aiDeviceNum-1)){
                   msg +=QString("\n");
                }
                isImageInfer = true;
            }else{
                quint8 mode = (quint8)data.nCommandData.at(0);
                qDebug()<<mode;
                if (mode == 1){
                    count++;
                    msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.stop+myLan.succeed;
                    if(i != (aiDeviceNum-1)){
                       msg +=QString("\n");
                    }
                }else if(mode == 0){
                    msg += QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.stop+myLan.error;
                    if(i != (aiDeviceNum-1)){
                       msg +=QString("\n");
                    }
                    isImageInfer = true;
                }else{
                    msg += QString("aiDevice %1 ").arg(i+1) + myLan.image_reasoning+myLan.stop+myLan.setting_value_error;
                    if(i != (aiDeviceNum-1)){
                       msg +=QString("\n");
                    }
                    isImageInfer = true;
                }
            }

        }

        if(count == aiDeviceNum){
            imageInferBtn->setText(myLan.image_reasoning+myLan.begin);
            QPalette patlette;
            patlette.setColor(QPalette::ButtonText, Qt::black);
            imageInferBtn->setPalette(patlette);
            isImageInfer = false;
            msg.clear();
            msg = myLan.image_reasoning+myLan.stop+myLan.succeed;
            aiInfoWidget->setMsgHeightRatio(1);
            aiInfoWidget->setLabelText(msg);
            myFlow.sleep(4);
            aiInfoWidget->hide();
            return;
        }


//        MyUpd.writeDatagram(CMD_AI_IMAGE_INFER, 0, 1, args, address, AI_UDP_SEND_PORT);
////        infoWidget->setLabelText("图像推理停止");
////        infoWidget->setLabelText(myLan.image_reasoning+myLan.stop);

////        infoWidget->delayShow();
////        myFlow.sleep(1);
//        ret = MyUpd.readUdpDatagrams(&data,13);
//        if(ret == -1){
////            infoWidget->setLabelText("图像推理停止失败1");
//            infoWidget->setLabelText(myLan.image_reasoning+myLan.stop+myLan.error + "1");

//            infoWidget->delayShow();
//            isImageInfer = true;
//        }else if(ret== -2){
////            infoWidget->setLabelText("图像推理停止失败2");
//            infoWidget->setLabelText(myLan.image_reasoning+myLan.stop+myLan.error + "2");

//            infoWidget->delayShow();
//            isImageInfer = true;
//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<mode;
//            if (mode == 1){
////                imageInferBtn->setText("图像推理开始");
//                imageInferBtn->setText(myLan.image_reasoning+myLan.begin);

//                QPalette patlette;
//                patlette.setColor(QPalette::ButtonText, Qt::black);
//                imageInferBtn->setPalette(patlette);
////                infoWidget->setLabelText("图像推理停止成功");
//                infoWidget->setLabelText(myLan.image_reasoning+myLan.stop+myLan.succeed);

//                infoWidget->delayShow();
//                isImageInfer = false;
//            }else if(mode == 0){
////                infoWidget->setLabelText("图像推理停止失败");
//                infoWidget->setLabelText(myLan.image_reasoning+myLan.stop+myLan.error);

//                infoWidget->delayShow();
//                isImageInfer = true;

//            }else{
////                infoWidget->setLabelText("图像推理停止返回值错误");
//                infoWidget->setLabelText(myLan.image_reasoning+myLan.stop+myLan.setting_value_error);
//                infoWidget->delayShow();
//                isImageInfer = true;
//            }
//        }
//        myFlow.sleep(3);
//        infoWidget->hide();
        isImageInfer = true;
        aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
        aiInfoWidget->setLabelText(msg);
        myFlow.sleep(4);
        aiInfoWidget->hide();
    }

}

void AisMainWidget::onImageSendBtnPressedSlt(){
    if(isImageInfer){
//        infoWidget->setLabelText("图像推理进行中");
        infoWidget->setLabelText(myLan.image_reasoning+myLan.beginning);

        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    isImageSend = !isImageSend;
    if(isImageSend){
        QByteArray args;
        AI_Data_Protocol_D data;
        int ret;
        args[0] = 1;
        MyUpd.writeDatagram(CMD_AI_IMAGE_SEND, 0, 1, args, address, AI_UDP_SEND_PORT);
//        infoWidget->setLabelText("图像采集开始");
        infoWidget->setLabelText(myLan.image_acquisition+myLan.begin);

        infoWidget->delayShow();
//        myFlow.sleep(1);
        ret = MyUpd.readUdpDatagrams(&data,13);
        if(ret == -1){
//            infoWidget->setLabelText("图像采集开始失败1");
            infoWidget->setLabelText(myLan.image_acquisition+myLan.begin+myLan.error + "1");

            infoWidget->delayShow();
            isImageSend = false;
        }else if(ret == -2){
//            infoWidget->setLabelText("图像采集开始失败2");
            infoWidget->setLabelText(myLan.image_acquisition+myLan.begin+myLan.error + "2");

            infoWidget->delayShow();
            isImageSend = false;
        }else{
            quint8 mode = (quint8)data.nCommandData.at(0);
            qDebug()<<mode;
            if (mode == 1){
//                imageSendBtn->setText("图像采集停止");
                imageSendBtn->setText(myLan.image_acquisition+myLan.stop);

                QPalette patlette;
                patlette.setColor(QPalette::ButtonText, Qt::red);
                imageSendBtn->setPalette(patlette);
//                infoWidget->setLabelText("图像采集开始成功");
                infoWidget->setLabelText(myLan.image_acquisition+myLan.begin+myLan.succeed);
                infoWidget->delayShow();
                //相机板发送ai协议
                for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++) {
                    MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 1, 3);
                }
                isImageSend = true;
            }else if(mode == 0){
//                infoWidget->setLabelText("图像采集开始失败");
                infoWidget->setLabelText(myLan.image_acquisition+myLan.begin+myLan.error);

                infoWidget->delayShow();
                isImageSend = false;

            }else{
//                infoWidget->setLabelText("图像采集开始返回值错误");
                infoWidget->setLabelText(myLan.image_acquisition+myLan.begin+myLan.setting_value_error);

                infoWidget->delayShow();
                isImageSend = false;
            }
        }
        myFlow.sleep(3);
        infoWidget->hide();

    }else{
        //相机板发送ai协议
        for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++) {
            MySerial.com1Write(CMD_UNIT_AI_CAPTURE_ENABLE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 0, 3);
        }
        myFlow.msleep(100);
        QByteArray args;
        AI_Data_Protocol_D data;
        args[0] = 0;
        MyUpd.writeDatagram(CMD_AI_IMAGE_SEND, 0, 1, args, address, AI_UDP_SEND_PORT);
//        infoWidget->setLabelText("图像采集停止");
        infoWidget->setLabelText(myLan.image_acquisition+myLan.stop);

        infoWidget->delayShow();
//        myFlow.sleep(1);
        int ret = MyUpd.readUdpDatagrams(&data,13);
        if(ret == -1){
//            infoWidget->setLabelText("图像采集停止失败1");
            infoWidget->setLabelText(myLan.image_acquisition+myLan.stop+myLan.error + "1");

            infoWidget->delayShow();
            isImageSend = true;
        }else if(ret == -2){
//            infoWidget->setLabelText("图像采集停止失败2");
            infoWidget->setLabelText(myLan.image_acquisition+myLan.stop+myLan.error + "2");

            infoWidget->delayShow();
            isImageSend = true;
        }else{
            quint8 mode = (quint8)data.nCommandData.at(0);
            qDebug()<<mode;
            if (mode == 1){
//                imageSendBtn->setText("图像采集开始");
                imageSendBtn->setText(myLan.image_acquisition+myLan.begin);

                QPalette patlette;
                patlette.setColor(QPalette::ButtonText, Qt::black);
                imageSendBtn->setPalette(patlette);
//                infoWidget->setLabelText("图像采集停止成功");
                infoWidget->setLabelText(myLan.image_acquisition+myLan.stop+myLan.succeed);

                infoWidget->delayShow();
                isImageSend = false;
            }else if(mode == 0){
//                infoWidget->setLabelText("图像采集停止失败");
                infoWidget->setLabelText(myLan.image_acquisition+myLan.stop+myLan.error);

                infoWidget->delayShow();
                isImageSend = true;

            }else{
//                infoWidget->setLabelText("图像采集停止返回值错误");
                infoWidget->setLabelText(myLan.image_acquisition+myLan.stop+myLan.setting_value_error);

                infoWidget->delayShow();
                isImageSend = true;
            }
        }
        myFlow.sleep(3);
        infoWidget->hide();
    }


}
void AisMainWidget::CreatePagePixel(){

    ejectorNum = 0;
    startPixelNum =  struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit];
    endPixelNum =  struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit];

    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);

    unitGroup = new myGroupBox("",pagePixel);
    unitGroup->setFixedSize(SMALL_BTN_WIDTH*8, SMALL_BTN_WIDTH+20);
    unitGroup->setFont(config->getFont());
    unitViewBtn = new myPushButton(myLan.front_view, QIcon(),true,true);
    unitPlusBtn = new myPushButton("",myIcon.Action_Plus);
    unitLcdNum = new QLCDNumber;
    unitLcdNum->display(struGsh.nUnit/2+1);
    unitLcdNum->setDigitCount(2);
    unitLcdNum->setFixedSize(btnSize);
    unitMinusBtn = new myPushButton("",myIcon.Action_Minus);

    unitViewBtn->setFixedSize(btnSize);
    unitViewBtn->setFixedWidth(SMALL_BTN_WIDTH+10);
    unitPlusBtn->setFixedSize(btnSize);
    unitMinusBtn->setFixedSize(btnSize);
    unitGridLayout = new QGridLayout(unitGroup);
    unitGridLayout->setHorizontalSpacing(50);
    unitGridLayout->addWidget(unitViewBtn, 0, 1, Qt::AlignHCenter );
    unitGridLayout->addWidget(unitPlusBtn, 0, 2, Qt::AlignHCenter);
    unitGridLayout->addWidget(unitLcdNum, 0, 3, Qt::AlignHCenter);
    unitGridLayout->addWidget(unitMinusBtn, 0, 4, Qt::AlignHCenter);
    unitGridLayout->setContentsMargins(5,5,5,5);

    startPixelLbl = new myLabel(myLan.pixel+" "+myLan.begin, pagePixel);
    startPixelEdit = new myLineEdit(QString("%1").arg(startPixelNum), pagePixel);
    startPixelEdit->setReadOnly(true);
    startPixelEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    endPixelLbl = new myLabel(myLan.pixel+" "+myLan.end, pagePixel);
    endPixelEdit = new myLineEdit(QString("%1").arg(endPixelNum), pagePixel);
    endPixelEdit->setReadOnly(true);
    endPixelEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    QVBoxLayout *pagePixelLayout = new QVBoxLayout(pagePixel);

    QHBoxLayout *startPixelHLayout = new QHBoxLayout();
    startPixelHLayout->addWidget(startPixelLbl);
    startPixelHLayout->addWidget(startPixelEdit);
    startPixelHLayout->addSpacing(300);
    QHBoxLayout *endPixelHLayout = new QHBoxLayout();
    endPixelHLayout->addWidget(endPixelLbl);
    endPixelHLayout->addWidget(endPixelEdit);
    endPixelHLayout->addSpacing(300);
    QSpacerItem* space = new QSpacerItem(200,80);
    pagePixelLayout->addSpacerItem(space);
    pagePixelLayout->addWidget(unitGroup);
    pagePixelLayout->addLayout(startPixelHLayout);
    pagePixelLayout->addLayout(endPixelHLayout);

    connect(startPixelEdit, SIGNAL(pressed()), this, SLOT(onStartPixelEditPressedSlt()));
    connect(endPixelEdit, SIGNAL(pressed()), this, SLOT(onEndPixelEditPressedSlt()));
    connect(unitViewBtn, SIGNAL(pressed()), this, SLOT(onUnitViewClicked()));
    connect(unitPlusBtn, SIGNAL(pressed()), this, SLOT(onUnitPlusBtnClicked()));
    connect(unitMinusBtn, SIGNAL(pressed()), this, SLOT(onUnitMinusBtnClicked()));

}

void AisMainWidget::updatePagePixel(){
    startPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]));
    endPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]));
}


/* 板号增加按钮 */
void AisMainWidget::onUnitPlusBtnClicked()
{
    int idTotal = 0;
    switch(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal) {
    case 1:     // 单视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
        break;
    case 2:     // 双视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        break;
    }
    if(struGsh.nUnit < idTotal-2) {
        struGsh.nUnit += 2;
    }
    unitLcdNum->display(struGsh.nUnit/2+1);
    startPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]));
    endPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]));
}


/* 板号减少按钮 */
void AisMainWidget::onUnitMinusBtnClicked()
{
    if(struGsh.nUnit > 1) {
        struGsh.nUnit -= 2;
    }
    unitLcdNum->display(struGsh.nUnit/2+1);
    startPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]));
    endPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]));
}


/* 前后视切换按钮 */
void AisMainWidget::onUnitViewClicked()
{
    struGsh.nUnit += (struGsh.nUnit%2 == 0) ? 1 : -1;
    if (struGsh.nUnit%2 == 0) {
        unitViewBtn->setText(myLan.front_view);
    } else {
        unitViewBtn->setText(myLan.rear_view);
    }
    startPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]));
    endPixelEdit->setText( QString::number(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]));
}

void AisMainWidget::onStartPixelEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, startPixelNum);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        startPixelNum = inputDlg.getValue();
        QString str = QString("%1").arg(startPixelNum);
        startPixelEdit->setText(str);
    }
}

void AisMainWidget::onEndPixelEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, endPixelNum);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        endPixelNum = inputDlg.getValue();
        QString str = QString("%1").arg(endPixelNum);
        endPixelEdit->setText(str);
    }
}

void AisMainWidget::CreatePageEjector(){
//    ejectorDelay = 0;
//    ejectorDynamicDelay = 0;
//    ejectorBlowTime = 0;
    //单通道喷嘴数量
    ejectorNum = struCnfg.nEjectorsPerChute;
//  ejectorNum =  struCnfg.nEjectorsPerChute*struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2;
    ejectorNumLbl = new myLabel(myLan.ejector, pagePixel);
    ejectorNumEdit = new myLineEdit(QString("%1").arg(ejectorNum), pagePixel);

    ejectorNumEdit->setReadOnly(true);
    ejectorNumEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

//    QHBoxLayout *ejectorHLayout = new QHBoxLayout();
//    ejectorHLayout->addWidget(ejectorNumLbl);
//    ejectorHLayout->addWidget(ejectorNumEdit);
//    ejectorHLayout->addSpacing(300);

    ejectorDelayLbl = new myLabel(myLan.fixed_delay+"(ms)", pageEjector);
    ejectorDelayEdit = new myLineEdit(QString("%1").arg(ejectorDelay), pageEjector);
    ejectorDelayEdit->setReadOnly(true);
    ejectorDelayEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    ejectorDynamicDelayLbl = new myLabel(myLan.dynamic_delay+"(ms)", pageEjector);
    ejectorDynamicDelayEdit = new myLineEdit(QString("%1").arg(ejectorDynamicDelay), pageEjector);
    ejectorDynamicDelayEdit->setReadOnly(true);
    ejectorDynamicDelayEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    ejectorBlowTimeLbl = new myLabel(myLan.eject_time+"(ms)", pageEjector);
    ejectorBlowTimeEdit = new myLineEdit(QString("%1").arg(ejectorBlowTime), pageEjector);
    ejectorBlowTimeEdit->setReadOnly(true);
    ejectorBlowTimeEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    QGridLayout *ejectorDelayGridLayout = new QGridLayout(pageEjector);
    ejectorDelayGridLayout->setContentsMargins(50, 50, 50, 50);
    ejectorDelayGridLayout->addWidget(ejectorNumLbl, 0, 0);
    ejectorDelayGridLayout->addWidget(ejectorNumEdit, 0, 1);
    ejectorDelayGridLayout->addWidget(ejectorDelayLbl, 1, 0);
    ejectorDelayGridLayout->addWidget(ejectorDelayEdit, 1, 1);
    ejectorDelayGridLayout->addWidget(ejectorDynamicDelayLbl, 2, 0);
    ejectorDelayGridLayout->addWidget(ejectorDynamicDelayEdit, 2, 1);
    ejectorDelayGridLayout->addWidget(ejectorBlowTimeLbl, 3, 0);
    ejectorDelayGridLayout->addWidget(ejectorBlowTimeEdit, 3, 1);
//    ejectorDelayHLayout->addSpacing(300);

    connect(ejectorNumEdit, SIGNAL(pressed()), this, SLOT(onEjectorNumEditPressedSlt()));
    connect(ejectorDelayEdit, SIGNAL(pressed()), this, SLOT(onEjectorDelayEditPressedSlt()));
    connect(ejectorDynamicDelayEdit, SIGNAL(pressed()), this, SLOT(onEjectorDynamicDelayEditPressedSlt()));
    connect(ejectorBlowTimeEdit, SIGNAL(pressed()), this, SLOT(onEjectorBlowTimeEditPressedSlt()));

}

void AisMainWidget::onEjectorNumEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 256, ejectorNum);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        ejectorNum = inputDlg.getValue();
        QString str = QString("%1").arg(ejectorNum);
        ejectorNumEdit->setText(str);
    }
}

void AisMainWidget::onEjectorDelayEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, ejectorDelay);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        ejectorDelay = inputDlg.getValue();
        QString str = QString("%1").arg(ejectorDelay);
        ejectorDelayEdit->setText(str);
    }
}

void AisMainWidget::onEjectorDynamicDelayEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, ejectorDynamicDelay);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        ejectorDynamicDelay = inputDlg.getValue();
        QString str = QString("%1").arg(ejectorDynamicDelay);
        ejectorDynamicDelayEdit->setText(str);
    }
}

void AisMainWidget::onEjectorBlowTimeEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 256, ejectorBlowTime);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        ejectorBlowTime = inputDlg.getValue();
        QString str = QString("%1").arg(ejectorBlowTime);
        ejectorBlowTimeEdit->setText(str);
    }
}



void AisMainWidget::CreatePageImgHeight(){
//    imgFetchHeight = 500;
//    imgInferHeight = 100;

    imgFetchHeightLbl = new myLabel(myLan.acquisition_height, pageImgHeight);
    imgFetchHeightEdit = new myLineEdit(QString("%1").arg(imgFetchHeight), pageImgHeight);

    imgFetchHeightEdit->setReadOnly(true);
    imgFetchHeightEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    imgInferHeightLbl = new myLabel(myLan.reasoning_height, pageImgHeight);
    imgInferHeightEdit = new myLineEdit(QString("%1").arg(imgInferHeight), pageImgHeight);

    imgInferHeightEdit->setReadOnly(true);
    imgInferHeightEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    QHBoxLayout * imgFetchHeightHLayout = new QHBoxLayout();
    imgFetchHeightHLayout->addWidget(imgFetchHeightLbl);
    imgFetchHeightHLayout->addWidget(imgFetchHeightEdit);
    imgFetchHeightHLayout->addSpacing(300);

    QHBoxLayout * imgInferHeightHLayout = new QHBoxLayout();
    imgInferHeightHLayout->addWidget(imgInferHeightLbl);
    imgInferHeightHLayout->addWidget(imgInferHeightEdit);
    imgInferHeightHLayout->addSpacing(300);

    aiEnbaleImageSendCbx= new myCustomCheckBox(myLan.enable+myLan.image_acquisition, false);


    imgFetchHeightLbl->hide();
    imgFetchHeightEdit->hide();
    imgInferHeightLbl->hide();
    imgInferHeightEdit->hide();

    QVBoxLayout *pageImgHeightLayout = new QVBoxLayout(pageImgHeight);
    pageImgHeightLayout->addLayout(imgFetchHeightHLayout);
    pageImgHeightLayout->addLayout(imgInferHeightHLayout);
    pageImgHeightLayout->addWidget(aiEnbaleImageSendCbx);
    connect(imgFetchHeightEdit, SIGNAL(pressed()), this, SLOT(onImgFetchHeightEditPressedSlt()));
    connect(imgInferHeightEdit, SIGNAL(pressed()), this, SLOT(onImgInferHeightEditPressedSlt()));
    connect(aiEnbaleImageSendCbx, SIGNAL(pressed()), this, SLOT(onAiEnbaleImageSendCbxClicked()));

}

void AisMainWidget::onAiEnbaleImageSendCbxClicked(){
    struGsh.isImageSend = !struGsh.isImageSend;
    aiEnbaleImageSendCbx->setChecked(struGsh.isImageSend);
    qDebug()<< struGsh.isImageSend;
}


void AisMainWidget::onImgFetchHeightEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, imgFetchHeight);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        imgFetchHeight = inputDlg.getValue();
        QString str = QString("%1").arg(imgFetchHeight);
        imgFetchHeightEdit->setText(str);
    }
}

void AisMainWidget::onImgInferHeightEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, imgInferHeight);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        imgInferHeight = inputDlg.getValue();
        QString str = QString("%1").arg(imgInferHeight);
        imgInferHeightEdit->setText(str);
    }
}


void AisMainWidget::CreatePageModePara(){

    pageModelParaLayout = new QVBoxLayout(pageModePara);

    m_sigMapper = new QSignalMapper(this);
    m_sigCheckMapper = new QSignalMapper(this);

    for(int i=0; i< 10 ; i++){
       modelNameCbx[i] =  new myCustomCheckBox("",  true,
                                               CB_STYLE_APPLY,pageModePara);
       thresholdLbl[i] = new myLabel(myLan.sensitivity, pageModePara);
       thresholdLineEdit[i] = new myLineEdit(QString("%1").arg(""), pageModePara);
       thresholdLineEdit[i]->setReadOnly(true);
       thresholdLineEdit[i]->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);
       paraModelHLayout[i] = new QHBoxLayout();
       paraModelHLayout[i]->addWidget(modelNameCbx[i]);
       paraModelHLayout[i]->addStretch(1);
       paraModelHLayout[i]->addWidget(thresholdLbl[i]);
       paraModelHLayout[i]->addWidget(thresholdLineEdit[i]);
       paraModelHLayout[i]->addSpacing(100);
       pageModelParaLayout->addLayout(paraModelHLayout[i]);

       m_sigCheckMapper->setMapping(modelNameCbx[i], i);
       connect(modelNameCbx[i], SIGNAL(pressed()), m_sigCheckMapper, SLOT(map()));

       m_sigMapper->setMapping(thresholdLineEdit[i], i);
       connect(thresholdLineEdit[i], SIGNAL(pressed()), m_sigMapper, SLOT(map()));
    }

    connect(m_sigMapper, SIGNAL(mapped(int)), this, SLOT(setThresholdLineEdit(int)));

    connect(m_sigCheckMapper, SIGNAL(mapped(int)), this, SLOT(setModelNameCbx(int)));

    updateModeParaInfo();

}

void AisMainWidget::setModelNameCbx(int index){
    if( modelNameCbx[index]->getChecked()){
        modeParaStr[index].isApply = "1";
    }else{
        modeParaStr[index].isApply = "0";
    }
    qDebug()<<modeParaStr[index].isApply ;
}

void AisMainWidget::setThresholdLineEdit(int index){
    myInputPanel inputDlg(intType, 0, 100, thresholdLineEdit[index]->text().toInt());
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        int thresholdNum = inputDlg.getValue();
        QString str = QString("%1").arg(thresholdNum);
        thresholdLineEdit[index]->setText(str);
        modeParaStr[index].threshold = str;
    }
}

void AisMainWidget::updateModeParaInfo(){

    QString  modelId;
    QSqlQuery query2;
    query2.prepare("SELECT modelId, modelName, isApply FROM modelInfo where isApply = ?");
    query2.bindValue(0,"1");
    if (!query2.exec()) {
        qDebug() << "modelInfo 查询失败：" << query2.lastError();
        return;
    } else {
        while (query2.next()) {
            modelId = query2.value(0).toString();
            break;
        }
//        qDebug() << modelId;
    }

    QSqlQuery query;
    query.prepare("SELECT modelId, id, zhName, enName, threshold, isApply, chgTime FROM modelParaInfo  "
                        "where modelId = ? order by id asc");

    query.bindValue(0,modelId);

    modeParaCount = 0;
    for(int i=modeParaCount; i<10; i++){
       modelNameCbx[i]->show();
       thresholdLbl[i]->show();
       thresholdLineEdit[i]->show();
    }

    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError();
    } else {
        while (query.next()) {
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

            modelNameCbx[modeParaCount]->setCheckBoxName(zhName);
            modelNameCbx[modeParaCount]->setChecked(isApply.toInt());
            thresholdLineEdit[modeParaCount]->setText(threshold);
            modeParaCount++;
        }
    }
    qDebug() <<"modeParaCount: " <<modeParaCount;

    for(int i=modeParaCount; i<10; i++){
       modelNameCbx[i]->hide();
       thresholdLbl[i]->hide();
       thresholdLineEdit[i]->hide();
    }

}


void AisMainWidget::CreatePageModeUpdate(){
    // 创建一个新的QFtp对象
//    uploader = new FtpUploader(ftpIpAddress, 21, SFTP_USER, SFTP_PASSWORD);
//    uploaderList.append(uploader);
    for(int i=0; i<aiDeviceNum; i++){
        uploaderList.append(new FtpUploader(ftpIpAddressList.at(i), 21, SFTP_USER, SFTP_PASSWORD));
    }
    uploader = uploaderList.at(0);

    modelListWidget = new myListWidget(QSize(ICON_WID, ICON_HEI),this);
    modelListWidget->setFixedWidth(610);
    modelListWidget->setFixedHeight(500);
    updateModelListWidget();

    modelListWidget->setStyleSheet("background-color:transparent");
    modelListWidget->setFrameStyle(QFrame::Panel);

    applyModeBtn = new myPushButton(myLan.apply, QIcon(""));
    deleteModelBtn = new myPushButton(myLan.del, QIcon(""));
    renameModeBtn = new myPushButton(myLan.rename, QIcon(""));
    addModeBtn = new myPushButton(myLan.add, QIcon(""));

    applyModeBtn->setFixedSize(QSize(DEFAULT_BTN_WIDTH,BTN_HEIGHT+10));
    deleteModelBtn->setFixedSize(QSize(DEFAULT_BTN_WIDTH,BTN_HEIGHT+10));
    renameModeBtn->setFixedSize(QSize(DEFAULT_BTN_WIDTH,BTN_HEIGHT+10));
    addModeBtn->setFixedSize(QSize(DEFAULT_BTN_WIDTH,BTN_HEIGHT+10));

    applyModeBtn->hide();

    QHBoxLayout *operatePageHBLayout  = new QHBoxLayout();
    operatePageHBLayout->addWidget(applyModeBtn);
    operatePageHBLayout->addWidget(addModeBtn);
    operatePageHBLayout->addWidget(renameModeBtn);
    operatePageHBLayout->addWidget(deleteModelBtn);

    QGridLayout *pageModelUpdateGridLayout = new QGridLayout(pageModeUpdate);
    pageModelUpdateGridLayout->addWidget(modelListWidget,0,0,6,1);
    pageModelUpdateGridLayout->addLayout(operatePageHBLayout,6,0,1,1,Qt::AlignCenter);

    connect(addModeBtn, SIGNAL(pressed()), this, SLOT(onAddModeBtnPressedSlt()));
    connect(applyModeBtn, SIGNAL(pressed()), this, SLOT(onApplyModeBtnPressedSlt()));
    connect(renameModeBtn, SIGNAL(pressed()), this, SLOT(onRenameModeBtnPressedSlt()));
    connect(deleteModelBtn, SIGNAL(pressed()), this, SLOT(onDeleteModelBtnPressedSlt()));

}

void AisMainWidget::onDeleteModelBtnPressedSlt(){
    if(modelListWidget->currentItem() < 0 ){
        infoWidget->setLabelText("先选择模型");
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    if(!modelListWidget->count()){
        infoWidget->setLabelText(myLan.model_list_blank);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    QStringList list =  modelListWidget->currentItem()->text().split("-");
    if(list.size() < 2){
        infoWidget->setLabelText(myLan.model_para_error);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    if(modelListWidget->currentItem() == NULL ||
           (!modelListWidget->currentItem()->text().contains("-"))){
        infoWidget->setLabelText("先选择模型");
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
   }
    QString  modelId = list.at(1);
    if(modelId.isEmpty()){
//        infoWidget->setLabelText("模型id为空");
        infoWidget->setLabelText(myLan.model + myLan.id_blank);

        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }

    myMessageBox msgBox(MSG_QUES, myLan.del+myLan.model+"?");
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted){
        QSqlQuery sql_update;
//        qDebug()<<"modelId: "<<modelId;

        sql_update.prepare("delete from modelInfo  WHERE modelId = ?");
        sql_update.bindValue(0, modelId);
        bool result = sql_update.exec();

        QSqlQuery sql_update1;
        sql_update1.prepare("delete from modelParaInfo  WHERE modelId = ?");
        sql_update1.bindValue(0, modelId);
        bool result1 = sql_update1.exec();

        if(!result || !result1){
//            infoWidget->setLabelText("删除模型错误");
            infoWidget->setLabelText(myLan.del+myLan.model+myLan.error);

            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
        }else{
//            infoWidget->setLabelText("删除模型成功");
            infoWidget->setLabelText(myLan.del+myLan.model+myLan.succeed);

            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
        }
    }
    updateModelListWidget();

}

void AisMainWidget::onRenameModeBtnPressedSlt(){
    if(modelListWidget->currentItem() < 0 ){
        infoWidget->setLabelText("先选择模型");
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    if(!modelListWidget->count()){
        infoWidget->setLabelText(myLan.model_list_blank);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    if(modelListWidget->currentItem() == NULL ||
           (!modelListWidget->currentItem()->text().contains("-"))){
        infoWidget->setLabelText("先选择模型");
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
   }
    QStringList list =  modelListWidget->currentItem()->text().split("-");
    if(list.size() < 2){
        infoWidget->setLabelText(myLan.model_para_error);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    QString  modelId = list.at(1);
    if(modelId.isEmpty()){
//        infoWidget->setLabelText("模型id为空");
        infoWidget->setLabelText(myLan.model + myLan.id_blank);

        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }

//    myMessageBox msgBox(MSG_QUES, "重命名模型");
    myMessageBox msgBox(MSG_QUES, myLan.rename + myLan.model + "?");

    int ret = msgBox.exec();

    if (ret == QDialog::Accepted){
        if (modelListWidget->currentItem()) {
            QString strInput = QString("%1").arg(list.at(0));
            myInputMethod inputPanel(myLan.msg_name_input, strInput);
            ret = inputPanel.exec();
            if (ret == QDialog::Accepted) {
                QString strName = inputPanel.getText();
                QSqlQuery sql_update;
                sql_update.prepare("UPDATE modelInfo SET modelName = ? WHERE modelId = ?");
                sql_update.bindValue(0, strName);
                sql_update.bindValue(1, modelId);
                bool result = sql_update.exec();
                if(!result){
                    qDebug()<<"重命名模型错误";
//                    infoWidget->setLabelText("重命名模型错误");
                    infoWidget->setLabelText(myLan.rename + myLan.model+myLan.error);

                    infoWidget->delayShow();
                    myFlow.sleep(3);
                    infoWidget->hide();
                }
            }
        }
    }
    updateModelListWidget();
}

void AisMainWidget::onApplyModeBtnPressedSlt(){
    if(!modelListWidget->count()){
        infoWidget->setLabelText(myLan.model_list_blank);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    QStringList list =  modelListWidget->currentItem()->text().split("-");
    if(list.size() < 2){
        infoWidget->setLabelText(myLan.model_para_error);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    QString  modelId = list.at(1)+".dlc";
    if(list.at(1).isEmpty()){
        infoWidget->setLabelText(myLan.model + myLan.id_blank);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }

    QByteArray args = modelId.toLatin1();

    aiInfoWidget->setLabelText(myLan.apply+" ......");
    aiInfoWidget->setMsgHeightRatio(2);
    aiInfoWidget->delayShow();
    AI_Data_Protocol_D data;
    int count = 0;
    QString msg = QString("");

    for(int i=0; i< aiDeviceNum; i++){
       MyUpd.writeDatagram(CMD_AI_MODEL_UPDATE, i, modelId.length(), args, addressList.at(i), AI_UDP_SEND_PORT);
       int ret = MyUpd.readUdpDatagrams(&data,13);
       if(ret == -1){
           msg += QString("aiDevice %1 ").arg(i+1) + myLan.model+myLan.apply + "1";
           if(i != (aiDeviceNum-1)){
              msg +=QString("\n");
           }
       }else if(ret == -2){
           msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.model+myLan.apply + "2";
           if(i != (aiDeviceNum-1)){
              msg +=QString("\n");
           }
       }else{
           quint8 mode = (quint8)data.nCommandData.at(0);
           qDebug()<<mode;
           if (mode == 1){
               count++;
               msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.model+myLan.apply+myLan.succeed;
               if(i != (aiDeviceNum-1)){
                  msg +=QString("\n");
               }
           }else if(mode == 0){
               msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.model+myLan.apply+myLan.error;
               if(i != (aiDeviceNum-1)){
                  msg +=QString("\n");
               }
           }else{
               msg +=  QString("aiDevice %1 ").arg(i+1) + myLan.model+myLan.apply+myLan.setting_value_error;
               if(i != (aiDeviceNum-1)){
                  msg +=QString("\n");
               }
           }
       }
    }
    if(count == aiDeviceNum){
        QSqlQuery sql_update1;
        sql_update1.prepare("UPDATE modelInfo SET isApply = ? WHERE modelId = ?");
        sql_update1.bindValue(0, "1");
        sql_update1.bindValue(1, list.at(1));
        bool result1 = sql_update1.exec();
        QSqlQuery sql_update2;
        sql_update2.prepare("UPDATE modelInfo SET isApply = ? WHERE modelId != ?");
        sql_update2.bindValue(0, "0");
        sql_update2.bindValue(1, list.at(1));
        bool result2 = sql_update2.exec();
        msg.clear();
        if(!result1 || !result2){
            qDebug()<<"更新默认模型错误";
            msg = myLan.model+myLan.apply+myLan.error;
        }else{
            msg = myLan.model+myLan.apply+myLan.succeed;
        }
        aiInfoWidget->setMsgHeightRatio(1);
        aiInfoWidget->setLabelText(msg);
        myFlow.sleep(4);
        aiInfoWidget->hide();
        return;
    }

//    infoWidget->setLabelText("模型应用开始");
//    infoWidget->setLabelText(myLan.model+myLan.apply+myLan.begin);

//    infoWidget->delayShow();
//    myFlow.sleep(1);


//    if(ret == -1){
//        infoWidget->setLabelText(myLan.model+myLan.apply+myLan.error+"1");
//        infoWidget->delayShow();
//    }else if(ret == -2){
//        infoWidget->setLabelText(myLan.model+myLan.apply+myLan.error+"2");
//        infoWidget->delayShow();
//    }else{
//        quint8 mode = (quint8)data.nCommandData.at(0);
//        qDebug()<<mode;
//        if (mode == 1){
////            infoWidget->setLabelText("模型应用成功");
//            infoWidget->setLabelText(myLan.model+myLan.apply+myLan.succeed);

//            infoWidget->delayShow();
//            QSqlQuery sql_update1;
//            sql_update1.prepare("UPDATE modelInfo SET isApply = ? WHERE modelId = ?");
//            sql_update1.bindValue(0, "1");
//            sql_update1.bindValue(1, list.at(1));
//            bool result1 = sql_update1.exec();
//            QSqlQuery sql_update2;
//            sql_update2.prepare("UPDATE modelInfo SET isApply = ? WHERE modelId != ?");
//            sql_update2.bindValue(0, "0");
//            sql_update2.bindValue(1, list.at(1));
//            bool result2 = sql_update2.exec();
//            if(!result1 || !result2){
//                qDebug()<<"更新默认模型错误";
////                infoWidget->setLabelText("模型应用失败");
//                infoWidget->setLabelText(myLan.model+myLan.apply+myLan.error);

//                infoWidget->delayShow();
//            }

//        }else if(mode == 0){
////            infoWidget->setLabelText("模型应用失败");
//            infoWidget->setLabelText(myLan.model+myLan.apply+myLan.error);

//            infoWidget->delayShow();
//        }else{
////            infoWidget->setLabelText("模型应用返回值错误");
//            infoWidget->setLabelText(myLan.model+myLan.apply+myLan.setting_value_error);

//            infoWidget->delayShow();
//        }
//    }
//    myFlow.sleep(3);
//    updateModelListWidget();

//    infoWidget->hide();
    aiInfoWidget->setMsgHeightRatio(aiDeviceNum*2/3);
    aiInfoWidget->setLabelText(msg);
    myFlow.sleep(4);
    aiInfoWidget->hide();


}

void AisMainWidget::onAddModeBtnPressedSlt(){
    QString modelDirString =  QString(LOCAL_MODEL_PATH);
    QFileDialog dialog(pageModeUpdate);
    dialog.setWindowTitle("Open Json File");
    dialog.setDirectory(modelDirString);
    dialog.setNameFilter("Json文件 (*.json)");
    dialog.setFileMode(QFileDialog::ExistingFile); // 只允许选择已存在的文件
    dialog.setOption(QFileDialog::DontUseNativeDialog);  // 必须禁用原生对话框
//    dialog.setWindowFlags(dialog.windowFlags() | Qt::FramelessWindowHint);  // 无边框窗口

    // 设置自定义样式表
    dialog.setStyleSheet("QFileDialog {"
                         "    background-color: #f0f0f0;"
                         "    font-size :20px;"

                         "}"
                         "QListView {"
                         "    alternate-background-color: #e0e0e0;"
                         "    font-size :20px;"
//                         "    height :35px;"

                         "}"
                         "QListView::item  {"
                         "    height :30px;"
                         "}"
                         "QTreeView  {"
                         "    alternate-background-color: #e0e0e0;"
                         "    font-size :20px;"
//                         "    height :35px;"

                         "}"
                         "QTreeView::item  {"
                         "    height :30px;"
                         "}"
                         "QToolButton {"
                         "    padding: 6px;"
                         "    width :16px;"
                         "    height :16px;"

                         "}"
                         "QPushButton {"
                         "    background-color: #4a86e8;"
                         "    color: white;"
                         "    border-radius: 4px;"
                         "    padding: 6px;"
                         "    font-size :20px;"
                         "}");

    QString filePath;
    if (dialog.exec()) {
        filePath = dialog.selectedFiles()[0];
    }
    if(filePath.isEmpty()){
        //文件未选择
        return;
    }
    QString msg = QString("");

    int count = 0;
    for(int i=0; i<aiDeviceNum; i++){
        if(!uploaderList.at(i)->getFptStatus()){
            count++;
            msg += QString("aiDevice %1 ").arg(i+1) + myLan.ftp_status_error;
            if(i != (aiDeviceNum-1)){
               msg +=QString("\n");
            }
        }
    }
    if(count!=0){
        aiInfoWidget->setLabelText(msg);
        aiInfoWidget->setMsgHeightRatio(2);
        aiInfoWidget->delayShow();
        myFlow.sleep(4);
        aiInfoWidget->hide();
        return;
    }


    QString baseString = QFileInfo(filePath).baseName();

    QSqlQuery query;
    QSqlQuery sql_insert;
    query.prepare("SELECT modelId, modelName, isApply FROM modelInfo WHERE modelId = ?");
    query.bindValue(0, baseString);

    if(query.exec()){
        //新的模型
        if (!query.next()) {
            myMessageBox msgBox(MSG_QUES, myLan.add+myLan.model+"?");
            if (msgBox.exec() == QDialog::Accepted) {
                if(filePath.isEmpty()){
                    infoWidget->setLabelText(myLan.open+myLan.model+myLan.json_file+myLan.error);
//                    infoWidget->setLabelText("打开模型json失败");

                    infoWidget->delayShow();
                    myFlow.sleep(2);
                    infoWidget->hide();
                    return;
                }

                QFileInfo fileInfo(modelDirString + baseString+".dlc");
                if (!fileInfo.exists()) {
//                    infoWidget->setLabelText("模型dlc文件不存在");
                    infoWidget->setLabelText(myLan.model+myLan.dlc_file+myLan.notexist);

                    infoWidget->delayShow();
                    myFlow.sleep(2);
                    infoWidget->hide();
                    return;
                }

                //json文件解析
                QFile file(filePath);
                file.open(QFile::ReadOnly);
                QString json=  QString::fromUtf8(file.readAll());
                file.close();
                bool ok;
                QVariantMap resultMap = QJson::parse(json.toUtf8(), ok).toMap();
                if(!ok){
                     qDebug() << "json open failed";
//                     infoWidget->setLabelText("json文件内容错误");
                     infoWidget->setLabelText(myLan.json_file+myLan.content+myLan.error);

                     infoWidget->delayShow();
                     myFlow.sleep(2);
                     infoWidget->hide();
                }else{
                     qDebug() <<resultMap;
                }
                if(!resultMap.keys().contains("class")){
                    infoWidget->setLabelText(myLan.json_file+myLan.content+myLan.error);
                    infoWidget->delayShow();
                    myFlow.sleep(2);
                    infoWidget->hide();
                    return;
                }

                QString remoteFilePath = REMOTE_MODEL_PATH;

                QString localDlcString = modelDirString + baseString+".dlc";
                QString localJsonString = modelDirString + baseString+".json";



                int count1 = 0;
                for(int i=0; i<aiDeviceNum; i++){
                    if(!uploaderList.at(i)->getFptStatus()){
                        count1++;
                        msg += QString("aiDevice %1 ").arg(i+1) + myLan.ftp_status_error;
                        if(i != (aiDeviceNum-1)){
                           msg +=QString("\n");
                        }
                    }
                }
                if(count1!=0){
                    aiInfoWidget->setLabelText(msg);
                    aiInfoWidget->setMsgHeightRatio(2*aiDeviceNum/3);
                    aiInfoWidget->delayShow();
                    myFlow.sleep(4);
                    aiInfoWidget->hide();
                    return;
                }


                aiInfoWidget->setLabelText(myLan.model+myLan.upload +"...");
                aiInfoWidget->setMsgHeightRatio(2);
                aiInfoWidget->delayShow();
                msg.clear();
                int count2=0;
                for(int i=0; i<aiDeviceNum; i++){
                    if (uploaderList.at(i)->uploadFile(localDlcString, remoteFilePath+baseString+".dlc")) {
                        qDebug() << QString("aiDevice %1 ").arg(i+1)+"dlc文件上传成功";
                        if(uploaderList.at(i)->uploadFile(localJsonString, remoteFilePath+baseString+".json")){
                            count2++;
                            qDebug() << QString("aiDevice %1 ").arg(i+1)+"json文件上传成功";
                        }else{
                            qDebug() << QString("aiDevice %1 ").arg(i+1)+"json文件上传出现错误";
    //                        infoWidget->setLabelText("json文件上传出现错误");
                            msg+=QString("aiDevice %1 ").arg(i+1) + myLan.json_file+myLan.upload+myLan.error;
                            if(i != (aiDeviceNum-1)){
                               msg +=QString("\n");
                            }
                        }
                    } else {
                        qDebug() << QString("aiDevice %1 ").arg(i+1)+"dlc文件上传出现错误";
    //                    infoWidget->setLabelText("dlc文件上传出现错误");
                        msg+=QString("aiDevice %1 ").arg(i+1) + myLan.dlc_file+myLan.upload+myLan.error;
                        if(i != (aiDeviceNum-1)){
                           msg +=QString("\n");
                        }
                    }
                }
                if(count2 !=aiDeviceNum){
                    aiInfoWidget->setLabelText(msg);
                    aiInfoWidget->setMsgHeightRatio(2*aiDeviceNum/3);
                    aiInfoWidget->delayShow();
                    myFlow.sleep(4);
                    aiInfoWidget->hide();
                    return;
                }

                aiInfoWidget->setMsgHeightRatio(2);
                //新增modelInfo表
                sql_insert.prepare("insert into modelInfo(modelId, modelName, isApply,chgTime) values (?,?,?,?)");
                sql_insert.bindValue(0,baseString);
                sql_insert.bindValue(1,"模型");
                sql_insert.bindValue(2,"0");
                sql_insert.bindValue(3,QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                bool result = sql_insert.exec();
                if(result){
                    qDebug() << "insert modelInfo表 新模型成功";
                }else{
                    qDebug() << "insert modelInfo表 新模型失败";
//                    infoWidget->setLabelText("模型上传失败1");
                    infoWidget->setLabelText(myLan.model+myLan.upload+myLan.error+"1");

                }

                QList<QVariant> classList = resultMap["class"].toList();
//                qDebug() <<classList;
                bool result1;
                int  k,j;
                int nUnitAddr;
                for(int i=0 ; i<classList.size(); i++){
                    QSqlQuery sql_insert1;
                    for (j = 0; j < struCnfg.nLevelTotal; j++) {
                        for (k = 0; k < struCnfg.struLevelInfo[j].nIdentifyGroupTotal; k++) {
//                            nUnitAddr = struCnfg.struLevelInfo[j].nUnitId[k];
//                            qDebug() <<nUnitAddr;
                            sql_insert1.prepare("insert into modelParaInfo(modelId, id, zhName, levelTotal, identifyGroupTotal, threshold,chgTime,isApply) values (?,?,?,?,?,?,?,?)");
                            sql_insert1.bindValue(0,baseString);
                            sql_insert1.bindValue(1,classList[i].toMap()["id"].toString());
                            sql_insert1.bindValue(2,classList[i].toMap()["name"].toString());
                            sql_insert1.bindValue(3,j);
                            sql_insert1.bindValue(4,k);
                            sql_insert1.bindValue(5,classList[i].toMap()["threshold"].toString());
                            sql_insert1.bindValue(6,QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                            sql_insert1.bindValue(7,"1");
                            result1 = sql_insert1.exec();
                            if(!result1){
                                result = false;
                                qDebug() << "insert modelParaInfo表 模型参数失败";
                                break;
                            }else{
                                qDebug() << "insert modelParaInfo表 模型参数成功";

                            }

                        }
                    }
                }
                if(result){
                    aiInfoWidget->setLabelText(myLan.model+myLan.upload+myLan.succeed);
                }else{
                    aiInfoWidget->setLabelText(myLan.model+myLan.upload+myLan.error+"2");
                }
                updateModelListWidget();

                aiInfoWidget->delayShow();
                myFlow.sleep(2);
                aiInfoWidget->hide();

            }else{
                infoWidget->delayShow();
                myFlow.sleep(3);
                infoWidget->hide();
                return;
            }
        }else{
           //旧的模型
           myMessageBox msgBox(MSG_INFO, myLan.model_exist_msg);
           msgBox.exec();
           QString remoteFilePath =  REMOTE_MODEL_PATH;

           QString localDlcString = modelDirString + baseString+".dlc";
           QString localJsonString = modelDirString + baseString+".json";

           int count = 0;
           for(int i=0; i<aiDeviceNum; i++){
               if(!uploaderList.at(i)->getFptStatus()){
                   count++;
                   msg += QString("aiDevice %1 ").arg(i+1) + myLan.ftp_status_error;
                   if(i != (aiDeviceNum-1)){
                      msg +=QString("\n");
                   }
               }
           }
           if(count!=0){
               aiInfoWidget->setLabelText(msg);
               aiInfoWidget->setMsgHeightRatio(2*aiDeviceNum/3);
               aiInfoWidget->delayShow();
               myFlow.sleep(4);
               aiInfoWidget->hide();
               return;
           }

//           if(!uploader->getFptStatus()){
//               infoWidget->setLabelText(myLan.ftp_status_error);
//               infoWidget->delayShow();
//               myFlow.sleep(2);
//               infoWidget->hide();
//               return;
//           }
           aiInfoWidget->setLabelText(myLan.model+myLan.upload +"...");
           aiInfoWidget->setMsgHeightRatio(2);
           aiInfoWidget->delayShow();


           msg.clear();
           int count2=0;
           for(int i=0; i<aiDeviceNum; i++){
               if (uploaderList.at(i)->uploadFile(localDlcString, remoteFilePath+baseString+".dlc")) {
                   qDebug() << QString("aiDevice %1 ").arg(i+1)+"dlc文件上传成功";
                   if(uploaderList.at(i)->uploadFile(localJsonString, remoteFilePath+baseString+".json")){
                       count2++;
                       qDebug() << QString("aiDevice %1 ").arg(i+1)+"json文件上传成功";
                   }else{
                       qDebug() << QString("aiDevice %1 ").arg(i+1)+"json文件上传出现错误";
//                        infoWidget->setLabelText("json文件上传出现错误");
                       msg+=QString("aiDevice %1 ").arg(i+1) + myLan.json_file+myLan.upload+myLan.error;
                       if(i != (aiDeviceNum-1)){
                          msg +=QString("\n");
                       }
//                            infoWidget->setLabelText(myLan.json_file+myLan.upload+myLan.error);

//                            infoWidget->delayShow();
//                            myFlow.sleep(2);
//                            infoWidget->hide();
//                            return;
                   }
               } else {
                   qDebug() << QString("aiDevice %1 ").arg(i+1)+"dlc文件上传出现错误";
//                    infoWidget->setLabelText("dlc文件上传出现错误");
                   msg+=QString("aiDevice %1 ").arg(i+1) + myLan.dlc_file+myLan.upload+myLan.error;
                   if(i != (aiDeviceNum-1)){
                      msg +=QString("\n");
                   }
//                        infoWidget->setLabelText(myLan.dlc_file+myLan.upload+myLan.error);

//                        infoWidget->delayShow();
//                        myFlow.sleep(2);
//                        infoWidget->hide();
//                        return;
               }
           }
           if(count2 !=aiDeviceNum){
               aiInfoWidget->setLabelText(msg);
               aiInfoWidget->setMsgHeightRatio(2*aiDeviceNum/3);
               aiInfoWidget->delayShow();
               myFlow.sleep(4);
               aiInfoWidget->hide();
               return;
           }else{
               aiInfoWidget->setMsgHeightRatio(2);
               aiInfoWidget->setLabelText(myLan.model+myLan.upload+myLan.succeed);
               updateModelListWidget();
               aiInfoWidget->delayShow();
               myFlow.sleep(4);
               aiInfoWidget->hide();
           }


//           if (uploader->uploadFile(localDlcString, remoteFilePath+baseString+".dlc")) {
//               qDebug() << "dlc文件上传成功";
//               if(uploader->uploadFile(localJsonString, remoteFilePath+baseString+".json")){
//                   qDebug() << "json文件上传成功";
//                   infoWidget->setLabelText(myLan.model+myLan.upload+myLan.succeed);
//                   updateModelListWidget();
//                   infoWidget->delayShow();
//                   myFlow.sleep(2);
//                   infoWidget->hide();
//               }else{
//                   qDebug() << "json文件上传出现错误";
//                   infoWidget->setLabelText(myLan.json_file+myLan.upload+myLan.error);
//                   infoWidget->delayShow();
//                   myFlow.sleep(2);
//                   infoWidget->hide();

//               }
//           } else {
//               qDebug() << "dlc文件上传出现错误";
//               infoWidget->setLabelText(myLan.dlc_file+myLan.upload+myLan.error);
//               infoWidget->delayShow();
//               myFlow.sleep(2);
//               infoWidget->hide();

//           }
        }
    }

//    int count = 0;
//    int currentIndex = 0;
//    QSqlQuery query2;
//    if (!query2.exec("SELECT modelId, modelName, isApply FROM modelInfo order by chgTime desc")) {
//        qDebug() << "查询失败：" << query.lastError();
//    } else {
//        while (query.next()) {
//            QString modelId = query.value(0).toString();
//            QString modelName = query.value(1).toString();
//            int isApply =  query.value(2).toInt();
//            qDebug() << modelId << modelName<<isApply;
//            count ++
//            if(baseString.compare(modelId)){
//                currentIndex = count-1;
//            }
//        }
//        qDebug() << currentIndex;
//        modelListWidget->setCurrentRow(currentIndex);
//    }

}



void AisMainWidget::updateModelListWidget(){
    modelListWidget->clear();
    defaultModelListWidget = 0;
    int count = 0;
    QSqlQuery query;
    if (!query.exec("SELECT modelId, modelName, isApply FROM modelInfo order by chgTime desc")) {
        qDebug() << "查询失败：" << query.lastError();
    } else {
        while (query.next()) {
            QString modelId = query.value(0).toString();
            QString modelName = query.value(1).toString();
            int isApply =  query.value(2).toInt();
            qDebug() << modelId << modelName<<isApply;

            myListWidgetItem *modelListItem = new myListWidgetItem(modelName+"-"+modelId, QIcon(), QSize(602,50));
            modelListItem->setTextAlignment(Qt::AlignCenter);
            QPixmap pixmap(":/res/png/material.png");
            pixmap = pixmap.scaled(QSize(605,50),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            modelListItem->setBackground(QBrush(pixmap));
            modelListWidget->addItem(modelListItem);
            modelListWidget->setSpacing(1);
            count ++;
//            if(isApply == 1){
//                defaultModelListWidget = count-1;
//                defaultModelId = modelId;
//                continue;
//            }
        }
//        qDebug() << defaultModelListWidget << defaultModelId;

//        modelListWidget->setCurrentRow(defaultModelListWidget);
    }

}

void AisMainWidget::CreatePageVersion(){
    versionInfo = "";
    QHBoxLayout *versionInfoHLayout[MAX_AI_DEVICE_NUM];
    QVBoxLayout *versionInfoMainLayout = new QVBoxLayout(pageVersion);

    for(int i=0; i<MAX_AI_DEVICE_NUM; i++){
        versionLbl[i] = new myLabel(QString("aiDevice %1 ").arg(i+1)+myLan.ai_version+" : ", pageVersion);
        versionInfoLbl[i] = new myLabel(versionInfo, pageVersion);
        versionInfoHLayout[i] = new QHBoxLayout();
        versionInfoHLayout[i]->addWidget(versionLbl[i]);
        versionInfoHLayout[i]->addWidget(versionInfoLbl[i]);
        versionInfoHLayout[i]->addSpacing(300);
        versionInfoMainLayout->addLayout(versionInfoHLayout[i]);
        if(i >= aiDeviceNum){
            versionLbl[i]->hide();
            versionInfoLbl[i]->hide();
        }
    }

}
void AisMainWidget::CreatePageDiskValid(){
    diskValidInfo = "";
    QHBoxLayout *diskValidHLayout[MAX_AI_DEVICE_NUM];
    QVBoxLayout *diskValidMainLayout = new QVBoxLayout(pageDiskValid);

    for(int i=0; i<MAX_AI_DEVICE_NUM; i++){
        diskValidLbl[i] = new myLabel(QString("aiDevice %1 ").arg(i+1)+myLan.disk_space+" : ", pageDiskValid);
        diskValidLblInfoLbl[i] = new myLabel(diskValidInfo, pageDiskValid);
        diskValidHLayout[i] = new QHBoxLayout();
        diskValidHLayout[i]->addWidget(diskValidLbl[i]);
        diskValidHLayout[i]->addWidget(diskValidLblInfoLbl[i]);
        diskValidHLayout[i]->addSpacing(300);
        diskValidMainLayout->addLayout(diskValidHLayout[i]);
        if(i >= aiDeviceNum){
            diskValidLbl[i]->hide();
            diskValidLblInfoLbl[i]->hide();
        }
    }
}

void AisMainWidget::CreatePageEjectTest(){
    bFlagLoop  = false;
    nDisplayNum = 0;
    nMaxEjector = struCnfg.nEjectorsPerChute*struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2-1;
    ejectTestInterval = 300;
    ejectTestInterval2 = 100;
    ejectTestIntervalTotal = 1000;

    // 线程创建
    threadStart = new AITestEjectThread;

    ejectTestStartBtn = new myPushButton(myLan.start, QIcon(""));
    ejectTestEndBtn = new myPushButton(myLan.stop, QIcon(""));
    ejectTestStartBtn->setFixedSize(QSize(BTN_WIDTH+20,BTN_HEIGHT));
    ejectTestEndBtn->setFixedSize(QSize(BTN_WIDTH+20,BTN_HEIGHT));

    ejectorTestNumLbl = new myLabel(myLan.spray_valve_number, pageEjectTest);
//    ejectorTestNumLbl = new myLabel("喷阀编号", pageEjectTest);

    ejectorTestNumEdit = new myLineEdit(QString("%1").arg(0), pageEjectTest);

    ejectorTestNumEdit->setReadOnly(true);
    ejectorTestNumEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    nozzleTestNumLbl = new myLabel(myLan.nozzle_number, pageEjectTest);
//    nozzleTestNumLbl = new myLabel("喷嘴编号", pageEjectTest);

    nozzleTestNumEdit = new myLineEdit(QString("%1").arg(nDisplayNum), pageEjectTest);

    nozzleTestNumEdit->setReadOnly(true);
    nozzleTestNumEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

//    ejectorTestDelayLbl = new myLabel("喷阀间隔(ms)", pageEjectTest);
    ejectorTestDelayLbl = new myLabel(myLan.spray_time_interval+"(ms)", pageEjectTest);

    ejectorTestDelayEdit = new myLineEdit(QString("%1").arg(ejectTestIntervalTotal), pageEjectTest);

    ejectorTestDelayEdit->setReadOnly(true);
    ejectorTestDelayEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    loopCbx = new myCustomCheckBox(myLan.ejector_keep_loop, false,CB_STYLE_APPLY);

    QHBoxLayout * ejectorNumHLayout = new QHBoxLayout();
    ejectorNumHLayout->addWidget(ejectorTestNumLbl);
    ejectorNumHLayout->addWidget(ejectorTestNumEdit);
    ejectorNumHLayout->addSpacing(200);

    QHBoxLayout * nozzleNumHLayout = new QHBoxLayout();
    nozzleNumHLayout->addWidget(nozzleTestNumLbl);
    nozzleNumHLayout->addWidget(nozzleTestNumEdit);
    nozzleNumHLayout->addSpacing(200);

    QHBoxLayout * ejectorTestDelayHLayout = new QHBoxLayout();
    ejectorTestDelayHLayout->addWidget(ejectorTestDelayLbl);
    ejectorTestDelayHLayout->addWidget(ejectorTestDelayEdit);
    ejectorTestDelayHLayout->addSpacing(200);

    QHBoxLayout * ejectorTestOperaHLayout = new QHBoxLayout();
    ejectorTestOperaHLayout->addWidget(ejectTestStartBtn);
    ejectorTestOperaHLayout->addWidget(ejectTestEndBtn);

   QVBoxLayout *pageEjectTestVLayout = new QVBoxLayout(pageEjectTest);
   pageEjectTestVLayout->addLayout(ejectorNumHLayout);
   pageEjectTestVLayout->addLayout(nozzleNumHLayout);
   pageEjectTestVLayout->addLayout(ejectorTestDelayHLayout);
   pageEjectTestVLayout->addWidget(loopCbx);
   pageEjectTestVLayout->addLayout(ejectorTestOperaHLayout);
   pageEjectTestVLayout->addSpacing(50);
//   pageEjectTestVLayout->addStretch();

   connect(ejectorTestNumEdit, SIGNAL(pressed()), this, SLOT(onEjectorTestNumEditPressedSlt()));
   connect(nozzleTestNumEdit, SIGNAL(pressed()), this, SLOT(onNozzleTestNumEditPressedSlt()));
   connect(ejectorTestDelayEdit, SIGNAL(pressed()), this, SLOT(onEjectorTestDelayEditPressedSlt()));
   connect(ejectTestStartBtn, SIGNAL(pressed()), this, SLOT(onEjectTestStartBtnPressedSlt()));
   connect(ejectTestEndBtn, SIGNAL(pressed()), this, SLOT(onEjectTestEndBtnPressedSlt()));
   connect(loopCbx             ,SIGNAL(pressed())          ,this,   SLOT(onLoopCbxClickedSlt()));
   connect(threadStart         ,SIGNAL(ejectorChangeSig()) ,this,   SLOT(ejectPageThreadStartSlt()));
   connect(this                ,SIGNAL(ejectTestDonesig())    ,threadStart,   SLOT(ejectTestDoneSlt()));


}


void AisMainWidget::ejectPageThreadStartSlt(){
     if (threadStart->isRunning()) {
         ejectorTestNumEdit->setText("0");
         nozzleTestNumEdit->setText(QString("%1").arg(nDisplayNum));
         QByteArray args;
         AI_Data_Protocol_D data;
//         args[0] = ejectorTestNumEdit->text().toInt();
         int ejectorIndex = nDisplayNum/struCnfg.nEjectorsPerChute;
         args[0] = 0;
         args[1] = nDisplayNum%struCnfg.nEjectorsPerChute;
         args[2] = (ejectorTestDelayEdit->text().toInt())/256;
         args[3] = (ejectorTestDelayEdit->text().toInt())%256;
         MyUpd.writeDatagram(CMD_AI_EJECTOR_FIXED_BGN, ejectorIndex, 4, args, addressList.at(ejectorIndex), AI_UDP_SEND_PORT);
         myFlow.msleep(100);
         MyUpd.readUdpDatagrams(&data,13);
//         if(MyUpd.readUdpDatagrams(&data,13) == -1){
//             threadStart->stop();
//             threadStart->wait();
//             infoWidget->setLabelText(QString("%1%2").arg(nDisplayNum).arg("喷阀自检启动失败1"));
//             infoWidget->delayShow();
//             myFlow.msleep(1000);

//         }else{
//             quint8 mode = (quint8)data.nCommandData.at(0);
//             if (mode == 1){
////                 infoWidget->delayShow();
//             }else if(mode == 0){
//                 threadStart->stop();
//                 threadStart->wait();
//                 infoWidget->setLabelText(QString("%1%2").arg(nDisplayNum).arg("喷阀自检启动失败"));
//                 infoWidget->delayShow();
//                 myFlow.msleep(1000);

//             }else{
//                 threadStart->stop();
//                 threadStart->wait();
//                 infoWidget->setLabelText(QString("%1%2").arg(nDisplayNum).arg("喷阀自检启动失败1"));
//                 infoWidget->delayShow();
//                 myFlow.msleep(1000);

//             }
//         }
//         infoWidget->hide();
         args.clear();
         myFlow.msleep(400);
         args[0] = 0;
//         args[0] = nDisplayNum/struCnfg.nEjectorsPerChute;
         args[1] = nDisplayNum%struCnfg.nEjectorsPerChute;

//         args[0] = ejectorTestNumEdit->text().toInt();
//         args[1] = nDisplayNum;
         MyUpd.writeDatagram(CMD_AI_EJECTOR_FIXED_END, ejectorIndex, 2, args, addressList.at(ejectorIndex), AI_UDP_SEND_PORT);
         myFlow.msleep(100);
         MyUpd.readUdpDatagrams(&data,13);
//         if(MyUpd.readUdpDatagrams(&data,13) == -1){
//           threadStart->stop();
//           threadStart->wait();
//           infoWidget->setLabelText(QString("%1%2").arg(nDisplayNum).arg("喷阀自检停止失败1"));
//           infoWidget->delayShow();
//           myFlow.msleep(1000);

//         }else{
//             quint8 mode = (quint8)data.nCommandData.at(0);
//             if (mode == 1){
////                 infoWidget->delayShow();
//             }else if(mode == 0){
//                 threadStart->stop();
//                 threadStart->wait();
//                 infoWidget->setLabelText(QString("%1%2").arg(nDisplayNum).arg("喷阀自检停止失败"));
//                 infoWidget->delayShow();
//                 myFlow.msleep(1000);

//             }else{
//                 threadStart->stop();
//                 threadStart->wait();
//                 infoWidget->setLabelText(QString("%1%2").arg(nDisplayNum).arg("喷阀自检停止失败1"));
//                 infoWidget->delayShow();
//                 myFlow.msleep(1000);
//             }
//         }
//         infoWidget->hide();

         myFlow.msleep(400);

         if (!bFlagLoop) {
           nDisplayNum++;
         }
         if (nDisplayNum > nMaxEjector) {
             nDisplayNum = nMaxEjector;
             if (!bFlagLoop) {
                 ejectTestStartBtn->setEnabled(true);
                 ejectTestEndBtn->setEnabled(false);
                 threadStart->stop();
                 threadStart->wait();
                 nDisplayNum = 0;
                 nozzleTestNumEdit->setText(QString("%1").arg(nDisplayNum));
                 ejectorTestNumEdit->setText(QString("%1").arg(nDisplayNum));

             }
         }
     }
     emit ejectTestDonesig();

}

void AisMainWidget::onLoopCbxClickedSlt()
{
    bFlagLoop = !bFlagLoop;
}

void AisMainWidget::onEjectorTestNumEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2, ejectorTestNumEdit->text().toInt());
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        QString str = QString("%1").arg(inputDlg.getValue());
        ejectorTestNumEdit->setText(str);
    }
}

void AisMainWidget::onNozzleTestNumEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, nMaxEjector, nozzleTestNumEdit->text().toInt());
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        QString str = QString("%1").arg(inputDlg.getValue());
        nDisplayNum = inputDlg.getValue();
        nozzleTestNumEdit->setText(str);
    }
}

void AisMainWidget::onEjectorTestDelayEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, ejectorTestDelayEdit->text().toInt());
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        QString str = QString("%1").arg(inputDlg.getValue());
        ejectorTestDelayEdit->setText(str);
        threadStart->setDelayTime(inputDlg.getValue());
    }
}

void AisMainWidget::onEjectTestStartBtnPressedSlt(){
    if (threadStart->isRunning()) {
        threadStart->stop();
        ejectTestStartBtn->setEnabled(true);
        ejectTestEndBtn->setEnabled(false);

    } else {
        threadStart->start();
        ejectTestStartBtn->setEnabled(false);
        ejectTestEndBtn->setEnabled(true);
    }

//    QByteArray args;
//    AI_Data_Protocol_D data;
//    isFixedTest = !isFixedTest;
//    if(!isFixedTest){

//        args[0] = ejectorTestNumEdit->text().toInt();
//        args[1] = nozzleTestNumEdit->text().toInt();
//        args[2] = (ejectorTestDelayEdit->text().toInt())/256;
//        args[3] = (ejectorTestDelayEdit->text().toInt())%256;

//        MyUpd.writeDatagram(CMD_AI_EJECTOR_FIXED_BGN, 0, 4, args, address, AI_UDP_SEND_PORT);
//        myFlow.sleep(1);

//        if(MyUpd.readUdpDatagrams(&data,13) == -1){
//            infoWidget->setLabelText("定检启动失败1");
//            infoWidget->delayShow();

//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<"mode:"<<mode;
//            if (mode == 1){
//                infoWidget->setLabelText("定检启动成功");
//                isFixedTest = true;
//                fixedTestBtn->setText("定检停止");
//                orderTestBtn->setEnabled(false);

//                infoWidget->delayShow();
//            }else if(mode == 0){
//                infoWidget->setLabelText("定检启动失败");
//                infoWidget->delayShow();

//            }else{
//                infoWidget->setLabelText("定检启动返回值错误");
//                infoWidget->delayShow();
//            }
//        }
//        myFlow.sleep(3);
//        infoWidget->hide();
//    }else{
//        MyUpd.writeDatagram(CMD_AI_EJECTOR_FIXED_END, 0, 0, args, address, AI_UDP_SEND_PORT);
//        myFlow.sleep(1);

//        if(MyUpd.readUdpDatagrams(&data,13) == -1){
//            orderTestBtn->setEnabled(false);

//            infoWidget->setLabelText("定检停止失败1");
//            infoWidget->delayShow();
//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<"mode:"<<mode;
//            if (mode == 1){
//                infoWidget->setLabelText("定检停止成功");
//                isFixedTest =  false;
//                fixedTestBtn->setText("定检开始");
//                orderTestBtn->setEnabled(true);
//                infoWidget->delayShow();
//            }else if(mode == 0){
//                orderTestBtn->setEnabled(false);

//                infoWidget->setLabelText("定检停止失败");
//                infoWidget->delayShow();
//            }else{
//                orderTestBtn->setEnabled(false);

//                infoWidget->setLabelText("定检停止返回值错误");
//                infoWidget->delayShow();
//            }
//        }
//        myFlow.sleep(3);
//        infoWidget->hide();
//    }

}
void AisMainWidget::onEjectTestEndBtnPressedSlt(){
    if (threadStart->isRunning()) {
        threadStart->stop();
        ejectTestStartBtn->setEnabled(true);
        ejectTestEndBtn->setEnabled(false);
    } else {
        threadStart->start();
        ejectTestStartBtn->setEnabled(false);
        ejectTestEndBtn->setEnabled(true);
    }
//    QByteArray args;
//    AI_Data_Protocol_D data;
//    isOrderTest = !isOrderTest;

//    if(!isOrderTest){
//        args[0] = ejectorTestNumEdit->text().toInt();
//        args[1] = nozzleTestNumEdit->text().toInt();
//        args[2] = ejectorTestDelayEdit->text().toInt()/256;
//        args[3] = ejectorTestDelayEdit->text().toInt()%256;

//        MyUpd.writeDatagram(CMD_AI_EJECTOR_ORDER_BGN, 0, 4, args, address, AI_UDP_SEND_PORT);
//        myFlow.sleep(1);

//        if(MyUpd.readUdpDatagrams(&data,13) == -1){
//            infoWidget->setLabelText("顺检启动失败1");
//            infoWidget->delayShow();

//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<"mode:"<<mode;
//            if (mode == 1){
//                infoWidget->setLabelText("顺检启动成功");
//                orderTestBtn->setText("顺检停止");
//                fixedTestBtn->setEnabled(false);
//                isOrderTest = true;
//                infoWidget->delayShow();
//            }else if(mode == 0){
//                infoWidget->setLabelText("顺检启动失败");
//                infoWidget->delayShow();
//            }else{
//                infoWidget->setLabelText("顺检启动返回值错误");
//                infoWidget->delayShow();
//            }
//        }
//        myFlow.sleep(3);
//        infoWidget->hide();
//    }else{
//        MyUpd.writeDatagram(CMD_AI_EJECTOR_ORDER_END, 0, 0, args, address, AI_UDP_SEND_PORT);
//        myFlow.sleep(1);

//        if(MyUpd.readUdpDatagrams(&data,13) == -1){

//            infoWidget->setLabelText("顺检停止失败1");
//            infoWidget->delayShow();
//        }else{
//            quint8 mode = (quint8)data.nCommandData.at(0);
//            qDebug()<<"mode:"<<mode;
//            if (mode == 1){
//                infoWidget->setLabelText("顺检停止成功");
//                orderTestBtn->setText("顺检开始");
//                isOrderTest = false;
//                fixedTestBtn->setEnabled(true);
//                infoWidget->delayShow();
//            }else if(mode == 0){
//                infoWidget->setLabelText("顺检停止失败");
//                infoWidget->delayShow();
//            }else{
//                infoWidget->setLabelText("顺检停止返回值错误");
//                infoWidget->delayShow();
//            }
//        }
//        myFlow.sleep(3);
//        infoWidget->hide();
//    }
}

void AisMainWidget::onListWidgetRowChangedSlt(int nIndex)
{
    stackedWidget->setCurrentIndex(nIndex);
    switch(stackedWidget->currentIndex()){
    case AI_PAGE_MODEL_PARA:
        updateModeParaInfo();
        break;
    case AI_PAGE_MODEL_UPDATE:
        updateModelListWidget();
        break;
    default:
        break;
    }
}


AisMainWidget::~AisMainWidget()
{

}
FtpUploader :: FtpUploader(const QString &host, quint16 port, const QString &user, const QString &password)
    : ftp(new QFtp(this)), eventLoop(new QEventLoop(this)) {
    this->user = user;
    this->password = password;
    this->host = host;
    this->port = port;
    timer = new QTimer(this);
    ftpStatus = false;
    timer->start(20000);
    timer->setSingleShot(false);
    ftp->connectToHost(host, port);
    stopped = false;
    ftp->setTransferMode(QFtp::Active);
    ftp->login(user, password);
    connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(onCommandFinished(int ,bool)));
    connect(timer, SIGNAL(timeout()), this, SLOT(onConnectTimeout()));
    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(onFTPListInfo(const QUrlInfo &)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(onDataTransferProgress(qint64, qint64)));

}

FtpUploader :: ~FtpUploader() {
    ftp->close();
}


bool FtpUploader::downloadDirectory(const QString &remoteDir, const QString &localDir){
    // 保存远程和本地目录信息
    this->remoteDir = remoteDir;
    this->localDir = localDir;
    filesPath.clear();
    openedFiles.clear();
    downloadQueue.clear();
    totalFiles = 0;
    transferredFiles = 0;
    finalTotalFiles = 0;
    stopped = false;
    isPutProcess = false;
    // 连接到 FTP 服务器的信号和槽
//    connect(ftp, SIGNAL(done(bool)), this, SLOT(onFTPDone(bool)));
//    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(onFTPListInfo(const QUrlInfo &)));
//    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(onDataTransferProgress(qint64, qint64)));
//    ftp->connectToHost(this->host, this->port);
    QString ftpDir(REMOTE_IMG_PATH);
    ftpDir.chop(1);
    ftp->setTransferMode(QFtp::Active);
    ftp->login(this->user, this->password);
    if(!ftpStatus){
        return false;
    }
    ftp->cd(ftpDir);
    ftp->list();
    return true;
}

// 文件下载进度的槽函数
void FtpUploader::onDataTransferProgress(qint64 bytesRead, qint64 totalBytes){
     if (ftp->currentCommand() == QFtp::Get){
         qDebug() << "已下载" << bytesRead << "字节，总共" << totalBytes << "字节";
         if (bytesRead >= totalBytes) {
//             myFlow.msleep(100);
             if(localFile->isOpen()){
                  localFile->close();
             }
//             myFlow.msleep(200);
             isPutProcess = false;

             transferredFiles++;
             if (transferredFiles >= finalTotalFiles) {
                 qDebug() << "目录下载操作完成";
                 // 在这里可以进行获取目录完成后的操作
                 emit downloadFinished();
             }
         }else{
             isPutProcess =true;
         }
     }
}

void FtpUploader::stop()
{
    stopped =true;
    if(localFile->isOpen()){
         localFile->close();

    }
}


void FtpUploader::run(){
    stopped = false;
    isPutProcess = false;
    int i = 0;
    while (!stopped) {
         if (!downloadQueue.isEmpty()&& !isPutProcess &&(finalTotalFiles == totalFiles)) {
              DownloadTask task = downloadQueue.takeFirst();
              localFile = new QFile(task.localFilePath);
              i++;
              try {
                  if (!localFile->open((QIODevice::WriteOnly)))
                  {
                      qDebug() << "文件打开异常:" << task.localFilePath<<localFile->errorString();

                      if(localFile->isOpen()){
                           localFile->close();
                      }
                      continue;
                  }
                  ftp->get(task.infoName,localFile);
                  qDebug() << "下载文件序号："<<i;
              } catch (const std::ios_base::failure& e) {

                  if(localFile->isOpen()){
                       localFile->close();
                  }
                   std::cerr << "I/O操作失败: " << e.what() << std::endl;
                   continue;
              }
            isPutProcess =true;
         }
    }
}

// 获取到 FTP 文件信息的槽函数
void FtpUploader::onFTPListInfo(const QUrlInfo &info){
    QString remoteFilePath = remoteDir  + info.name();
    QString localFilePath = localDir + info.name();
    if (info.isFile()){
        totalFiles++;
//        qDebug() << "remoteFilePath: "<<remoteFilePath;

      DownloadTask task1;
      task1.infoName = info.name();
      task1.localFilePath = localFilePath;
      downloadQueue.append(task1);

       // 将打开的文件指针保存起来，以便后删除处理
//            openedFiles.append(localFile);
       filesPath.append(remoteFilePath);
    }else if (info.isDir() && info.name()!= "." && info.name()!= ".."){
        // 如果是目录，创建本地目录，并递归下载
        QDir local(localDir);
        local.mkdir(info.name());
        QString newRemoteDir = remoteFilePath;
        QString newLocalDir = localFilePath;
        // 递归调用下载函数
        downloadDirectory(newRemoteDir, newLocalDir);
    }
}


void FtpUploader :: deleteDir(){
    if(filesPath.size() <=0){
        qDebug() << "原图像文件为空"<<endl;
    }
    for (int i =0 ; i<filesPath.size(); i++) {
        qDebug() << (filesPath.at(i));
        ftp->remove(filesPath.at(i));
        if(i == filesPath.size()-1){
            emit removeFinished();
        }
    }
}

bool FtpUploader :: uploadFile(const QString &localFilePath, const QString &remoteFilePath) {
    QFile file(localFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件：" << localFilePath;
        return false;
    }
//    ftp->connectToHost(this->host, this->port);
    ftp->setTransferMode(QFtp::Active);
    ftp->login(this->user, this->password);
    ftp->put(&file, remoteFilePath);
    file.close();
    eventLoop->exec(); // 等待上传完成

    if (ftp->error() == QFtp::NoError) {
        qDebug() << "文件上传成功：" << remoteFilePath;
        return true;
    } else {
        qDebug() << "文件上传失败：" << ftp->errorString();
        return false;
    }
}

void  FtpUploader :: onCommandFinished(int id, bool error) {
    if (error) {
        qDebug() << "命令失败：" << ftp->errorString();
        ftpStatus = false;
        return;
    } else {
        qDebug() << "命令成功";
        ftpStatus =true;
    }
    if(ftp->currentCommand() == QFtp::Put){
        ftpStatus =true;
        qDebug() << "上传成功";
        eventLoop->quit(); // 完成上传
    }

    if(ftp->currentCommand() == QFtp::Get){
        ftpStatus =true;
        qDebug() << "下载成功";
    }

    if(ftp->currentCommand() == QFtp::List){
        ftpStatus =true;
        finalTotalFiles = totalFiles;
        if(filesPath.size() == 0){
            emit blankDirListFinished();
            return;
        }
        qDebug() << "list成功,文件数量: "<<finalTotalFiles;
    }

//    timer->stop();
}


QString FtpUploader::getDownLoadPercent(){
    if(finalTotalFiles == 0){
        return "0%";
    }
    if(transferredFiles >= finalTotalFiles){
        return "100%";
    }
    double progress = (double)transferredFiles / finalTotalFiles;  // 计算进度的小数表示
    QString percentageStr = QString::number(progress * 100, 'f', 2) + "%";  // 转换为百分比字符串，保留2位小数并添加%符号
    qDebug() << "进度百分比: " << percentageStr;
    return percentageStr;
}


void FtpUploader::onConnectTimeout(){
       ftp->rawCommand("NOOP");
        // 超时处理，比如输出提示信息或者进行其他清理操作
//      if(ftpStatus){
//          qDebug() << "Connect to FTP server "+this->host+" .";

//      }else{
//          qDebug() << "Connect to FTP server "+this->host+" timed out.";
//          // 可以在这里进行一些清理工作，比如关闭连接等操作
//          // ftp->abort();
//      }

}
bool FtpUploader:: getFptStatus(){
    return ftpStatus;
}


/**
 * 创建喷阀自检线程响应函数
 */
AITestEjectThread::AITestEjectThread()
{
    stopped = false;
    delayTime = 1000;
}

void AITestEjectThread::run()
{
    emit ejectorChangeSig();
    stopped = false;
    while(!stopped){
        myFlow.msleep(delayTime);
    }
}
void AITestEjectThread::ejectTestDoneSlt()
{
    if (!stopped)
    {
        emit ejectorChangeSig();
    }
}

void AITestEjectThread::stop()
{
    stopped =true;
}

void AITestEjectThread:: setDelayTime(int delayTime){
    this->delayTime = delayTime;
}

