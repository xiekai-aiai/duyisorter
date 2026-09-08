/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        autoanalysiswidget.cpp
 * \brief       彩色机型自动分析(碎米率仿真)头文件
 * \date        2017.02.14
 */

#ifndef AUTOANALYSISWIDGET_H
#define AUTOANALYSISWIDGET_H

#include <QWidget>
#include <vector>
#include <QPainter>
#include <QtCore>
#include <QPushButton>
#include <QTimer>
#include <QGroupBox>
#include <QComboBox>
#include "common/mylcdnumber.h"
#include <QPixmap>
#include "globalparams.h"
#include "common/mypushbutton.h"
#include "common/mygroupbox.h"
#include "common/mylabel.h"
#include "common/myvaluebar.h"
#include "rgb/aishare.h"
//#include "myvideo.h"

#ifdef Q_OS_UNIX
#include "common/myusb/myusb.h"
#endif

#define IMG_WID   2048
#define IMG_HEI   500

class AutoAnalysisWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AutoAnalysisWidget(QWidget *parent = 0);
    ~AutoAnalysisWidget();
    /* 图像采集界面布局 */
        void initLocalParams(void);         // 初始化局部参数
        void createPage(void);              // 创建自动分析界面
        void connectSigAndSlt(void);        // 连接界面控件中的信号与槽
        void updatePage();                  // 更新界面
        void refreshPageDisplay();
        void refreshLabelDisplay();

        /* 图像处理部分流程函数 */
        char *getHead(const char *mem, int size, const char *str);          /*!<< 私有函数：获取一帧数据的头指针 */

        bool getImageFromCamera();
        void dealSimulateImage();       // 图片的仿真
        void displayImage();            // 显示未仿真的图片
        void displaySimImage();         // 显示仿真的图片

        /* 图像仿真处理流程函数*/
        bool gray(void);                    // 图像二值化
        void erosion(void);                 // 图像的腐蚀运算
        void dilation(void);                // 图像的膨胀运算
        bool IsBack(int x, int y);          // 是否为背景点
        bool DFS(int x, int y, uint label);  // DFS深度搜索
        void findMaterial(void);            // 寻找物料点
        void findSmallAndDisplay(void);     // 寻找小物料并显示

    signals:

        void backToHomePageSig();
        void backToPageSig();
    public slots:
        void onBackBtnClickedSlt_test();
        void onCaptureBtnClicked();
        void onSimulateBtnClicked();
        void onMatAreaChangedSlt();
        void onPlusChanNumPressedSlt();
        void onMinusChanNumPressedSlt();
        void onLeftBtnClicked();
        void onRightBtnClicked();
    private:
        /* 定义物料参数 */
        struct MaterialParams {
            int nLabel;         // 物料标签号
            int nNum;           // 物料像素点个数
        };

        myLabel        *m_pImgLabel;
        myPushButton   *m_leftBtn;
        myPushButton   *m_rightBtn;
        myLabel        *m_pInfoLbl;
        myValueBar     *m_valueBar;
        myPushButton   *m_backBtn;
        myGroupBox     *m_pPixelNumGroup;
        myGroupBox     *m_pChartGroup;
        QLCDNumber     *m_chanNum;                      /*!<< LCD: 通道数目 */
        myPushButton   *m_plusChanNumBtn;               /*!<< 按钮：增加通道数目 */
        myPushButton   *m_minusChanNumBtn;              /*!<< 按钮：减少通道数目 */
        myPushButton   *m_pCaptureBtn;
        myPushButton   *m_pSimulateBtn;                 /*!<< 按钮：仿真按钮*/

        /* 图像采集页面数据 */
        g_Config *m_config;
        int m_nImgWid;
        int m_nImgHei;
        uchar *m_pImgData;
        uchar *m_pDispData;
        uint  *m_labelData;
        QVector <MaterialParams> m_vMatParams;

        QImage m_img,m_simImg;
        int m_nNum;         // 记录当前物料的像素点数目
        int m_nTotal;       // 米总数
        int m_nSmall;       // 碎米总数
        int m_nPixelNum;    // 物料点总数

        bool m_bIsLeft;     // 标志是否是当前显示的2048图片的左1024
        bool m_bIsSim;      // 仿真状态

    
};

#endif // AUTOANALYSISWIDGET_H
