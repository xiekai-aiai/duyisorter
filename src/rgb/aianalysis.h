/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aianalysis.h
 * \brief       彩色机型智能分析界面头文件
 * \date        2015.01.14
 */
#ifndef AIANALYSIS_H
#define AIANALYSIS_H

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTableWidget>
#include "common/globalconfig.h"
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/myvaluebar.h"
#include "common/mymessagebox.h"
#include "common/myinputmethod.h"
#include "common/mymutex.h"
#include "common/mygroupbox.h"
#include "common/mylcdnumber.h"
#include "globalparams.h"
#include "globalflow.h"
#include "mylanguage.h"

class QToolButton;
#ifdef Q_OS_UNIX
#include "common/myusb/myusb.h"
#endif

#define TEST    1
#define IMG_MAX_NUM     18


enum{
    IMAGE_WIDTH = 1024,
    IMAGE_HEIGHT = 500,
    INF_WIDTH = 512,
    INF_HEIGHT = 250
};

enum{
    MAX_SELECT_TIMES = 100,
    MAX_PIXEL_NUM = 10000,
    MIN_PIXEL_NUM = 1000
};

#if(LCD_WIDTH == 640)
    enum{
        DISPLAY_WID = 512,
        DISPLAY_HEI = 256
    };
#elif(LCD_WIDTH == 800)
    enum{
        DISPLAY_WID = 600,
        DISPLAY_HEI = 300
    };
#elif(LCD_WIDTH == 1024)
    enum{
        DISPLAY_WID = 768,
        DISPLAY_HEI = 500
    };
#endif

struct AIParameter{
    int iScale;             // 当前图片的显示尺寸
    bool bDataIsOk;         // 标志数据是否准备完毕
    int iMaxMoveTimes;      // 水平与垂直的最大移动次数
    int iHMoveTimes;        // 水平移动次数(右移加1，左移减1)
    int iVMoveTimes;        // 垂直移动次数(下移加1，上移减1)
    int iSpaceX;            // 水平移动的间隔
    int iSpaceY;            // 垂直移动的间隔
    int iIndex;             // 显示区域在原始数据中位置的索引号
    bool bHasTar;           // 标记手动框中是否含有物料像素点
    int iSelTimes;          // 已选择物料的数目
    uchar selBuf[MAX_PIXEL_NUM*3];          // 保存已选物料的RGB分量
    QPoint selectedPoint[MAX_PIXEL_NUM];    // 已选择的物料点在原始图像上的坐标
    int iPixelNum[MAX_SELECT_TIMES];        // 记录每一次手选方框中被标记为物料像素点的数目
    int iPixelSum;                          // 记录所有被选物料像素点的数目
    int iDisplayNum[MAX_SELECT_TIMES];      // 记录每一次框选后高亮显示像素点的数目
    int iDisplaySum;        // 记录高亮显示的像素点数目
    QRect disRect;          // 保存鼠标移动形成的方框坐标信息
    QRect oriRect;          // 显示上述坐标在原始数据中的位置
    uchar displayColor[3];  // 存储被选择像素点的显示颜色
    bool bIsOptCal;         // 标志当前物料图片是否为优化计算
    bool bIsOptSam;         // 标记当前物料图片是否是优化取样
    int  imgIndex;          // 图像列表索引号
};

struct matterQuo{
    float nCons;           // 常数
    float nQuo1;           // 系数1
    float nQuo2;           // 系数2
    float nQuo3;           // 系数3
    float nMax[3];         // RGB值的最大值
    float nMin[3];         // RGB值的最小值
};

struct multiMatterQuo{
    float nCons;           // 常数
    float nQuo[9];         // 系数数组
    float nMax[9];         // 最大值数组
    float nMin[9];         // 最小值数组
};

/*!
 * \brief 彩色机型智能分析界面类
 */
class AIAnalysis : public QWidget
{
    Q_OBJECT
    
public:
    AIAnalysis(QWidget *parent = 0);
    ~AIAnalysis();

    void createAIMainPage(void);        // 创建智能分析界面
    void createAISetPage(void);         // 创建智能分析设置界面
    void createAISetMatterPage(void);   // 创建物料设置界面
    void createAICaptureImage(void);    // 创建图像采集界面

    void initMatterWidgetStatus(void);  // 初始化物料设置界面各控件的状态
    void initAIParams(void);            // 初始化智能分析参数
    void initMatterParams(void);        // 初始化物料参数
    void showMatSel(int index);         // 显示某一个物料设置
    void hideMatSel(int index);         // 隐藏某一个物料设置
    void updateMatterParams(void);      // 更新物料参数
    void setAIParams(void);             // 设置智能分析参数
    bool getImageData(QString fileName);// 根据文件名填充缓冲区数据
    void imageOpenError(void);          // 用于处理文件打开失败的情况
    void clearImageStatus(bool clearImgIndex = true);   // 用于清空所有图片的智能参数
    void clearSingleImageStatus(int index);             // 用于清空单个物料的智能参数
    void refreshBtnStatus(void);        // 根据当前图片的尺寸刷新按钮的状态
    void updateImage(void);             // 根据需要更新显示的照片
    void changeImage(void);             // 根据需要改变显示的照片
    void refreshDisplay(void);          // 根据优化计算和优化取样的标志位刷新显示
    void displaySimulate(void);         // 显示图片仿真后的效果
    bool pixelIsBackground(uchar r, uchar g, uchar b);                  // 判断原始图像像素点是否为背景点
    bool pixelIsWanted(uchar r, uchar g, uchar b);                      // 判断像素点是否为坏物料点
    bool pixelIsSelected(uchar r, uchar g, uchar b);                    // 判断图像中像素点为原始数据点还是被标记的点
    bool pixelIsReSelected(uchar r, uchar g, uchar b);                  // 优化计算时判断像素点是否为冗余点
    bool pixelIsEdge(int x, int y);                                     // 优化取样时判断像素点是否为边缘点
    bool pixelIsEdgeImage(QImage *image,int x, int y);                  // 优化取样时判断像素点是否为边缘点
    bool pixelIsBad(int index, uchar r, uchar g, uchar b);              // 根据选定的方程判断像素点是否为坏点
    void showMesLabel(void);                                            // 提示框中显示不同的信息
    void setDisplayColor(int index);                                    // 设置高亮显示的颜色
    bool imageIsSubBackground();                                        // 判断采集到的图像是否为去背景图像
    QColor getBackgroundColor(char *buf, QSize tmpSize);                // 获取去背景图像中的背景
    bool getImageFromUART_1024(void);                                   // 通过串口获取1024分辨率图片
    bool getImageFromUART_2048(void);                                   // 通过串口获取2048分辨率图片
    char *getHead(const char *mem, int size, const char *str);          // 获取一帧数据的头指针
    bool getImageFromUSB_1024(void);                                    // 通过USB获取1024分辨率图片
    bool getImageFromUSB_2048(void);                                    // 通过USB获取2048分辨率图片
    void enableAIArithmetic(void);                                      // 根据需要使能智能算法
    void setGlobalMatterParams(void);                                   // 设置全局参数中的智能部分参数
    void saveBMPFile(QString name, int width, int height, uchar *data); // 将图片数据保存为BMP文件
    bool saveDataToNormalFile(bool bIsAddCur = false);                  // 普通模式下将选择的像素点数据保存到文件中
    bool saveDataToMultiFile(bool bIsAddCur);                           // 多分类模式下将选择的像素点数据保存到文件中
    bool normalModeAICal(void);                                         // 普通智能算法计算
    bool multiModeAICal(void);                                          // 多分类智能算法计算
    bool multiPixelIsBad(uchar r, uchar g, uchar b);                    // 多分类模式下某像素点是否为坏物料点
    int getUnitGroup(int level, int unit);                              // 智能算法参数复制设别组号设置
    bool resetTrainFile(QString sFileName);                             // 重置训练样本
    void showInfoLabel(void);                                           // 显示当前层和前后视信息

signals:
    void goToHomePage();
    void colorCamToFirstSig();
    void infCamToFirstSig();
    void updateArithmeticSig();

public slots:
    void onStackedWidgetIndexChangedSlt(int index);
    void refreshDisplaySlt();       // 刷新智能分析界面的显示

    /* 智能算法主页面的按钮响应函数 */
    void onReWholeBtnClicked();
    void onReLastBtnClicked();
    void onOptCalBtnClicked();
    void onOptSamBtnClicked();
    void onSetMatterBtnClicked();
    void onLastBtnClicked();
    void onNextBtnClicked();
    void onAIBackBtnClicked();    
    void onUpBtnClicked();
    void onDownBtnClicked();
    void onLeftBtnClicked();
    void onRightBtnClicked();
    void onEnlargeBtnClicked();
    void onReduceBtnClicked();
    void onBoardNumPlusBtnClicked();
    void onBoardNumMinusBtnClicked();
    void onLayerBtnClicked();
    void onAICalBtnClicked();
    void onCaptureBtnClicked();
    void onDeriveBtnClicked();
    void onSimulateBtnClicked();
    void onUpdateBtnClicked();
    void onImgCapBtnClicked();
    void onClearSamBtnClicked();
    void onLayerAddBtnClicked();

    /* 智能分析设置页面的按钮响应函数 */
    void onAISetBtnClicked();
    void onNormalModeBtnClicked();
    void onInfModeBtnClicked();
    void onMultiModeBtnClicked();
    void onAISetBackBtnClicked();
    void onAISetSureBtnClicked();
    void onSensVlaueChangedSlt(int value);
    void onTriValueChangedSlt(int value);
    void onSamColorIndexChangedSlt(int index);
    void onTransModeChangedSlt(int index);
    void onSamModeChangedSlt(int index);

    /* 物料设置页面的按钮响应函数 */
    void onSetMatterSureBtnClicked();
    void onSetMatterBackBtnClicked();
    void onEnableBtn1Clicked();
    void onEnableBtn2Clicked();
    void onEnableBtn3Clicked();
    void onEnableBtn4Clicked();
    void onNameBtn1Clicked();
    void onNameBtn2Clicked();
    void onNameBtn3Clicked();
    void onNameBtn4Clicked();
    void onIsGoodBtn1Clicked();
    void onIsGoodBtn2Clicked();
    void onIsGoodBtn3Clicked();
    void onIsGoodBtn4Clicked();
    void onIsBadBtn1Clicked();
    void onIsBadBtn2Clicked();
    void onIsBadBtn3Clicked();
    void onIsBadBtn4Clicked();

    /*物料图片页面像元函数*/
    void onOkCapBtnClicked();
    void onCancelCapBtnClicked();
    void imageListChange(int index);
    void updateImgList();
    void onDeleteBtnSlt();
    void onPreBtnSlt();
    void onNextBtnSlt();
    void reloadImgList();
    void onCommBtnSlt();
    void onAutoStudySlt();

    void gotoFirstPageSlt();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    enum{
        AI_MAIN_PAGE,           // 智能分析主页面
        AI_SET_PAGE,            // 智能分析设置页面
        AI_SET_MATTER_PAGE,     // 物料设置界面
        AI_CAP_IMAGE_PAGE       // 图像采集页面
    };

    enum{
        MIN_SCALE = 1,          // 定义图片显示最小尺寸为原始尺寸的1倍
        MAX_SCALE = 8,          // 定义图片显示最小尺寸为原始尺寸的8倍
        NO_MATTER = 10          // 当前无物料的标志
    };

    AIParameter param[MAT_MAX];
    matterQuo myMatterQuo[MAT_MAX];
    multiMatterQuo myMultiQuo;
    QStackedWidget *stackedWidget;
    g_Config *config;    

    char oriBuf[IMAGE_WIDTH*IMAGE_HEIGHT*3];        // 用于存储原始图片的数据
    uchar infBuf[INF_WIDTH*INF_HEIGHT*3];           // 用于存储红外图片的数据
    uchar changeBuf[IMAGE_WIDTH*IMAGE_HEIGHT*3];    // 用于存储被修改的图片数据
    uchar displayBuf[DISPLAY_WID*DISPLAY_HEI*3];    // 用于存储显示的图片信息
    uint imgsIndex[MAT_MAX];                        // 用于存储物料设置界面中使能的物料编号
    uint matterIndex;                               // 用于存储当前处理图片的索引号
    uint arrayIndex;                                // 图片索引号在物料编号数组中的位置
    QPoint displayPoints[MAX_PIXEL_NUM];            // 记录高亮像素点的坐标
    int displayNum;                                 // 记录每次高亮像素点的数目
    bool bRespondMouse;                             // 标志当前界面是否响应鼠标操作

    int m_nProfile;                                 // 记录当前的方案编号
    int m_nSamColor;                                // 记录当前取样颜色
    int m_nSamSens;                                 // 记录当前取样灵敏度
    int m_nColorSpace;                              // 记录当前颜色空间
    int m_nSamMode;                                 // 记录当前取样模式
    int m_nTriThreshold;                            // 记录当前取样阈值
    int m_nNormalMode;                              // 记录当前是否为普通模式
    int m_nInfMode;                                 // 记录当前是否为红外模式
    bool m_nMultiMode;                              // 记录当前是否为多分类模式
    bool m_bIsSimulate;                             // 标志当前是否处于仿真模式
    int matterPixelSum[MAT_MAX];                    // 记录每种物料像素点的总数

    /* 记录鼠标在显示区域与原始区域中的起始位置与终止位置 */
    int labelXPre;                                  // 记录选中区域在显示区域起始点的横坐标
    int labelYPre;                                  // 记录选中区域在显示区域起始点的纵坐标
    int labelXNext;                                 // 记录选中区域在显示区域终止点的横坐标
    int labelYNext;                                 // 记录选中区域在显示区域终止点的纵坐标
    int startX;                                     // 记录选中区域在原始区域起始点的横坐标
    int startY;                                     // 记录选中区域在原始区域起始点的纵坐标
    int endX;                                       // 记录选中区域在原始区域终止点的横坐标
    int endY;                                       // 记录选中区域在原始区域终止点的纵坐标

    int offsetX;                                    // imageLabel与左边界的距离
    int offsetY;                                    // imageLabel与右边界的距离

    /* 智能分析主页面的控件 */
    QWidget *AIWidget;
    QLabel *mesLabel;
    QLabel *imageLabel;
    QLabel *AIWarningLabel;
    QScrollArea *scrollArea;
    myPushButton *upBtn;
    myPushButton *downBtn;
    myPushButton *leftBtn;
    myPushButton *rightBtn;
    myPushButton *enlargeBtn;
    myPushButton *reduceBtn;
    myPushButton *setBtn;
    myPushButton *deriveBtn;
    myPushButton *simulateBtn;
    myPushButton *reWholeBtn;
    myPushButton *reLastBtn;
    myPushButton *optCalBtn;
    myPushButton *optSampleBtn;
    myPushButton *clearSamBtn;
    myPushButton *matterSetBtn;
    myPushButton *calBtn;
    myPushButton *AIBackBtn;
    myPushButton *lastBtn;
    myPushButton *nextBtn;
    myPushButton *updateBtn;
    myPushButton *imgBtn;
    myLabel      *infoLabel;

    /* 智能分析设置页面的控件 */
    QWidget *AISetWidget;
    myMutex *colorGroup;
    myMutex *colorSpaceGroup;
    myMutex *samModeGroup;
    myPushButton *normalModeBtn;
    myPushButton *infModeBtn;
    myPushButton *multiModeBtn;
    myValueBar *sensValueBar;
    myValueBar *triggerValueBar;
    myPushButton *AISetSureBtn;
    myPushButton *AISetBackBtn;

    /* 物料设置页面的控件 */
    QWidget *AIMatterWidget;
    QLabel *labels[MAT_MAX];
    myPushButton *enableBtns[MAT_MAX];
    myPushButton *nameBtns[MAT_MAX];
    myPushButton *isGoodBtns[MAT_MAX];
    myPushButton *isBadBtns[MAT_MAX];
    QString matterName[MAT_MAX];
    bool isEnable[MAT_MAX];
    bool isGood[MAT_MAX];
    bool isSelected[MAT_MAX];
    myPushButton *setMatterSureBtn;
    myPushButton *setMatterBackBtn;

    /* 物料采集页面*/
    QWidget *AICapImageWidget;
    QSignalMapper *imgSignalMapper;
    myPushButton *captureBtn;
    QLCDNumber *boardNum;
    myPushButton *boardNumPlusBtn;
    myPushButton *boardNumMinusBtn;
    myPushButton *layerBtn;
    myPushButton *commBtn;
    myPushButton *deletCapBtn;
    myPushButton *okCapBtn;
    myPushButton *preCapBtn;
    myPushButton *nextCapBtn;
    myPushButton *cancelCapBtn;
    myGroupBox *leftGroup;
    myGroupBox *rightGroup;
    QHBoxLayout *capHB1Layout;
    QHBoxLayout *capHBLayout;
    QGridLayout *capImgListGrid;
    QGridLayout *capBtnListGridLayout;
    QVBoxLayout *capVBLayout;
    myPushButton *imageBtn[6];
    myPushButton *imageNameBtn[6];
    QTableWidget *tableWidget;
    myPushButton *autoStudyBtn;

    myPushButton *layerAddBtn;         //前视后视切换按钮

    int imgListStat[IMG_MAX_NUM];
    int imgListNum;
    int imgListIndex;
    int imgListCur;
    int imgPageCur;
    QString imageListName[IMG_MAX_NUM];
};

#endif // AIANALYSIS_H
