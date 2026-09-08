#ifndef AIINPUTPARAMS_H
#define AIINPUTPARAMS_H

#include <QWidget>
#include "common/myinputpanel.h"
#include "common/mylabel.h"
#include "common/mypushbutton.h"
#include "common/mylineedit.h"
#include "globalparams.h"
#include "globalconfig.h"

//! 定义智能算法参数个数
#define AI_PARAMS_NUM 10

//! 定义QTabBar控件单个tab的宽度
#define QTABBAR_TAB_WIDTH 140

/*!
 * \enum 枚举智能算法参数类型
 */
enum {
    AI_PARAMS_R = 0,    /*!< R */
    AI_PARAMS_G,        /*!< G */
    AI_PARAMS_B,        /*!< B */
    AI_PARAMS_RG,       /*!< RG */
    AI_PARAMS_RB,       /*!< RB */
    AI_PARAMS_GB,       /*!< GB */
    AI_PARAMS_RR,       /*!< RR */
    AI_PARAMS_GG,       /*!< GG */
    AI_PARAMS_BB,       /*!< BB */
    AI_PARAMS_CONS      /*!< CONS */
};

class AIInputParams : public QWidget
{
    Q_OBJECT
public:
    explicit AIInputParams(QWidget *parent = 0);

    void createPage(void);              /*!< 创建页面 */
    void connectSigAndSlt(void);        /*!< 连接信号与槽 */
    void refreshDisplay(void);          /*!< 刷新显示 */
    void refreshTabBarDisplay(void);    /*!< 刷新QTabBar的显示 */
    void refreshParamsDisplay(void);    /*!< 刷新智能参数的显示 */
    void setAIParams(int nIndex, int nVal);     /*!< 设置智能参数 */
    
signals:
    void backToHomePageSig(void);       /*!< 信号: 返回上一级页面 */
    
public slots:
    void onTabBarIndexChangedSlt(void);         /*!< 槽函数: 识别组号变化 */
    void onLineEditPressedSlt(int nIndex);      /*!< 槽函数: 编辑框按下 */
    void onBackBtnPressed(void);                /*!< 槽函数: 返回 */

private:
    g_Config *m_config;
    QSignalMapper *m_sigMap;

    QTabBar *m_tabBar;
    myLabel *m_labels[AI_PARAMS_NUM];
    myLineEdit *m_lineEdits[AI_PARAMS_NUM];
    myPushButton *m_backBtn;

    int m_nIndexOfArithIntel;
};

#endif // AIINPUTPARAMS_H
