#ifndef UPGRADEPROGRAMWIDGET_H
#define UPGRADEPROGRAMWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "upgradefpgawidget.h"

class UpgradeProgramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UpgradeProgramWidget(QWidget *parent = 0);
    void createPage(void);      /*!< 创建页面 */
    void refreshDisplay(void);  /*!< 刷新页面显示 */

signals:
    void sBackToFactorySetPage();
public slots:
    void backToFactorySet();
private:
    //! 枚举页面索引
    enum {
        UPGRADE_FPGA_WID_ID = 0,    /*!< 升级FPGA页面 */
        SELECT_FILE_WID_ID          /*!< 选择文件页面 */
    };

    QStackedWidget *m_stackedWidget;
    UpgradeFPGAWidget *m_upgradeFPGAWid;
};

#endif // UPGRADEPROGRAMWIDGET_H
