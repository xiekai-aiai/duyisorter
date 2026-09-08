#ifndef AISMAINWIDGET_H
#define AISMAINWIDGET_H

//#include <QWebView>
#include <QWidget>
#include <QStackedWidget>
#include <QBitmap>
#include <QPainter>
#include <QBrush>
#include <QPixmap>
#include <QtGui>
#include <QFileDialog>
#include <QSqlError>
#include <QSqlQuery>
#include <QFtp>
#include <QList>
#include <exception>
#include <iostream>

#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/myinputpanel.h"
#include "common/mycustomcheckbox.h"
#include "common/myinputmethod.h"
#include "aicommunicate.h"
#include "mylogger.h"
#include "globalflow.h"
#include "mymessagebox.h"

using QJson::JsonObject;
using QJson::JsonArray;

class Qtimer;

struct MODEL_PARA_INFO2 {
    QString id;        // id
    QString name;      // name
    QString threshold; // threshold
    QString isApply;   // isApply
};
struct DownloadTask {
       QString infoName;
       QString localFilePath;
   };

enum {
    AI_PAGE_PIXEL,          //像元信息界面
    AI_PAGE_EJECTOR,        //喷阀参数
    AI_PAGE_IMG_HEIGHT,     //图像高度
    AI_PAGE_MODEL_PARA,     //模型参数
    AI_PAGE_MODEL_UPDATE,   //模型升级
    AI_PAGE_VERSION,        //版本查询
    AI_DISK_VALID,           //磁盘空间
    AI_DISK_EJECTOR_TEST     //磁盘空间

};

class FtpUploader : public QThread {
    Q_OBJECT
public:
    FtpUploader(const QString &host, quint16 port, const QString &user, const QString &password);
    ~FtpUploader();

     bool uploadFile(const QString &localFilePath, const QString &remoteFilePath);
     bool downloadDirectory(const QString &remoteDir, const QString &localDir);
     void deleteDir();
     bool getFptStatus();
     QString getDownLoadPercent();
     void stop();

private slots:
    void onCommandFinished(int id, bool error);
    void onConnectTimeout();
    void onDataTransferProgress(qint64 bytesRead, qint64 totalBytes);
    void onFTPListInfo(const QUrlInfo &info);

signals:
    // 下载完成的信号
    void downloadFinished();
    // 空文件夹list的信号
    void blankDirListFinished();
    // 删除完成的信号
    void removeFinished();
protected:
    void run();
private:
    QFtp *ftp;
    QEventLoop *eventLoop;
    QTimer *timer;
    bool ftpStatus;
    QString remoteDir;
    QString localDir;
    QList<QFile *> openedFiles;
    QList<QString> filesPath;
    int totalFiles;
    int finalTotalFiles;
    int transferredFiles;
    QFile *localFile;
    QList<DownloadTask> downloadQueue;
    volatile bool stopped;
    volatile bool isPutProcess;
    QString user;
    QString password;
    QString host;
    quint16 port;


};

// 喷阀自检线程
class AITestEjectThread : public QThread
{
    Q_OBJECT
public:
   explicit AITestEjectThread();
   void stop();
   void setDelayTime(int delayTime);

signals:
   void ejectorChangeSig();         // 喷阀自检数据变化信号
protected:
    void run();
private slots:
    void ejectTestDoneSlt();
private:
    volatile bool stopped;
    int delayTime;
};


/*!
 * \brief 主界面类
 */
class AisMainWidget : public QWidget
{
    Q_OBJECT
    
public:
    AisMainWidget(QWidget *parent = 0);
    ~AisMainWidget();

    void CreatePagePixel();            //像元信息页面
    void CreatePageEjector();          //喷阀参数页面
    void CreatePageImgHeight();        //图像高度页面
    void CreatePageModePara();         //模型参数
    void CreatePageModeUpdate();       //模型升级
    void CreatePageVersion();          //版本查询
    void CreatePageDiskValid();        //磁盘空间
    void CreatePageEjectTest();        //磁盘空间

    void updateModelListWidget();      //更新模型列表
    void updateModeParaInfo();

    void updatePagePixel();            //更新像元信息

    void saveAiPara(QString paraCol, int paraVal);

    int ejectorNum;
    int startPixelNum;
    int endPixelNum;
    int ejectorDelay;
    int ejectorDynamicDelay;
    int ejectorBlowTime;

    int imgFetchHeight;
    int imgInferHeight;

    QString versionInfo;
    QString diskValidInfo;

    bool isImageSend;
    bool isImageInfer;

    MODEL_PARA_INFO2 modeParaStr[10];

    int defaultModelListWidget;
    QString defaultModelId;
    int modeParaCount;

protected:


signals:
    void backToHomePageSig();       // 返回上一级信号
    void ejectTestDonesig();

public slots:
    void onListWidgetRowChangedSlt(int nIndex); // 响应列表行变化
    void onImageSendBtnPressedSlt();
    void onImageInferBtnPressedSlt();
    void onSetBackBtnClicked();      // 响应返回按钮按下事件
    void getDefaultPara();
    void onSaveParaBtnPressedSlt();

    void onApplyBtnPressedSlt();
    void onDownLoadImgBtnPressedSlt();
    void onDownloadFinishedSlt();
    void onBlankDirListFinishedSlt();
    void onRemoveFinishedSlt();
    void onDownPercentTimeout();

    void onStartPixelEditPressedSlt();
    void onEndPixelEditPressedSlt();
    void onUnitPlusBtnClicked();            // 响应板号增加按钮按下事件
    void onUnitMinusBtnClicked();           // 响应板号减少按钮按下事件
    void onUnitViewClicked();               // 响应前后视切换按钮按下事件


    void onEjectorNumEditPressedSlt();
    void onEjectorDelayEditPressedSlt();
    void onEjectorDynamicDelayEditPressedSlt();
    void onEjectorBlowTimeEditPressedSlt();


    void onImgFetchHeightEditPressedSlt();
    void onImgInferHeightEditPressedSlt();

    void onAddModeBtnPressedSlt();
    void onApplyModeBtnPressedSlt();
    void onRenameModeBtnPressedSlt();
    void onDeleteModelBtnPressedSlt();

    void setThresholdLineEdit(int);
    void setModelNameCbx(int);

    void onEjectorTestNumEditPressedSlt();
    void onNozzleTestNumEditPressedSlt();
    void onEjectorTestDelayEditPressedSlt();
    void onEjectTestStartBtnPressedSlt();
    void onEjectTestEndBtnPressedSlt();
    void onLoopCbxClickedSlt();
    void ejectPageThreadStartSlt();
    void getParaInfo();
    QString incrementLastOctet(const QString & ip);
    void onAiEnbaleImageSendCbxClicked();

private slots:


private:
    g_Config *config;

    int aiDeviceNum;

    QList<QHostAddress>     addressList;
    QList<QString>         ftpIpAddressList;

    QHostAddress     address;
    QString          ftpIpAddress;

    QGridLayout      *mainGridLayout;

//    AiCommunicate    *aiCommunicate;             // ai设备网络服务

    QStackedWidget   *stackedWidget;
    QFrame           *lineTitle;
    myPushButton     *imageSendBtn;
    myPushButton     *imageInferBtn;
    myPushButton     *ApplyBtn;
    myPushButton     *downLoadImgBtn;
    myPushButton     *backPushButton;
    myPushButton     *saveParaPushButton;


    QTimer           *downPercentTimer;

    myListWidget      *listWidget;
    myListWidgetItem  *statusListItem[10];
    QVector<int>      listIndex;

    //像元信息界面
    QWidget           *pagePixel;
    myLabel           *ejectorNumLbl;
    myLineEdit        *ejectorNumEdit;
    myLabel           *startPixelLbl;
    myLineEdit        *startPixelEdit;
    myGroupBox        *unitGroup;
    QLCDNumber        *unitLcdNum;
    myPushButton      *unitViewBtn;
    myPushButton      *unitPlusBtn;
    myPushButton      *unitMinusBtn;
    QGridLayout       *unitGridLayout;

    //喷阀参数页面
    QWidget           *pageEjector;
    myLabel           *endPixelLbl;
    myLineEdit        *endPixelEdit;
    myLabel           *ejectorDelayLbl;
    myLineEdit        *ejectorDelayEdit;
    myLabel           *ejectorDynamicDelayLbl;
    myLineEdit        *ejectorDynamicDelayEdit;
    myLabel           *ejectorBlowTimeLbl;
    myLineEdit        *ejectorBlowTimeEdit;

    //图像高度页面
    QWidget           *pageImgHeight;
    myLabel           *imgFetchHeightLbl;
    myLineEdit        *imgFetchHeightEdit;
    myLabel           *imgInferHeightLbl;
    myLineEdit        *imgInferHeightEdit;
    myCustomCheckBox  *aiEnbaleImageSendCbx;


    //模型参数
    QWidget           *pageModePara;
    QHBoxLayout       *paraModelHLayout[10];
    QVBoxLayout       *pageModelParaLayout;

    myCustomCheckBox  *modelNameCbx[10];
    myLabel           *thresholdLbl[10];
    myLineEdit        *thresholdLineEdit[10];
    QSignalMapper     *m_sigMapper;
    QSignalMapper     *m_sigCheckMapper;


    //模型升级
    QWidget           *pageModeUpdate;
    myListWidget      *modelListWidget ;
    myPushButton      *applyModeBtn ;
    myPushButton      *deleteModelBtn ;
    myPushButton      *renameModeBtn ;
    myPushButton      *addModeBtn ;

    QVariantMap       ModelInfoMap;
    FtpUploader      *uploader;
    QList<FtpUploader *> uploaderList;

    //版本查询页面
    QWidget           *pageVersion;
    myLabel           *versionLbl[MAX_AI_DEVICE_NUM];
    myLabel           *versionInfoLbl[MAX_AI_DEVICE_NUM];

    //磁盘空间
    QWidget           *pageDiskValid;
    myLabel           *diskValidLbl[MAX_AI_DEVICE_NUM];
    myLabel           *diskValidLblInfoLbl[MAX_AI_DEVICE_NUM];

    //喷阀检测
    QWidget           *pageEjectTest;
    myLabel           *ejectorTestNumLbl;
    myLineEdit        *ejectorTestNumEdit;
    myLabel           *nozzleTestNumLbl;
    myLineEdit        *nozzleTestNumEdit;

    myLabel           *ejectorTestDelayLbl;
    myLineEdit        *ejectorTestDelayEdit;
    myCustomCheckBox  *loopCbx;
    myPushButton      *ejectTestStartBtn ;
    myPushButton      *ejectTestEndBtn ;
    AITestEjectThread    *threadStart;

    bool        bFlagLoop;
    int         nDisplayNum;
    int         nMaxEjector;
    int         ejectTestInterval;
    int         ejectTestInterval2;
    int         ejectTestIntervalTotal;

};




#endif // AISMAINWIDGET_H
