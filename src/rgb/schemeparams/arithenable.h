#ifndef ARITHENABLE_H
#define ARITHENABLE_H

#include <QWidget>
#include <QFormLayout>
#include <QScrollArea>
#include "functions.h"

/*!
 * \brief 彩色机型识别参数类
 */
class PageIdentify : public QWidget
{
    Q_OBJECT

public:
    PageIdentify(QWidget *parent = 0);
    ~PageIdentify();

    void setIdentifyParams();
    void resetIdentifyParams();
    int  getMatAssembleMode();
    void setMatAssembleMode();
    int  getArithmeticEnable();
    void setArithmeticEnable();
    QList<int> getMatAssembleModeList();
    QString getMatAssembleModeName(int mode);

signals:
    void pageUpdated();
    void pageChanged(int pageId);
    void sArithmeticChanged(int count);

public slots:
    void resetArithmeticEnable();
    void changePage(int pageId);
    void setMatModeCombo(int index);
    void setArithButton(int btnId);
    void changeCurrentSlt(int index);

    void updateMatModeCombo(int mode);
    void updateArithmeticList();
    void updateArithmeticName();
    void updateArithmeticState();
    void updateArithmeticLayout();

    void createArithmeticList();
    void createPage();
    void updatePage();
    void layoutPage();

private:
    MyTabBar                *tabBar;
    QSignalMapper           *sigBoxMapper;
    QSignalMapper           *sigBtnMapper;
    QVector<MyCheckBox *>   arithmeticBox;
    QVector<myPushButton *> arithmeticBtn;
    myLabel                 *matModeLabel;
    MyComboBox              *matModeCombo;
    QWidget                 *arithmeticGroup;

    QHBoxLayout             *matModeLayout;
    QVector<QFormLayout *>   arithmeticLayout;
    QGridLayout             *gridLayout;

    QVector<int>            arithmeticSeq;

    QVector<MyCheckBox *>   InfParamsBox;
    QVector<myPushButton *> InfParamsBtn;
    QVector<QFormLayout *>  InfParamsLayout;

    int m_nMatMode;
    int m_nArithmeticEnable[MAX_LEVEL][ARITHMETIC_TOTAL];
};

#endif // ARITHENABLE_H
