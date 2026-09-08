#include "selectfiledialog.h"

SelectFileDialog::SelectFileDialog(QWidget *parent) :
    QDialog(parent)
{
    setDialogAttr();

    createPage();

    connectSigAndSlt();
}

SelectFileDialog::SelectFileDialog(QStringList strList, QWidget *parent) :
    QDialog(parent)
{
    setDialogAttr();

    createPage();

    connectSigAndSlt();

    setFilter(strList);
}

/*!
 * \brief 设置对话框属性
 */
void SelectFileDialog::setDialogAttr()
{
    setWindowFlags(Qt::FramelessWindowHint);

    int w = LCD_WIDTH*0.75;
    int h = LCD_CONTENT_HEIGTH*0.9;
    int x = (LCD_WIDTH-w)/2;
    int y = (LCD_HEIGHT-h)/2;

    setGeometry(x, y, w, h);
}

/*!
 * \brief 创建页面
 */
void SelectFileDialog::createPage()
{
    config = new g_Config();
    QFont font;
    font.setPixelSize(20);
    QFont titleFont;
    titleFont.setPixelSize(25);
    this->setStyleSheet(g_style2);

    //! 布局管理器
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *downLay = new QHBoxLayout;
    QHBoxLayout *upLay = new QHBoxLayout;

    m_titleLabel = new QLabel;
    m_titleLabel->setFixedSize(BTN_WIDTH*4, BTN_HEIGHT);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->hide();
    //! 页面控件
    m_listWidget = new myListWidget;
    m_listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listWidget->setFont(font);
    m_okBtn = new myPushButton(myLan.ok, QIcon(":res/png/icon_ok.png"));
    m_okBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_cancelBtn = new myPushButton(myLan.cancel, QIcon(":res/png/icon_cancel.png"));
    m_cancelBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    upLay->addWidget(m_titleLabel);
    downLay->addWidget(m_okBtn);
    downLay->addStretch();
    downLay->addWidget(m_cancelBtn);
    mainLay->addLayout(upLay);
    mainLay->addWidget(m_listWidget);
    mainLay->addLayout(downLay);
}

/*!
 * \brief 连接信号与槽
 */
void SelectFileDialog::connectSigAndSlt()
{
    connect(m_okBtn, SIGNAL(pressed()), this, SLOT(onOkBtnPressed()));
    connect(m_cancelBtn, SIGNAL(pressed()), this, SLOT(onCancelBtnPressed()));
}

/*!
 * \brief 设置过滤器
 * \param strList 过滤字符串
 */
void SelectFileDialog::setFilter(QStringList strList)
{
    QDir dir(g_sUpgradeDir);
    m_titleLabel->hide();
    m_fileStrList.clear();
    m_fileStrList = dir.entryList(strList);

    for (int i = 0; i < m_fileStrList.size(); i++) {
        QString str = QString("\n%1").arg(m_fileStrList.at(i));
        m_listWidget->addItem(str);
    }

    if (m_fileStrList.size() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

void SelectFileDialog::setDir(QString strDir,QStringList strList)
{
    m_titleLabel->setText(myLan.select_png_dir);
    m_titleLabel->show();
    QDir dir(strDir);
    dir.setFilter(QDir::NoDotAndDotDot|QDir::Dirs);
    m_fileStrList.clear();
    m_fileStrList = dir.entryList(strList);

    for (int i = 0; i < m_fileStrList.size(); i++) {
        QString str = QString("\n%1").arg(m_fileStrList.at(i));
        m_listWidget->addItem(str);
    }

    if (m_fileStrList.size() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

/*!
 * \brief 获取文件名
 * \return 文件名
 */
QString SelectFileDialog::getFileName()
{
    QString str = QString("");

    int nRow = m_listWidget->currentRow();
    if (nRow >= 0 && m_fileStrList.size() > nRow) {
        str = m_fileStrList.at(nRow);
    }

    return str;
}

/*!
 * \brief 槽函数: 确定
 */
void SelectFileDialog::onOkBtnPressed()
{
    accept();
}

/*!
 * \brief 槽函数: 取消
 */
void SelectFileDialog::onCancelBtnPressed()
{
    reject();
}
