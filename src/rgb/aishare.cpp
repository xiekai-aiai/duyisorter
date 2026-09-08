/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aishare.cpp
 * \brief       彩色机型智能分析共享参数源文件
 * \date        2015.01.14
 */
#include "aishare.h"

AIShare::AIShare()
{
    resetAIShareParams();
    memset(m_backMax,0,IMAGE_WIDTH_MAX*3);
    memset(m_backMin,0,IMAGE_WIDTH_MAX*3);
}

/* 重置智能分析的共享参数 */
void AIShare::resetAIShareParams()
{
    m_nImgIndex = -1;

    for(int i=0; i<IMG_MAX_NUM; i++)
    {
        m_nSelPixelNum[i] = 0;
        m_nDisPixelNum[i] = 0;
        m_bIsOptSam[i] = false;
        m_nMatStatus[i] = CLASS_NULL;
    }
}

/* 判断原始图像像素点是否为背景点 */
bool AIShare::pixelIsBackground(uchar r, uchar g, uchar b)
{
    int backgroudID = 0;
    backgroudID = struGsh.nUnit % 2;

    //杂粮自动背景后上传的图像背景为黑（r:0  g:0  b:0）
    if (myFlow.getProductLineNo() == 0) {
        if (struCnfp.nAutoBgEnable[struGsh.nLevel][struGsh.nUnit] == 1) {
            if ((r == 0) && (g == 0) && (b == 0)) {
                return true;
            } else {
                return false;
            }
        }
    }


    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
    case 0: //黑
        if(r+g+b < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3)
            return true;
        break;
    case 1: //红
        if(r>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0*r/(r+g+b)>struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 2: //绿
        if(g>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0*g/(r+g+b)>struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 4: //蓝
        if(b>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0*b/(r+g+b)>struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 7: //白
        if(r+g+b>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3 &&
                r+g+b<struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3+(int)struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID]*2.55)
            return true;
        break;
    default:
        break;
    }
    return false;
}

// 自动背景时，判断原始图像像素点是否为背景点
bool AIShare::pixelIsBackgroundAuto(uchar r, uchar g, uchar b, int pixelIndex)
{
    //像素点超出图片范围
    if(pixelIndex >=IMAGE_WIDTH_MAX){
        return false;
    }
    uchar rMax = m_backMax[pixelIndex*3];
    uchar gMax = m_backMax[pixelIndex*3+1];
    uchar bMax = m_backMax[pixelIndex*3+2];
    uchar rMin = m_backMin[pixelIndex*3];
    uchar gMin = m_backMin[pixelIndex*3+1];
    uchar bMin = m_backMin[pixelIndex*3+2];

    bool rflag = (r<=rMax)&&(r>=rMin);
    bool gflag = (g<=gMax)&&(g>=gMin);
    bool bflag = (b<=bMax)&&(b>=bMin);

    if(rflag&&gflag&&bflag)
    {
        return true;
    }
    else
    {
        //qDebug("r:%d  g:%d  b:%d  rMin:%d  rMax:%d   gMin:%d  gMax:%d   bMin:%d  bMax:%d k:%d\n",r,g,b,rMin,rMax,gMin,gMax,bMin,bMax,pixelIndex);
        return false;
    }
}

// 判断原始图像像素点是否为背景点
bool AIShare::pixelIsBackgroundcomm(uchar r, uchar g, uchar b, int pixelIndex)
{
    return pixelIsBackground(r,g,b);
}


/************************************************
 *descriptipon:下位机自动背景条件下，图片背景信息校验
 *called by: 1. setSorterConsistenceWidget::getImage
 *           2.UnsupervisedAIMainWidget::getImage
 *input:
 *output:
 *return: 0-成功    1-失败
 ************************************************/
bool AIShare::AutoBackgroundDataCheck(char *data)
{
    int retCode = 0;
    char *pTmp;
    char *backgroundMaxAll = NULL;
    char *backgroundMinAll = NULL;
    backgroundMaxAll = (char *)malloc(CAP_WID*3*2);
    if (backgroundMaxAll == NULL)
    {
        printf("malloc failed for getAutoBackgroundData[backgroundMaxAll].\n");
        retCode = -1;
        goto EXIT;
    }
    backgroundMinAll = (char *)malloc(CAP_WID*3*2);
    if (backgroundMinAll == NULL)
    {
        printf("malloc failed for getAutoBackgroundData[backgroundMinAll].\n");
        retCode = -1;
        goto EXIT;
    }

    //取自动背景最大值
    pTmp = data;
    memcpy(backgroundMaxAll,data+(CAP_WID*3*2+FRAME_LEN_NOT_DATA),CAP_WID*3*2);
    for(int n=0; n<CAP_WID*3*2; n++)
    {
        if(pTmp[n] != backgroundMaxAll[n])
        {
            printf("AutoBackground Max data check failed.\n");
            retCode = 1;
            goto EXIT;
        }
    }

    //取自动背景最小值(2048像素)
    pTmp = data+2*(CAP_WID*3*2+FRAME_LEN_NOT_DATA);
    memcpy(backgroundMinAll,data+3*(CAP_WID*3*2+FRAME_LEN_NOT_DATA),IMAGE_WIDTH*3*2);
    for(int n=0; n<CAP_WID*3*2; n++)
    {
        if(pTmp[n] != backgroundMinAll[n])
        {
            printf("AutoBackground Max data check failed.\n");
            retCode = 1;
            goto EXIT;
        }
    }

EXIT:
    fflush(0);
    if (backgroundMaxAll != NULL) {
        free(backgroundMaxAll);
        backgroundMaxAll = NULL;
    }
    if (backgroundMinAll != NULL) {
        free(backgroundMinAll);
        backgroundMinAll = NULL;
    }

    if(retCode !=0)
        return false;
    else
        return true;
}

/*!
 * \brief AIMainWidget::RGBToHSV
 * \param r
 * \param g
 * \param b
 * \return
 */
HSVType AIShare::RGBToHSV(int r, int g, int b)
{
    HSVType hsv;

    double min,max,delta,temp;

    min = MIN(r,MIN(g,b));
    max = MAX(r,MAX(g,b));
    delta = max - min;

    hsv.v = (int)max;
    if(!delta)
    {
        hsv.h = hsv.s = 0;
    }
    else
    {
        temp = delta/max;
        hsv.s = (int)(temp*255);

        if(r == (int)max)
        {
            temp = (double)(g-b)/delta;
        }
        else
            if(g == (int)max)
            {
                temp = 2.0 + ((double)(b-r)/delta);
            }
            else
            {
                temp = 4.0 + ((double)(r-g)/delta);
            }
        temp *= 60;
        if(temp < 0)
        {
            temp+=360;
        }
        if(temp == 360)
        {
            temp = 0;
        }
        hsv.h = (int)temp;
    }
    hsv.x = hsv.s*cos(hsv.h*PI/180.0)+255;
    hsv.y = hsv.s*sin(hsv.h*PI/180.0)+255;
    hsv.z = hsv.v;
    return hsv;
}

AIShare myAIShare;
