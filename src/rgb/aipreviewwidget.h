/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aipreviewwidget.h
 * \brief       彩色机型智能分析图片预览界面头文件
 * \date        2015.01.14
 */
#ifndef AIPREVIEWWIDGET_H
#define AIPREVIEWWIDGET_H

#include <QWidget>
#include <QtGui>
#include "aishare.h"
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylcdnumber.h"
#include "common/myinputpanel.h"
#include "common/mymessagebox.h"

#define MIN_SCALE   1
#define MAX_SCALE   8

/*!
 * \enum 像素点类型
 */
enum {
    PIXEL_BACKGROUND = 0,   /*!< 背景点 */
    PIXEL_MATTER = 1,       /*!< 物料点 */
    PIXEL_MATTER_BAD = 2    /*!< 物料坏点 */
};

/* 所有物料图片的公有参数 */
struct AIUniversalParams
{
    int nScale;             // 当前图片的显示尺寸
    int nMaxMoveTimes;      // 水平与垂直的最大移动次数
    int nHMoveTimes;        // 水平移动次数(右移加1，左移减1)
    int nVMoveTimes;        // 垂直移动次数(下移加1，上移减1)
    int nSpaceX;            // 水平移动的间隔
    int nSpaceY;            // 垂直移动的间隔
    int nIndex;             // 显示区域在原始数据中位置的索引号
    bool bHasTar;           // 标记手动框中是否含有物料像素点
    QRect disRect;          // 保存鼠标移动形成的方框坐标信息
    QRect oriRect;          // 显示上述坐标在原始数据中的位置
    QPoint selectedPoint[MAX_PIXEL_NUM];    // 已选择的物料点在原始图像上的坐标
};

/* 每幅物料图片的单独参数 */
struct AIAloneParams
{
    int nSelTimes;                          // 勾选物料点的次数
    int nSelPixelNum[MAX_SELECT_TIMES];     // 记录每一次手选方框中被标记为物料像素点的数目
    int nSelPixelSum;                       // 记录所有被选物料像素点的数目
    int nDisPixelNum[MAX_SELECT_TIMES];     // 记录每一次框选后高亮显示像素点的数目
    int nDisPixelSum;                       // 记录高亮显示的像素点数目
    QColor displayColor;                    // 选中像素点高亮显示的颜色
};

/*!
 * \brief 彩色机型智能分析图片预览界面类
 */
class AIPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AIPreviewWidget(QWidget *parent = 0);

    void initAIDisParams(void);             // 初始化智能分析中的显示参数
    void initAIAloneParams(void);           // 初始化智能分析中的独立参数
    void initAIUniParams(void);             // 初始化智能分析中的公有参数
    void createAIPreviewPage(void);         // 生成智能分析预览页面
    void createMoveBtns(void);              // 生成上下左右按钮
    void getDataAndShow(void);              // 根据当前状态显示图片
    bool getImageData(QString fileName);    // 根据图片索引获取数据
    void refreshSamDisplay(void);           // 刷新取样组的显示
    void refreshAIParamsDisplay(void);      // 刷新智能行、病癍显示
    void refreshBtnStatus(void);            // 根据当前图片的尺寸刷新按钮的状态
    void refreshMatStatusLabel(void);       // 根据当前状态刷新物料状态的显示
    void updateImage(void);                 // 将彩色图片缓冲区中的数据更新到显示页面中
    void updateInfImage(void);              // 将红外图片缓冲区中的数据更新到显示页面中
    void changeImage(void);                 // 根据勾选的情况修改缓冲区中的数据
    void refreshImageDisplay(void);         // 根据优化取样的标志位刷新显示
    void normalAISimulate(uchar *pStatus);  // 普通智能算法下的仿真
    void multiAISimulate(uchar *pStatus);   // 多分类算法下的仿真
    void displaySimulate(void);             // 显示图片仿真后的效果
    void changeImgBySim(uchar *pStatus);    // 根据仿真的效果修改显示的图像
    void resetDisplayColor(void);           // 重置高亮显示的颜色
    void drawSelRect(void);                 // 绘制手选的框
    void getSelPointsFromFile(void);        // 从文件中获取已选物料点的坐标信息
    void saveSelPointsToFile(void);         // 在退出时将选择的物料点保存到文件中

    int getUnitGroup(int level, int unit);                              // 智能算法参数复制设别组号设置
    bool pixelIsMatter(uchar r, uchar g, uchar b, int pixelIndex);                      // 判断像素点是否为物料点
    bool pixelIsSelected(uchar r, uchar g, uchar b);                    // 判断图像中像素点为原始数据点还是被标记的点
    bool pixelIsEdge(int x, int y);                                     // 优化取样时判断像素点是否为边缘点
    bool pixelIsBad(int index, uchar r, uchar g, uchar b);              // 普通智能算法下判断像素点是否为坏点
    bool multiPixelIsBad(int r, int g, int b);                    // 多分类智能算法下判断像素点是否为坏点

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    
signals:
    void backToAIMainPageSig(void);         // 返回智能分析主页面的信号
    
public slots:
    /* 图片预览页面相关槽函数 */
    void refreshDisplaySlt();
    void onDelCurImgSlt();
    void onDelAllImgSlt();

    /* 页面控件对应的槽函数 */
    void onReLastBtnClicked();
    void onOptSamBtnClicked();
    void onBackBtnClicked();
    void onUpBtnClicked();
    void onDownBtnClicked();
    void onLeftBtnClicked();
    void onRightBtnClicked();
    void onEnlargeBtnClicked();
    void onReduceBtnClicked();
    void onSimulateBtnClicked();
    void onStartPurSimBtnClicked();
    void onInfModeBtnClicked();
    void onSamColorBtnClicked();
    void onSamModeBtnClicked();
    void onSamSensLcdPressed();
    void onAISensLetPressed();
    void onAIRowLetPressed();
    void onAIPercentLetPressed();

private:
    g_Config *m_config;
    int m_nImgIndex;
    bool m_bIsSimulate;
    bool m_bIsPurSim;
    int m_nDisPixelNum;
    QPoint m_disPoints[MAX_PIXEL_NUM];
    bool m_bFlgTest;

    /* 记录鼠标在显示区域与原始区域中的起始位置与终止位置 */
    int m_nDisStartX;           // 记录选中区域在显示区域起始点的横坐标
    int m_nDisStartY;           // 记录选中区域在显示区域起始点的纵坐标
    int m_nDisEndX;             // 记录选中区域在显示区域终止点的横坐标
    int m_nDisEndY;             // 记录选中区域在显示区域终止点的纵坐标
    int m_nOriStartX;           // 记录选中区域在原始图片起始点的横坐标
    int m_nOriStartY;           // 记录选中区域在原始图片起始点的纵坐标
    int m_nOriEndX;             // 记录选中区域在原始图片终止点的横坐标
    int m_nOriEndY;             // 记录选中区域在原始图片终止点的纵坐标
    int m_nDistanceX;           // 图片显示区域与左边界的距离
    int m_nDistanceY;           // 图片显示区域与右边界的距离

    /* 图像预览页面的相关控件 */
    myLabel *m_imgLabel;
    myPushButton *m_upBtn;
    myPushButton *m_downBtn;
    myPushButton *m_leftBtn;
    myPushButton *m_rightBtn;
    myPushButton *m_enlargeBtn;
    myPushButton *m_reduceBtn;
    myPushButton *m_simulateBtn;
    myPushButton *m_puritySimBtn;
    myPushButton *m_infModeBtn;
    myPushButton *m_samColorBtn;
    myPushButton *m_samModeBtn;
    myLCDNumber *m_samSensLcd;
    myPushButton *m_relastBtn;
    myPushButton *m_optSamBtn;
    myPushButton *m_backBtn;
    myLabel *m_matStatusLabel;
    myLabel *m_pixelColorLabel;
    QGroupBox *m_moreGroup;

    myLabel *AISensLabel;
    myLineEdit *AISensLbe;
    myLabel *AIRowLabel;
    myLineEdit *AIRowLbe;
    myLabel *AIPercentLabel;
    myLineEdit *AIPercentLbe;
};

#endif // AIPREVIEWWIDGET_H
