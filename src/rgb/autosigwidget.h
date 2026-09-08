#ifndef AUTOSIGWIDGET_H
#define AUTOSIGWIDGET_H

#include <QWidget>

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

#include "mylogger.h"
#include "rgb/factoryset.h"
#include "statusinfopage.h"
#include "manageschemepage.h"
#include "rgb/managenewscheme.h"
#include "systeminitialization.h"
#include "manageengineer.h"
#include "rgb/setmaterialsens.h"
#include "setwipe.h"
#include "globalparams.h"
#include "mythread.h"
#include "globalflow.h"
#include "mydelaycode.h"
#include "rgb/sensbias.h"

enum {
      SIG_MAIN_PAGE = 0,
      SIG_SETTING_PAGE
};

class autoSigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit autoSigWidget(QWidget *parent = 0);

    //! mainwidge
    myPushButton        *m_autoSigBtn;
    myPushButton        *m_autoSigInitBtn;
    myPushButton        *m_autoSigBackBtn;
    myPushButton        *m_autoSigSettingBtn;

    //! settingwidget
    myGroupBox      *m_chuteGroup;
    myLabel         *m_chuteLbe;
    myPushButton    *m_plusBtn;
    myPushButton    *m_minusBtn;
    QLCDNumber      *m_lcdNum;
    myLabel         *m_feedSetLbe;
    myLineEdit      *m_feedLne;
    myCustomCheckBox *m_enableCbx;
    myPushButton    *m_settingBackBtn;

    QStackedWidget      *m_stackedSigWid;
    QWidget             *m_pageSigMainWid;
    QWidget             *m_pageSigSettingWid;

    void createSigMainWidget();
    void createSigSettingWidget();
signals:
    void goToHomePage();                            /*!< 信号函数：返回智能主界面 */

public slots:
    void updateAutoSigWid();
    void onAutoSigBtnPressedSlt();
    void onAutoSigInitBtnPressedSlt();
    void onAutoSigSettingBtnPressedSlt();
    void onAutoSigBackBtnPressedSlt();
    void onSettingPageCommonProcessedSlt();
private:
    int m_nFrontBkgLamp;
    int m_nRearBkgLamp;

    void adjustBackground(int nLevelId, bool bIsFront);
    int adjust(int nLevelId, int nAddr, int reference);
    void getBkgValue(int nUnit, int &nVal, int &nPer);  /*!< 获取单相机背景值 */
    void setBkgValueAuto(bool bAdjust);                 /*!< 自动设置背景值 */
    int  nTmpSens[ARITHMETIC_TOTAL][6];
    int  nIntelEnable;
    void getHist(void);
};

#endif // AUTOSIGWIDGET_H
