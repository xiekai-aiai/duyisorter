#ifndef AI_H
#define AI_H

#include <QWidget>
#include "functions.h"
#include <QSqlError>
#include <QSqlQuery>


/*!
 * \brief ai参数类
 */
class PageAi : public QWidget
{
    Q_OBJECT

public:
    PageAi(QWidget *parent = 0);
    void setIntelParams();
    void resetIntelParams();

signals:
    void pageUpdated();

public slots:
    // share
    void createPage();
    void updatePage();
    void layoutPage();
    void updateListWidget();
    void changeCurrentSlt(int index);
//    void onApplyModeBtnPressed();
    void onCurrentRowChanged(int index);

private:
    MyTabBar            *tabBar;
    myListWidget      *modelListWidget ;
//    myPushButton      *applyModeBtn ;
    QString defaultModelId;
    stu_group_identify m_struGroupIdentify[MAX_LEVEL*MAX_GROUP_IDTNTIFY];

};

#endif // AI_H
