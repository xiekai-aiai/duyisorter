#include <QPainter>

#include "myqrwidget.h"
#include "myqrcode.h"

MyQRWidget::MyQRWidget(QWidget *parent) :
        QWidget(parent),
        data("http://www.chinameyer.com")
{
    /*
    QString url = "1、传感器分辨率、帧频和延迟时间关系"
                  "2、层、组之间的算法使能"
                  "3、高速串口类"
                  "4、定制机型参数"
                  "5、灯控"
                  "6、JKKZ控制调试CF1机型"
                  "7、串口发送模块"
                  "8、玉米机型远程升级"
                  "9、传感器分辨率和行列尺寸关"
                  "10、不同机型串口1波特率设置, 玉米机型为921600波特率"
                  "11、延迟时间单位0.1ms和0.05ms关系"
                  "12、形状算法和长度算法使能独立或公用"
   */
}

void MyQRWidget::setQRData(const QString &data)
{
    this->data = data;
    update();
}

void MyQRWidget::mousePressEvent(QMouseEvent *event)
{
	emit pressed();
        QWidget::mousePressEvent(event);
}

#if 0
void MyQRWidget::paintEvent(QPaintEvent *pe)
{
    int x, y, width;
    unsigned char *p;

    QPainter painter(this);        //NOTE: I have hardcoded some parameters here that would make more sense as variables.
    QRcode *qrcode = QRcode_encodeString(data.toStdString().c_str(), 1, QR_ECLEVEL_L, QR_MODE_8, 0);
    if (qrcode != NULL) {
        QColor fg("black");
        QColor bg("white");
        painter.setBrush(bg);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width(), height());
        painter.setBrush(fg);

        width = qrcode->width > 0 ? qrcode->width : 1;
        const double w = this->width();
        const double h = this->height();
        const double aspect=w/h;
        const double scale=((aspect>1.0)?h:w)/s;
        for(int y=0; y < s; y++){
            const int yy=y*s;
            for (int x=0; x < s; x++) {
                const int xx=yy+x;
                const unsigned char b=qrcode->data[xx];
                if(b&0x01) {
                    const double rx1=x*scale,ry1=y*scale;
                    QRectF r(rx1, ry1, scale, scale);
                    painter.drawRects(&r, 1);
                }

            }
        }
        QRcode_free(qrcode);
    }
    else {
        QColor error("red");
        painter.setBrush(error);
        painter.drawRect(0, 0, width(), height());
        qDebug()<<"QR Fail: "<< ("errno");
    }

    // logo
    QLabel *lb = new QLabel(this);
    lb->resize(50, 50);
    lb->move((this->width()-50)/2, (this->height()-50)/2);
    QPixmap pe1("./images/logo.png");
    QPixmap tmp = pe1.scaled(50,50,Qt::IgnoreAspectRatio);
    lb->setPixmap(tmp);
    lb->show();

    qrcode = NULL;
}
#endif

void MyQRWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QImage qrImage = MyQRCode().encode(this->data).toImage();

    if (!qrImage.isNull()) {
        int scale = qMin(this->width(), this->height());
        QRectF target((this->width()-scale)/2, (this->height()-scale)/2, scale, scale);

        painter.drawImage(target, qrImage.scaled(scale, scale, Qt::KeepAspectRatio));
    }
}
