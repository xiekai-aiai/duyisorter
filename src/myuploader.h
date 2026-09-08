/*!
 * \Copyright   Copyright (C) 2015 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myuploader.h
 * \brief       网络上传头文件
 * \date        2015.04.25
 */
#ifndef MYUPLOADER_H
#define MYUPLOADER_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QSslError>
#include <QUrl>
#include <QTimer>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include "3rdparty/qjson/qjson.h"

#define DEF_OPENTIMEOUT	(10*1000) /* ms */
#define DEF_READTIMEOUT	(10*1000) /* ms */
#define DEF_POSTTIMEOUT (10*1000) /* ms */

using QJson::JsonObject;
using QJson::JsonArray;

QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

/*!
 * \brief 网络上传类
 */
class MyUploader : public QObject
{
    Q_OBJECT

    typedef enum _tagOPENMODE {
        OPEN_LOG,
        OPEN_ALARM,
        OPEN_STATISTIC,
        OPEN_OTHRE
    } OPENMODE;

    typedef enum _tagBLOCKMODE {
        MODE_BLOCK,
        MODE_NONBLOCK
    } BLOCKMODE;

    typedef enum _tagREADMODE {
        MODE_READ,
        MODE_READALL,
        MODE_READLINE
    } READMODE;

public:
    explicit MyUploader(QObject *parent = 0);
    virtual ~MyUploader();

    void    setUrl(const QString &url);
    void    setUrl(const QUrl &url);
    QUrl    getUrl() const;

    void setOpenTimeout(int nTimeout);
    int  getOpenTimeout() const;
    void setReadTimeout(int nTimeout);
    int  getReadTimeout() const;

    void setPostTimeout(int nTimeout);
    int	 getPostTimeout() const;

    int     getResponseCode() const;
    QString getResponseString() const;

    QNetworkReply::NetworkError getNetworkError() const;
    QNetworkReply::NetworkError response2error(int nResponse) const;

    QNetworkReply *getNetworkReply();
    void setNetworkProxy(QNetworkProxy *networkProxy);

    bool isFinished() const;
    bool isRunning()  const;
    bool isThreadRunning() const;
    bool isGuiThread() const;

    bool waitForConnect(int nTimeout, QNetworkAccessManager *pManager);
    bool waitForResponse(int nTimeout);

    bool open();
    void close();

    QByteArray read(qint64 maxlen);
    QByteArray readAll();
    QByteArray readLine(qint64 maxlen = 0);

    QUrl getServer(OPENMODE mode);
    QString getDevID();
    QString getVendorID();
    QString getProductID();

    QString getLogType(QString log);
    QString getLogTime(QString log);
    QString getLogData(QString log);

    JsonObject getMatPoint();
    JsonObject getBadPoint();
    JsonObject getEjectTimes();
    JsonObject getHistogram();
    JsonObject getBgInfo();
    JsonArray  getAlarm();
    JsonObject getSystemState();
	JsonObject getDescription();
    
    QByteArray encodeLog(QString log);
    QByteArray encodeAlarm();
    QByteArray encodeStatistic();
    QByteArray encodeInfo();

    bool isUploaded(QString log);
    bool postLog(QString log);
    bool markLog(QString log);

protected:
    void init();
    void clear();
    void stopNetworkThread();

    bool openUrl();
    void readData(QByteArray *data, qint64 maxlen, READMODE eReadMode);
    bool httpRequestPost(const QUrl &url, const QByteArray &data);
    bool httpRequestGet(const QUrl &url);

signals:
    void timeout();

public slots:
    bool uploadLog();
    bool uploadStatistic();
    bool uploadAlarm();

private slots:
    void slotOpen(void *pReturnSuccess, void *pLoop);
    void slotRead(void *pLoop, QByteArray *data, qint64 maxlen, READMODE eReadMode);
    void slotWaitTimeout();
    void slotFinished();
    void slotError(QNetworkReply::NetworkError);
//    void slotSslErrors(QList<QSslError>);
    void slotProxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator);
    void slotAuthenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator);

private:
    QUrl m_url;

    int m_nOpenTimeout;
    int m_nReadTimeout;
    int m_nPostTimeout;

    bool m_bWaitTimeout;

    int  m_nResponse;
    QString m_strResponse;

    QString m_strUser;
    QString m_strPassword;

    QEventLoop m_qEventLoop;
    QThread   *m_pNetworkThread;
    QTimer    *m_pTimer;

    QNetworkAccessManager *m_pNetworkManager;
    QNetworkReply         *m_pNetworkReply;
    QNetworkProxy         *m_pNetworkProxy;

    QNetworkReply::NetworkError m_NetworkError;
//    QList<QSslError> m_sslErrors;
};

#endif // MYUPLOADER_H
