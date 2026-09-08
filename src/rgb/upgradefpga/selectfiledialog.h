#ifndef SELECTFILEDIALOG_H
#define SELECTFILEDIALOG_H

#include <QDialog>
#include "globalparams.h"
#include "common/mypushbutton.h"
#include "common/mylistwidget.h"

//! 定义升级文件目录
static const QString g_sUpgradeDir = QString("/udisk/");

class SelectFileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SelectFileDialog(QWidget *parent = 0);
    explicit SelectFileDialog(QStringList strList, QWidget *parent = 0);

    void setDialogAttr(void);       /*!< 设置对话框属性 */
    void createPage(void);          /*!< 创建页面 */
    void connectSigAndSlt(void);    /*!< 连接信号与槽 */
    void setFilter(QStringList strList);    /*!< 设置过滤器 */
    void setDir(QString dir,QStringList strList);
    QString getFileName(void);      /*!< 获取文件名 */

signals:

public slots:
    void onOkBtnPressed(void);      /*!< 槽函数: 确定 */
    void onCancelBtnPressed(void);  /*!< 槽函数: 取消 */
private:
    g_Config *config;
    myPushButton *m_okBtn;          /*!< 确定 */
    myPushButton *m_cancelBtn;      /*!< 取消 */
    myListWidget *m_listWidget;     /*!< 文件列表 */
    QStringList m_fileStrList;      /*!< 文件名列表 */
    QLabel *m_titleLabel;
};

#endif // SELECTFILEDIALOG_H
