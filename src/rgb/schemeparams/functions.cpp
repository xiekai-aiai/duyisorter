#include "functions.h"

/**
 * @brief removeTab
 * @param tabBar
 * remove all the tabs
 */
void removeTabBar(MyTabBar *tabBar)
{
    for (int i = tabBar->count()-1; i >= 0; i--) {
        tabBar->removeTab(i);
    }
}

/**
 * @brief updateTabBar
 * @param tabBar
 * @param bIsIdentify
 * update all the tabs
 */
void updateTabBar(MyTabBar *tabBar, bool bIsIdentify, bool bIsHide)
{
    /* clear all the items */
    removeTabBar(tabBar);

    /* refresh */
    if(bIsIdentify == false) {
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
                tabBar->insertTab(i*struCnfg.struLevelInfo[i].nTickGroupTotal+j, myString.sTickGroupName[i][j]);
            }
        }
    } else {
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            for (int j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
                tabBar->insertTab(i*struCnfg.struLevelInfo[i].nIdentifyGroupTotal+j, myString.sIdentifyGroupName[i][j]);
            }
        }

        int  caseValue = 0;
        if (tabBar->getTabBarType() == 0) {         //可见光
            caseValue = struCnfp.nMatAssembleMode;
        }

        switch (caseValue) {
        case PARAMS_ALL_SEPARATE:
            for (int i = 0; i < tabBar->count(); i++) {
                tabBar->setTabEnabled(i, true);
            }
            break;
        case PARAMS_FRONT_REAR_SAME:
            for (int i = 0; i < tabBar->count(); i++) {
                if (i%2 == 0)
                    tabBar->setTabEnabled(i, true);
                else
                    tabBar->setTabEnabled(i, false);
            }
            break;
        case PARAMS_FIRST_SECOND_SAME:
            for (int j = 0; j < struCnfg.nLevelTotal; j++) {
                for (int i = 0;i < tabBar->count()/struCnfg.nLevelTotal;i++) {
                    if (i < 2)
                        tabBar->setTabEnabled(i+j*(tabBar->count()/struCnfg.nLevelTotal) ,true);
                    else
                        tabBar->setTabEnabled(i+j*(tabBar->count()/struCnfg.nLevelTotal),false);
                }
            }
            break;
        case PARAMS_ALL_SAME:
            for (int i = 0; i < tabBar->count(); i++) {
                if (i == 0)
                    tabBar->setTabEnabled(i, true);
                else
                    tabBar->setTabEnabled(i, false);
            }
            break;
        default:
            break;
        }
    }
    /* set current index to first page */
    tabBar->setCurrentIndex(FIRST_UNIT);

    /* hide | show */
    tabBar->setHidden(bIsHide);
}


/**
 * @brief 根据TD机型不同层的前后视配置设置TabBar
 * @param MyTabBar *tabBar
 */
void setIdIndex (MyTabBar *tabBar)
{
    int nIndex = 0;
    for (int i = 0; i < MAX_LEVEL*2; i++) {
        struGsh.idIndex[i] = 0;
    }
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        if (struCnfg.struLevelInfo[i].nViewTotal == 1) {
            struGsh.idIndex[nIndex] = 2*i;
            tabBar->insertTab(nIndex, myString.sIdentifyGroupName[i][0]);
            nIndex++;
        } else if (struCnfg.struLevelInfo[i].nViewTotal == 2) {
            //! 前视
            struGsh.idIndex[nIndex] = 2*i;
            tabBar->insertTab(nIndex, myString.sIdentifyGroupName[i][0]);
            nIndex++;

            //! 后视
            struGsh.idIndex[nIndex] = 2*i + 1;
            tabBar->insertTab(nIndex, myString.sIdentifyGroupName[i][1]);
            nIndex++;
        }
    }
}

/**
 *  TD机型根据IdIndex数组值获取对应的识别组真实位置
 */
int getIdIndex(int *idIndex, int index){

    if (index < 0) {
        return -1;
    } else {
        return *(idIndex+index);
    }

}

/**
 * @brief TD机型根据识别组在TabBar中的位置获取当前层
 * @param int *idIndex,int index
 * @return
 */
int getCurrentLevelIndex(int index)
{
    if (index > MAX_LEVEL*2) {
        return 0;
    }

    return struGsh.idIndex[index]/2;
}

/**
 * @brief TD机型根据识别组在TabBar中的位置获取当前识别组位置
 * @param int *inIndex, int index
 * @return
 */
int getCurrentIdetifyGroupIndex(int*idIndex, int index){
    if (struCnfg.nLevelTotal == 3){
        if(idIndex[0] == 0 && idIndex[1] == 2 && idIndex[2] == 4
                && idIndex[3] == 0 && idIndex[4] == 0 && idIndex[5] == 0){
            switch (index){
            case 0:
            case 1:
            case 2:
                return 0;
                break;
            default:
                return 0;
                break;
            }
        }
        if (idIndex[0] == 0 && idIndex[1] == 1
              && idIndex[2] == 2 && idIndex[3] == 4
              && idIndex[4] == 0 && idIndex[5] == 0){
             switch (index){
             case 0:
             case 2:
             case 3:
                return 0;
                break;
             case 1:
                return 1;
                break;
              default:
                 return 0;
                 break;
            }
        }
        if (idIndex[0] == 0 && idIndex[1] == 2
               && idIndex[2] == 3 && idIndex[3] == 4
               && idIndex[4] == 0 && idIndex[5] == 0){
             switch (index){
             case 0:
             case 1:
             case 3:
                return 0;
                break;
              case 2:
                 return 1;
                 break;
              default:
                 return 0;
                 break;
            }
        }
        if (idIndex[0] == 0 && idIndex[1] == 2
               && idIndex[2] == 4 && idIndex[3] == 5
               && idIndex[4] == 0 && idIndex[5] == 0){
             switch (index){
             case 0:
             case 1:
             case 2:
                return 0;
                break;
              case 3:
                 return 1;
                 break;
              default:
                 return 0;
                 break;
            }
        }
        if (idIndex[0] == 0 && idIndex[1] == 1
               && idIndex[2] == 2 && idIndex[3] == 3
               && idIndex[4] == 4 && idIndex[5] == 0){
             switch (index){
             case 0:
             case 2:
             case 4:
                return 0;
                break;
             case 1:
             case 3:
                return 1;
                break;
              default:
                 return 0;
                 break;
            }
        }
        if (idIndex[0] == 0 && idIndex[1] == 1
               && idIndex[2] == 2 && idIndex[3] == 4
               && idIndex[4] == 5 && idIndex[5] == 0){
             switch (index){
             case 0:
             case 2:
             case 3:
                return 0;
                break;
             case 1:
             case 5:
                return 1;
                break;
              default:
                 return 0;
                 break;
            }
        }
        if (idIndex[0] == 0 && idIndex[1] == 2
               && idIndex[2] == 3 && idIndex[3] == 4
               && idIndex[4] == 5 && idIndex[5] == 0){
             switch (index){
             case 0:
             case 1:
             case 3:
                return 0;
                break;
             case 2:
             case 4:
                return 1;
                break;
              default:
                 return 0;
                 break;
            }
        }
        if (idIndex[0] == 0 && idIndex[1] == 1
            && idIndex[2] == 2 && idIndex[3] == 3
              && idIndex[4] == 4 && idIndex[5] == 5){
             switch (index){
             case 0:
             case 2:
             case 4:
                return 0;
                break;
             case 1:
             case 3:
             case 5:
                return 1;
                break;;
              default:
                 return 0;
                 break;
            }
        }
    }
    if(struCnfg.nLevelTotal == 2){
        if(idIndex[0] == 0 && idIndex[1] == 2
                && idIndex[2] == 0 && idIndex[3] == 0){
            switch(index){
            case 0:
            case 1:
                return 0;
                break;
            default:
                return 0;
                break;
            }
        }
        if(idIndex[0] == 0 && idIndex[1] == 1
                && idIndex[2] == 2 &&idIndex[3] == 0){
            switch(index){
            case 0:
            case 2:
                return 0;
                break;
            case 1:
                return 1;
                break;
            default:
                return 0;
                break;
            }
        }
        if(idIndex[0] == 0 && idIndex[1] == 2
                && idIndex[2] == 3 && idIndex[3] == 0){
            switch(index){
            case 0:
            case 1:
                return 0;
                break;
            case 2:
                return 1;
                break;
            default:
                return 0;
                break;
            }
        }
        if(idIndex[0] == 0 && idIndex[1] == 1
                && idIndex[2] == 2 && idIndex[3] == 3){
            switch(index){
            case 0:
            case 2:
                return 0;
                break;
            case 1:
            case 3:
                return 1;
                break;
            default:
                return 0;
                break;
            }
        }
    }

    return 0;
}

/**
  * 根据算法索引号获取页面ID
  */
int getPageIndex(int nArithIndex)
{
    int pageId;
    switch (nArithIndex) {
    case ARITH_GREY_A:
        pageId = Page_Grey_A;
        break;
    case ARITH_GREY_B:
        pageId = Page_Grey_B;
        break;
    case ARITH_DISCOLOR_A:
        pageId = Page_Discolor_A;
        break;
    case ARITH_DISCOLOR_B:
        pageId = Page_Discolor_B;
        break;
    case ARITH_CROSS:
        pageId = Page_Cross;
        break;
    case ARITH_SHAPE:
        pageId = Page_Shape;
        break;
    case ARITH_SHAPE_LENGTH:
        pageId = Page_Shape_Length;
        break;
    case ARITH_SHAPE_POLE_A:
        pageId = Page_Shape_Pole_A;
        break;
    case ARITH_SHAPE_POLE_B:
        pageId = Page_Shape_Pole_B;
    break;
    case ARITH_SHAPE_SLICE:
        pageId = Page_Shape_Slice;
    break;
    case ARITH_SHAPE_LEAF:
        pageId = Page_Shape_Leaf;
    break;
    case ARITH_SCALE:
        pageId = Page_Scale;
        break;
    case ARITH_SCALE_B:
        pageId = Page_Scale_B;
        break;
    case ARITH_BUD_1:
        pageId = Page_Bud_1;
        break;
    case ARITH_BUD_2:
        pageId = Page_Bud_2;
        break;
    case ARITH_PEANUT_A:
        pageId = Page_PeanutAbcd_A;
        break;
    case ARITH_PEANUT_B:
        pageId = Page_PeanutAbcd_B;
        break;
    case ARITH_PEANUT_C:
        pageId = Page_PeanutAbcd_C;
        break;
    case ARITH_PEANUT_D:
        pageId = Page_PeanutAbcd_D;
        break;
    case ARITH_INTEL_A:
        pageId = Page_Intel_A;
        break;
    case ARITH_INTEL_B:
        pageId = Page_Intel_B;
        break;
    case ARITH_INTEL_C:
        pageId = Page_Intel_C;
        break;
    case ARITH_INTEL_D:
        pageId = Page_Intel_D;
        break;
    case ARITH_RESERVED:
        pageId = Page_Reserved;
        break;
    case ARITH_CANDY:
        pageId = Page_Candy;
        break;
    case ARITH_PISTACHIO:
        pageId = Page_PISTACHIO;
        break;
    default:
        pageId = -1;
        break;
    }

    return pageId;
}

/**
  * 根据页面ID获取对应算法索引号
  */
int getArithIndex(int pageId)
{
    int nArithIndex = -1;
    switch (pageId) {
    case Page_Params:
    case Page_Background:
        break;
    case Page_Grey_A:
        nArithIndex = ARITH_GREY_A;
        break;
    case Page_Grey_B:
        nArithIndex = ARITH_GREY_B;
        break;
    case Page_Discolor_A:
        nArithIndex = ARITH_DISCOLOR_A;
        break;
    case Page_Discolor_B:
        nArithIndex = ARITH_DISCOLOR_B;
        break;
    case Page_Cross:
        nArithIndex = ARITH_CROSS;
        break;
    case Page_Shape:
        nArithIndex = ARITH_SHAPE;
        break;
    case Page_Shape_Length:
        nArithIndex = ARITH_SHAPE_LENGTH;
        break;
    case Page_Shape_Pole_A:
        nArithIndex = ARITH_SHAPE_POLE_A;
        break;
    case Page_Shape_Pole_B:
        nArithIndex = ARITH_SHAPE_POLE_B;
        break;
    case Page_Shape_Slice:
        nArithIndex = ARITH_SHAPE_SLICE;
        break;
    case Page_Shape_Leaf:
        nArithIndex = ARITH_SHAPE_LEAF;
        break;
    case Page_Scale:
        nArithIndex = ARITH_SCALE;
        break;
    case Page_Scale_B:
        nArithIndex = ARITH_SCALE_B;
        break;
    case Page_Bud_1:
        nArithIndex = ARITH_BUD_1;
        break;
    case Page_Bud_2:
        nArithIndex = ARITH_BUD_2;
        break;
    case Page_PeanutAbcd_A:
        nArithIndex = ARITH_PEANUT_A;
        break;
    case Page_PeanutAbcd_B:
        nArithIndex = ARITH_PEANUT_B;
        break;
    case Page_PeanutAbcd_C:
        nArithIndex = ARITH_PEANUT_C;
        break;
    case Page_PeanutAbcd_D:
        nArithIndex = ARITH_PEANUT_D;
        break;
    case Page_Reserved_Grey_A:
        nArithIndex = ARITH_GREY_A;
        break;
    case Page_Reserved_Grey_B:
        nArithIndex = ARITH_GREY_B;
        break;
    case Page_Reserved_Discolor_A:
        nArithIndex = ARITH_DISCOLOR_A;
        break;
    case Page_Reserved_Discolor_B:
        nArithIndex = ARITH_DISCOLOR_B;
        break;
    case Page_Reserved_Intel_A:
        nArithIndex = ARITH_INTEL_A;
        break;
    case Page_Reserved_Intel_B:
        nArithIndex = ARITH_INTEL_B;
        break;
    case Page_Reserved_Intel_C:
        nArithIndex = ARITH_INTEL_C;
        break;
    case Page_Reserved_Intel_D:
        nArithIndex = ARITH_INTEL_D;
        break;
    case Page_Reserved_PeanutAbcd_D:
        nArithIndex = ARITH_PEANUT_D;
        break;
    case Page_Reserved_Shape_Pole_A:
        nArithIndex = ARITH_SHAPE_POLE_A;
        break;
    case Page_Candy:
        nArithIndex = ARITH_CANDY;
        break;
    case Page_PISTACHIO:
        nArithIndex = ARITH_PISTACHIO;
        break;
    case Page_Intel_A:
        nArithIndex = ARITH_INTEL_A;
        break;
    case Page_Intel_B:
        nArithIndex = ARITH_INTEL_B;
        break;
    case Page_Intel_C:
        nArithIndex = ARITH_INTEL_C;
        break;
    case Page_Intel_D:
        nArithIndex = ARITH_INTEL_D;
        break;
    case Page_Reserved:
        nArithIndex = ARITH_RESERVED;
        break;
    }

    return nArithIndex;
}

/**
  * 获取颜色属性对应的文本
  */
QString getColorText(int color)
{
    QString text("");
    switch (color) {
    case COLOR_OPT_BLACK:
        text = myLan.black;
        break;
    case COLOR_OPT_R:
        text = myLan.red;
        break;
    case COLOR_OPT_G:
        text = myLan.green;
        break;
    case COLOR_OPT_B:
        text = myLan.blue;
        break;
    case COLOR_OPT_WHITE:
        text = myLan.white;
        break;
    case COLOR_OPT_AUTO:
        text = myLan.automatic;
        break;
    default:
        text = myLan.unknown;
        break;
    }
    return text;
}

/**
  * 获取层数对应的文本
  */
QString getLevelText(int level)
{
    QString text;
    text = QString("%1 %2").arg(myLan.layer).arg(level+1);
    return text;
}

/**
  * 根据算法索引号获取保留页面的文本显示
  */
QString getReservedText(int nArithIndex)
{
    QString text;
    switch (nArithIndex) {
    case ARITH_GREY_A:
        text = QString("%1 %2").arg(myLan.material_defect_grey_a).arg(myLan.reserved);
        break;
    case ARITH_GREY_B:
        text = QString("%1 %2").arg(myLan.material_defect_grey_b).arg(myLan.reserved);
        break;
    case ARITH_DISCOLOR_A:
        text = QString("%1 %2").arg(myLan.material_defect_discolor_a).arg(myLan.reserved);
        break;
    case ARITH_DISCOLOR_B:
        text = QString("%1 %2").arg(myLan.material_defect_discolor_b).arg(myLan.reserved);
        break;
    case ARITH_INTEL_A:
        text = QString("%1 %2").arg(myString.sArithmeticName[ARITH_INTEL_A]).arg(myLan.reserved);
        break;
    case ARITH_INTEL_B:
        text = QString("%1 %2").arg(myString.sArithmeticName[ARITH_INTEL_B]).arg(myLan.reserved);
        break;
    case ARITH_INTEL_C:
        text = QString("%1 %2").arg(myString.sArithmeticName[ARITH_INTEL_C]).arg(myLan.reserved);
        break;
    case ARITH_INTEL_D:
        text = QString("%1 %2").arg(myString.sArithmeticName[ARITH_INTEL_D]).arg(myLan.reserved);
        break;
    case ARITH_PEANUT_D:
        text = QString("%1 %2").arg(myLan.material_defect_d).arg(myLan.reserved);
        break;
    case ARITH_SHAPE_POLE_A:
    case ARITH_SHAPE_POLE_B:
        text = QString("%1 %2").arg(myLan.material_shape_pole).arg(myLan.reserved);
        break;
    case ARITH_RESERVED:
        text = QString("%1 %2").arg(myLan.general).arg(myLan.material_reserved);
        break;
    default:
        break;
    }

    return text;
}

/**
  * 根据算法索引号获取保留页面ID
  */
int getReservedIndex(int nArithIndex)
{
    int pageId = -1;
    switch (nArithIndex) {
    case ARITH_GREY_A:
        pageId = Page_Reserved_Grey_A;
        break;
    case ARITH_GREY_B:
        pageId = Page_Reserved_Grey_B;
        break;
    case ARITH_DISCOLOR_A:
        pageId = Page_Reserved_Discolor_A;
        break;
    case ARITH_DISCOLOR_B:
        pageId = Page_Reserved_Discolor_B;
        break;
    case ARITH_INTEL_A:
        pageId = Page_Reserved_Intel_A;
        break;
    case ARITH_INTEL_B:
        pageId = Page_Reserved_Intel_B;
        break;
    case ARITH_INTEL_C:
        pageId = Page_Reserved_Intel_C;
        break;
    case ARITH_INTEL_D:
        pageId = Page_Reserved_Intel_D;
        break;
    case ARITH_PEANUT_D:
        pageId = Page_Reserved_PeanutAbcd_D;
        break;
    case ARITH_SHAPE_POLE_A:
    case ARITH_SHAPE_POLE_B:
        pageId = Page_Reserved_Shape_Pole_A;
        break;
    default:
        break;
    }

    return pageId;
}
