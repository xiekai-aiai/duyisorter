#ifndef UDPIMAGERECEIVER_H
#define UDPIMAGERECEIVER_H

#include <QObject>
#include <QImage>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>

class UdpImageReceiver : public QObject
{
    Q_OBJECT
public:
    explicit UdpImageReceiver(int port, int width, int height, QObject *parent = nullptr);
    ~UdpImageReceiver();

    void SetImageWidth(int width);
    void SetImageHegiht(int height);

signals:
    // 接收一张完整的图片
    void imageReady(const QImage &image);

    // 接收状态
    void statusChanged(const QString &status);

    // 统计信息
    void statisticsChanged(quint64 packetCount,quint64 frameCount,quint64 errorCount);

public slots:
    void start();
    void stop();

private slots:
    void onReadyRead();

private:
    void processDatagram(const QByteArray &datagram);

    bool IsPkgHeader(const QByteArray &datagrame);

private:
    bool is_running_;                  // 是否运行
    int width_;                        // 图片宽度
    int height_;                       // 图片高度
    int bytesPerLine_;                 // 每一行BGR数据大小
    quint16 port_;                     // 接收端口
    QImage cur_image_;                 // 当前图片
    int    cur_row_;                   // 当前行
    QByteArray line_buffer_;           // 一行像素buffer
    QUdpSocket *socket_;

    // 统计
    quint64 packetCount_;
    quint64 frameCount_;
    quint64 errorCount_;

};

#endif // UDPIMAGERECEIVER_H
