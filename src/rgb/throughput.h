#ifndef THROUGHPUT_H
#define THROUGHPUT_H

#include <QWidget>
#include <QStackedWidget>
#include <QPainter>
#include <QtCore>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QPixmap>
#include "common/mygroupbox.h"
#include "common/mycustomcheckbox.h"
#include "globalflow.h"

// 通过量测试页面
enum {
    PAGE_MAIN,
    PAGE_TEST,
    PAGE_SHOWMAIN
};

#define MAX_STATIC_NUM  5

class Throughput : public QWidget
{
    Q_OBJECT
public:
    explicit Throughput(QWidget *parent = 0);
    void createPage();
    void updatePage();
    void resetParams();
    void setParams();
    void createMainPage();
    void updateMainPage();
    void createTestPage();
    void updateTestPage();
    quint64  getStaticsFun();
    void updateThroughtputPage();
    void createShowMainPage();


signals:
    void backToHomePageSig(void);

public slots:
    void onBackBtnSlt();
    void onTestBtnSlt();
    void onClearTestBtnSlt();
    void onUnitViewBtnSlt();
    void onUnitPlusBtnSlt();
    void onUnitMinusBtnSlt();
    // pageMain
    void onTimeLdtSlt();
    void onThroughputLdtSlt();
    void onStaticStartBtn();
    // pageTest
    void onNumBoxSlt(int);
    void onTestPixelLdtSlt(int);
    void onTestPutLdtSlt(int);
    void onTestTimeLdtSlt(int);
    void onaFactorLdtSlt();
    void onbFactorLdtSlt();
    void onBeginBtnSlt();
    void onComputeBtnSlt();
    void throughputTimeoutSlt();
    void onTestMainButtonSlt();
    void TimeOutSlt();                  // 定时器
    void onChuteListLocalLdtSlt(int);
    void onChuteListAllLdtSlt(int);



private:
    QStackedWidget *throughputWidget;
    QWidget        *mainPage;
    QWidget        *testPage;
    QWidget        *showMainPage;

    QTimer         *throughputTimer;

    myPushButton   *backBtn;
    myPushButton   *testButton;
    myPushButton   *clearTestBtn;
    myPushButton   *unitViewBtn;
    myPushButton   *unitPlusBtn;
    QLCDNumber     *unitLcdNum;
    myPushButton   *unitMinusBtn;
    myGroupBox     *unitGroup;

    // mainPage
    myLabel        *timeLbl;
    myLineEdit     *timeLdt;
    myLabel        *throughputLbl;
    myLineEdit     *throughputLdt;
    myPushButton   *staticStartBtn;

    int            m_time;
    float          m_throughput;
    float          m_allthroughput;

    quint64        pixelSum;

    // testPage
    myCustomCheckBox        *numBox[MAX_STATIC_NUM];
    myLabel        *testPixelLbl;
    myLineEdit     *testPixelLdt[MAX_STATIC_NUM];
    myLabel        *testPutLbl;
    myLineEdit     *testPutLdt[MAX_STATIC_NUM];
    myLabel        *testTimeLbl;
    myLineEdit     *testTimeLdt[MAX_STATIC_NUM];
    myPushButton   *beginBtn;
    myPushButton   *computeBtn;
    myLabel        *aFactorLbl;
    myLineEdit     *aFactorLdt;
    myLabel        *bFactorLbl;
    myLineEdit     *bFactorLdt;

    int            isNumCheck[MAX_STATIC_NUM];
    quint64        testPixelNum[MAX_STATIC_NUM];
    float          testPutNum[MAX_STATIC_NUM];
    int            testTime[MAX_STATIC_NUM];

    double          afactor;
    double          bfactor;

    // showMainPage
    myPushButton    *testMainButton;
    QGridLayout     *chuteListLayout;

    myLabel         *chuteLabel;
    myLabel         *chuteListNumLabel[MAX_UNIT/2];
    myLabel         *chuteLocalLabel;
    myLabel         *chuteAllLabel;

    myLineEdit      *chuteListLocalLdt[MAX_UNIT/2];
    myLineEdit      *chuteListAllLdt[MAX_UNIT/2];
    QTimer          *timer;
    myLabel         *chuteListAllLabel;
    myLabel         *chuteListLoaclNumLabel;
    myLabel         *chuteListAllNumLabel;
    quint64         m_localThroughtPut;
    quint64         m_AllThroughtPut;

    
};

#endif // THROUGHPUT_H
