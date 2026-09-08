#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QComboBox>
#include <QListView>
#include <globalconfig.h>

class myComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit myComboBox(QWidget *parent = 0);
    
signals:
    
public slots:
private:
    g_Config *m_config;
};

#endif // MYCOMBOBOX_H
