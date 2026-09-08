#ifndef LAMP_H
#define LAMP_H

#include <QWidget>
#include "functions.h"

/*!
 * \brief 彩色机型灯控界面类
 */
class PageLamp : public QWidget
{
    Q_OBJECT

public:
    PageLamp(QWidget *parent = 0);
    void setLampParams();
    void getLampParams();
    void resetLampParams();
    void updatePage();

signals:
    void lampChanged();

private slots:
    void setLampSwitch(int btnId);
    void setLampLight(int btnId);
    void setLampExtra();
    void changeCurrentSlt(int index);

private:
    void createLampSwitch();
    void createLampLight();
    void createLampExtra();
    void updateLampText();
    void updateLampState();
    void updateLampSwitch();
    void updateTabBar();
    void updateMachineParams();
    void updateLampLight();
    void createPage();
    void layoutPage();

private:
    QVector<MyCheckBox *>   lampSwitchCbx;
    QVector<myLabel *>      lampLightLabel;
    QVector<myLineEdit *>   lampLightValue;
    QSignalMapper           *lampSwitchMap;
    QSignalMapper           *lampLightMap;

    MyTabBar                *tabBar;
    myGroupBox              *lampSwitchGroup;
    myGroupBox              *lampLightGroup[8];

    QFrame                  *topLine;
    QFrame                  *bottomLine;
    myPushButton            *lampExtraBtn;

    QGridLayout             *lampLayout;
    QGridLayout             *lampLightGroupLayout;
    QVBoxLayout             *lampExtraLayout;
    QVBoxLayout             *lampLightLayout[8];

    int flagExtra;
    int m_nLampTotal;
    int m_nLamp[MAX_LAMP];
    int m_nLampLight[MAX_LAMP];
};

#endif // LAMP_H
