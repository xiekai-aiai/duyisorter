/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aimainwidget.h
 * \brief       彩色机型智能分析主界面头文件
 * \date        2015.01.14
 */
#ifndef AIMAINWIDGET_H
#define AIMAINWIDGET_H

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QStackedWidget>
#include <QFormLayout>
#include <QElapsedTimer>
#include "common/globalconfig.h"
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/myvaluebar.h"
#include "common/mymessagebox.h"
#include "common/myinputmethod.h"
#include "common/mymutex.h"
#include "common/mygroupbox.h"
#include "common/mylcdnumber.h"
#include "common/mywid.h"
#include "common/myinputpanel.h"
#include "common/mywid.h"
#include "globalparams.h"
#include "globalflow.h"
#include "mylanguage.h"
#include "aishare.h"
#include "aipreviewwidget.h"
#include "myvideo.h"
#include "aiinputparams.h"
#include "upgradefpga/selectfiledialog.h"
#ifdef Q_OS_UNIX
    #include "common/myusb/myusb.h"
#endif

#define TEST    1
#define MAX_IMG_NUM         18
#define MAX_PER_LIST        6
#define MAX_LIST_PAGE       3
#define MAX_USUAL_IMG_NUM   4
#define MAX_MUL_QUO         9
#define MAX_MUL_QUO1        6
#define MAX_USUAL_QUO       3

#define BUFFER_SIZE 10*1024*1024  // 修正缓冲区大小计算（10MB）
#define FRAME_HEADER 0xA6A66A6A   // 帧头常量（便于对比）

struct classQuo{
    float nCons;                     /*!<< 常数 */
    float nQuo[MAX_MUL_QUO];         /*!<< 系数数组 */
    float nMax[MAX_MUL_QUO];         /*!<< 最大值数组 */
    float nMin[MAX_MUL_QUO];         /*!<< 最小值数组 */
};

typedef struct {
    int nUnitAddr;                          /*!<< 执行本次抓拍的相机地址 */
    int nSeq;                               /*!<< 执行本次抓拍的相机顺序,从1开始*/
    int nProtoType;                         /*!<< 执行本次抓拍的相机与上位机数据传输协议类型 */
    int nViewNum;                           /*!<< 执行本次抓拍的相机所属背景组组号 */
    int nTransType;                         /*!<< 执行本次抓拍的相机数据传输媒介类型 */
    int dataLen;                            /*!<< 执行本次抓拍的协议数据包字节总长度 */
    int dataDepth;                          /*!<< 执行本次抓拍的协议数据包行数 */
    int result;                             /*!<< 执行本次抓拍的结果信息 */
    QString unitViewName;                   /*!<< 执行本次抓拍的相机所属背景组名称 */
}CaptureConfig;

/*!
 * \brief 彩色机型智能分析主界面类
 */
class AIMainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AIMainWidget(QWidget *parent = 0);
    void updateLayerStat();
    void updateLayerStatMaize();
    void updateChuteLCDStatMaize();
    int  multipleClassAICalForSoleView(int indexOfTrain);    /*!<< 智能学习单个背景组 */
    void displayCalResult(QList<int> &list);                 /*!<< 打印学习结果 */
    void getImage();                                         /*!<< 新拍照功能函数*/
    int  getUnitProtocal(int nUnit);                         /*!<< 获取该相机的图片数据传输协议 */
    void setDataLenByProtocal(int proto, int &dataLenTotal, int &dataDeepth);   /*!<< 根据图片数据传输协议获取协议包长 */
    void setCaptureAttr(QList <CaptureConfig> &qlCaptureConfig, int nUnitElem); /*!<< 设置拍照相机的相关参数 */
    int  getUnitCameraType(int nUnit);                                          /*!<< 获取该相机的相机类型 */
    int  getUnitViewNum(int nUnit);                                             /*!<< 获取该相机的背景组编号 */
    void autoCopyIntellArgs();                                                  /*!<< 自动复制参数到其他智能模式 */

    void getAllUnitsAddrForOneChute(QList <int> &qlnUnit, int nUnitElem);       /*!<< 获取该相机所在通道的所有相机编号 */
    void getAllUnitsAddrForOneChuteForII(QList <int> &qlnUnit, int nUnitElem);
    void getAllUnitsAddrForOneChuteForOneViewPerLayer(QList <int> &qlnUnit, int nUnitElem);
    void getAllUnitsAddrForOneChuteForTwoViewPerLayer(QList <int> &qlnUnit, int nUnitElem);
    void getAllUnitsAddrForOneChuteForFourViewPerLayer(QList <int> &qlnUnit, int nUnitElem);
    void getAllUnitsAddrForOneChuteForFourViewForRSC(QList <int> &qlnUnit, int nUnitElem);
    void startCapture(QList <CaptureConfig> &qlCaptureConfig);                  /*!<< 下发拍照命令 */
    int  checkFrame(const char *data, int dataLen);                             /*!<< 校验包 */
    int  swapData(char *data, int blockLen, int blockCount);                    /*!<< 翻转数据包 */
    int  getAutoBackgroundData(char *data, CaptureConfig &captureConfig);        //从图片取自动背景数据
    int  stripFrameData(char *data, CaptureConfig &captureConfig);              /*!<< 将数据包转换成纯图片数据 */
    void displayImgInfo(char *data, CaptureConfig &captureConfig);              /*!<< 图片的保存、显示 */
    void processImageDataFromCameraByUART(CaptureConfig &captureConfig);        /*!<< 数据的接收函数 */
    void processImageDataFromCameraByUSB(CaptureConfig &captureConfig);         /*!<< 数据的接收函数 */
    void processImageDataFromCamera(CaptureConfig &captureConfig);              /*!<< 数据的接收函数 */
    void processCaptureResult(QList<CaptureConfig> &qlCaptureConfig);           /*!<< 拍照功能完成之后的相关处理 */
    bool pixelIsBackground(uchar r, uchar g, uchar b,int nUnitAddr);            /*!<< 拍照时根据背景确定物料在哪一侧*/
    void refreshImpurityName(void);                                             /*!<< 更新杂质名称*/
    void processImageDataFromCameraByAIEN(CaptureConfig &captureConfig);         /*!<< 数据的接收函数 */

    void updateParams();                                                        /*!<< 智能参数页面参数更新*/
    void updateAIParamsPage();
    void upTabBar();

public:
    enum{
        AI_PREPARE_PAGE,                            /*!<< 智能预备页面索引号 */
        AI_MAIN_PAGE,                               /*!<< 智能主页面索引号 */
        AI_PREVIEW_PAGE,                            /*!<< 智能预览页面索引号 */
        AI_IMAGE_PAGE,                              /*!<< 智能图像处理页面索引号 */
        AI_INPUT_PARAMS_PAGE,                               /*!<< 输入智能参数页面索引号 */
        AI_PARAMS_SET_PAGE
    };
    
signals:
    void goToHomePage();                            /*!<< 信号函数：返回智能主界面 */
    void updatePreviewPageSig();                    /*!<< 信号函数：更新智能预览界面 */
    void deleteCurImgSig();                         /*!<< 信号函数：删除图片 */
    void deleteAllImgSig();                         /*!<< 信号函数：删除全部图片 */
    void goToIdentifyPagesig();                 /*!<< 信号函数：返回识别参数主界面 */

public slots:
    void onStackedWidgetIndexChangedSlt(int index); /*!<< 槽函数：页面切换*/
    void onLayerBtnPressedSlt();                    /*!<< 槽函数：层间或者前后视切换 */
    void onLayerBtnPressedSltAdd();                 /*!<< 槽函数：层间或者前后视切换，LDS2补充按钮 */
    void onPlusChanNumPressedSlt();                 /*!<< 槽函数：通道数增加 */
    void onMinusChanNumPressedSlt();                /*!<< 槽函数：通道数减少 */

    void onLayerBtnPressedSltMaize();               /*!<< 槽函数：层间或者前后视切换-玉米定制*/
    void onPlusChanNumPressedSltMaize();            /*!<< 槽函数：通道数增加-玉米定制*/
    void onMinusChanNumPressedSltMaize();           /*!<< 槽函数：通道数减少-玉米定制*/
    void onExportImgBtnPressedSlt();                /*!<< 槽函数：图片导出 */
    void onCalSetBtnSlt();                          /*!<< 槽函数：前后视或上下层参数设置 */
    void onClassModeBtnPressedSlt();                /*!<< 槽函数：智能化模式使能 */
    void onAIParamsSetBtnPressedSlt();              /*!<< 槽函数：智能参数 */
    void onAIStudyBtnPressedSlt();                  /*!<< 槽函数：一键智能学习 */
    void onStartCapBtnPressedSlt();                 /*!<< 槽函数：开始采集功能 */
    void onDeleteBtnPressedSlt();                   /*!<< 槽函数：删除图片功能 */
    void onDeleteBtnReleasedSlt();
    void onDeleteTimerOutSlt();
    void onPreBtnPressedSlt();                      /*!<< 槽函数：切换至上一页 */
    void onNextBtnPressedSlt();                     /*!<< 槽函数：切换至下一页 */
    void onBackBtnPressedSlt();                     /*!<< 槽函数：返回至主界面 */
    void imageListChangeSlt(int index);             /*!<< 槽函数：信号栈相应函数，图片状态变化，物料属性变化 */
    void goBackToMainWidget();                      /*!<< 槽函数：返回至智能主界面 */
    void refreshDisplaySlt();                       /*!<< 槽函数：刷新智能分析界面的显示 */
    void gotoFirstPageSlt();                        /*!<< 槽函数：初始化操作 */
    void dropRecieveImageData();                    /*!<< 槽函数：终止图像采集 */

    /* 大蒜机型槽函数开始 */
    void onOkBtnClicked();                          /*!<< 槽函数：预备页面应用按钮 */
    void onCancelBtnClicked();                      /*!<< 槽函数：预备页面返回按钮*/
    void onBoxMapClicked(int index);                /*!<< 槽函数：预备页面算法选择框按钮 */
    void onBtnMapClicked(int index);                /*!<< 槽函数：预备页面算法按钮*/
    /* 大蒜机型槽函数结束 */

    QString getImgName(int imgIndex, bool isNeedSuffix = false);
    void setImgName(int imgIndex, QString suffix);
    void onImgImportBtnPressedSlt();
    void onImgExportBtnPressedSlt();
    void onImgThresholdLineEditPressedSlt();
    void onImgBackBtnPressedSlt();
    void onHighSpeedPressedSlt();
    void onLowSpeedPressedSlt();
    void onAIParaAutoCopyPressedSlt();
    void onAICaptureAllViewPressedSlt();
    void onAICuteOffBgPressedSlt();
    void onBeginChLePressedSlt(void);       /*!< 槽函数：触发起始象元 */
    void onEndChLePressedSlt(void);         /*!< 槽函数：触发截止象元 */
    void onEnableAIAuto1CBXPressedSlt();    /*!< 槽函数：使能1信号*/
    void onEnableAIAuto2CBXPressedSlt();    /*!< 槽函数：使能2信号*/
    void onReservedSensLbePressedSlt();     /*!< 槽函数: 保留腹白*/
    void onMaterPer1LbePressedSlt(void);    /*!< 槽函数：物料含杂1*/
    void onMaterPer2LbePressedSlt(void);    /*!< 槽函数：物料含杂2*/
    void onMaterPer3LbePressedSlt(void);    /*!< 槽函数：物料含杂3*/
    void onImpurityNameLePressed(void);     /*!< 槽函数：杂质名称 */

    void onAIParamsInputSlt(void);          /*!< 槽函数：输入智能参数 */
    void goBackToAIImageWidget();           /*!< 槽函数：输入智能参数页面返回 */

    void changeCurrentSlt(int indexOfUnit); /*!< 槽函数：智能参数页面tabBar响应函数*/
    void setImpurityNameSlt();
    void setSensValueSlt();
    void setRowValueSlt();
    void setPercentValueSlt();
    void onAIParamsBackBtnPressedSlt();     /*!< 槽函数：智能参数页面返回*/
    void onColorSpaceBtnGpIndexChangedSlt(int nIndex);      /*!< 槽函数：颜色空间 */
    void onDimensionBtnGpIndexChangedSlt(int nIndex);       /*!< 槽函数：维度 */

private:
    /*!
     * \name 私有枚举
     * \{
     */
    /*! \enum page level */
    enum{
        FIRST_PAGE,                             /*!<< 第一页索引号 */
        SECOND_PAGE,                            /*!<< 第二页索引号 */
        THIRD_PAGE                              /*!<< 第三页索引号 */
    };
    /*! \enum the state of sample */
    enum{
        SAMPLED_NO,                             /*!<< 没取样 */
        SAMPLED_YES                             /*!<< 已取样 */
    };
    /*! \enum 物料属性设置错误返回标志 */
    enum{
        ERR_NO_BAD,                             /*!<< 返回标志：没有设置坏料 */
        ERR_NO_GOOD,                            /*!<< 返回标志：没有设置好料 */
        ERR_OVERRUN,                            /*!<< 返回标志：设置种类超过最大限制 */
        ERR_NO_SET,                             /*!<< 返回标志：没有设置 */
        SUCCEED,                                /*!<< 返回标志：设置成功 */
        ERR_NO_CAL                              /*!<< 返回标志：未计算 */
    };
    /*! \enum 分类数目 */
    enum{
        NO_GOOD_NO_BAD = 0,                     /*!<< 分类数目：0好0坏 */
        ONE_GOOD_ONE_BAD,                       /*!<< 分类数目：1好1坏 */
        ONE_GOOD_TWO_BAD,                       /*!<< 分类数目：1好2坏 */
        ONE_GOOD_THREE_BAD,                     /*!<< 分类数目：1好3坏 */
        TWO_GOOD_ONE_BAD,                       /*!<< 分类数目：2好1坏 */
        TWO_GOOD_TWO_BAD,                       /*!<< 分类数目：2好2坏 */
        THREE_GOOD_ONE_BAD                      /*!<< 分类数目：3好1坏 */
    };
    /*! \enum 读取样本标志 */
    enum {
        DATA_IMG  = 0,                          /*!<< 读取标志：从图片读取 */
        DATA_DISK = 1                           /*!<< 读取标志：从本地读取 */
    };

    /*! \enum 采集图像功能返回的错误信息 */
    enum {
        ERR_SUCCESS,        //成功
        ERR_TIMEOUT,        //超时
        ERR_USB_INIT,       //USB初始化失败
        ERR_DATA,           //像素数据错误
        ERR_CONFIG,         //拍照相机设置参数设置错误
        ERR_MALLOC_FAILED,  //内存分配失败
        ERR_INPUT_ARG,      //函数入参错误
        ERR_AUTO_BACKGROUND_CHECKERROR,   //自动背景校验错误
        ERR_UNKOWN          //未知错误
    };

    /*!
     * \name 控件的私有成员变量
     * \{
     */
    QStackedWidget  *m_stackedWidget;               /*!<< 指针：智能分析页面堆栈 */

    QWidget         *m_AIArithListWidget;           /*!<< 指针：智能分析预备页面 */
    QWidget         *m_AIMainWidget;                /*!<< 指针：智能分析主页面 */
    AIPreviewWidget *m_AIPreviewWidget;             /*!<< 指针：智能分析预览页面 */
    QWidget         *m_AIImageWidget;               /*!<< 指针：智能分析图像倒入导出页面 */
    AIInputParams   *m_AIInputParamsWidget;         /*!<< 指针: 输入智能参数页面 */
    QWidget         *m_AIParamsSetWidget;           /*!<< 指针: 智能参数页面 */
    QGridLayout    *m_imgListGridLayout;            /*!<< 布局：图片列表 */
    QGridLayout    *m_channelGridLayout;            /*!<< 布局：通道设置 */

    QVBoxLayout    *m_commVBLayout;                 /*!<< 布局：通信方式 */
    QVBoxLayout    *m_setVBLayout;                  /*!<< 布局：设置部分（包括通道设置，通信方式，智能设置，智能计算等） */

    QHBoxLayout    *m_upHBLayout;                   /*!<< 布局：上部分（由图片列表与设置部分组成） */
    QHBoxLayout    *m_downHBLayout;                 /*!<< 布局：下部分（开始采集，返回等按钮组成） */
    QVBoxLayout    *m_mainVBLayout;                 /*!<< 布局：主要布局（上部分和下部分） */

    myGroupBox     *m_imgListGBox;                  /*!<< 组：图片列表 */
    myGroupBox     *m_channelGBox;                  /*!<< 组：通道设置（包括当前通道，增加与减少） */
    myGroupBox     *m_commGBox;                     /*!<< 组：通信方式 */

    myPushButton   *m_imgListBtn[MAX_PER_LIST];     /*!<< 按钮：图片列表按钮 */
    myPushButton   *m_imgPropertyBtn[MAX_PER_LIST]; /*!<< 按钮：图片列表属性 */

    myPushButton   *m_layerBtn;                     /*!<< 按钮：前后视（CF），上下层（其他）按钮 */
    myPushButton   *m_layerBtnAdd;                  /*!<< 按钮： m_layerBtn的补充按钮，如对于LDS2表示前后视*/
    QLCDNumber     *m_chanNum;                      /*!<< LCD: 通道数目 */
    myPushButton   *m_plusChanNumBtn;               /*!<< 按钮：增加通道数目 */
    myPushButton   *m_minusChanNUmBtn;              /*!<< 按钮：减少通道数目 */
    myPushButton   *m_exportImgBtn;                 /*!<< 按钮：导出图片按钮 */

    myPushButton   *m_calSetBtn;                    /*!<< 按钮：智能参数设置按钮 */
    myPushButton   *m_classModeBtn;                 /*!<< 按钮：智能模式设置按钮 */
    myPushButton   *m_AIParamsSetBtn;               /*!<< 按钮：智能模式设置按钮 */
    myPushButton   *m_AIStudyBtn;                   /*!<< 按钮：一键智能设置按钮 */

    myPushButton   *m_startCapBtn;                  /*!<< 按钮：开始采集按钮 */
    myPushButton   *m_delImgBtn;                    /*!<< 按钮：删除图片按钮 */
    myPushButton   *m_preListBtn;                   /*!<< 按钮：上一页按钮 */
    myPushButton   *m_nextListBtn;                  /*!<< 按钮：下一页按钮 */
    myPushButton   *m_backBtn;                      /*!<< 按钮：返回主界面按钮 */

    QSignalMapper  *m_imgListSMapper;               /*!<< 信号栈：图片列表信号栈 */

    myPushButton    *m_imgImportBtn;                /*!<< 按钮：图像导入按钮 */
    myPushButton    *m_imgExportBtn;                /*!<< 按钮：图像导出按钮 */
    myPushButton    *m_inputParamsBtn;              /*!<< 按钮：输入智能参数按钮 */
    myLabel         *m_imgThresholdLabel;           /*!<< 文本: 像素阈值  */
    myLineEdit      *m_imgThresholdLineEdit;        /*!<< 输入框: 像素阈值*/
    myLabel         *m_rangeLbl;                    /*!< 文本: 象元范围 */
    myLineEdit      *m_beginChLe;                   /*!< 输入框: 起始象元 */
    myLineEdit      *m_endChLe;                     /*!< 输入框: 终止象元 */
    myCustomCheckBox *m_highSpeedCbx;               /*!<< 选择框: 高速 */
    myCustomCheckBox *m_lowSpeedCbx;                /*!<< 选择框: 低速 */
    myCustomCheckBox *m_AIParaAutoCopy;             /*!<< 选择框: 自动复制 */
    myCustomCheckBox *m_AICaptureAllView;           /*!<< 选择框: 全视角拍照 */
    myCustomCheckBox *m_AICuteOffBgCBx;             /*!<< 选择框: 去背景拍照*/
    myPushButton    *m_imgBackBtn;                  /*!<< 按钮：图像返回按钮 */
    myLineEdit      *m_impurityNameLe;              /*!< 输入框: 杂质名称 */

    myCustomCheckBox *m_enableAIAuto1CBx;           /*!< 选择框：使能1 */
    myCustomCheckBox *m_enableAIAuto2CBx;           /*!< 选择框：使能2 */
    myLabel          *m_reservedSensLabel;          /*!< 文本：腹白保留 */
    myLineEdit       *m_reservedSensLbe;            /*!< 输入框：腹白保留 */
    myLabel          *m_materPer1Label;             /*!< 文本：物料含杂1 */
    myLineEdit       *m_materPer1Lbe;               /*!< 输入框：物料含杂1 */
    myLabel          *m_materPer2Label;             /*!< 文本：物料含杂2 */
    myLineEdit       *m_materPer2Lbe;               /*!< 输入框：物料含杂2 */
    myLabel          *m_materPer3Label;             /*!< 文本：物料含杂3 */
    myLineEdit       *m_materPer3Lbe;               /*!< 输入框：物料含杂3 */

    myGroupBox *arithAIGroup;
    QVBoxLayout *vBoxBtnLayout;
    QVBoxLayout *m_preVboxlayout;

    QVector<MyCheckBox *> aiBox;
    QVector<myPushButton *> aiBtn;
    QVector<QFormLayout *> boxBtnLayout;

    QSignalMapper *sigBoxMapper;
    QSignalMapper *sigBtnMapper;
    myPushButton *okButton;
    myPushButton *cancelButton;

    MyButtonGroup *m_colorSpaceBtnGp;
    MyButtonGroup *m_dimensionBtnGp;

    /*!
     * \name 私有参数
     * \}
     */
    int m_nStatOfImg[MAX_VIEW_LOCATION][MAX_IMG_NUM];        /*!<< 参数：图片状态：设置好料，设置坏料，无设置 ,+1:四视*/
    int m_nNumOfImg;                                 /*!<< 参数：采集图片数目 */
    int m_nIndexOfImg;                               /*!<< 参数：当前图片索引号 */
    int m_nIndexOfPage;                              /*!<< 参数：当前页面索引号 */
    QString m_sNameOfImg[MAX_VIEW_LOCATION][MAX_IMG_NUM];    /*!<< 参数：图片名称*/
    int m_nViewNo[MAX_IMG_NUM];                      /*!<< 参数：各个图片所属的视角id:0,1,2,3 */
    bool m_bSampedOfImg[MAX_IMG_NUM];                /*!<< 参数：是否取样标志 */
    int m_nClassNum[MAX_VIEW_LOCATION];                      /*!<< 参数：分类数目，仅用于传统智能算法*/
    int m_nClassGoodNum[MAX_VIEW_LOCATION];                  /*!<< 参数：分类好物料数目 */
    int m_nClassBadNum[MAX_VIEW_LOCATION];                   /*!<< 参数：分类坏物料数目 */
    bool m_bStatDelImg;                              /*!<< 参数：删除图片状态标志 */
    struct classQuo m_struClassQuo[MAX_VIEW_LOCATION];       /*!<< 参数：物料方程结构体 */
    bool m_bCalSet;                                  /*!<< 参数：智能参数设置标志 ,false:相同设置，true:独立设置*/
    int  m_nLayer;                                   /*!<< 参数：层数或只前后视 */
    bool m_bIsAutoGetData;                           /*!<< 参数：是否自动从图片读取数据 */
    int  m_nLevelToTal;                              /*!<< 参数：前后视标志（CF，LD），层数设置（LDS，TD） */
    uchar m_cRGBStat[IMAGE_WIDTH*IMAGE_HEIGHT][3];   /*!<< 参数：RGB状态 */
    int  m_nRGBNum;
    bool m_bIsImageRecvTerminate;                    /*!<< 参数：终止图像采集的标志 */
    int m_nIndexOfArithIntel;                        /*!<< 参数：当前智能计算对应的智能算法序号，A:0,B:1,C:2,D:3 */
    int m_nPicType;                                  /*!<< 参数: 当前拍图片对应的相机回包类型，默认为0，1为红外，该变量目前I机型使用，2015.7.10 */
    bool m_bIsAutoCopy[MAX_AI][MAX_AI];              /*!<< 参数: 当前智能多分类是否需要自动复制智能参数至其他组智能 */
    bool m_bIsCaptureAllView;                        /*!<< 参数: 当前拍照方式，0：普通拍照，1：单通道所有相机都拍照 */
    bool m_bIsCuteOffBg;
    int m_nImgNum;

    int m_nPressTimeStart;                          /*!<< 参数: 删除图片按钮点击开始时间 */

    int m_currentChan;
    int m_algorithmType;                              //0:当前列表为可见，1：红外

    //智能参数设置页面控件
    QTabBar         *tabBar;
    myGroupBox      *nameGroup;
    myLineEdit      *nameValue;
    myGroupBox      *AISensListCbx;
    myLabel         *AISensLabel;
    myLineEdit      *AISensLbe;
    myLabel         *AIRowLabel;
    myLineEdit      *AIRowLbe;
    myLabel         *AIPercentLabel;
    myLineEdit      *AIPercentLbe;

    myPushButton    *AIParamsBackBtn;

    QTimer *delTimer;

    /*!
     * \name 最大值与最小值
     * \}
     */
    int m_nMaxR[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinR[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxG[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinG[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxRG[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinRG[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxRB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinRB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxGB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinGB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxRR[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinRR[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxGG[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinGG[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMaxBB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];
    int m_nMinBB[MAX_VIEW_LOCATION][MAX_USUAL_IMG_NUM];

private:
    void createAIMainWidget(void);                    /*!<< 私有函数：创建智能分析主页面 */
    void createAIArithListWidget(void);               /*!<< 私有函数：创建智能分析预备页面 */
    void initAIMainWidget();                          /*!<< 私有函数：开机第一次初始化参数 */
    void updateImgList();                             /*!<< 私有函数：更新当前页面图片列表 */
    void updateImgName(int indexOfImg, bool bFlagSet);/*!<< 私有函数：更新当前页面图片属性 */
    void updateBtnStatus();                           /*!<< 私有函数：更新按钮状态 */
    void displayCurImg(int indexOfImg);               /*!<< 私有函数：显示当前图片状态 */
    void deleteCurImg(int indexOfImg);                /*!<< 私有函数：删除当前图片 */
    void updateImgListStat(void);                     /*!<< 私有函数：更新当前图片状态，选中：变绿 */
    char *getHead(const char *mem, int size, const char *str);          /*!<< 私有函数：获取一帧数据的头指针 */
    bool getImageFromUSB(void);                                         /*!<< 私有函数：通过USB获取图片(1024*3) */
    bool getImageFromUSB_2048_All(void);                                    /*!<< 私有函数：通过USB获取图片 */
    void saveBMPFile(QString name, int width, int height, uchar *data); /*!<< 私有函数：将图片数据保存为BMP文件 */
    void saveBMPFileForMono(QString name, int width, int height, uchar *data);
    bool pixelIsEdgeImage(QImage *image,int x, int y);                  /*!<< 私有函数：优化取样时判断像素点是否为边缘点 */
    bool multipleClassAICal(int indexOfTrain=0);                        /*!<< 私有函数：多分类智能计算 */
    void usualClassAICal(void);                                         /*!<< 私有函数：传统智能计算 */
    bool AIClassComputer(int classMode, int indexOfTrain);              /*!<< 私有函数：智能计算 */
    bool setGlobalAIParam(int classMode, int indexOfTrain); /*!<< 私有函数：direct,0:将智能计算结果设置全局参数,1:A全局复制到B,2:B全局复制到A*/
    int  getUnitGroup(int level, int unit);                             /*!<< 私有函数：智能算法参数复制设别组号设置 */
    void getGlobalAIParam(int classMode,int levelNum, int groupNum, int indexOfTrain);  /*!<< 私有函数：获取全局智能参数 */
    void getDataMaxMin(int index,int r, int g, int b);                  /*!<< 私有函数：获取最大值与最小值 */
    void resetDataMaxMin(int index);                                    /*!<< 私有函数：恢复最大值与最小值为默认值 */
    bool saveDataMaxMin(int classMode,int indexOfTrain);                /*!<< 私有函数：保存最大最小值 */
    bool saveDataFromImg(int nDataFrom,int classMode,int classLevel, int indexOfImg, int indexOfTrain); /*!<< 私有函数：保存样本参数 */
    int enableAICal();                                                  /*!<< 私有函数：检查图片类别属性设置 */
    bool enableAIArithmetic(int classMode);                             /*!<< 私有函数：根据需要使能智能算法 */
    bool IsRGBExit(uchar r, uchar g, uchar b);                          /*!<< 私有函数：判断样本点是否存在*/
    void doUnsupervisedLearning();                                      /*!< 私有函数：无监督自学习操作 */
    void unsupervisedLearning();                                        /*!< 私有函数：无监督自学习 */
    bool IsImageOk(int nImgNum);                                        /*!< 私有函数：图片是否满足要求 */
    bool saveAIConfig(int nMaterPer, int nImgNum, int nFlgView, int nResMode, int nResSens);                             /*!< 私有函数：保存智能设置 */

    void createAIImageWidget(void);                                     /*!<< 私有函数：创建图像导入导出页面 */
    void createAIModeParamsWidget(void);
};

#endif // AIMAINWIDGET_H
