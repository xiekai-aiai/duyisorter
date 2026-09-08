#ifndef MYQRCODE_H
#define MYQRCODE_H

#include <QtGui>

class MyQRCode : public QObject
{
    Q_OBJECT

public:
    MyQRCode(QObject *parent = 0);
    ~MyQRCode();
    QPixmap encode(const QString &data);
    QString decode(const QImage &data);
private:
};
#endif // MYQRCODE_H
