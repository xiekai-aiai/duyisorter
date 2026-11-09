#include "udpimagereceiver.h"
#include "unilog.h"
#include "sortertypes.h"

UdpImageReceiver::UdpImageReceiver(int port, int width, int height, QObject* parent) :QObject(parent)
, port_(port)
, width_(width)
, height_(height)
, socket_(nullptr)
, bytesPerLine_(width * 3)
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
    bytesPerLine_ = 3 * width_;
}

void UdpImageReceiver::SetImageHegiht(int height)
{
    LOG_INFO_STM("is runnning:" << is_running_ << ", set height:" << height);
    height_ = height;
}

void UdpImageReceiver::resetStreamImage(CameraStreamState& state)
{
    state.cur_image = QImage(width_, height_,
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        QImage::Format_BGR888);
#else
        QImage::Format_RGB888);
#endif
    state.cur_image.fill(Qt::black);
    state.cur_row = 0;
    state.line_buffer.clear();
}

UdpImageReceiver::CameraStreamState& UdpImageReceiver::streamStateForKey(const QString& key)
{
    auto it = stream_states_.find(key);
    if (it == stream_states_.end())
    {
        CameraStreamState state;
        resetStreamImage(state);
        it = stream_states_.insert(key, state);
    }
    else if (it->cur_image.width() != width_ || it->cur_image.height() != height_)
    {
        resetStreamImage(*it);
    }
    return *it;
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

    stream_states_.clear();
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

    stream_states_.clear();
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
        processDatagram(datagram, sender, senderPort);
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

void UdpImageReceiver::processDatagram(const QByteArray& datagram,
    const QHostAddress& sender,
    quint16 senderPort)
{
    CameraStreamState& state = streamStateForKey(sender.toString());

    state.line_buffer.append(datagram);
    if (state.line_buffer.size() < (bytesPerLine_ + MIN_PKGA_LEN))
    {
        return;
    }

    if (!IsPkgHeader(state.line_buffer))
    {
        LOG_WARN_STM("image package head is not correct, data size : " << datagram.size()
            << ", sum size:" << state.line_buffer.length() << ", body:" << state.line_buffer.toHex(' ').toStdString());

        while (!state.line_buffer.isEmpty() && !IsPkgHeader(state.line_buffer))
        {
            state.line_buffer.remove(0, 1);
        }

        return;
    }

    quint32 idx{ 4 };
    // 两个字节的地址
    quint16 addr = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(state.line_buffer.constData() + idx));
    idx += 2;
    // 两个字节的长度
    quint16 len = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(state.line_buffer.constData() + idx));
    idx += 2;
    // 四个字节的包序号
    quint32 no = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(state.line_buffer.constData() + idx));
    idx += 4;

    // 图片数据
    if (state.line_buffer.size() < MIN_PKGA_LEN + bytesPerLine_)
    {
        LOG_TRACE_STM("line buffer size:" << state.line_buffer.size() << ", line image len:" << len
            << ", bytesPerLine_:" << bytesPerLine_);
        return;
    }

    uchar* dst = state.cur_image.scanLine(state.cur_row);

    /*
     * UDP中的数据本身就是BGR。
     */
    memcpy(dst, state.line_buffer.constData() + idx, bytesPerLine_);

    state.line_buffer.remove(0, idx + bytesPerLine_);

    LOG_TRACE_STM("no:" << no << ", idx:" << idx << ", bytesPerLine_:" << bytesPerLine_);

    state.cur_row++;

    /*
     * 一张完整图片接收完成
     */
    if (state.cur_row == height_)
    {
        frameCount_++;

        LOG_TRACE_STM("image read:" << frameCount_ << ", addr:" << addr);

        // xknote: 对于x5板卡， 相机传过来的数据是-128的
        uchar* data = state.cur_image.bits();
        const int size = state.cur_image.width() * state.cur_image.height() * 3;  // Qt 5.9 compat
        for (int i = 0; i < size; ++i)
        {
            data[i] = static_cast<uchar>(
                static_cast<int>(static_cast<signed char>(data[i])) + 128);
        }

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
        state.cur_image = state.cur_image.rgbSwapped();
#endif

        emit imageReady(state.cur_image, addr);

        // 准备接收下一张图片
        resetStreamImage(state);
    }
}

