#ifndef EJECTTIME_H
#define EJECTTIME_H

#include <QWidget>
#include "functions.h"

/*!
 * \brief 彩色机型吹气设置类
 */
class PageEjectTime : public QWidget
{
    Q_OBJECT

    enum {
        Index_Eject_Duration,
        Index_Eject_Delay,
        Index_Eject_Bias,
        Index_Assist_Duration,
        Index_Assist_Delay,
        Index_Infra_Delay
    };

public:
    PageEjectTime(QWidget *parent = 0);
    ~PageEjectTime();
    void initLocalParams(void);         //! 初始化局部参数
    void setEjectTimeParams();
    void resetEjectTimeParams();
    bool checkContainAssist(int nTick); //! 检查是否包含辅配相机
    void showAssistEjectSet(void);      //! 显示辅配吹气设置
    void hideAssistEjectSet(void);      //! 隐藏辅配吹气设置
    void refreshEjectSetDisplay();

signals:
    void pageUpdated();
    void currentChanged(int index);
    void refreshTabBarDisplay();

public slots:
    void setEjectTimeSlt(int index);
    void setEjectOnOffSlt(bool enable);
    void changeCurrentSlt(int index);

    void createPage();
    void displayPage();
    void updatePage();
    void layoutPage();

    void setEjectorDelayEditSlt();
    void setEjectorDynamicDelayEditSlt();
    void setEjectorBlowTimeEditSlt();

private:
    MyTabBar                *tabBar;
    myGroupBox              *ejectTimeGroup;
    QGridLayout             *gridLayout;
    QVector<myLabel *>      ejectTimeLabel;
    QVector<myLineEdit *>   ejectTimeValue;
    QSignalMapper           *sigMapper;
    MyCheckBox              *ejectorOnCheckBox;

    myLabel                  *aiEjectorDelayLbl;
    myLineEdit               *aiEjectorDelayEdit;
    myLabel                  *aiEjectorDynamicDelayLbl;
    myLineEdit               *aiEjectorDynamicDelayEdit;
    myLabel                  *aiEjectorBlowTimeLbl;
    myLineEdit               *aiEjectorBlowTimeEdit;

    int m_nAiEjectTime[MAX_LEVEL*MAX_GROUP_TICK][3];
    int m_nEjectTime[MAX_LEVEL*MAX_GROUP_TICK][6];
    int m_nEjectTimeCent[MAX_LEVEL*MAX_GROUP_TICK][6];//时间精度中的百分位
};

#endif // EJECTTIME_H
