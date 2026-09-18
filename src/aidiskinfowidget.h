/*
 * @Description: AI磁盘信息页面
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-10 18:19:48
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-15 14:43:35
 */

#ifndef AIDISKINFOWIDGET_H
#define AIDISKINFOWIDGET_H

#include <QMap>
#include <QWidget>
#include <QVBoxLayout>

class AiDiskInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AiDiskInfoWidget(QWidget* parent = nullptr);

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

private:
    void clear();

    /**
     * @brief: 根据相机数量，获取磁盘信息
     * @return {*}
    */
    QMap<int, quint32> GetDiskInfo();

private:
    QVBoxLayout* mainLayout;                          // 主布局
};

#endif // AIDISKINFOWIDGET_H
