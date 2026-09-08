#include "mycombobox.h"

/*!
 * \brief myComboBox构造函数
 * \param parent 父控件
 */
myComboBox::myComboBox(QWidget *parent) :
    QComboBox(parent)
{
    QString strColor = "color: #B0B0B0;";
    m_config = g_Config::getInstance();
    setFont(m_config->getFont());
    QString strStyle = QString("QComboBox {"
                               "%1"
                           "}"

                           "QScrollBar:vertical {"
                               "width: 50px;"
                               "%2"
                           "}").arg(strColor).arg(strColor);

    setStyleSheet(strStyle);
    QListView *view = (QListView *)this->view();
    view->setStyleSheet(strColor);
}

