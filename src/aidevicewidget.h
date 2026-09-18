/*
 * @Description:
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-17 19:33:07
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-18 13:44:30
 */
#ifndef AIDEVICEWIDGET_H
#define AIDEVICEWIDGET_H

#include <QWidget>
#include <QThread>
#include <QVector>
#include <QProgressDialog>
#include <QListWidget>
#include <QMutex>
#include <QStackedWidget>
#include "mylistwidget.h"
#include "sftpworker.h"
#include "mypushbutton.h"
#include "sortertypes.h"
#include "aidiskinfowidget.h"
#include "aiimageacquisitionwidget.h"

class AiDeviceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AiDeviceWidget(QWidget* parent = nullptr);
    ~AiDeviceWidget();

signals:
    void backToHomePageSig();

public slots:
    /**
     * @brief: 列表行变化槽函数
     * @param {int} nIndex
     * @return {*}
     */
    void onListWidgetRowChangedSlt(int nIndex);

    /**
     * @brief: 应用按钮按下槽函数
     * @return {*}
     */
    void onApplyBtnPressed();

    /**
     * @brief: 返回按钮按下槽函数
     * @return {*}
     */
    void onBackBtnPressed();

    /**
     * @brief: 下载按钮按下槽函数
     * @return {*}
     */
    void onDownBtnPressed();

    /**
     * @brief: 下载完成 
     * @param {int} idx
     * @param {bool} success
     * @return {*}
     */    
    void onDownloadFinished(int idx, bool success);

    /**
     * @brief: 下载进度 
     * @param {int} idx
     * @param {int} current
     * @param {int} total
     * @return {*}
     */    
    void onProgressChanged(int idx, int current, int total);

private:
    void destroySftp();

private:
    QVector<QThread*> sftpThreads;              // sftp下载线程
    QVector<SftpWorker*> sftpWorkers;           // sftp下载工作者
    QProgressDialog* progressDlg;               // 下载进度条
    myPushButton* downBtn;                      // 下载按钮
    myPushButton* backBtn;                      // 返回按钮
    myPushButton* applyBtn;                     // 应用按钮
    myListWidget* listWidget;
    QStackedWidget* stackedWidget;
    QMutex mutex;
    DownProgressItem downProgressArr[AI_SFTP_DEV_NUM];
};

#endif // AIDEVICEWIDGET_H
