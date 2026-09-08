/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setmaterialsens.h
 * \brief       彩色机型设置灵敏度界面头文件
 * \date        2015.01.14
 */
#ifndef SETMATERIALSENS_H
#define SETMATERIALSENS_H

#include <QWidget>
#include <QSqlError>
#include <QSqlQuery>
#include <QScrollArea>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/myinputpanel.h"
#include "common/mycustomcheckbox.h"
#include "common/myinputmethod.h"
#include "globalparams.h"
#include "globalflow.h"
#include "schemeparams/schemeparams.h"
#include "schemeparams/reserved.h"


/*!
 * \brief 彩色机型设置灵敏度界面类
 */
class setMaterialSens : public QWidget
{
    Q_OBJECT
public:
    explicit setMaterialSens(QWidget *parent = 0);
    
signals:
    void goToHomePageSig();         // 返回主界面
    void goToSensBiasPageSig();     // 跳转至灵敏度偏置页面信号

public slots:
    // share
    void onCancelBtnClickedSlt();
    void updateAll();
    void upTabBar();
    void updateListWidget();
    void updateStackWidget(int indexOfUnit);
    void onSensBiasBtnPressed(void);
    void updateSensBiasBtn(void);

    // page General
    void createGeneralPage();
    void updateGeneralList();
    void getGeneralIndex(int index);

    // page AI
    void createAIPage();
    void updateAIList();
    void getAIIndex(int index);
    void onAIRsvBtnsPressed(int index);
    void setWidgetVisible(bool bEn);

    // page Shape
    void createShapePage();
    void updateShapeList();
    void getShapeIndex(int index);

    // page Peanut
    void createPeanutPage();
    void updatePeanutList();
    void getPeanutIndex(int index);

    // page Watermelon
    void createWatermelonPage();
    void updateWatermelonList();
    void changeWatermelonValue(int index);

    // page Tea
    void createTeaPage();
    void updateTeaList();
    void getTeaIndex(int index);


    //page general reserved
    void createRsvPage();
    void updateRsvList();
    void getRsvIndex(int index);

    void createAiPage();

    void createPageRSC(void);       //! 创建RSC机型的灵敏度页面
    void updatePageRSC(void);       //! 刷新RSC机型的灵敏度页面

    QString  getColorModeText(int colorMode);
    QString  getColorModeText2(int colorMode);

    void setThresholdLineEdit(int);
    void setModelNameCbx(int);
    void updateModeParaInfo();
    void setModeParaInfo();

private:
    enum{
        generalSensNum = 4,         //! 通用算法灵敏度数量
        aiSensNum = 10              //! 智能化算法灵敏度数量
    };
    int algorithmType;  //0:当前列表为可见，1：红外
    QTabBar      *tabBar;
    myListWidget      *listWidget;
    myListWidgetItem  *statusListItem[8];

    QStackedWidget    *stackedWidget;
    QWidget           *pageGeneral;
    QWidget           *pageAI;
    QWidget           *pageShape;
    QWidget           *pagePeanut;
    QWidget           *pageWatermelon;
    QWidget           *pageTea;
    QWidget           *m_pageRSC;
    PageReserved      *m_pageAIRsv;
    QWidget           *m_pageGeneralRsv;
    QScrollArea       *m_scrollArea;
    QWidget           *pageAi;
    int currentPage;
    int currentChan;

    QGridLayout     *sensPageGridLayout;
    QHBoxLayout     *sensPageHBLayout1;
    QHBoxLayout     *sensPageHBLayout2;
    QHBoxLayout     *sensPageHBLayout3;
    QSpacerItem     *horizontalSpacer;
    QSpacerItem     *horizontalSpacer_1;

    // page General
    QSignalMapper *generalSignalMapper;
    myGroupBox    *generalSensListCbx[MAX_GENERAL];
    myLabel       *generalSensLabel[MAX_GENERAL];
    myLineEdit    *generalSensLbe[MAX_GENERAL];
    myLabel       *generalRowLabel[MAX_GENERAL];
    myLineEdit    *generalRowLbe[MAX_GENERAL];
    myLabel       *generalPercentLabel[MAX_GENERAL];
    myLineEdit    *generalPercentLbe[MAX_GENERAL];
    MyCheckBox    *reservedCheckBox[MAX_GENERAL];                //玉米深霉算法保留焦糊
    QHBoxLayout   *generalHBoxLayout[MAX_GENERAL];
    QVBoxLayout   *generalVBoxLayout[MAX_GENERAL];
    QVBoxLayout   *generalMainLayout;

    // page AI
    QSignalMapper *AISignalMapper;
    myGroupBox    *AISensListCbx[MAX_AI];
    myLabel       *AISensLabel[MAX_AI];
    myLineEdit    *AISensLbe[MAX_AI];
    myLabel       *AISensRatioLabel[MAX_AI];
    myLineEdit    *AISensRatioLbe[MAX_AI];
    myLabel       *AIRowLabel[MAX_AI];
    myLineEdit    *AIRowLbe[MAX_AI];
    myLabel       *AIPercentLabel[MAX_AI];
    myLineEdit    *AIPercentLbe[MAX_AI];
    myLabel       *AIBalanceLabel[MAX_AI];
    myLineEdit    *AIBalanceLbe[MAX_AI];
    myPushButton  *AIModeBtn[MAX_AI];
    myPushButton  *AIModeTypeBtn[MAX_AI];
    myPushButton  *AIModeGeneralTypeBtn[MAX_AI];
    myPushButton  *AIModeRatioTypeBtn[MAX_AI];
    myLabel       *AIModeGeneralTypeBtnLabel[MAX_AI];
    myLabel       *AIModeRatioTypeBtnLabel[MAX_AI];
    myLabel       *AIMelonWhiteLabel[MAX_AI];
    myLineEdit    *AIMelonWhiteLbe[MAX_AI];
    QVBoxLayout   *AIBoxLayout[MAX_AI];
    QVBoxLayout   *AIMainLayout;
    myPushButton  *m_AIRsvBtn[MAX_AI];
    QSignalMapper *m_AIRsvSigMap;

    // page Shape
    QSignalMapper *shapeSignalMapper;
    myGroupBox    *shapeSensListCbx[MAX_SHAPE+2];
    myLabel       *shapeSensLabel[MAX_SHAPE+2];
    myLineEdit    *shapeSensLbe[MAX_SHAPE+2];
    myLabel       *shapeRowLabel[MAX_SHAPE+2];
    myLineEdit    *shapeRowLbe[MAX_SHAPE+2];
    myLabel       *shapePercentLabel[MAX_SHAPE+2];
    myLineEdit    *shapePercentLbe[MAX_SHAPE+2];
    QHBoxLayout   *shapeBoxLayout[MAX_SHAPE+2];
    QVBoxLayout   *shapeMainLayout;

    // page GeneralRsv
    QSignalMapper *rsvSignalMapper;
    myGroupBox    *rsvSensListCbx[MAX_RSV];
    myLabel       *rsvRowLabel[MAX_RSV];
    myLineEdit    *rsvRowLbe[MAX_RSV];
    myLabel       *rsvThresholdLabel_s[MAX_RSV];
    myLineEdit    *rsvThresholdLbe_s[MAX_RSV];
    myLabel       *rsvThresholdLabe_r[MAX_RSV];
    myLineEdit    *rsvThresholdLbe_r[MAX_RSV];
    QHBoxLayout   *rsvBoxLayout[MAX_RSV];
    QVBoxLayout   *rsvMainLayout;

    // page Peanut
    QSignalMapper *peanutSignalMapper;
    myGroupBox    *peanutSensListCbx[MAX_PEANUT];
    myLabel       *peanutSensLabel[MAX_PEANUT];
    myLineEdit    *peanutSensLbe[MAX_PEANUT];
    myLabel       *peanutRowLabel[MAX_PEANUT];
    myLineEdit    *peanutRowLbe[MAX_PEANUT];
    myLabel       *peanutPercentLabel[MAX_PEANUT];
    myLineEdit    *peanutPercentLbe[MAX_PEANUT];
    QHBoxLayout   *peanutBoxLayout[MAX_PEANUT];
    QVBoxLayout   *peanutMainLayout;

    // page Watermelon
    QSignalMapper *watermelonSignalMapper;
    myGroupBox    *watermelonListGbx;
    myLabel       *watermelonLabel[MAX_WATERMELON];
    myLineEdit    *watermelonLineEdit[MAX_WATERMELON];
    QGridLayout   *watermelonLayout;
 
    // page Tea
    QSignalMapper *TeaSignalMapper;
    myGroupBox    *TeaSensListCbx[ARITHMETIC_TOTAL];
    myLabel       *TeaSensLabel[ARITHMETIC_TOTAL];
    myLineEdit    *TeaSensLbe[ARITHMETIC_TOTAL];
    myLabel       *TeaPercentLabel[ARITHMETIC_TOTAL];
    myLineEdit    *TeaPercentLbe[ARITHMETIC_TOTAL];
    myLabel       *TeaBalanceLabel[ARITHMETIC_TOTAL];
    myLineEdit    *TeaBalanceLbe[ARITHMETIC_TOTAL];
    myLabel       *TeaWidthLabel[ARITHMETIC_TOTAL];
    myLineEdit    *TeaWidthLbe[ARITHMETIC_TOTAL];
    myLabel       *TeaAreaLabel[ARITHMETIC_TOTAL];
    myLineEdit    *TeaAreaLbe[ARITHMETIC_TOTAL];
    myPushButton  *TeaModeBtn[ARITHMETIC_TOTAL];
    QHBoxLayout   *TeaBoxLayout[ARITHMETIC_TOTAL];
    QVBoxLayout   *TeaMainLayout;

    myPushButton  *cancelBtn;
    myPushButton *m_sensBiasBtn;

    //模型参数
    QHBoxLayout       *paraModelHLayout[10];
    QVBoxLayout       *pageModelParaLayout;
    MODEL_PARA_INFO   modeParaStr[10];
    myCustomCheckBox  *modelNameCbx[10];
    myLabel           *thresholdLbl[10];
    myLineEdit        *thresholdLineEdit[10];
    QSignalMapper     *m_sigMapper;
    QSignalMapper     *m_sigCheckMapper;
    int modeParaCount;
};

#endif // SETMATERIALSENS_H
