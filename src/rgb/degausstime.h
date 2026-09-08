#ifndef DEGAUSSTIME_H
#define DEGAUSSTIME_H

#include <QWidget>
#include "globalparams.h"
#include "globalflow.h"
#include "common/globalconfig.h"
#include "common/mypushbutton.h"
#include "common/mymessagebox.h"
#include "common/mywid.h"
#include "schemeparams/functions.h"

class DegaussTime : public QWidget
{
    Q_OBJECT
public:
    explicit DegaussTime(QWidget *parent = 0);
    void sendDegauessTime(int);
    void getMaxAndMinValue(int);
    
signals:
    void backToVenderset();
    
public slots:
    void updateDegaussTimePage();

private slots:
    //! 设置消磁、截断时间页面
    void onDegaussTimeTabChanged(int);

    void onDegaussTimeMinusBtnClicked(int);
    void onDegaussTimePlusBtnClicked(int);
    void onDegaussTimeValueChanged(int);

    void onDegaussTimeBackBtnClicked();
private:

    void createDegaussTimePage();

    g_Config *m_config;

    //! 设置消磁时间和截断时间页面
    /*组的索引号*/
    enum{
        GROUPS_NUM = 4,
        GROUP_DEGAUSS = 0,
        GROUP_TRUNCATION,
        GROUP_ASSIST_DEGAUSS,
        GROUP_ASSIST_TRUNCATION
    };

    QWidget      *degaussTimeWidget;
    MyTabBar     *degaussTimeTabBar;
    myPushButton *degaussTimeBackBtn;

    myGroupBox *m_groups[GROUPS_NUM];
    myLCDNumber *m_lcdNums[GROUPS_NUM];
    myPushButton *m_MinusBtn[GROUPS_NUM];
    myPushButton *m_PlusBtn[GROUPS_NUM];
//    mySlider *m_Slider[GROUPS_NUM];

    QSignalMapper *m_minusBtnMapper;
    QSignalMapper *m_plusBtnMapper;
    QSignalMapper *m_sliderMapper;
    QSignalMapper *m_lcdMapper;
//    QSignalMapper *m_sigMapper;

     int currentChan;
     double m_min,m_max;
    
};

#endif // DEGAUSSTIME_H
