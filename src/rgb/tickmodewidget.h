#ifndef TICKMODEWIDGET_H
#define TICKMODEWIDGET_H

#include <QWidget>
#include "globalparams.h"
#include "globalflow.h"
#include "common/globalconfig.h"
#include "common/mypushbutton.h"
#include "common/mymessagebox.h"
#include "common/mywid.h"
#include "schemeparams/functions.h"

class tickModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit tickModeWidget(QWidget *parent = 0);
    void initLocalParams();
    void saveAndSendTickParams();
    void creatPage();
    void layoutPage();
    void updatePage();
    void updateParamsDisplay();
    void getInputType(int);


    
signals:
    void backToHomePageSig();
    
public slots:
    void OnTickParamsValueChange(int);
    void onTickModeTabChanged(int);
    void onMutexIndexChangedSlt(int);
    void onBackBtnPressedSlt();
private:
    enum {
        EJECT_LENTH_MAX = 0,
        EJECT_LENTH_DIF,
        EJECT_LENTH_P1,
        EJECT_LENTH_P2,
        EJECT_LENTH_THRESHOLD,
        REVERSE_ROUNDS,
        EJECT_PARAMS_NUM
    };

    QWidget *m_tickModePage;
    MyTabBar *tickModeTabBar;
    QSignalMapper *tickModeSigMapper;

    QGroupBox *group1;
    QGroupBox *group2;

    myMutex *m_mutex;
    myLabel *m_paramsLel[EJECT_PARAMS_NUM];
    myLineEdit *m_paramsLet[EJECT_PARAMS_NUM];
    myPushButton *m_backBtn;

    int m_nTickMode;
    int m_Params[EJECT_PARAMS_NUM];

    int currentChan;
    int max;
    int min;
    inputType type;
    
};

#endif // TICKMODEWIDGET_H
