/*
 * @Description: 算法设置-AI智能算法页面头文件
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-08 15:23:00
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-22 20:16:07
 */
#ifndef AI_H
#define AI_H

#include <QWidget>
#include "functions.h"
#include <QSqlError>
#include <QSqlQuery>
#include "sortertypes.h"


 /*!
  * \brief ai参数类
  */
class PageAi : public QWidget
{
    Q_OBJECT

public:
    PageAi(QWidget* parent = 0);
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
    /**
     * @brief: 模型上传
     * @param {ModelInfo&} info
     * @return {*}
     */
    bool modelUpload(const ModelInfo& info);
    /**
     * @brief: 模型应用
     * @param {ModelInfo&} info
     * @return {*}
     */
    bool modelApply(const ModelInfo& info);

private:
    MyTabBar* tabBar;
    myListWidget* modelListWidget;
    stu_group_identify m_struGroupIdentify[MAX_LEVEL * MAX_GROUP_IDTNTIFY];

};

#endif // AI_H
