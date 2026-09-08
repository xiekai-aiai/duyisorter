#ifndef TICK_H
#define TICK_H

#include <QWidget>
#include "functions.h"

//! 传染、腐蚀及膨胀最大值宏定义
static const int IMPURED_MAX = 62;
static const int IMPURED_MAX_RICE = 32;
static const int CORR_MAX = 32;
static const int CORR_MAX_RICE = 3;
static const int EXPAND_MAX = 32;
static const int EXPAND_MAX_RICE = 4;

/*!
 * \enum 枚举色选与形选前后视逻辑关系及正反选模式
 */
enum {
    COLOR_LOGIC_MODE = 0,
    SHAPE_LOGIC_MODE,
    COLOR_SORT_MODE,
    SHAPE_SORT_MODE,
    GROUP_NUM
};

/*!
 * \brief 彩色机型剔除参数类
 */
class PageTick : public QWidget
{
    Q_OBJECT

public:
    PageTick(QWidget *parent = 0);
    ~PageTick();
    void setTickParams();
    void resetTickParams();
    bool checkReverseSort(void);    //! 检查反选开传染
    void refreshDisplayByProduct(void);    //! 根据机型更新界面显示

signals:
    void sLogicalModeChanged(int mode);
    void sSortModeChanged(int mode);

public slots:
    void setHuffModeSlt(int index);
    void setLogicalModeSlt(int index);
    void setShapeLogicModeSlt(int index);
    void setSortModeSlt(int index);
    void setShapeSortModeSlt(int index);
    void setEdgeCutValueSlt();
    void setBadNumValueSlt();
    void setHuffWidthValueSlt();
    void setInfectModeSlt();
    void setCorrSlt();
    void setExpandSlt();
    void changeCurrentSlt(int index);
    void setEdgeReviseSlt(bool enable);
    void setEnableBalanceSlt(bool enable);
    void onColorImpuredLePressed(void);     //! 槽函数: 色选传染
    void onShapeImpuredLePressed(void);     //! 槽函数: 形选传染
    void onShapeCorrLePressed(void);        //! 槽函数: 形选腐蚀
    void onShapeExpLePressed(void);         //! 槽函数: 形选膨胀

    void createPage();
    void updatePage();
    void layoutPage();

private:
    MyTabBar   *tabBar;
    myGroupBox *huffModeGroup;
    myGroupBox *infectGroup;
    myGroupBox *adParamsGroup;
    myGroupBox *badNumGroup;
    myGroupBox *logicalModeGroup;
    myGroupBox *sortModeGroup;
    myGroupBox *edgeCutGroup;

    MyRadioGroup *huffModeValue;
    MyButtonGroup *logicalModeValue;
    MyButtonGroup *m_shapeLogicModeRadio;
    MyButtonGroup *sortModeValue;
    MyButtonGroup *m_shapeSortModeRadio;

    MyCheckBox  *edgeReviseCbx;
    MyCheckBox  *enableBalanceCbx;

    myLabel     *edgeCutLabel;
    myLineEdit  *edgeCutValue;
    myLabel     *badNumLabel;
    myLineEdit  *badNumValue;
    myLabel     *huffWidthLabel;
    myLineEdit  *huffWidthValue;

    //! 传染
    MyCheckBox  *infectChkBox;
    myLabel *m_colorImpuredLbl;
    myLabel *m_shapeImpuredLbl;
    myLineEdit *m_colorImpuredLe;
    myLineEdit *m_shapeImpuredLe;

    //! 膨胀、腐蚀
    myLabel  *corrValueEditLabel;
    myLabel  *expandValueEditLabel;
    myLineEdit  *corrValueEdit;
    myLineEdit  *expandValueEdit;
    myLabel *m_shapeCorrLbl;
    myLineEdit *m_shapeCorrLe;
    myLabel *m_shapeExpLbl;
    myLineEdit *m_shapeExpLe;

    QHBoxLayout *lowerHbLayout;
    QHBoxLayout *upperHbLayout;

    myLabel *m_colorLogicLbl;
    myLabel *m_shapeLogicLbl;
    myLabel *m_colorSortLbl;
    myLabel *m_shapeSortLbl;

    myLabel *blankLbl1;
    myLabel *blankLbl2;

    stu_group_tick m_struGroupTick[MAX_LEVEL*MAX_GROUP_TICK];
};

#endif // TICK_H
