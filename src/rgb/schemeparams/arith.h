#ifndef ARITH_H
#define ARITH_H

#include <QWidget>
#include "functions.h"

enum {
    MODE_OPT_DARK,
    MODE_OPT_LIGHT
};

/*!
 * \brief 彩色机型算法列表类
 */
class PageArith : public QWidget
{
    Q_OBJECT

public:
    PageArith(int nArithIndex = 0, QWidget *parent = 0);
    void setArithParams();
    void resetArithParams();
    void setArithIndex(int index);
    int  getArithIndex();

signals:
    void pageUpdated();
    void pageChanged(int pageId);
    void currentChanged(int index);

public slots:
    void setImpurityNameSlt();
    void setColorValueSlt(int index);
    void setModeValueSlt(int index);
    void setColorModeValueSlt(int index);
    void setSensValueSlt();
    void setSens2ValueSlt();
    void setRowValueSlt();
    void setPercentValueSlt();
    void setLimitValueSlt();
    void setReservedStateSlt(bool enable);
    void changeReservedPageSlt();
    void changeCurrentSlt(int index);
    void onRsvPerLePressed(void);
    void displayPage();
    void createPage();
    void updatePage();
    void layoutPage();
    void updateRsvPerDisplay(void);

private:
    MyTabBar *tabBar;

    myGroupBox *nameGroup;
    myGroupBox *modeGroup;
    myGroupBox *sensGroup;
    myGroupBox *reservedGroup;

    MyButtonGroup *colorRadio;
    MyButtonGroup *modeRadio;

    MyButtonGroup *colorModeRadio;

    myLabel    *sensLabel;
    myLabel    *sensLabel2;
    myLabel    *rowLabel;
    myLabel    *percentLabel;
    myLabel    *limitLabel;

    myLineEdit *nameValue;
    myLineEdit *sensValue;
    myLineEdit *sensValue2;
    myLineEdit *rowValue;
    myLineEdit *percentValue;
    myLineEdit *limitValue;

    myLabel *m_rsvPerLbl;
    myLineEdit *m_rsvPerLe;

    MyCheckBox *reservedCheckBox;   //原保留参数的使能按钮，先改为玉米专用的焦糊保留
    myPushButton *reservedButton;

    int m_nArithIndex;
    stu_group_identify m_struGroupIdentify[MAX_LEVEL*MAX_GROUP_IDTNTIFY];
};

#endif // ARITH_H
