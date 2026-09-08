#ifndef RESERVED_H
#define RESERVED_H

#include <QWidget>
#include "functions.h"

/*!
 * \brief 彩色机型识别算法保留参数类
 */
class PageReserved : public QWidget
{
    Q_OBJECT

public:
    enum {
        COMPONENT_TABBAR
    };

    PageReserved(int nArithIndex = 0, QWidget *parent = 0);
    void setReservedParams();
    void resetReservedParams();
    void setArithIndex(int index);
    int  getArithIndex();
    void setComponentsHidden(QList<int> &component);
    void setTabBarIndex(int index);

signals:
    void pageUpdated();

public slots:
    void setImpurityNameSlt();
    void setModeValueSlt(int btnId);
    void setColorValueSlt(int btnId);
    void setSensValueSlt(int btnId);
    void setPercentValueSlt(int btnId);
    void changeCurrentSlt(int index);
    void setColorModeValueSlt(int btnId);

    void createPage();
    void updatePage();
    void layoutPage();

private:
    MyTabBar   *tabBar;
    myGroupBox *nameGroup;
    myLineEdit *nameValue;

    QVector<myGroupBox *>   reservedGroup;

    QVector<MyButtonGroup *> colorValue;
    QVector<MyButtonGroup *> modeValue;
    QVector<myLabel *>       sensLabel;
    QVector<myLineEdit *>    sensValue;
    QVector<myLabel *>       percentLabel;
    QVector<myLineEdit *>    percentValue;

    QVector<MyButtonGroup *> colorModeRadio;

    QSignalMapper *modeMapper;
    QSignalMapper *colorMapper;
    QSignalMapper *sensMapper;
    QSignalMapper *percentMapper;

    QSignalMapper *colorModeMapper;


    int m_nArithIndex;
    QList<int> hideComponent;   //0:tabBar,其他后续添加
    stu_reserved m_struReserved[MAX_LEVEL*MAX_GROUP_IDTNTIFY];
};

#endif // RESERVED_H
