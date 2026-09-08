#include "arith.h"

/* 构造识别算法参数页面 */
PageArith::PageArith(int nArithIndex, QWidget *parent)
    : QWidget(parent)
{
    setArithIndex(nArithIndex);

/* page init */
    createPage();

/* page update */
    updatePage();

/* page layout */
    layoutPage();
}

/* 应用当前识别算法参数设置 */
void PageArith::setArithParams()
{
    /* write params to memory */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
        for (int j = 0; j < step; j++) {
            struCnfp.struGroupIdentify[i][j] = m_struGroupIdentify[i*step+j];
        }
    }

    /* write params to fpga */
    switch(struCnfe.nMachine) {
    case MACHINE_CF:
        myFlow.materialCopyAssemble(0, tabBar->currentIndex(), 0, m_nArithIndex, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, tabBar->currentIndex(), 0, m_nArithIndex, 0);
        break;
    default:
        break;
    }

    /* 重置延迟时间 */
    myFlow.resetEjectTime();
}

/* 重置当前识别算法参数设置 */
void PageArith::resetArithParams()
{
/* reset params */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
        for (int j = 0; j < step ; j++) {
            m_struGroupIdentify[i*step+j] = struCnfp.struGroupIdentify[i][j];
        }
    }

/* reset page */
    emit pageUpdated();
}

/* 设置当前识别算法索引号 */
void PageArith::setArithIndex(int index)
{
    m_nArithIndex = index;
}

/* 获取当前识别算法索引号 */
int PageArith::getArithIndex()
{
    return m_nArithIndex;
}

/* 响应杂质名称命名设置 */
void PageArith::setImpurityNameSlt()
{
    int tabIndex = tabBar->currentIndex();

    myMessageBox msgBox(MSG_QUES, myLan.cfm_rename_impurity);

    if (msgBox.exec() == QDialog::Accepted) {
        myInputMethod inputDlg(myLan.msg_input_name, nameValue->text());
        if (inputDlg.exec() == QDialog::Accepted) {
            QString text = inputDlg.getText();

            /* update global params */
            switch (m_nArithIndex) {
            case ARITH_GREY_A:
            case ARITH_GREY_B:
            case ARITH_DISCOLOR_A:
            case ARITH_DISCOLOR_B:
                memcpy(m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_CROSS:
                memcpy(m_struGroupIdentify[tabIndex].struCross.sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SHAPE:
                memcpy(m_struGroupIdentify[tabIndex].struShape.sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SHAPE_LENGTH:
                memcpy(m_struGroupIdentify[tabIndex].struShape.sLengthName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SHAPE_POLE_A:
                memcpy(m_struGroupIdentify[tabIndex].struPole.sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SHAPE_POLE_B:
                memcpy(m_struGroupIdentify[tabIndex].struPole.sPoleName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SHAPE_SLICE:
                memcpy(m_struGroupIdentify[tabIndex].struSlice.sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SHAPE_LEAF:
                memcpy(m_struGroupIdentify[tabIndex].struLeaf.sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SCALE:
                memcpy(m_struGroupIdentify[tabIndex].struScale[0].sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_SCALE_B:
                memcpy(m_struGroupIdentify[tabIndex].struScale[1].sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_BUD_1:
            case ARITH_BUD_2:
                memcpy(m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].sName, text.toLatin1(), MAX_NAME);
                break;
            case ARITH_PEANUT_A:
            case ARITH_PEANUT_B:
            case ARITH_PEANUT_C:
            case ARITH_PEANUT_D:
                memcpy(m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].sName, text.toLatin1(), MAX_NAME);
                break;
            }

            /* update button text */
            nameValue->setText(text);
        }
    }
}

void PageArith::setColorModeValueSlt(int index){

    int tabIndex = tabBar->currentIndex();

    QString text;
    double tmp;
    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex].nColor = 1 << index/2;
        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode = index%2;
        tmp = index%2 == 0 ? m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin :
                           m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax;
        text.sprintf("%.1f%%", tmp*0.1);
        break;
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex].nDiscolor = index/2;
        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode = index%2;
        tmp = index%2 == 0 ? m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin :
                           m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax;
        text.sprintf("%.2f%%", tmp*0.01);
        break;
    case ARITH_CROSS:
    case ARITH_SHAPE:
    case ARITH_SHAPE_LENGTH:
    case ARITH_SHAPE_POLE_A:
    case ARITH_SHAPE_POLE_B:
    case ARITH_SHAPE_SLICE:
    case ARITH_SHAPE_LEAF:
    case ARITH_SCALE:
    case ARITH_SCALE_B:
    case ARITH_BUD_1:
    case ARITH_BUD_2:
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_B:
    case ARITH_PEANUT_C:
    case ARITH_PEANUT_D:
        break;
    }
    sensValue->setText(text);
}

/* 响应颜色选择设置 */
void PageArith::setColorValueSlt(int index)
{
    int tabIndex = tabBar->currentIndex();


    switch (m_nArithIndex) {
//    case ARITH_GREY_A:
//    case ARITH_GREY_B:
//        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex].nColor = 1 << index;
//        break;
//    case ARITH_DISCOLOR_A:
//    case ARITH_DISCOLOR_B:
//        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex].nDiscolor = index;
//        break;
    case ARITH_CROSS:
        m_struGroupIdentify[tabIndex].struCross.nColor = 1 << index;
        break;
    case ARITH_SHAPE:
    case ARITH_SHAPE_LENGTH:
    case ARITH_SHAPE_POLE_A:
    case ARITH_SHAPE_POLE_B:
    case ARITH_SHAPE_SLICE:
    case ARITH_SHAPE_LEAF:
    case ARITH_SCALE:
    case ARITH_SCALE_B:
    case ARITH_BUD_1:
    case ARITH_BUD_2:
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_B:
    case ARITH_PEANUT_C:
    case ARITH_PEANUT_D:
        break;
    }
}

/* 响应模式选择设置 */
void PageArith::setModeValueSlt(int index)
{
    QString text;
    double tmp;
    int tabIndex = tabBar->currentIndex();


    switch (m_nArithIndex) {
//    case ARITH_GREY_A:
//    case ARITH_GREY_B:
//        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode = index;
//        tmp = index == 0 ? m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin :
//                           m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax;
//        text.sprintf("%.1f%%", tmp*0.1);
//        break;
//    case ARITH_DISCOLOR_A:
//    case ARITH_DISCOLOR_B:
//        m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode = index;
//        tmp = index == 0 ? m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin :
//                           m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax;
//        text.sprintf("%.2f%%", tmp*0.01);
//        break;
    case ARITH_CROSS:
        break;
    case ARITH_SHAPE:
        m_struGroupIdentify[tabIndex].struShape.nMode = index;
        tmp = index == 0 ? m_struGroupIdentify[tabIndex].struShape.nAreaMax :
                           m_struGroupIdentify[tabIndex].struShape.nAreaMin;
        text.sprintf("%.1f%%", tmp*0.1);
        break;
    case ARITH_SHAPE_LENGTH:
        m_struGroupIdentify[tabIndex].struShape.nLengthMode = index;
        tmp = index == 0 ? m_struGroupIdentify[tabIndex].struShape.nLengthMin :
                           m_struGroupIdentify[tabIndex].struShape.nLengthMax;
        text.sprintf("%.1f%%", tmp*0.1);
        break;
    case ARITH_SHAPE_POLE_A:
    case ARITH_SHAPE_POLE_B:
    case ARITH_SHAPE_SLICE:
    case ARITH_SHAPE_LEAF:
        break;
    case ARITH_SCALE:
        m_struGroupIdentify[tabIndex].struScale[0].nMode = index;
        tmp = m_struGroupIdentify[tabIndex].struScale[0].nValue;
        text = QString("%1").arg(tmp);
        break;
    case ARITH_SCALE_B:
        m_struGroupIdentify[tabIndex].struScale[1].nMode = index;
        tmp = m_struGroupIdentify[tabIndex].struScale[1].nValue;
        text = QString("%1").arg(tmp);
        break;
    case ARITH_BUD_1:
    case ARITH_BUD_2:
        break;
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_C:
    case ARITH_PEANUT_D:
        break;
    case ARITH_PEANUT_B:
        m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nMode = index;
        tmp = index == 0 ? m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMin :
                           m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMax;
        text.sprintf("%.1f%%", tmp*0.1);
        break;
    }

    sensValue->setText(text);
}

/* 响应灵敏度参数设置 */
void PageArith::setSensValueSlt()
{
    QString text;
    inputType type;
    int tabIndex;
    double tmp, min, max;

    /* get current value */
    tabIndex = tabBar->currentIndex();


    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
        type = floatType;
        min = 0;
        max = 100;
        if (m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode == MODE_OPT_DARK)
            tmp = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin*0.1;
        else
            tmp = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax*0.1;
        break;
    case ARITH_DISCOLOR_A://高精度色差灵敏度,精确到小数点后两位可调
    case ARITH_DISCOLOR_B:
        type = floatType;
        min = 0;
        max = 100;
        if (m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode == MODE_OPT_DARK)
            tmp = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin*0.01;
        else
            tmp = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax*0.01;
        break;
    case ARITH_CANDY:
        type = floatType;
        min = 0;
        max = 100;
        tmp = QString().sprintf("%.1f",(255-m_struGroupIdentify[tabIndex].struCandy.nRation1)/2.55).toDouble();
        break;
    case ARITH_CROSS:
        type = floatType;
        min = 0;
        max = 100;
        tmp = m_struGroupIdentify[tabIndex].struCross.nSens*0.1;
        break;
    case ARITH_SHAPE:
        type = floatType;
        min = 0;
        max = 100;
        if (m_struGroupIdentify[tabIndex].struShape.nMode == MODE_OPT_DARK)
            tmp = m_struGroupIdentify[tabIndex].struShape.nAreaMax*0.1;
        else
            tmp = m_struGroupIdentify[tabIndex].struShape.nAreaMin*0.1;
        break;
    case ARITH_SHAPE_LENGTH:
        type = floatType;
        min = 0;
        max = 100;
        if (m_struGroupIdentify[tabIndex].struShape.nLengthMode == MODE_OPT_DARK)
            tmp = m_struGroupIdentify[tabIndex].struShape.nLengthMin*0.1;
        else
            tmp = m_struGroupIdentify[tabIndex].struShape.nLengthMax*0.1;
        break;
    case ARITH_SHAPE_POLE_A:
        type = intType;
        min = 0;
        max = 255;
        tmp = m_struGroupIdentify[tabIndex].struPole.nArea;
        break;
    case ARITH_SHAPE_POLE_B:
        type = intType;
        min = 0;
        max = 255;
        tmp = m_struGroupIdentify[tabIndex].struPole.nPolePercent;
        break;
    case ARITH_SHAPE_SLICE:
        type = intType;
        min = 0;
        max = 1023;
        tmp = m_struGroupIdentify[tabIndex].struSlice.nArea;
        break;
    case ARITH_SHAPE_LEAF:
        type = intType;
        min = 0;
        max = 255;
        tmp = m_struGroupIdentify[tabIndex].struLeaf.nThreshold;
        break;
    case ARITH_SCALE:
        type = intType;
        min = 0;
        if (struCnfc.nSensorType == SENSOR_T_2566) {
            max = 4095;
        } else {
            max = 65535;
        }
        tmp = m_struGroupIdentify[tabIndex].struScale[0].nValue;
        break;
    case ARITH_SCALE_B:
        type = intType;
        min = 0;
        if (struCnfc.nSensorType == SENSOR_T_2566) {
            max = 4095;
        } else {
            max = 65535;
        }
        tmp = m_struGroupIdentify[tabIndex].struScale[1].nValue;
        break;
    case ARITH_BUD_1:
    case ARITH_BUD_2:
        type = floatType;
        min = 0;
        max = 100;
        tmp = m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nArea;
       break;
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_B:
    case ARITH_PEANUT_C:
    case ARITH_PEANUT_D:
        type = floatType;
        min = 0;
        max = 100;
        if (m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nMode == MODE_OPT_DARK)
            tmp = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMin*0.1;
        else
            tmp = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMax*0.1;
        break;
    case ARITH_PISTACHIO:
        type = intType;
        min = 0;
        max = 255;
        tmp = m_struGroupIdentify[tabIndex].struPistachio.nAreaMin;
        break;
    }

    /* trigger input dialog */
    myInputPanel inputDlg(type, min, max, tmp);
    if (inputDlg.exec() == QDialog::Accepted) {

        /* get input value */
        tmp = inputDlg.getValue();

        /* update params */
        switch (m_nArithIndex) {
        case ARITH_GREY_A:
        case ARITH_GREY_B:
            if (m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode == MODE_OPT_DARK)
                m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin = tmp*10+0.5;
            else
                m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax = tmp*10+0.5;
            text.sprintf("%.1f%%", tmp);
            break;
        case ARITH_DISCOLOR_A://高精度色差灵敏度,精确到小数点后两位可调
        case ARITH_DISCOLOR_B:
            if (m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode == MODE_OPT_DARK)
                m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin = tmp*100.0+0.5;
            else
                m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax = tmp*100.0+0.5;
            text.sprintf("%.2f%%", tmp);
            break;
        case ARITH_CANDY:
            m_struGroupIdentify[tabIndex].struCandy.nRation1 = (100-tmp)*2.55+0.5;
            text.sprintf("%.1f%%",tmp);
            break;
        case ARITH_CROSS:
            m_struGroupIdentify[tabIndex].struCross.nSens = tmp*10;
            text.sprintf("%.1f%%", tmp);
            break;
        case ARITH_SHAPE:
            if (m_struGroupIdentify[tabIndex].struShape.nMode == MODE_OPT_DARK)
                m_struGroupIdentify[tabIndex].struShape.nAreaMax = tmp*10;
            else
                m_struGroupIdentify[tabIndex].struShape.nAreaMin = tmp*10;

            text.sprintf("%.1f%%", tmp);
            break;
        case ARITH_SHAPE_LENGTH:
            if (m_struGroupIdentify[tabIndex].struShape.nLengthMode == MODE_OPT_DARK)
                m_struGroupIdentify[tabIndex].struShape.nLengthMin = tmp*10;
            else
                m_struGroupIdentify[tabIndex].struShape.nLengthMax = tmp*10;

            text.sprintf("%.1f%%", tmp);
            break;
        case ARITH_SHAPE_POLE_A:
            m_struGroupIdentify[tabIndex].struPole.nArea = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_SHAPE_POLE_B:
            m_struGroupIdentify[tabIndex].struPole.nPolePercent = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_SHAPE_SLICE:
            m_struGroupIdentify[tabIndex].struSlice.nArea = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_SHAPE_LEAF:
            m_struGroupIdentify[tabIndex].struLeaf.nThreshold = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_SCALE:
            m_struGroupIdentify[tabIndex].struScale[0].nValue = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_SCALE_B:
            m_struGroupIdentify[tabIndex].struScale[1].nValue = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_BUD_1:
        case ARITH_BUD_2:
            m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nArea = tmp;
            text.sprintf("%.1f%%", tmp);
            //text = QString("%1").arg(tmp);
            break;
        case ARITH_PEANUT_A:
        case ARITH_PEANUT_B:
        case ARITH_PEANUT_C:
        case ARITH_PEANUT_D:
            if (m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nMode == MODE_OPT_DARK)
                m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMin = tmp*10+0.5;
            else
                m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMax = tmp*10+0.5;

            text.sprintf("%.1f%%", tmp);
            break;
        case ARITH_PISTACHIO:
            m_struGroupIdentify[tabIndex].struPistachio.nAreaMin = tmp;
            text = QString("%1").arg(tmp);
            break;
        }
        printf("%d\n",m_struGroupIdentify[tabIndex].struCandy.nRation1);

        fflush(0);
        /* update page */
        sensValue->setText(text);

        /* write params */
        setArithParams();
    }
}

/* 响应红外灵敏度上限参数设置 */
void PageArith::setSens2ValueSlt()
{
    QString text;
    inputType type;
    int tabIndex;
    double tmp, min, max;

    /* get current value */
    tabIndex = tabBar->currentIndex();


    switch (m_nArithIndex) {
    case ARITH_SHAPE_POLE_A:
        type = intType;
        min = 0;
        max = 255;
        tmp = m_struGroupIdentify[tabIndex].struPole.nPercent;
        break;
    case ARITH_SHAPE_POLE_B:
        type = intType;
        min = 0;
        max = 255;
        tmp = m_struGroupIdentify[tabIndex].struPole.nPoleWidthMax;
        break;
    case ARITH_SHAPE_LEAF:
        type = intType;
        min = 0;
        max = 31;
        tmp = m_struGroupIdentify[tabIndex].struLeaf.nWidthMax;
        break;
    case ARITH_CANDY:
        type = floatType;
        min = 0;
        max = 100;
        tmp = QString().sprintf("%.1f",(255-m_struGroupIdentify[tabIndex].struCandy.nRation2)/2.55).toDouble();
        break;
    case ARITH_PISTACHIO:
        type = intType;
        min = 0;
        max = 255;
        tmp = 255 - m_struGroupIdentify[tabIndex].struPistachio.nGrayThreshold_1;
        break;
    default:
        break;
    }

    /* trigger input dialog */
    myInputPanel inputDlg(type, min, max, tmp);
    if (inputDlg.exec() == QDialog::Accepted) {

        /* get input value */
        tmp = inputDlg.getValue();

        /* update params */
        switch (m_nArithIndex) {
        case ARITH_SHAPE_POLE_A:
            m_struGroupIdentify[tabIndex].struPole.nPercent = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_SHAPE_POLE_B:
            m_struGroupIdentify[tabIndex].struPole.nPoleWidthMax = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_SHAPE_LEAF:
            m_struGroupIdentify[tabIndex].struLeaf.nWidthMax = tmp;
            text = QString("%1").arg(tmp);
            break;
        case ARITH_CANDY:
            m_struGroupIdentify[tabIndex].struCandy.nRation2 = (100-tmp)*2.55+0.5;
            text.sprintf("%.1f%%",tmp);
            break;
        case ARITH_PISTACHIO:
            m_struGroupIdentify[tabIndex].struPistachio.nGrayThreshold_1 = 255-tmp;
            text = QString("%1").arg(tmp);
            break;
        default:
            break;
        }

        /* update page */
        sensValue2->setText(text);

    /* write params */
        setArithParams();
    }
}

/* 响应行列尺寸参数设置 */
void PageArith::setRowValueSlt()
{
    int tabIndex;
    int tmp, row, col, per;

    tabIndex = tabBar->currentIndex();


    /* get current value */
    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        tmp = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nRow;
        per = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nPercent;
        break;
    case ARITH_CROSS:
        tmp = m_struGroupIdentify[tabIndex].struCross.nRow;
        per = m_struGroupIdentify[tabIndex].struCross.nPercent;
        break;
    case ARITH_CANDY:
        tmp = m_struGroupIdentify[tabIndex].struCandy.nCycles;
        break;
    case ARITH_SHAPE:
    case ARITH_SHAPE_LENGTH:
    case ARITH_SHAPE_POLE_A:
    case ARITH_SHAPE_POLE_B:
    case ARITH_SHAPE_SLICE:
    case ARITH_SHAPE_LEAF:
    case ARITH_SCALE:
    case ARITH_SCALE_B:
        break;
    case ARITH_BUD_1:
    case ARITH_BUD_2:
        tmp = m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nRow;
        per = m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nPercent;
        break;
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_B:
    case ARITH_PEANUT_C:
    case ARITH_PEANUT_D:
        tmp = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nRow;
        per = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nPercent;
        break;
    }

    /* trigger input dialog */
    int max = struGsh.maxRowNumber;
    if(m_nArithIndex == ARITH_CANDY)
    {
        max= 32;
    }

    myInputPanel inputDlg(intType, 1, max, tmp);
    if (inputDlg.exec() == QDialog::Accepted) {

        /* get input value */
        tmp = inputDlg.getValue();
        row = tmp;
        col = tmp*struGsh.rowColumnRelation;
        per = per < row*col ? per : row*col;

        /* update params */
         switch (m_nArithIndex) {
        case ARITH_GREY_A:
        case ARITH_GREY_B:
        case ARITH_DISCOLOR_A:
        case ARITH_DISCOLOR_B:
            m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nRow = row;
            m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nColumn = col;
            m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nPercent = per;
            break;
        case ARITH_CROSS:
            m_struGroupIdentify[tabIndex].struCross.nRow = row;
            m_struGroupIdentify[tabIndex].struCross.nColumn = col;
            m_struGroupIdentify[tabIndex].struCross.nPercent = per;
            break;
         case ARITH_CANDY:
             m_struGroupIdentify[tabIndex].struCandy.nCycles = row;
             break;
        case ARITH_SHAPE:
        case ARITH_SHAPE_LENGTH:
        case ARITH_SHAPE_POLE_A:
        case ARITH_SHAPE_POLE_B:
        case ARITH_SHAPE_SLICE:
        case ARITH_SHAPE_LEAF:
        case ARITH_SCALE:
        case ARITH_SCALE_B:
            break;
        case ARITH_BUD_1:
        case ARITH_BUD_2:
            m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nRow = row;
            m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nColumn = col;
            m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nPercent = per;
            break;
        case ARITH_PEANUT_A:
        case ARITH_PEANUT_B:
        case ARITH_PEANUT_C:
        case ARITH_PEANUT_D:
            m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nRow = row;
            m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nColumn = col;
            m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nPercent = per;
            break;
        }

        /* update page */
        rowValue->setText(QString().setNum(row));
        percentValue->setText(QString().setNum(per));

        /* write params */
        setArithParams();
    }
}

/* 响应纯度参数设置 */
void PageArith::setPercentValueSlt()
{
    int tabIndex, tmp, row, col, per, min, max;

    tabIndex = tabBar->currentIndex();


    /* get current value */
    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        tmp = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nPercent;
        row = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nRow;
        col = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nColumn;
        min = 1;
        max = row*col;
        break;
    case ARITH_CROSS:
        tmp = m_struGroupIdentify[tabIndex].struCross.nPercent;
        row = m_struGroupIdentify[tabIndex].struCross.nRow;
        col = m_struGroupIdentify[tabIndex].struCross.nColumn;
        min = 1;
        max = row*col;
        break;
    case ARITH_SHAPE:
    case ARITH_SHAPE_LENGTH:
    case ARITH_SCALE:
    case ARITH_SCALE_B:
        break;
    case ARITH_SHAPE_POLE_A:
        tmp = m_struGroupIdentify[tabIndex].struPole.nWidthMin;
        min = 0;
        max = 31;
        break;
    case ARITH_SHAPE_POLE_B:
        tmp = m_struGroupIdentify[tabIndex].struPole.nPoleArea;
        min = 1;
        max = 31;
        break;
    case ARITH_SHAPE_SLICE:
        break;
    case ARITH_SHAPE_LEAF:
        tmp = m_struGroupIdentify[tabIndex].struLeaf.nPercent;
    min = 0;
    max = 255;
    break;
    case ARITH_BUD_1:
    case ARITH_BUD_2:
        tmp = m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nPercent;
        row = m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nRow;
        col = m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nColumn;
        min = 1;
        max = row*col;
        break;
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_B:
    case ARITH_PEANUT_C:
    case ARITH_PEANUT_D:
        tmp = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nPercent;
        row = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nRow;
        col = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nColumn;
        min = 1;
        max = row*col;
        break;
    case ARITH_PISTACHIO:
        tmp = m_struGroupIdentify[tabIndex].struPistachio.nGrayThreshold_2;
        min = 0;
        max = 255;
        break;
    }

    /* trigger input dialog */
    myInputPanel inputDlg(intType, min, max, tmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        tmp = inputDlg.getValue();
        per = tmp;

        /* update params */
        switch (m_nArithIndex) {
        case ARITH_GREY_A:
        case ARITH_GREY_B:
        case ARITH_DISCOLOR_A:
        case ARITH_DISCOLOR_B:
            m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nPercent = per;
            break;
        case ARITH_CROSS:
            m_struGroupIdentify[tabIndex].struCross.nPercent = per;
            break;
        case ARITH_SHAPE:
        case ARITH_SHAPE_LENGTH:
        case ARITH_SCALE:
        case ARITH_SCALE_B:
            break;
        case ARITH_SHAPE_POLE_A:
            m_struGroupIdentify[tabIndex].struPole.nWidthMin = per;
            break;
        case ARITH_SHAPE_POLE_B:
            if(tmp<10)
            {
                per = 10;
            }
            m_struGroupIdentify[tabIndex].struPole.nPoleArea = per;
            break;
        case ARITH_SHAPE_SLICE:
            break;
        case ARITH_SHAPE_LEAF:
            m_struGroupIdentify[tabIndex].struLeaf.nPercent = per;
            break;
        case ARITH_BUD_1:
        case ARITH_BUD_2:
            m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nPercent = per;
            break;
        case ARITH_PEANUT_A:
        case ARITH_PEANUT_B:
        case ARITH_PEANUT_C:
        case ARITH_PEANUT_D:
            m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nPercent = per;
            break;
        case ARITH_PISTACHIO:
            m_struGroupIdentify[tabIndex].struPistachio.nGrayThreshold_2 = per;
            break;
        }

        /* update page */
        percentValue->setText(QString().setNum(per));

    /* write params */
    setArithParams();
    }
}

/* 响应阈值下限设置 */
void PageArith::setLimitValueSlt()
{
    int tabIndex, min, max;
    double tmp;
    QString text;
    inputType type;

    /* get current value */
    tabIndex = tabBar->currentIndex();


    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
        break;
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        type = intType;
        min  = 1;
        max  = 255;
        tmp  = m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nLightLimit;
        break;
    case ARITH_CROSS:
        break;
    case ARITH_SHAPE:
        type = intType;
        min = 0;
        if (struCnfc.nSensorType == SENSOR_T_2566) {
            max = 255;
        } else {
            max = 65535;
        }
        tmp = m_struGroupIdentify[tabIndex].struShape.nAreaLimit;
        break;
    case ARITH_SHAPE_LENGTH:
        type = intType;
        min = 0;
        max = 255*(struGsh.maxRowNumber/16)*(struCnfc.nSensorPixelLevel[0]/SENSOR_PIXELS_1024);
        tmp = m_struGroupIdentify[tabIndex].struShape.nLengthLimit;
        break;
    case ARITH_SHAPE_POLE_A:
        type = intType;
        min = 0;
        max = 31;
        tmp = m_struGroupIdentify[tabIndex].struPole.nWidthMax;
        break;
    case ARITH_SHAPE_POLE_B:
    case ARITH_SHAPE_SLICE:
    break;
    case ARITH_SHAPE_LEAF:
    break;
    case ARITH_SCALE:
        type = intType;
        min = 0;
        if (struCnfc.nSensorType == SENSOR_T_2566) {
            max = 255;
        } else {
            max = 65535;
        }
        tmp = m_struGroupIdentify[tabIndex].struScale[0].nValueLimit;
        break;
    case ARITH_SCALE_B:
        type = intType;
        min = 0;
        if (struCnfc.nSensorType == SENSOR_T_2566) {
            max = 255;
        } else {
            max = 65535;
        }
        tmp = m_struGroupIdentify[tabIndex].struScale[1].nValueLimit;
        break;
    case ARITH_BUD_1:
    case ARITH_BUD_2:
        type = intType;
        min  = 1;
        max  = struGsh.maxRowNumber*struGsh.maxRowNumber*struGsh.rowColumnRelation;
        tmp  = m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nPercentLimit;
        break;
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_B:
    case ARITH_PEANUT_C:
        type = floatType;
        min  = 0;
        max  = 100;
        tmp = m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensReserved*0.1;
        break;
    case ARITH_PEANUT_D:
        break;
    case ARITH_PISTACHIO:
        type = intType;
        min = 0;
        max = 255;
        tmp = m_struGroupIdentify[tabIndex].struPistachio.nGrayThreshold_3;
        break;
    }

    /* trigger input dialog */
    myInputPanel inputDlg(type, min, max, tmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        tmp = inputDlg.getValue();
/* update params */
        switch (m_nArithIndex) {
        case ARITH_GREY_A:
        case ARITH_GREY_B:
            break;
        case ARITH_DISCOLOR_A:
        case ARITH_DISCOLOR_B:
            m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nLightLimit = tmp;
            break;
        case ARITH_CROSS:
            break;
        case ARITH_SHAPE:
            m_struGroupIdentify[tabIndex].struShape.nAreaLimit = tmp;
           break;
        case ARITH_SHAPE_LENGTH:
             m_struGroupIdentify[tabIndex].struShape.nLengthLimit = tmp;
            break;
        case ARITH_SHAPE_POLE_A:
            m_struGroupIdentify[tabIndex].struPole.nWidthMax = tmp;
            break;
        case ARITH_SHAPE_POLE_B:
        case ARITH_SHAPE_SLICE:
        break;
        case ARITH_SHAPE_LEAF:
            break;
        case ARITH_SCALE:
            m_struGroupIdentify[tabIndex].struScale[0].nValueLimit = tmp;
            break;
        case ARITH_SCALE_B:
            m_struGroupIdentify[tabIndex].struScale[1].nValueLimit = tmp;
            break;
        case ARITH_BUD_1:
        case ARITH_BUD_2:
            m_struGroupIdentify[tabIndex].struBud[m_nArithIndex-ARITH_BUD_1].nPercentLimit = tmp;
            break;
        case ARITH_PEANUT_A:
        case ARITH_PEANUT_B:
        case ARITH_PEANUT_C:
            m_struGroupIdentify[tabIndex].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensReserved = tmp*10;
            break;
        case ARITH_PEANUT_D:
            break;
        case ARITH_PISTACHIO:
            m_struGroupIdentify[tabIndex].struPistachio.nGrayThreshold_3 = tmp;
            break;
        }

        /* update page */
        if ((m_nArithIndex == ARITH_PEANUT_A) || (m_nArithIndex == ARITH_PEANUT_B) || (m_nArithIndex == ARITH_PEANUT_C)) {
            text.sprintf("%.1f%%", tmp);
            limitValue->setText(text);
        }
        else {
            limitValue->setText(QString().setNum(tmp));
        }

        /* write params */
        setArithParams();
    }
}

/* 响应焦糊保留参数勾选框设置 */
void PageArith::setReservedStateSlt(bool enable)
{
    int tabIndex = tabBar->currentIndex();
    //reservedButton->setEnabled(enable);

    m_struGroupIdentify[tabIndex].struGreyColor[m_nArithIndex-ARITH_GREY_A].nBurntReservedFlag = enable;
    setArithParams();
}

/* 响应保留参数按键设置，并触发对应算法索引号的保留页面ID */
void PageArith::changeReservedPageSlt()
{
    emit pageChanged(getReservedIndex(m_nArithIndex));
}

/* 响应tab页面切换 */
void PageArith::changeCurrentSlt(int index1)
{
    QString text;
    int color, mode, row, per;
    double sens,limit;
    int index = index1;
    /* prevent where is no tab page */
    if (index < 0) {
        return;
    }


    /* display page contens */
    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].sName);
        nameValue->setText(text);

        /* color */
        color = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nColor/2;
//        colorRadio->setCurrentIndex(color);

        /* mode */
        mode = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode;
//        modeRadio->setCurrentIndex(mode);

        colorModeRadio->setCurrentIndex(2*color+mode);
        /* sens */
        if (mode == MODE_OPT_DARK)
            sens = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin*0.1;
        else
            sens = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax*0.1;

        text.sprintf("%.1f%%", sens);
        sensValue->setText(text);

        /* row */
        row = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nRow;
        rowValue->setText(QString("%1").arg(row));

        /* percent */
        per = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nPercent;
        percentValue->setText(QString("%1").arg(per));

        /* limit */
        limit = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nLightLimit;
        limitValue->setText(QString("%1").arg(limit));
        break;
    case ARITH_CANDY:
        /* sens1 */
        sens = (255-m_struGroupIdentify[index].struCandy.nRation1)*0.392;
        text.sprintf("%.1f%%", sens);
        sensValue->setText(text);

        /* sens2 */
        sens = (255-m_struGroupIdentify[index].struCandy.nRation2)*0.392;
        text.sprintf("%.1f%%", sens);
        sensValue2->setText(text);

        /* cycles*/
        row = m_struGroupIdentify[index].struCandy.nCycles;
        rowValue->setText(QString("%1").arg(row));
        break;
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].sName);
        nameValue->setText(text);

        /* color */
        color = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nDiscolor;
//        colorRadio->setCurrentIndex(color);

        /* mode */
        mode = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nMode;
//        modeRadio->setCurrentIndex(mode);

        colorModeRadio->setCurrentIndex(2*color+mode);

        /* sens */
        if (mode == MODE_OPT_DARK)
            sens = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMin*0.01;
        else
            sens = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nSensMax*0.01;

        text.sprintf("%.2f%%", sens);
        sensValue->setText(text);

        /* row */
        row = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nRow;
        rowValue->setText(QString("%1").arg(row));

        /* percent */
        per = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nPercent;
        percentValue->setText(QString("%1").arg(per));

        /* limit */
        limit = m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nLightLimit;
        limitValue->setText(QString("%1").arg(limit));
        break;
    case ARITH_CROSS:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struCross.sName);
        nameValue->setText(text);

        /* color */
        color = m_struGroupIdentify[index].struCross.nColor/2;
        colorRadio->setCurrentIndex(color);

        /* mode */

        /* sens */
        sens = m_struGroupIdentify[index].struCross.nSens*0.1;
        text.sprintf("%.1f%%", sens);
        sensValue->setText(text);

        /* row */
        row = m_struGroupIdentify[index].struCross.nRow;
        rowValue->setText(QString("%1").arg(row));

        /* percent */
        per = m_struGroupIdentify[index].struCross.nPercent;
        percentValue->setText(QString("%1").arg(per));

        /* limit */
        break;
    case ARITH_SHAPE:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struShape.sName);
        nameValue->setText(text);

        /* color */

        /* mode */
        mode = m_struGroupIdentify[index].struShape.nMode;
        modeRadio->setCurrentIndex(mode);

        /* sens */
        if (mode == MODE_OPT_DARK)
            sens = m_struGroupIdentify[index].struShape.nAreaMax*0.1;
        else
            sens = m_struGroupIdentify[index].struShape.nAreaMin*0.1;

        text.sprintf("%.1f%%", sens);
        sensValue->setText(text);

        /* row */

        /* percent */

        /* limit */
        /* limit */
        limit = m_struGroupIdentify[index].struShape.nAreaLimit;
        limitValue->setText(QString("%1").arg(limit));
        break;
    case ARITH_SHAPE_LENGTH:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struShape.sLengthName);
        nameValue->setText(text);

        /* color */

        /* mode */
        mode = m_struGroupIdentify[index].struShape.nLengthMode;
        modeRadio->setCurrentIndex(mode);

        /* sens */
        if (mode == MODE_OPT_DARK)
            sens = m_struGroupIdentify[index].struShape.nLengthMin*0.1;
        else
            sens = m_struGroupIdentify[index].struShape.nLengthMax*0.1;

        text.sprintf("%.1f%%", sens);
        sensValue->setText(text);

        /* row */

        /* percent */

        /* limit */
        limit = m_struGroupIdentify[index].struShape.nLengthLimit;
        limitValue->setText(QString("%1").arg(limit));
        break;
    case ARITH_SHAPE_POLE_A:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struPole.sName);
        nameValue->setText(text);
        /* color */
        /* mode */

        /* sens */
        sens = m_struGroupIdentify[index].struPole.nArea;
        text = QString("%1").arg(sens);
        sensValue->setText(text);

    /* precent */
        sens = m_struGroupIdentify[index].struPole.nPercent;
        text = QString("%1").arg(sens);
        sensValue2->setText(text);

        /* minimumize width */
    per = m_struGroupIdentify[index].struPole.nWidthMin;
    text = QString("%1").arg(per);
    percentValue->setText(text);

        /* maximumize width */
        limit = m_struGroupIdentify[index].struPole.nWidthMax;
    text = QString("%1").arg(limit);
        limitValue->setText(text);
        break;
    case ARITH_SHAPE_POLE_B:
    /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struPole.sPoleName);
        nameValue->setText(text);
        /* color */
        /* mode */

        /* sensitivity */
        sens = m_struGroupIdentify[index].struPole.nPolePercent;
        text = QString("%1").arg(sens);
        sensValue->setText(text);

    /* width max limit */
        sens = m_struGroupIdentify[index].struPole.nPoleWidthMax;
        text = QString("%1").arg(sens);
        sensValue2->setText(text);

    /* percent */
        sens = m_struGroupIdentify[index].struPole.nPoleArea;
        text = QString("%1").arg(sens);
        percentValue->setText(text);
        break;
    case ARITH_SHAPE_SLICE:
        /* slice name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struSlice.sName);
        nameValue->setText(text);

        /* slice area */
        sens = m_struGroupIdentify[index].struSlice.nArea;
        text = QString("%1").arg(sens);
        sensValue->setText(text);
    break;
    case ARITH_SHAPE_LEAF:
    /* leaf name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struLeaf.sName);
        nameValue->setText(text);

    /* leaf sens */
        sens = m_struGroupIdentify[index].struLeaf.nThreshold;
        text = QString("%1").arg(sens);
        sensValue->setText(text);

    /* leaf percent */
        sens = m_struGroupIdentify[index].struLeaf.nWidthMax;
        text = QString("%1").arg(sens);
        sensValue2->setText(text);

    /* leaf width max */
        sens = m_struGroupIdentify[index].struLeaf.nPercent;
        text = QString("%1").arg(sens);
        percentValue->setText(text);
    break;
    case ARITH_SCALE:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struScale[0].sName);
        nameValue->setText(text);

        /* color */

        /* mode */
        mode = m_struGroupIdentify[index].struScale[0].nMode;
        modeRadio->setCurrentIndex(mode);

        /* sens */
        sens = m_struGroupIdentify[index].struScale[0].nValue;
        sensValue->setText(QString("%1").arg(sens));

        /* row */

        /* percent */

        /* limit */
        limit = m_struGroupIdentify[index].struScale[0].nValueLimit;
        limitValue->setText(QString("%1").arg(limit));
        break;
    case ARITH_SCALE_B:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struScale[1].sName);
        nameValue->setText(text);

        /* color */

        /* mode */
        mode = m_struGroupIdentify[index].struScale[1].nMode;
        modeRadio->setCurrentIndex(mode);

        /* sens */
        sens = m_struGroupIdentify[index].struScale[1].nValue;
        sensValue->setText(QString("%1").arg(sens));

        /* row */

        /* percent */

        /* limit */
        limit = m_struGroupIdentify[index].struScale[1].nValueLimit;
        limitValue->setText(QString("%1").arg(limit));
        break;
    case ARITH_BUD_1:
    case ARITH_BUD_2:
        /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struBud[m_nArithIndex-ARITH_BUD_1].sName);
        nameValue->setText(text);

        /* color */

        /* mode */

        /* sens */
        sens = m_struGroupIdentify[index].struBud[m_nArithIndex-ARITH_BUD_1].nArea;
        text.sprintf("%.1f%%", sens);
        sensValue->setText(QString("%1").arg(text));

        /* row */
        row = m_struGroupIdentify[index].struBud[m_nArithIndex-ARITH_BUD_1].nRow;
        rowValue->setText(QString("%1").arg(row));

        /* percent */
        per = m_struGroupIdentify[index].struBud[m_nArithIndex-ARITH_BUD_1].nPercent;
        percentValue->setText(QString("%1").arg(per));

        /* limit */
        limit = m_struGroupIdentify[index].struBud[m_nArithIndex-ARITH_BUD_1].nPercentLimit;
        limitValue->setText(QString("%1").arg(limit));
        break;
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_B:
    case ARITH_PEANUT_C:
    case ARITH_PEANUT_D:
         /* name */
        text = QString().fromLatin1(m_struGroupIdentify[index].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].sName);
        nameValue->setText(text);

        /* color */

        /* mode */
        mode = m_struGroupIdentify[index].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nMode;
        modeRadio->setCurrentIndex(mode);

        /* sens */
        if (mode == MODE_OPT_DARK)
            sens = m_struGroupIdentify[index].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMin*0.1;
        else
            sens = m_struGroupIdentify[index].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensMax*0.1;

        text.sprintf("%.1f%%", sens);
        sensValue->setText(text);

        /* row */
        row = m_struGroupIdentify[index].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nRow;
        rowValue->setText(QString("%1").arg(row));

        /* percent */
        per = m_struGroupIdentify[index].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nPercent;
        percentValue->setText(QString("%1").arg(per));

        /* limit */
        limit = m_struGroupIdentify[index].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].nSensReserved*0.1;
        text.sprintf("%.1f%%", limit);
        limitValue->setText(text);

        break;

    case ARITH_PISTACHIO:
        /* sens */
        sens = m_struGroupIdentify[index].struPistachio.nAreaMin;
        sensValue->setText(QString("%1").arg(sens));

        /* sens2 */
        sens = 255-m_struGroupIdentify[index].struPistachio.nGrayThreshold_1;
        sensValue2->setText(QString("%1").arg(sens));

        /* percent */
        per = m_struGroupIdentify[index].struPistachio.nGrayThreshold_2;
        percentValue->setText(QString("%1").arg(per));

        /* limit */
        limit = m_struGroupIdentify[index].struPistachio.nGrayThreshold_3;
        limitValue->setText(QString("%1").arg(limit));

        break;
    }

    /* reserved group */
    reservedCheckBox->setVisible(false);
    if (struCnfe.nEnableMaizeDoubleView == 1) {
        if (m_nArithIndex == ARITH_GREY_A) {
            reservedCheckBox->setChecked(m_struGroupIdentify[index].struGreyColor[m_nArithIndex-ARITH_GREY_A].nBurntReservedFlag);
            reservedCheckBox->setVisible(true);
        }
    }

    updateRsvPerDisplay();
}

/*!
 * \brief 保留比例
 */
void PageArith::onRsvPerLePressed()
{
    int nVal = m_rsvPerLe->text().remove(QChar('%')).toInt();
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 100, nVal);

    if (inputPanel->exec() == QDialog::Accepted) {
        int nGroup = tabBar->currentIndex();
        nVal = inputPanel->getValue();
        struCnfp.struGroupIdentify[struGsh.nLevel][nGroup].struGreyColor[m_nArithIndex].struReserved.nPercent = nVal;
        m_struGroupIdentify[nGroup].struGreyColor[m_nArithIndex].struReserved.nPercent = nVal;
        m_rsvPerLe->setText(QString("%1%").arg(nVal));

        /* write params */
        setArithParams();
    }
}

/* 初始化识别算法参数页面 */
void PageArith::createPage()
{
    /* tab widget */
    tabBar = new MyTabBar();

    /* name group */
    nameGroup  = new myGroupBox(myLan.defect_name);
    nameValue  = new myLineEdit(tr(""));
    nameValue->setAlignment(Qt::AlignCenter);
    nameValue->setMinimumHeight(ICON_HEI);
    nameValue->setMaximumHeight(BTN_HEIGHT);

    /* color & mode group */
    modeGroup = new myGroupBox(myLan.mode);
    QStringList colorList = QStringList() << myLan.red
                                          << myLan.green
                                          << myLan.blue;
    colorRadio = new MyButtonGroup(colorList);

    QStringList modeList = QStringList() << myLan.dark
                                         << myLan.light;
    modeRadio = new MyButtonGroup(modeList);

    /* sens group */
    sensGroup = new myGroupBox(myLan.sensitivity);

//    QStringList colorModeList =QStringList() << "红低"
//                                             << "红高"
//                                             << "绿低"
//                                             << "绿高"
//                                             << "蓝低"
//                                             << "蓝高";
    QStringList colorModeList =QStringList() << myLan.red+myLan.lower
                                             << myLan.red+myLan.upper
                                             << myLan.green+myLan.lower
                                             << myLan.green+myLan.upper
                                             << myLan.blue+myLan.lower
                                             << myLan.blue+myLan.upper;
    colorModeRadio = new MyButtonGroup(colorModeList);

    sensLabel = new myLabel(myLan.sensitivity);
    sensValue = new myLineEdit(tr(""));
    sensValue->setMinimumHeight(ICON_HEI);
    sensValue->setMaximumHeight(BTN_HEIGHT);
    sensValue->setMaximumWidth(BTN_WIDTH);

    sensLabel2 = new myLabel(myLan.sensitivity_maximum);
    sensValue2 = new myLineEdit(tr(""));
    sensValue2->setMinimumHeight(ICON_HEI);
    sensValue2->setMaximumHeight(BTN_HEIGHT);
    sensValue2->setMaximumWidth(BTN_WIDTH);

    rowLabel = new myLabel(myLan.scale);
    rowValue = new myLineEdit(tr(""));
    rowValue->setMinimumHeight(ICON_HEI);
    rowValue->setMaximumHeight(BTN_HEIGHT);
    rowValue->setMaximumWidth(BTN_WIDTH);

    percentLabel = new myLabel(myLan.purity);
    percentValue = new myLineEdit(tr(""));
    percentValue->setMinimumHeight(ICON_HEI);
    percentValue->setMaximumHeight(BTN_HEIGHT);
    percentValue->setMaximumWidth(BTN_WIDTH);

    limitLabel = new myLabel(myLan.light_limit);
    limitValue = new myLineEdit(tr(""));
    limitValue->setMinimumHeight(ICON_HEI);
    limitValue->setMaximumHeight(BTN_HEIGHT);
    limitValue->setMaximumWidth(BTN_WIDTH);

    /* reserved group */
    reservedGroup = new myGroupBox(myLan.reserved);
    reservedCheckBox = new MyCheckBox(myLan.reserved_burnt);
    reservedButton   = new myPushButton(myLan.reserved, QIcon(), true, true);
    reservedButton->setMinimumHeight(BTN_HEIGHT);

    //! reserved percent
    m_rsvPerLbl = new myLabel(myLan.proportion);
    m_rsvPerLe = new myLineEdit;
    m_rsvPerLbl->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_rsvPerLe->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

/* signals to slots */
    connect(tabBar,           SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(nameValue,        SIGNAL(pressed()),           this, SLOT(setImpurityNameSlt()));
    connect(sensValue,        SIGNAL(pressed()),           this, SLOT(setSensValueSlt()));
    connect(sensValue2,       SIGNAL(pressed()),           this, SLOT(setSens2ValueSlt()));
    connect(rowValue,         SIGNAL(pressed()),           this, SLOT(setRowValueSlt()));
    connect(percentValue,     SIGNAL(pressed()),           this, SLOT(setPercentValueSlt()));
    connect(limitValue,       SIGNAL(pressed()),           this, SLOT(setLimitValueSlt()));
    connect(colorRadio,       SIGNAL(currentChanged(int)), this, SLOT(setColorValueSlt(int)));
    connect(modeRadio,        SIGNAL(currentChanged(int)), this, SLOT(setModeValueSlt(int)));
    connect(colorModeRadio,        SIGNAL(currentChanged(int)), this, SLOT(setColorModeValueSlt(int)));

    connect(reservedCheckBox, SIGNAL(toggled(bool)),       this, SLOT(setReservedStateSlt(bool)));
    connect(reservedButton,   SIGNAL(pressed()),           this, SLOT(changeReservedPageSlt()));
    connect(m_rsvPerLe, SIGNAL(pressed()), this, SLOT(onRsvPerLePressed()));

    connect(this, SIGNAL(pageUpdated()), this, SLOT(updatePage()));
}

/* 根据算法索引号显示不同的页面内容 */
void PageArith::displayPage()
{
    QStringList colorList;
    QStringList modeList;
    QStringList colorModeList;

    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
        modeGroup->show();
        reservedGroup->show();
        colorList = QStringList() << myLan.red
                                  << myLan.green
                                  << myLan.blue;
        modeList = QStringList()  << myLan.dark
                                  << myLan.light;
        colorRadio->setText(colorList);
        modeRadio->setText(modeList);
        colorRadio->hide();
        modeRadio->hide();
//        colorModeList =QStringList() << "红低"
//                                     << "红高"
//                                     << "绿低"
//                                     << "绿高"
//                                     << "蓝低"
//                                     << "蓝高";

        colorModeList =QStringList()  << myLan.red+myLan.lower
                                      << myLan.red+myLan.upper
                                      << myLan.green+myLan.lower
                                      << myLan.green+myLan.upper
                                      << myLan.blue+myLan.lower
                                      << myLan.blue+myLan.upper;
        colorModeRadio->setText(colorModeList);
        colorModeRadio->show();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->show();
        rowValue->show();
        percentLabel->setText(myLan.purity);
        percentLabel->show();
        percentValue->show();
        limitLabel->hide();
        limitValue->hide();
        break;
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        modeGroup->show();
        reservedGroup->show();
        colorList = QStringList() << myLan.red_green
                                  << myLan.red_blue
                                  << myLan.green_blue;
        modeList = QStringList()  << myLan.dark
                                  << myLan.light;
        colorRadio->setText(colorList);
        modeRadio->setText(modeList);
        colorRadio->hide();
        modeRadio->hide();
//        colorModeList =QStringList() << "红<绿"
//                                                 << "红>绿"
//                                                 << "红<蓝"
//                                                 << "红>蓝"
//                                                 << "绿<蓝"
//                                                 << "绿>蓝";
        colorModeList =QStringList() << myLan.red+"<"+myLan.green
                                     << myLan.red+">"+myLan.green
                                     << myLan.red+"<"+myLan.blue
                                     << myLan.red+">"+myLan.blue
                                     << myLan.green+"<"+myLan.blue
                                     << myLan.green+">"+myLan.blue;
        colorModeRadio->setText(colorModeList);
        colorModeRadio->show();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->show();
        rowValue->show();
        percentLabel->setText(myLan.purity);
        percentLabel->show();
        percentValue->show();
        limitLabel->setText(myLan.light_limit);
        limitLabel->show();
        limitValue->show();
        break;
    case ARITH_CROSS:
        modeGroup->show();
        reservedGroup->hide();
        colorList = QStringList() << myLan.red
                                  << myLan.green
                                  << myLan.blue;
        colorRadio->setText(colorList);
        colorRadio->show();
        modeRadio->hide();
        colorModeRadio->hide();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->show();
        rowValue->show();
        percentLabel->setText(myLan.purity);
        percentLabel->show();
        percentValue->show();
        limitLabel->hide();
        limitValue->hide();
        break;
    case ARITH_SHAPE:
        modeGroup->show();
        reservedGroup->hide();
        modeList = QStringList() << myLan.sort_circular
                                 << myLan.sort_long;
        modeRadio->setText(modeList);
        colorRadio->hide();
        modeRadio->show();
        colorModeRadio->hide();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->hide();
        percentValue->hide();
        limitLabel->setText(myLan.params_limit);
        limitLabel->show();
        limitValue->show();
        break;
    case ARITH_SHAPE_LENGTH:
        modeGroup->show();
        reservedGroup->hide();
        modeList = QStringList() << myLan.sort_short
                                 << myLan.sort_long;
        modeRadio->setText(modeList);
        colorRadio->hide();
        modeRadio->show();
        colorModeRadio->hide();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->hide();
        percentValue->hide();
        limitLabel->setText(myLan.params_limit);
        limitLabel->show();
        limitValue->show();
        break;
    case ARITH_SHAPE_POLE_A:
        modeGroup->hide();
        reservedGroup->show();
        sensLabel->setText(myLan.area);
        sensLabel2->setText(myLan.purity);
        sensLabel2->show();
        sensValue2->show();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->setText(myLan.min_limit);
        percentLabel->show();
        percentValue->show();
        limitLabel->setText(myLan.max_limit);
        limitLabel->show();
        limitValue->show();
        break;
    case ARITH_SHAPE_POLE_B:
        modeGroup->hide();
        reservedGroup->hide();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->setText(myLan.max_limit);
        percentLabel->setText(myLan.scale);
        sensLabel2->show();
        sensValue2->show();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->show();
        percentValue->show();
        limitLabel->hide();
        limitValue->hide();
        break;
    case ARITH_SHAPE_SLICE:
        modeGroup->hide();
        reservedGroup->hide();
        sensLabel->setText(myLan.area);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->hide();
        percentValue->hide();
        limitLabel->hide();
        limitValue->hide();
        break;
    case ARITH_SHAPE_LEAF:
        modeGroup->hide();
        reservedGroup->hide();
        sensLabel->setText(myLan.threshold);
        sensLabel2->setText(myLan.max_limit);
        sensLabel2->show();
        sensValue2->show();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->setText(myLan.purity);
        percentLabel->show();
        percentValue->show();
        limitLabel->hide();
        limitValue->hide();
    break;
    case ARITH_SCALE:
    case ARITH_SCALE_B:
        modeGroup->show();
        reservedGroup->hide();
        modeList = QStringList() << myLan.sort_small
                                 << myLan.sort_big;
        modeRadio->setText(modeList);
        colorRadio->hide();
        colorModeRadio->hide();
        modeRadio->show();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->hide();
        percentValue->hide();
        limitLabel->setText(myLan.params_limit);
        limitLabel->show();
        limitValue->show();
        break;
    case ARITH_BUD_1:
    case ARITH_BUD_2:
        modeGroup->hide();
        reservedGroup->hide();
        switch (struCnfc.nSensorType) {
        case SENSOR_K:
        case SENSOR_T_2566:
            percentLabel->show();
            percentValue->show();
            sensLabel->setText(myLan.area);
            break;
        case SENSOR_T_2564:
        case SENSOR_D2:
            percentLabel->hide();
            percentValue->hide();
            sensLabel->setText(myLan.sensitivity);
            break;
        default:
            break;
        }
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->show();
        rowValue->show();
        limitLabel->setText(myLan.params_limit);
        limitLabel->show();
        limitValue->show();
        break;
    case ARITH_PEANUT_A:
    case ARITH_PEANUT_C:
        modeGroup->hide();
        reservedGroup->hide();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->show();
        rowValue->show();
        percentLabel->setText(myLan.purity);
        percentLabel->show();
        percentValue->show();
        if (m_nArithIndex == ARITH_PEANUT_A) {
            limitLabel->setText(myLan.reserved_dark_red);
        }
        else {
            limitLabel->setText(myLan.reserved_peeled);
        }

        limitLabel->show();
        limitValue->show();
        break;
    case ARITH_PEANUT_B:
        modeList = QStringList()  << myLan.dark
                                  << myLan.light;
        modeRadio->setText(modeList);
        modeGroup->show();
        colorRadio->hide();
        colorModeRadio->hide();
        reservedGroup->hide();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->show();
        rowValue->show();
        percentLabel->setText(myLan.purity);
        percentLabel->show();
        percentValue->show();
        limitLabel->setText(myLan.reserved_dark_red);
        limitLabel->show();
        limitValue->show();
        break;
     case ARITH_PEANUT_D:
        modeGroup->hide();
        reservedGroup->show();
        sensLabel->setText(myLan.sensitivity);
        sensLabel2->hide();
        sensValue2->hide();
        rowLabel->show();
        rowValue->show();
        percentLabel->setText(myLan.purity);
        percentLabel->show();
        percentValue->show();
        limitLabel->hide();
        limitValue->hide();
        break;
    case ARITH_CANDY:
        nameGroup->hide();
        modeGroup->hide();
        reservedGroup->hide();
        sensLabel->setText(myLan.sensitivity+"1");
        sensValue->show();
        sensLabel2->setText(myLan.sensitivity+"2");
        sensLabel2->show();
        sensValue2->show();
        rowLabel->show();
        rowValue->show();
        percentLabel->hide();
        percentValue->hide();
        limitLabel->hide();
        limitValue->hide();
        break;
    case ARITH_PISTACHIO:
        nameGroup->hide();
        modeGroup->hide();
        reservedGroup->hide();
        sensLabel->setText(myLan.area+myLan.min_limit);
        sensValue->show();
        sensLabel2->setText(myLan.grey+myLan.threshold+"1");
        sensLabel2->show();
        sensValue2->show();
        rowLabel->hide();
        rowValue->hide();
        percentLabel->setText(myLan.grey+myLan.threshold+"2");
        percentLabel->show();
        percentValue->show();
        limitLabel->show();
        limitValue->show();
        limitLabel->setText(myLan.grey+myLan.threshold+"3");
        break;
    default:
        break;
    }

    //! 重置保留比例控件状态
    m_rsvPerLbl->hide();
    m_rsvPerLe->hide();
}

/* 更新识别算法参数页面 */
void PageArith::updatePage()
{
/* page update */
    /* refresh all the tab items */
    tabBar->setTabBarType(0);
    updateTabBar(tabBar, true, false);
    tabBar->setCurrentIndex(0);

    /* dispaly page content */
    displayPage();

    updateRsvPerDisplay();
}

/* 布局识别算法参数页面 */
void PageArith::layoutPage()
{
/* page layout */
    /* name group layout */
    QHBoxLayout *nameLayout = new QHBoxLayout(nameGroup);
    nameLayout->setContentsMargins(10, 0, 10, 10);
    nameLayout->addWidget(nameValue);

    /* color & mode group layout */
    QGridLayout *modeLayout = new QGridLayout(modeGroup);
    modeLayout->setContentsMargins(10, 0, 10, 10);
    modeLayout->addWidget(colorRadio,0,0,Qt::AlignLeft);
    modeLayout->addWidget(modeRadio,0,1,Qt::AlignRight);
    modeLayout->addWidget(colorModeRadio,1,0,Qt::AlignCenter);

    /* sens group layout */
    QHBoxLayout *sensLayout = new QHBoxLayout;
    sensLayout->addWidget(sensLabel);
    sensLayout->addSpacing(10);
    sensLayout->addWidget(sensValue);
    sensLayout->addStretch(1);

    QHBoxLayout *sensLayout2 = new QHBoxLayout;
    sensLayout2->addWidget(sensLabel2);
    sensLayout2->addSpacing(10);
    sensLayout2->addWidget(sensValue2);
    sensLayout2->addStretch(1);

    QHBoxLayout *rowLayout = new QHBoxLayout;
    rowLayout->addWidget(rowLabel);
    rowLayout->addSpacing(10);
    rowLayout->addWidget(rowValue);

    QHBoxLayout *percentLayout = new QHBoxLayout;
    percentLayout->addWidget(percentLabel);
    percentLayout->addSpacing(10);
    percentLayout->addWidget(percentValue);

    QHBoxLayout *limitLayout = new QHBoxLayout;
    limitLayout->addWidget(limitLabel);
    limitLayout->addSpacing(10);
    limitLayout->addWidget(limitValue);

    QHBoxLayout *areaLayout = new QHBoxLayout(sensGroup);
    areaLayout->setContentsMargins(10, 0, 10, 10);
    areaLayout->addLayout(sensLayout);
    areaLayout->addStretch(1);
    areaLayout->addLayout(sensLayout2);
    areaLayout->addStretch(1);
    areaLayout->addLayout(rowLayout);
    areaLayout->addStretch(1);
    areaLayout->addLayout(percentLayout);
    areaLayout->addStretch(1);
    areaLayout->addLayout(limitLayout);

    /* reserved group layout */
    QHBoxLayout *reservedLayout = new QHBoxLayout(reservedGroup);
    reservedLayout->setContentsMargins(10, 0, 10, 10);
    reservedLayout->addWidget(reservedButton);
    reservedLayout->addStretch(1);
    reservedLayout->addWidget(reservedCheckBox);
    reservedLayout->addStretch(1);
    reservedLayout->addWidget(m_rsvPerLbl);
    reservedLayout->addWidget(m_rsvPerLe);

    /* grid layout */
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(tabBar,	 0, 0);
    gridLayout->addWidget(nameGroup,     1, 0);
    gridLayout->addWidget(modeGroup,     2, 0);
    gridLayout->addWidget(sensGroup,     3, 0);
    gridLayout->addWidget(reservedGroup, 4, 0);

    /* main layout */
    setLayout(gridLayout);
}

/*!
 * \brief 更新保留比例显示
 */
void PageArith::updateRsvPerDisplay()
{
    int nGroup = tabBar->currentIndex();
    int nVal = struCnfp.struGroupIdentify[struGsh.nLevel][nGroup].struGreyColor[m_nArithIndex].struReserved.nPercent;
    m_rsvPerLe->setText(QString("%1%").arg(nVal));
}
