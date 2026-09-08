#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QWidget>
#include "functions.h"

/*!
 * \brief 彩色机型背景设置类
 */
class PageBackground : public QWidget
{
    Q_OBJECT

public:
    PageBackground(QWidget *parent = 0);
    void setBackgroundParams();
    void resetBackgroundParams();
    void setMyCurveStart(bool start);
    void getBkgValue(int nUnit, int &nVal, int &nPer);  /*!< 获取单相机背景值 */
    void setBkgValueAuto(void);     /*!< 自动设置背景值 */
    void resetBkgLampIndex(void);               // 重置背景灯索引

signals:
    void pageUpdated();
    void sBackgroundColorCurveStart();
    void sBackgroundColorCurveStop();

public slots:
    void setColorMinusSlt();
    void setColorPlusSlt();
    void onColorGroupIndexChange(int index);
    void setThresholdValueSlt();
    void setPercentValueSlt();
    void setBackAngleValueSlt();
    void setLevelValueSlt();
    void resetBackgroundColorSlt();
    void onAutoBkgBtnPressed(void);
    void onAutoBkgBoxPressed();
    void setbgRangeValueSlt(int index);
    void displayText(int level);
    void createPage();
    void updatePage();
    void layoutPage();

    void onSwitchBtnTest();

    void setAutoBgThresholdValueSlt_RSC();

     void setFrontBkRangeValueSlt(int index);
     void setRearBkRangeValueSlt(int index);

private:
    MyCurve     *myCurve;
    myPushButton *colorMinus;
    myPushButton *colorPlus;
    myLabel     *colorLabel;
    myLabel     *thresholdLabel;
    myLabel     *percentLabel;
    myLabel     *backAngleLabel;
    myLabel     *bgRangeLabel[3];
    myMutex     *m_colorGroup;  //! CI专用
    myLineEdit  *thresholdValue;
    myLineEdit  *percentValue;
    myLineEdit  *backAngleValue;
    myLineEdit  *bgRangeValue[3];
    myPushButton *m_autoBkgBtn;
    MyCheckBox  *m_autoBkgBox;

    myLabel     *m_autoThresholdLabel_RSC;
    myLineEdit  *m_autoThresholdValue_RSC;
    MyCheckBox  *m_autoBkgBox_RSC;
    int m_nColor[MAX_LEVEL][MAX_BACKGROUND_GROUP];
    int m_nThreshold[MAX_LEVEL][MAX_BACKGROUND_GROUP];
    int m_nPercent[MAX_LEVEL][MAX_BACKGROUND_GROUP];
    int m_nBackgroundType;      //0:可见光背景，1:红外背景，目前I机型使用该变量,2015.7.8

    int m_nFrontBkgLamp;
    int m_nRearBkgLamp;

    myLabel     *frontBkLabel;
//    myLabel     *frontBkRangeLabel[3];
    myLineEdit  *frontBkRangeValue[3];

    myLabel     *rearBkLabel;
//    myLabel     *rearBkRangeLabel[3];
    myLineEdit  *rearBkRangeValue[3];


};

#endif // BACKGROUND_H
