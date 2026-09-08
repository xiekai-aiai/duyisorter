#ifndef MYQRWIDGET_H
#define MYQRWIDGET_H

#include <QWidget>
#include <QString>

class MyQRWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyQRWidget(QWidget *parent = 0);
    void setQRData(const QString &data);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
signals:
    void pressed();

protected:
    void paintEvent(QPaintEvent *);

private:
    QString data;
};

#endif // MYQRWIDGET_H
