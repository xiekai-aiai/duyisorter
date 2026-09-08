/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aishare.h
 * \brief       彩色机型智能分析共享参数头文件
 * \date        2015.01.14
 */
#ifndef AISHARE_H
#define AISHARE_H

#include <QtCore>
#include "globalparams.h"
#include "globalflow.h"

#define IMG_MAX_NUM     18

#define MAX(x,y)    ((x)>(y)?(x):(y))
#define MIN(x,y)    ((x)<(y)?(x):(y))
#define PI          3.1415926358

typedef struct stru_ai_hsv{
    int h;
    int s;
    int v;

    int x;
    int y;
    int z;
}HSVType;

/* 定义彩色图像与红外图像的宽高 */
enum{
    IMAGE_WIDTH_MAX  =2048,
    IMAGE_WIDTH  = 1024,
    IMAGE_HEIGHT = 500,
    INF_WIDTH    = 512,
    INF_HEIGHT   = 250,
    FRAME_LEN_NOT_DATA = 16,  //数据包中非数据的字节长度：包头10+包尾6
    FRAME_LEN_NOT_DATA_EN = 8
};

/* 拍照函数的功能模块划分 */
enum {
    CAP_ALL = 0,        //采集准备+上传
    CAP_PREPARE,    //采集准备
    CAP_UPLOAD      //采集上传
};

/* 定义取样点与取样次数的最大值与最小值 */
enum{
    MAX_SELECT_TIMES = 100,
    MAX_PIXEL_NUM = 20000,
    MIN_PIXEL_NUM = 1000
};

/* 定义图像显示的宽高 */
#if(LCD_WIDTH == 640)
    enum{
        DISPLAY_WIDTH = 512,
        DISPLAY_HEIGHT = 348
    };
#elif(LCD_WIDTH == 800)
    enum{
        DISPLAY_WIDTH = 600,
        DISPLAY_HEIGHT = 300
    };
#elif(LCD_WIDTH == 1024)
    enum{
        DISPLAY_WIDTH = 768,
        DISPLAY_HEIGHT = 500
    };
#endif

/* 定义当前图片对应的物料状态 */
/*! \enum mater class property */
enum{
    CLASS_BAD  = -1,                       /*!<< class: bad  property */
    CLASS_GOOD = 1,                        /*!<< class: good property */
    CLASS_NULL = 2                         /*!<< class: no property   */
};

/*! \enum AI Mode */
enum{
    MODE_NULL       = 0,                       /*!<< 禁止智能模式 */
    MODE_MULTIPLE   = 1,                       /*!<< 多分类智能模式 */
    MODE_USUAL      = 2                        /*!<< 传统智能模式 */
};

/*! \enum view No.*/
enum{
    VIEW_ZERO   =  0,   //视零
    VIEW_ONE    =  1,   //视一
    VIEW_TWO    =  2,   //视二
    VIEW_THREE  =  3    //视三
};

/*! \enum protocal No.*/
enum{
    PROTOCAL_0   =  0,  //单帧中1024(R+G+B) + 512(Infra),500帧
    PROTOCAL_1   =  1,  //单帧中2048(R+G+B),500帧
    PROTOCAL_2   =  2,  //单帧中1024(R+G),500帧
    PROTOCAL_3   =  3,   //单帧中2048,500帧（R+辅配相机）
    PROTOCAL_4   =  4    //单帧中1020(R+G+B),500帧
};

/*!
 * \enum 取样颜色类型
 */
enum {
    COLOR_RED = 1,          /*!< 红 */
    COLOR_GREEN = 2,        /*!< 绿 */
    COLOR_BLUE = 4,         /*!< 蓝 */
    COLOR_RED_GREEN = 6,    /*!< 红绿 */
    COLOR_RED_BLUE = 8,     /*!< 红蓝 */
    COLOR_GREEN_BLUE = 10   /*!< 绿蓝 */
};

/*!
 * \brief 彩色机型智能分析共享参数类
 */
class AIShare
{
public:
    AIShare();

    int m_nImgIndex;                        // 图片索引号
    int m_nSelPixelNum[IMG_MAX_NUM];        // 单幅图片已选像素点
    int m_nDisPixelNum[IMG_MAX_NUM];        // 单幅图片显示的已选像素点(是否优化取样)
    bool m_bIsOptSam[IMG_MAX_NUM];          // 单幅图片是否优化取样标志位
    int m_nMatStatus[IMG_MAX_NUM];          // 单幅图片的状态
    uchar m_backMax[IMAGE_WIDTH_MAX*3];   //自动背景最大值
    uchar m_backMin[IMAGE_WIDTH_MAX*3];   //自动背景最小值

    void resetAIShareParams(void);                          // 重置智能分析的共享参数
    bool pixelIsBackground(uchar r, uchar g, uchar b);      // 判断原始图像像素点是否为背景点
    bool pixelIsBackgroundAuto(uchar r, uchar g, uchar b, int pixelIndex);  //自动背景时，判断原始图像像素点是否为背景点
    bool pixelIsBackgroundcomm(uchar r, uchar g, uchar b, int pixelIndex);
    bool AutoBackgroundDataCheck(char *data);
    //void updateBackgroundCondition(uchar * imageData);    //更新自动背景判别条件
    HSVType RGBToHSV(int r, int g, int b);
};

extern AIShare myAIShare;

#endif // AISHARE_H
