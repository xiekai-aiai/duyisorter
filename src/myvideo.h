/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myvideo.h
 * \brief       图像视频显示界面头文件
 * \date        2015.08.17
 */
#ifndef MYVIDEO_H
#define MYVIDEO_H

#include <QWidget>
#include <vector>
#include <QPainter>
#include <QtCore>
#include <QPushButton>
#include <QTimer>
#include <QGroupBox>
#include <QComboBox>
#include <QPixmap>
#include "globalparams.h"
#include "common/mypushbutton.h"
#include "common/mygroupbox.h"
#include "common/mylabel.h"
#include "rgb/aishare.h"
#ifdef Q_OS_UNIX
#include "common/myusb/myusb.h"
#endif
#include <iostream>
#include <cstring>
#include <vector>
#ifdef Q_OS_UNIX
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
// #include <opencv2/opencv.hpp>
#endif


#define VIDEO_IMG_WID   ((struCnfc.nSensorPixelLevel[0]== 1024) ?1024 :2048)
#define VIDEO_SHOW_WID  1024    // 图像数据处理和保存只按照1024的width显示
#define VIDEO_IMG_HEI	256
#define VIDEO_ALL_IMG_HEI 2048

#define BUFFER_SIZE 10*1024*1024  // 修正缓冲区大小计算（10MB）
#define FRAME_HEADER 0xA6A66A6A   // 帧头常量（便于对比）

static const int UPDATE_TIMES = VIDEO_ALL_IMG_HEI/VIDEO_IMG_HEI*4-7;
char *getHead(const char *mem, int size, const char *str);

static QQueue<QByteArray> ImageQueue;
static QMutex imageQMutex;

static const QString g_strVideo = ":/res/png/Video.png";
static const QString g_strMeyer = ":/res/png/Vendor_Meyer.png";

static const QString g_blank = ":/res/png/blank.png";


static int g_nRow = 512;
static double g_fRealThroughPut;
static double g_fRealDirtPer;
static double g_fRealSmallMatPer;
static bool g_bIsSim = false;

static bool m_bIsViewOrUnitChanged = false;
/*!
 * \brief 视频图像采集制器类
 */
class MyVideoThread : public QThread
{
    Q_OBJECT

public:
   MyVideoThread();
   ~MyVideoThread();
   QByteArray getVideoImage(void);  //! 获取视频图像数据
   void dealImageQueue(QByteArray);
   char *simulateImage(const char *buffer, int bufferSize);     //! 对坏点进行标红处理
   void beginStatistic(void);   //! 开始统计
   void endStatistic(void);     //! 结束统计
   void switchUARTModeRowNum(); //! 切换串口模式行数
   bool checkStatisticData(QByteArray data);    //! 检查统计数据

signals:
   void sVideoImageFinished(QByteArray);

public slots:
    void stop(void);

protected:
    void run(void);

private:
    bool         bIsRunning;
    bool	 bIsFront;
    quint64      pixelNum;
    quint64      pixelNum1;
    quint64      badPointNum;
    QByteArray   materialStatic;
    QByteArray   materialStatic1;
    int          nStaticUnit;
    qint64 m_nBeginTime;
    qint64 m_nEndTime;

    bool         bIsUdpReady;
    #ifdef Q_OS_UNIX
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    std::vector<unsigned char> buffer;
    std::vector<unsigned char> img_vec;
    #endif
};

/*!
 * \brief 碎米率计算线程类
 */
class MyCalSmallMatThread : public QThread
{
    Q_OBJECT

public:
   MyCalSmallMatThread();
   ~MyCalSmallMatThread();

   bool getImageData();//QImage image
   void gray(void);                    // 图像二值化
   void erosion(void);                 // 图像的腐蚀运算
   void dilation(void);                // 图像的膨胀运算
   bool IsBack(int x, int y);          // 是否为背景点
   bool DFS(int x, int y, int label);  // DFS深度搜索
   void findMaterial(void);            // 寻找物料点
   void findSmallAndDisplay(void);     // 寻找小物料并显示

signals:
    //void sVideoImageFinished(QImage);

public slots:
    void stop(void);

protected:
    void run(void);

private:
    bool  bIsRunning;
    g_Config *m_config;
    uchar *m_pImgData;
    uchar *m_pDispData;

    bool m_bIsSim;
    QVector <MaterialParams> m_vMatParams;
    int m_nNum;             // 记录当前物料的像素点数目
    int m_nTotal;           // 米总数
    int m_nSmall;           // 碎米总数
    long m_nPixelNum;        // 物料点总数
    int m_nImgWid;          // 采集图像的宽度
    int m_nImgHei;          // 采集图像的高度

    uchar m_backMax[IMAGE_WIDTH_MAX*3];   //自动背景最大值
    uchar m_backMin[IMAGE_WIDTH_MAX*3];   //自动背景最小值
public:
    double m_smallMatPer;   //碎米率
    double m_realThroughPut;  //产量 kg/h
};

/*!
 g_bIsSim 视频图像显示控制类
 */
class MyVideo : public QWidget
{
    Q_OBJECT
public:
    explicit MyVideo(QWidget *parent = 0);
    ~MyVideo();
     void updateVideo();
     void resetLocalParams();

signals:
    void sBoardNumChanged();		// 板号变化信号
    void backToHomePageSig();       // 返回上一级信号
    void goToAutoAnalysisSig();     //跳转到碎米率计算页面信号

public slots:
    void onUnitPlusBtnClicked();	// 响应板号增加按钮按下事件
    void onUnitMinusBtnClicked();	// 响应板号减少按钮按下事件
    void onUnitViewClicked();		// 响应视角切换按钮按下事件
    void onCaptureBtnClicked();		// 停止采集事件
    bool isThreadRunning(void);		// 获取线程状态
    void startCalSmallMatPerRSC(bool bIsCapturing);
    void startCapture(bool bIsCapturing);
    void updateVideoImage(QByteArray img);
    void updateBoardNum();
    void resetCaptureState(bool);
    void resetCaptureMode(bool);
    void onSetBackBtnClicked();      // 响应返回按钮按下事件
    void updateVideoWidget();
    void onRowPlusBtnClicked();
    void onRowMinusBtnClicked();
    void onSimulateBtnClicked();
    void onAutoAnalysisBtnClicked();
    void updateVideoImageQueue(QByteArray);

private:
    bool m_bIsCapturing;		// 视频图像采集标志
    bool m_bOriCapStat;         //进入碎米率仿真之前视频采集状态标志
    MyVideoThread *m_pThread;
    MyCalSmallMatThread *m_pCalSmallMatThread;//碎米率计算线程
    myLabel      *m_pVideoLabel;
    myLabel      *m_pVideoLabel2;
    myGroupBox   *m_pUnitGroup;
    QLCDNumber   *m_pUnitLcdNum;
    myPushButton *m_pUnitViewBtn;
    myPushButton *m_pUnitPlusBtn;
    myPushButton *m_pUnitMinusBtn;
    myPushButton *m_pCaptureBtn;
    QHBoxLayout  *m_pUnitGridLayout;
    QVBoxLayout  *m_pVideoLayout;
    QLCDNumber   *m_Row;
    myPushButton *m_RowMinusBtn;
    myPushButton *m_RowPlusBtn;
    myPushButton *m_pSimulateBtn;
    myPushButton *m_pAutoAnalysisBtn;
    myLabel      *infoLbl;

};

#endif // MYVIDEO_H
