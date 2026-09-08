#ifndef SETSORTERCONSISTENCEWIDGET_H
#define SETSORTERCONSISTENCEWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QLCDNumber>
#include <QStackedWidget>
#include "../common/mylabel.h"
#include "../common/mylineedit.h"
#include "../common/mypushbutton.h"
#include "../common/mygroupbox.h"
#include "../common/mycustomcheckbox.h"
#include "../globalflow.h"
#include "../globalparams.h"
#include "aishare.h"

/*!
 * \brief 状态机
 */
enum{
    PAGE_SIM_MAIN = 0,
    PAGE_SIM_DETAIL,
    PAGE_SIM_CHUTE
};
/*!
 * \brief
 */
typedef struct stru_target{
    int nProbTarget[6];
    int nCurProb[6];
    int nAlgProb[6][10];
    int   nBadPixelSum[6];
    int   nPixelSum;
}stru_Alg_Target;

/*!
 * \brief The setSorterConsistenceWidget class
 */
class setSorterConsistenceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit setSorterConsistenceWidget(QWidget *parent = 0);

    QStackedWidget   *m_mainStackedWidget;
    QWidget          *m_pageMainWidget;
    QWidget          *m_pageDetailWidget;
    QWidget          *m_pageChuteWidget;

    //! pageMainWidget
    myLabel          *m_imgViewLbe;
    QScrollArea      *m_imgViewScrollArea;

    myGroupBox       *m_chuteGbx;
    myPushButton     *m_viewNumBtn;
    myPushButton     *m_chuteNumBtn;
    myPushButton     *m_captureImgBtn;
    myPushButton     *m_simImgBtn;

    myGroupBox       *m_simSensGbx;
    QScrollArea      *m_simScrollArea;
    myCustomCheckBox *m_algEnableCbx[6];
    myLabel          *m_sensNameLbe[6];
    myLabel          *m_sensLbe[6];
    myLineEdit       *m_sensLne[6];
    myPushButton     *m_detailBtn[6];

    myGroupBox       *m_settingGbx;
    myPushButton     *m_levelChgBtn;
    myPushButton     *m_consistenceBtn;
    myPushButton     *m_simBackBtn;

    //! pageDetailWidget
    myLabel          *m_cameraTitleLbe;
    myLabel          *m_cameraLbe[10];
    myLabel          *m_detailTitleLbe;
    myLabel          *m_sensDetailLbe[10];
    myLabel          *m_biasTitleLbe;
    myLineEdit       *m_sensBiasLne[10];
    myLabel          *m_percentTitleLbe;
    myLabel          *m_percentLbe[10];
    myPushButton     *m_detailResetBtn;
    myPushButton     *m_detailBackBtn;

    //! pageChuteWidget
    myGroupBox       *m_viewSettingGbx;
    myCustomCheckBox *m_viewSettingCbx[6];
    myGroupBox       *m_chuteSettingGbx;
    myPushButton     *m_chuteSettingBtn[10];
    myPushButton     *m_chuteBackBtn;

    //! param
    int              m_nCurView;
    int              m_nCurLevel;
    int              m_nCurUnit;
    int              m_nCurAlg;
    bool             m_bEnableAlg[6];
    int              m_nCurLayer;
    bool             m_bFlgSim;
    stru_Alg_Target  m_struCurAlgTarget;
    bool             m_bHighDiff;
    bool             m_bLightOrInfra;
    uint              m_hist[6][256];

    //! function
    void createMainWidget(void);
    void updateMainWidget(void);
    void updateInitImgSlt();                        /*!< 如果默认有图片，默认更新*/
    void createDetailWidget(void);
    void updateDetailWidget(void);
    void createChuteWidget(void);
    void updateChuteWidget(void);

private:
    int  getUnitGroup(int level, int unit);
    void updateChuteGbx();
    void updateImage();
    void updateProductAlg();
    void updateAlgStat(int nAlgId,int nLevel, int nGroup, bool bFlgShow);
    void updateInfraAlgStat(int nAlgId,int nLevel, int nGroup, bool bFlgShow);
    void setAlgSens(int nAlgId);
    void setAlgSensInfra(int nAlgId);
    void setAlgSensBias(int nUnit);
    void setAlgSensBiasInfra(int nUnit);
    bool getImage(int nUnit);
    bool getHist(int nUnit);
    bool initUSB();
    bool getMaterData(int nUnit);
    char *getHead(const char *mem, int size, const char *str);
    void updateSimImg();
    bool getAlgSim(int nAlgId, int hei, int wid);
    bool pixelIsBad(int nAlgId,int nUnit,int r, int g, int b);
    int pixelIsBadInfra(int nAlgId, int nUnit);
    void resetSensBias(int nAlgId,int nGroup);
    bool getTarget(bool bTarget,int nUnit);
    bool getTargetInfra(bool bTarget, int nUnit);
    void adjustSensBias(int nUnit);
    void adjustSensBiasInfra(int nUnit);
    bool getAlgSensBias(int nAlgId,int nUnit);
    bool getAlgSensBiasInfra(int nAlgId, int nUnit);
signals:
    void goToSetSensPageSig(void);

public slots:
    void commonProcessMainWidgetSlt();
    void commonProcessDetailWidgetSlt();
    void commonProcessChuteWidgetSlt();
    
};

#endif // SETSORTERCONSISTENCEWIDGET_H
