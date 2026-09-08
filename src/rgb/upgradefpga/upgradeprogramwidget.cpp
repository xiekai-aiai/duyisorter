#include "upgradeprogramwidget.h"

UpgradeProgramWidget::UpgradeProgramWidget(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(LCD_WIDTH-20,  LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_TITLE_HEIGHT-10);
    createPage();
}

/*!
 * \brief 创建页面
 */
void UpgradeProgramWidget::createPage()
{
    QHBoxLayout *mainLay = new QHBoxLayout(this);
    m_stackedWidget = new QStackedWidget;

    m_upgradeFPGAWid = new UpgradeFPGAWidget;
    m_stackedWidget->addWidget(m_upgradeFPGAWid);

    mainLay->setMargin(0);
    mainLay->addWidget(m_stackedWidget);

    connect(m_upgradeFPGAWid,SIGNAL(sBackToFactorySetPage()),this,SLOT(backToFactorySet()));
}

/*!
 * \brief 刷新页面显示
 */
void UpgradeProgramWidget::refreshDisplay()
{
    m_upgradeFPGAWid->refreshDisplay();
    m_stackedWidget->setCurrentIndex(UPGRADE_FPGA_WID_ID);
}

/*!
 * \brief 返回厂家设置界面
 */
void UpgradeProgramWidget::backToFactorySet()
{
    emit sBackToFactorySetPage();
}
