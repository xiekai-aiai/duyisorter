#ifndef INTEL_H
#define INTEL_H

#include <QWidget>
#include <QStackedWidget>
#include "functions.h"

#define MAX_INTEL_QUO   4
#define MAX_INTEL       4
#define MAX_MULTI_INTEL 10

/*!
 * \brief 彩色机型智能参数类
 */
class PageIntel : public QWidget
{
    Q_OBJECT

public:
    PageIntel(QWidget *parent = 0);
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

    // normal intel
    void createNormalIntelPage(QWidget *parent);
    void updateNormalIntelPage();
    void getNormalIntelIndex(int index);

    // multimode intel
    QString getMultiQuoText(int index);
    void setMultiQuoValue(int index, double value);
    void createMultiIntelPage(QWidget *parent);
    void updateMultiIntelPage();
    void getMultiIntelIndex(int index);

private:
    int currentChan;

    // share
    QStackedWidget      *stackWidget;
    myListWidget        *listWidget;
    myListWidgetItem    *listItem[2];
    MyTabBar            *tabBar;

    QWidget             *pageNormal;
    QWidget             *pageMulti;

    // normal intel
    QSignalMapper   *AISignalMapper;
    myGroupBox      *AISensListCbx[MAX_INTEL];
    myLabel         *AIRedLabel[MAX_INTEL];
    myLineEdit      *AIRedLbe[MAX_INTEL];
    myLabel         *AIGreenLabel[MAX_INTEL];
    myLineEdit      *AIGreenLbe[MAX_INTEL];
    myLabel         *AIBlueLabel[MAX_INTEL];
    myLineEdit      *AIBlueLbe[MAX_INTEL];
    myLabel         *AIConsLabel[MAX_INTEL];
    myLineEdit      *AIConsLbe[MAX_INTEL];

    QHBoxLayout     *AINormalBoxLayout[MAX_INTEL];
    QGridLayout     *AINormalLayout;

    // multimode intel
    QSignalMapper   *AIMultiSignalMapper;
    myGroupBox      *AIMultiQuoListCbx;
    myLabel         *AIMultiQuoLabel[MAX_MULTI_INTEL];
    myLineEdit      *AIMultiQuoLbe[MAX_MULTI_INTEL];

    QHBoxLayout     *AIMultiBoxLayout[MAX_MULTI_INTEL];
    QGridLayout     *AIMultiLayout;
};

#endif // INTEL_H
