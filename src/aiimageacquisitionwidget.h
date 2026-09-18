/*
 * @Description: 图像采集配置页面
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-10 18:19:48
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-17 18:16:55
 */

#ifndef AIIMAGEACQUISITIONWIDGET_H
#define AIIMAGEACQUISITIONWIDGET_H

#include <QWidget>
#include "mylineedit.h"
#include "mycombobox.h"
#include "mycustomcheckbox.h"

class AiImageAcquisitionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AiImageAcquisitionWidget(QWidget* parent = nullptr);

signals:

public slots:
    /**
     * @brief: 刷新页面配置参数
     * @return {*}
    */
    void onRefresh();

    /**
     * @brief: 保存页面配置
     * @return {*}
    */
    void onSave();

    /**
     * @brief: 处理使能采集复选框点击事件
     * @return {*}
     */
    void onEnableAcquComBoxClicked();

private:
    myLineEdit* acqPicNumLineEdt;            // 采集张数
    myCustomCheckBox* enabelAcquComBox;      // 是否使能采集
};

#endif // AIIMAGEACQUISITIONWIDGET_H
