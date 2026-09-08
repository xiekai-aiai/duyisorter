/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mycurve.h
 * \brief       波形显示界面头文件
 * \date        2015.01.14
 */
#ifndef MYCURVE_H
#define MYCURVE_H

#include <QWidget>
#include <vector>
#include <QPainter>
#include <QtCore>
#include <QPushButton>
#include <QTimer>
#include <QGroupBox>
#include <QComboBox>
#include "globalparams.h"
#include "common/mypushbutton.h"
#include "common/globalconfig.h"
#include "common/mygroupbox.h"

class MyAxis;

/* 绘制R、G、B的标志 */
enum{
    colorR = 0,
    colorG,
    colorB
};

/* 绘制前后视的标志 */
enum {
    FRONT_VIEW = 0,
    REAR_VIEW,
	ASSIST_FRONT_VIEW,
	ASSIST_REAR_VIEW
};

/* 绘制类型 */
enum {
    TYPE_WAVE = 0,      //! 绘制波形
    TYPE_EJECT_TIMES,   //! 绘制吹气次数
    TYPE_IDE_RESULT     //! 绘制识别结果
};

/* 显示波形的类型索引号 */
enum{
    WAVE_RED = 0,       //! 主配红
    WAVE_GREEN,         //! 主配绿
    WAVE_BLUE,          //! 主配蓝
    WAVE_INF,           //! 红外
    WAVE_GRAY,          //! 灰度
    WAVE_INF_RS,        //! RS辅配
    WAVE_ASSIST_RED,    //! 辅配红
    WAVE_ASSIST_GREEN,  //! 辅配绿
    WAVE_ASSIST_BLUE    //! 辅配蓝
};

/*!
 * \brief 信号采集线程类
 */
class MyCurveThread : public QThread
{
    Q_OBJECT

public:
   MyCurveThread();
   ~MyCurveThread();

   void getLenAndWait(void);    //! 获取波形包长及等待时间

signals:
    void sReceiveData();

public slots:
    void stop(void);
    void onCurveTypeChangedSlt(int type);

protected:
    void run(void);
private:
    int nWaittime;      // 获取波形等待时间
    int nDataLength;    // 获取每帧数据的长度
    bool bIsRunning;
    int nCurveType;    //当前页面的波形类型，0:背景设置，1：红外相机设置，2：可见相机设置
};


/*!
 * \brief 波形显示控制类
 */
class MyCurve : public QWidget
{
    Q_OBJECT
public:
    explicit MyCurve(int width=CURVE_WIDGET_WIDTH,
                     int height=CURVE_WIDGET_HEIGHT,
                     QWidget *parent = 0);
    ~MyCurve();

    void refreshCurveDisplay(void);         // 刷新坐标轴的界面显示
    void setModeBtnStatus(void);            // 根据辅助相机使能情况隐藏或显示模式按钮
    void hidePauseBtn(void);                // 隐藏波形开始/暂停按钮
    void showPauseBtn(void);                // 显示波形开始/暂停按钮
    void hideModeBtn(void);                 // 隐藏相机模式按钮
    void showModeBtn(void);                 // 显示相机模式按钮
    void setBorderShow(bool bFlag);         // 设置是否显示边界线
    void setMultiWaveBtnEnabled(bool bFlag);// 根据标志位设定绘制重叠波形按钮是否使能
    void setMultiWaveEnabled(bool bFlag);   // 设置是否显示重叠波形
    void clearPixmap(void);                 // 清空绘制重叠波形时保存的图像数据
    void setCurveType(int type);            // 设置波形的类型

signals:
    void boardNumChanged();                 // 板号变化信号
    void backgroudCurveTypeChanged(int type);        // 当前背景波形类型变化，0：可见光，1：红外

public slots:
    void onUnitPlusBtnClicked();            // 响应板号增加按钮按下事件
    void onUnitMinusBtnClicked();           // 响应板号减少按钮按下事件
    void onUnitPlusBtnMultiViewClicked();   // 响应玉米定制板号增加按钮按下事件
    void onUnitMinusBtnMultiViewClicked();  // 响应玉米定制板号减少按钮按下事件
    void onRowPauseBtnClicked();            // 响应停止采集波形按钮按下事件
    void onUnitViewClicked();               // 响应前后视切换按钮按下事件
    void onUnitViewAddtionClicked();        // 响应玉米定制新增相机位置切换按钮按下事件
    void onModeBtnClicked(void);            // 响应相机模式按钮按下事件
    void curveStartSlt();                   // 开始采集波形事件
    void curveStopSlt();                    // 停止采集波形事件
    void onMultiWaveBtnPressed(void);       // 响应绘制重叠波形按钮按下事件
    void onUpdatePixmapSlt(void);           // 响应刷新图像的信号
    bool isRunning(void);                   // 获取线程状态

private:
    QFont font;
    bool bIsWorking;                        // 标志当前是否正在采波形
    bool m_bMultiWave;                      // 标志是否显示重叠波形
    QPixmap m_pixmap;                       // 图像用于保存波形页面的显示
    int m_nCurveType;                       // 标志波形的类别,0:背景设置,1：红外相机设置,2：可见光相机设置

    g_Config *config;
    myGroupBox *unitGroup;
    QComboBox *unitComboBox;
    MyCurveThread *curveThread;
    QLCDNumber *unitLcdNum;
    myPushButton *unitViewBtn;
    myPushButton *unitViewAdditionBtn;
    myPushButton *unitPlusBtn;
    myPushButton *unitMinusBtn;
    myPushButton *rowPauseBtn;
    myPushButton *m_modeBtn;
    myPushButton *m_multiWaveBtn;
    MyAxis *axisWidget;
    QHBoxLayout *axisPageHBLayout;
    QGridLayout *unitGridLayout;
    QSpacerItem *horizontalSpacer;
};


/*!
 * \brief 坐标轴及波形绘制类
 */
class MyAxis : public QWidget
{
    Q_OBJECT
public:
    explicit MyAxis(QWidget *parent = 0);
    ~MyAxis();

    void setMinValueX(int value);
    void setMaxValueX(int value);
    void setMinValueY(int value);
    void setMaxValueY(int value);
    void setBorder(int left, int right, int up, int down);      // 设置坐标轴的边界
    int getLeftBorder(void);                            // 获取左边界
    int getRightBorder(void);                           // 获取右边界
    int getUpBorder(void);                              // 获取上边界
    int getDownBorder(void);                            // 获取下边界
    void setTickAttr(void);                             // 设置标记属性
    void setTickNum(void);                              // 设置标记代表的值
    void setTickValue(void);                            // 设置标记的数量
    void paintAxis(QPainter &painter);                  // 绘制整体的坐标轴
    void paintAxisX(QPainter &painter);                 // 绘制坐标轴x
    void paintAxisY(QPainter &painter);                 // 绘制坐标轴y
    void setXByData(void);                              // 根据数据设置横轴坐标
    void setYByData(void);                              // 根据数据设置纵轴坐标
    void drawWanted(QPainter &painter);                 // 绘制想要的波形
    void drawColorWave(QPainter &painter);              // 绘制彩色波形
    void drawGrayWave(QPainter &painter);               // 绘制灰度波形
    void drawSingleWave(QPainter &painter,int flag);    // 根据标志位显示单一的波形
    void getNewdata();                                  // 获取下一帧数据
    void setDrawType(int type);                         // 设置绘制的类型
    void drawEjectTimes(QPainter &painter);             // 绘制吹气次数
    void drawSingleTimes(QPainter &painter, int flag);  // 绘制单视的吹气次数波形
    void drawIdeResult(QPainter &painter);              // 绘制颜色算法识别结果
    void drawSingleResult(QPainter &painter, int flag); // 绘制单个算法识别结果
    void setBorderShow(bool bFlag);                     // 设置边界是否显示
    bool getBorderShow(void);                           // 获取边界是否显示
    void setMultiWaveFlag(bool bFlag);                  // 设置是否绘制重叠波形标志位
    void setPixmap(QPixmap pixmap);                     // 设置需要绘制的图像
    void setSignalType(int type);                       // 设置当前波形所属的相机类型，0：可见光，1：红外

protected:
    virtual void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    int m_nDrawType;
    g_Config *config;
    bool m_bDrawMultiWave;
    QPixmap m_pixmap;          // 图像用于保存波形页面的显示
    int m_nSignalType;              // 用于标志当前波形所属的相机类型，0：可见光，1：红外

    /* 绘制坐标轴类的参数 */
    QFont font;
    int minValueX;
    int maxValueX;
    int minValueY;
    int maxValueY;
    quint8 leftBorder;
    quint8 rightBorder;
    quint8 upBorder;
    quint8 downBorder;
    int minorTickValueX;
    int minorTickValueY;
    int majorTickValueX;
    int majorTickValueY;
    int majorXNum;
    int majorYNum;
    int minorXNum;
    int minorYNum;
    uchar *redData;
    uchar *greenData;
    uchar *blueData;
    qreal valueXPerPixel;
    qreal valueYPerPixel;

    /* 左边界与右边界位置 */
    int m_nLeft;
    int m_nRight;
    bool m_bDrawBoarder;
};

#endif // MYCURVE_H
