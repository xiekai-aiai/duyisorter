#include "udpimagereceiver.h"
#include "unilog.h"
#include "sortertypes.h"

UdpImageReceiver::UdpImageReceiver(int port, int width, int height, QObject* parent) :QObject(parent)
, port_(port)
, width_(width)
, height_(height)
, socket_(nullptr)
, bytesPerLine_(width * 3)
, cur_row_(0)
, packetCount_(0)
, frameCount_(0)
, errorCount_(0)
, is_running_(false)
{

}

UdpImageReceiver::~UdpImageReceiver()
{
    stop();
}

void UdpImageReceiver::SetImageWidth(int width)
{
    LOG_INFO_STM("is runnning:" << is_running_ << ", set width:" << width);
    width_ = width;
}

void UdpImageReceiver::SetImageHegiht(int heigth)
{
    LOG_INFO_STM("is runnning:" << is_running_ << ", set heigth:" << height_);
    height_ = heigth;
}

void UdpImageReceiver::start()
{
    if (is_running_)
    {
        return;
    }

    socket_ = new QUdpSocket(this);

    // UDP接收缓冲区
    socket_->setSocketOption(
        QAbstractSocket::ReceiveBufferSizeSocketOption,
        QVariant(MAX_SOCKET_RCV_BUFFER));

    bool ret = socket_->bind(
        QHostAddress::AnyIPv4,
        port_,
        QUdpSocket::ShareAddress);

    if (!ret)
    {
        LOG_ERROR_STM("UDP bind failed to port:" << port_ << ",error:" << socket_->errorString().toStdString());

        emit statusChanged(socket_->errorString());
        socket_->deleteLater();
        socket_ = nullptr;

        return;
    }

    connect(socket_, &QUdpSocket::readyRead,
        this, &UdpImageReceiver::onReadyRead);

    // 创建第一张图片
    cur_image_ = QImage(width_, height_, QImage::Format_BGR888);
    cur_image_.fill(Qt::black);

    cur_row_ = 0;
    packetCount_ = 0;
    frameCount_ = 0;
    errorCount_ = 0;
    bytesPerLine_ = 3 * width_;
    is_running_ = true;

    emit statusChanged(
        QString("UDP listening on port %1")
        .arg(port_));
}


void UdpImageReceiver::stop()
{
    if (!is_running_)
    {
        return;
    }

    is_running_ = false;

    if (socket_)
    {
        socket_->close();
        socket_->deleteLater();
        socket_ = nullptr;
    }

    cur_row_ = 0;
    emit statusChanged("UDP receiver stopped");
}


void UdpImageReceiver::onReadyRead()
{
    if (!socket_)
    {
        return;
    }

    /*
     * readyRead一次可能有多个UDP包。
     * 所以必须while循环，把当前已经到达的
     * UDP数据全部取出来。
     */
    while (socket_->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(static_cast<int>(socket_->pendingDatagramSize()));

        QHostAddress sender;
        quint16 senderPort = 0;

        qint64 size = socket_->readDatagram(
            datagram.data(),
            datagram.size(),
            &sender,
            &senderPort);

        if (size < 0)
        {
            errorCount_++;
            continue;
        }

        packetCount_++;
        processDatagram(datagram);
    }

    emit statisticsChanged(packetCount_, frameCount_, errorCount_);
}

bool UdpImageReceiver::IsPkgHeader(const QByteArray& datagram)
{
    if (datagram.size() < IMG_PACKAGE_HEAD_LEN)
    {
        return false;
    }

    if ((static_cast<unsigned char>(datagram[0]) != IMG_PACKAGE_HEAD0) ||
        (static_cast<unsigned char>(datagram[1]) != IMG_PACKAGE_HEAD1) ||
        (static_cast<unsigned char>(datagram[2]) != IMG_PACKAGE_HEAD2) ||
        (static_cast<unsigned char>(datagram[3]) != IMG_PACKAGE_HEAD3))
    {
        return false;
    }

    return true;
}

void UdpImageReceiver::processDatagram(const QByteArray& datagram)
{
    line_buffer_.append(datagram);
    if (line_buffer_.size() < (bytesPerLine_ + MIN_PKGA_LEN))
    {
        return;
    }

    if (!IsPkgHeader(line_buffer_))
    {
        LOG_WARN_STM("image package head is not correct, data size : " << datagram.size()
            << ", sum size:" << line_buffer_.length() << ", body:" << line_buffer_.toHex(' ').toStdString());

        while (!line_buffer_.isEmpty() && !IsPkgHeader(line_buffer_))
        {
            line_buffer_.remove(0, 1);
        }

        return;
    }

    quint32 idx{ 4 };
    // 两个字节的地址
    quint16 addr = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(line_buffer_.constData() + idx));
    idx += 2;
    // 两个字节的长度
    quint16 len = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(line_buffer_.constData() + idx));
    idx += 2;
    // 四个字节的包序号
    quint32 no = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(line_buffer_.constData() + idx));
    idx += 4;

    // 图片数据
    if (line_buffer_.size() < MIN_PKGA_LEN + bytesPerLine_)
    {
        LOG_TRACE_STM("line buffer size:" << line_buffer_.size() << ", line image len:" << len
            << ", bytesPerLine_:" << bytesPerLine_);
        return;
    }

    uchar* dst = cur_image_.scanLine(cur_row_);


    /*
     * UDP中的数据本身就是BGR。
     */
    memcpy(dst, line_buffer_.constData() + idx, bytesPerLine_);

    line_buffer_.remove(0, idx + bytesPerLine_);

    LOG_TRACE_STM("no:" << no << ", idx:" << idx << ", bytesPerLine_:" << bytesPerLine_);

    cur_row_++;

    /*
     * 一张完整图片接收完成
     */
    if (cur_row_ == height_)
    {
        frameCount_++;

        LOG_TRACE_STM("image read:" << frameCount_);

        // xknote: 对于x5板卡， 相机传过来的数据是-128的
        uchar* data = cur_image_.bits();
        const int size = cur_image_.sizeInBytes();
        for (int i = 0; i < size; ++i)
        {
            data[i] = static_cast<uchar>(
                static_cast<int>(static_cast<signed char>(data[i])) + 128);
        }

        emit imageReady(cur_image_);

        /*
         * 准备接收下一张图片
         */
        cur_image_ = QImage(width_, height_, QImage::Format_BGR888);
        cur_image_.fill(Qt::black);
        cur_row_ = 0;
    }
}

