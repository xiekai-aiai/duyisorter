/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymutex.h
 * \brief       互斥选择框控件头文件
 * \date        2015.01.14
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtCore>
#include <QtGui>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>
#include "globalconfig.h"
#include "mypushbutton.h"
#include "myicon.h"

/* 该类用于实现二种或三种间的互斥选择 */
class myMutex : public QWidget
{
    Q_OBJECT

public:
    explicit myMutex(Qt::Orientation flag, QWidget* parent = 0);
    explicit myMutex(Qt::Orientation flag, int selIndex, QWidget* parent = 0);
    ~myMutex();

    void createPage(Qt::Orientation flag);          //! 创建页面
    void connectSigAndSlt(void);        //! 连接页面中的信号与槽
    void setNum(int num);
    void setTitle(QString text);
    void setLabelText(QString str1 = "", QString str2 = "", QString str3 = "");
    void setLabelText(QStringList strList);
    void clearBtnsIcon(void);
    int getCurrentIndex(void);
    void setCurrentIndex(int index);
    void setIndexEnabled(int index, bool enabled);
    void setCheckIndicator(bool enable);    //! 选中标记改用对号图标（默认圆点）

signals:
    void indexChanged(int);

private slots:
    void onBtnsPressed(int nIndex);     //! 响应选择按钮按下

private:
    enum{
        MAX_NUM = 10
    };

    g_Config *config;
    QGroupBox *group;
    myPushButton *btns[MAX_NUM];
    QLabel *labels[MAX_NUM];
    quint8 curIndex;
    QSignalMapper   *m_sigMap;
    bool m_bCheckIndicator = false;     //! 是否使用对号图标标记选中
    bool m_bHasSelection = false;       //! 是否已有选中项

    QIcon makeCheckIcon(void) const;    //! 生成对号图标
    void updateSelectionIcon(int index); //! 刷新指定项的选中图标
};

#endif // GLOBAL_H
