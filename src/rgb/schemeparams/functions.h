#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QtCore>
#include "globalflow.h"
#include "common/mywid.h"

/*!
 * \enum 页面索引
 */
enum {
    Page_Params,
    Page_Background,
    Page_Feeder_Adjust,
    Page_Grey_A,
    Page_Grey_B,
    Page_Discolor_A,
    Page_Discolor_B,
    Page_Cross,
    Page_Shape,
    Page_Shape_Length,
    Page_Shape_Pole_A,
    Page_Shape_Pole_B,
    Page_Shape_Slice,
    Page_Shape_Leaf,
    Page_Scale,
    Page_Bud_1,
    Page_Bud_2,
    Page_PeanutAbcd_A,
    Page_PeanutAbcd_B,
    Page_PeanutAbcd_C,
    Page_PeanutAbcd_D,
    Page_Reserved_Grey_A,
    Page_Reserved_Grey_B,
    Page_Reserved_Discolor_A,
    Page_Reserved_Discolor_B,
    Page_Reserved_Intel_A,
    Page_Reserved_Intel_B,
    Page_Reserved_Intel_C,
    Page_Reserved_Intel_D,
    Page_Reserved_PeanutAbcd_D,
    Page_Reserved_Shape_Pole_A,
    Page_Intel_A,
    Page_Intel_B,
    Page_Intel_C,
    Page_Intel_D,

    Page_Scale_B,
    Page_Reserved,
    Page_Candy,
    Page_PISTACHIO
};

/*!
 * \enum 算法组合模式
 */
enum {
    MatMode_AllSeperate,    /*!< 全部独立 */
    MatMode_AllSame,        /*!< 全部相同 */
    MatMode_FrontRear,      /*!< 前后视相同 */
    MatMode_Sequence        /*!< 前后视不同 */
};

void removeTabBar(MyTabBar *tabBar);        //! 移除所有标签

void updateTabBar(MyTabBar *tabBar, bool bIsIdentify, bool bIsHide);    //! 更新所有标签

void setIdIndex (MyTabBar *tabBar);         //! 根据TD机型不同层的前后视配置设置TabBar

int getIdIndex(int *idIndex, int index);    //! TD机型根据IdIndex数组值获取对应的识别组真实位置

int getCurrentLevelIndex(int index);        //! TD机型根据识别组在TabBar中的位置获取当前层

int getCurrentIdetifyGroupIndex(int*idIndex, int index);    //! TD机型根据识别组在TabBar中的位置获取当前识别组位置

int getPageIndex(int nArithIndex);          //! 根据算法索引号获取页面ID

int getArithIndex(int pageId);              //! 根据页面ID获取对应算法索引号

QString getColorText(int color);            //! 获取颜色属性对应的文本

QString getLevelText(int level);            //! 获取层数对应的文本

QString getReservedText(int nArithIndex);   //! 根据算法索引号获取保留页面的文本显示

int getReservedIndex(int nArithIndex);      //! 根据算法索引号获取保留页面ID

#endif // FUNCTIONS_H
