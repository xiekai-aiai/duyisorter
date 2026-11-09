/*
 * @Description:
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-26 09:59:26
 * @LastEditors: xiekai
 * @LastEditTime: 2026-11-07 10:11:24
 */
#ifndef UDPIMAGERECEIVER_H
#define UDPIMAGERECEIVER_H

#include <QObject>
#include <QImage>
#include <QHash>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>

class UdpImageReceiver : public QObject
{
    Q_OBJECT
public:
    explicit UdpImageReceiver(int port, int width, int height, QObject* parent = nullptr);
    ~UdpImageReceiver();

    void SetImageWidth(int width);
    void SetImageHegiht(int height);

signals:
    // 接收一张完整的图片
    void imageReady(const QImage& image, quint16 addr);

    // 接收状态
    void statusChanged(const QString& status);

    // 统计信息
    void statisticsChanged(quint64 packetCount, quint64 frameCount, quint64 errorCount);

public slots:
    void start();
    void stop();

private slots:
    void onReadyRead();

private:
    struct CameraStreamState
    {
        QByteArray line_buffer;        // 图片缓冲buffer
        QImage cur_image;              // 当前图片
        int cur_row = 0;               // 当前行号
    };

    void processDatagram(const QByteArray& datagram, const QHostAddress& sender, quint16 senderPort);

    bool IsPkgHeader(const QByteArray& datagrame);

    CameraStreamState& streamStateForKey(const QString& key);

    void resetStreamImage(CameraStreamState& state);

private:
    bool is_running_;                  // 是否运行
    int width_;                        // 图片宽度
    int height_;                       // 图片高度
    int bytesPerLine_;                 // 每一行BGR数据大小
    quint16 port_;                     // 接收端口
    QUdpSocket* socket_;
    QHash<QString, CameraStreamState> stream_states_;     // 每个摄像头的流状态， key: ip

    // 统计
    quint64 packetCount_;
    quint64 frameCount_;
    quint64 errorCount_;
};

#endif // UDPIMAGERECEIVER_H
