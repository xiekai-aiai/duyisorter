/*!
 * \Copyright   Copyright (C) 2015 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myuploader.cpp
 * \brief       网络上传源文件
 * \date        2015.04.25
 */
#include "myuploader.h"
#include "globalparams.h"

/*!
 * \brief MyUploader::MyUploader
 * \param parent
 */
MyUploader::MyUploader(QObject *parent) :
    QObject(parent),
    m_pNetworkThread(NULL),
    m_pNetworkProxy(NULL)
{
    // Register user-defined type as metatype for invokeMethod
    qRegisterMetaType<READMODE>("READMODE");  
    qRegisterMetaType<OPENMODE>("OPENMODE"); 
    qRegisterMetaType<BLOCKMODE>("BLOCKMODE");
    qRegisterMetaType<QByteArray*>("QByteArray*");  

    // init
    clear();
    init();
}

/*!
 * \brief MyUploader::~MyUploader
 */
MyUploader::~MyUploader()
{
    // exit all event loops
    m_qEventLoop.exit(true);

    // close any connections
    close();

    // stop upload thread
    stopNetworkThread();
}

/*!
 * \brief MyUploader::stopNetworkThread()
 */
void MyUploader::stopNetworkThread()
{
    if (m_pNetworkThread != NULL) {
        m_pNetworkThread->quit();
        m_pNetworkThread->wait();

        delete m_pNetworkThread;
        m_pNetworkThread = NULL;
    }
}

/*!
 * \brief MyUploader::init()
 */
void MyUploader::init()
{
    // init
    m_pNetworkManager = new QNetworkAccessManager(this);
}

/*!
 * \brief MyUploader::clear()
 */
void MyUploader::clear()
{
    // close any connections
    m_pNetworkReply     = NULL;
    m_NetworkError      = QNetworkReply::NoError;
    m_nResponse         = 0;
    m_nOpenTimeout      = DEF_OPENTIMEOUT;
    m_nReadTimeout      = DEF_READTIMEOUT;
    m_bWaitTimeout      = false;
}

/*!
 * \brief MyUploader::getUrl()
 */
QUrl MyUploader::getUrl() const
{
    return m_url;
}

/*!
 * \brief MyUploader::setUrl()
 */
void MyUploader::setUrl(const QString & strUrl)
{
    m_url = QUrl::fromEncoded(strUrl.toUtf8());
}

/*!
 * \brief MyUploader::setUrl()
 */
void MyUploader::setUrl(const QUrl & Url)
{
    m_url = Url;
}

/*!
 * \brief MyUploader::isFinished()
 */
bool MyUploader::isFinished() const
{
    if (m_pNetworkReply != NULL)
        return m_pNetworkReply->isFinished();
    else
        return true;
}

/*!
 * \brief MyUploader::isFinished()
 */
bool MyUploader::isRunning() const
{
    if (m_pNetworkReply != NULL)
        return m_pNetworkReply->isRunning();
    else
        return false;
}

/*!
 * \brief MyUploader::isThreadRunning()
 */
bool MyUploader::isThreadRunning() const
{
    if (m_pNetworkThread != NULL)
        return m_pNetworkThread->isRunning();
    else
        return false;
}

/*!
 * \brief MyUploader::isGuiThread()
 */
bool MyUploader::isGuiThread() const
{
    QCoreApplication *pCoreApp = QCoreApplication::instance();

    if (pCoreApp == NULL) {
        return false;
    }

    return (QThread::currentThread() == pCoreApp->thread());

}

/*!
 * \brief MyUploader::getOpenTimeout()
 */
int MyUploader::getOpenTimeout() const
{
    return m_nOpenTimeout;
}

/*!
 * \brief MyUploader::setOpenTimeout()
 */
void MyUploader::setOpenTimeout(int nOpenTimeout)
{
    m_nOpenTimeout = nOpenTimeout;
}

/*!
 * \brief MyUploader::getReadTimeout()
 */
int MyUploader::getReadTimeout() const
{
    return m_nReadTimeout;
}

/*!
 * \brief MyUploader::setReadTimeout()
 */
void MyUploader::setReadTimeout(int nReadTimeout)
{
    m_nReadTimeout = nReadTimeout;
}

/*!
 * \brief MyUploader::getPostTimeout()
 */
int MyUploader::getPostTimeout() const
{
    return m_nPostTimeout;
}

/*!
 * \brief MyUploader::setPostTimeout()
 */
void MyUploader::setPostTimeout(int nPostTimeout)
{
    m_nPostTimeout = nPostTimeout;
}

/*!
 * \brief MyUploader::getNetworkError()
 */
QNetworkReply::NetworkError MyUploader::getNetworkError() const
{
    return m_NetworkError;
}

/*!
 * \brief MyUploader::getResponseCode()
 */
int MyUploader::getResponseCode() const
{
    return m_nResponse;
}

/*!
 * \brief MyUploader::getResponseString()
 */
QString MyUploader::getResponseString() const
{
    return m_strResponse;
}

/*!
 * \brief MyUploader::response2error()
 */
QNetworkReply::NetworkError MyUploader::response2error(int nResponse) const
{
    QNetworkReply::NetworkError error = QNetworkReply::NoError;

    switch (nResponse)
    {
    case 200:
    case 206:
        break;
    case 401:
        error = QNetworkReply::AuthenticationRequiredError;
        break;
    case 403:
        error = QNetworkReply::ContentOperationNotPermittedError;
        break;
    case 404:
        error = QNetworkReply::ContentNotFoundError;
        break;
    default:
        error = QNetworkReply::UnknownNetworkError;
        break;
    }

    return error;
}

/*!
 * \brief MyUploader::getNetReply()
 */
QNetworkReply *MyUploader::getNetworkReply()
{
    return m_pNetworkReply;
}

/*!
 * \brief MyUploader::setNetworkProxy()
 */
void MyUploader::setNetworkProxy(QNetworkProxy *networkProxy)
{
    m_pNetworkProxy = networkProxy;
}

/*!
 * \brief MyUploader::slotFinished()
 */
void MyUploader::slotFinished()
{
    QNetworkReply *reply = (QNetworkReply *)sender();//reply ?= NULL

    if (reply->error() != QNetworkReply::NoError) {
	reply->readAll();
    }

    reply->deleteLater();
}

/*!
 * \brief MyUploader::slotError()
 */
void MyUploader::slotError(QNetworkReply::NetworkError error)
{
    m_NetworkError = error;
}

/*!
 * \brief MyUploader::slotSslErrors()
 */
/*
void MyUploader::slotSslErrors(QList<QSslError> sslerrors)
{
    m_sslErrors.append(sslerrors);
}
*/

/*!
 * \brief MyUploader::slotProxyAuthenticationRequired()
 */
void MyUploader::slotProxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator)
{
    // Send credentials if changed and not empty
    authenticator->setUser(proxy.user());
    authenticator->setPassword(proxy.password());
}

/*!
 * \brief MyUploader::slotAuthenticationRequired()
 */
void MyUploader::slotAuthenticationRequired(QNetworkReply * /*reply*/, QAuthenticator * authenticator)
{
    if (!m_strUser.isEmpty() && !m_strPassword.isEmpty() &&
            (authenticator->user() != m_strUser ||
             authenticator->password() != m_strPassword)) {
        // Send credentials if changed and not empty
        authenticator->setUser(m_strUser);
        authenticator->setPassword(m_strPassword);
    }
}

/*!
 * \brief MyUploader::slotWaitTimeout()
 */
void MyUploader::slotWaitTimeout()
{
    // report wait timeout
    m_bWaitTimeout = true;
    emit timeout();
}

/*!
 * \brief MyUploader::open()
 */
bool MyUploader::open()
{
    bool bSuccess = false;

    if (isGuiThread()) {
        QMetaObject::invokeMethod(this, "slotOpen", Qt::QueuedConnection,
                                  Q_ARG(void *, &bSuccess),
                                  Q_ARG(void *, &m_qEventLoop));
        m_qEventLoop.exec();
    } else {
        QMetaObject::invokeMethod(this, "slotOpen", Qt::BlockingQueuedConnection,
                                  Q_ARG(void *, &bSuccess),
                                  Q_ARG(void *, &m_qEventLoop));
    }

    return bSuccess;
}

/*!
 * \brief MyUploader::slotOpen()
 */
void MyUploader::slotOpen(void *pReturnSuccess, void *pLoop)
{
    // get authorized token
    *(bool *)pReturnSuccess = openUrl();

    if (pLoop != NULL) {
        QMetaObject::invokeMethod((QEventLoop*)pLoop, "quit", Qt::QueuedConnection);
    }
}

/*!
 * \brief MyUploader::openUrl()
 */
bool MyUploader::openUrl()
{
    // reset all 
    clear();

    QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager;
    if (m_pNetworkProxy != NULL) {
        pNetworkManager->setProxy(*m_pNetworkProxy);
    }
    connect(pNetworkManager,
	                SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), 
	                SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    connect(pNetworkManager,
	                SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), 
	                SLOT(slotAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    // config network request
    QNetworkRequest request;
    request.setUrl(m_url);
    request.setRawHeader("User-Agent", "Meyer BigData");

    // send request */
    m_pNetworkReply = pNetworkManager->get(request);
    if (m_pNetworkReply == NULL) {
        qDebug() << "Unknown Net Error";
        m_NetworkError = QNetworkReply::UnknownNetworkError;
        return false;
    }

    // config network reply 
    connect(m_pNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)), 
                             SLOT(slotError(QNetworkReply::NetworkError)));
    
    if (!waitForConnect(m_nOpenTimeout, pNetworkManager)) {
        qDebug() << "Connect Timeout";
        m_NetworkError = QNetworkReply::TimeoutError;
        return false;
    }

    if (m_pNetworkReply == NULL) {
        qDebug() << "Connect Cancelled";
        m_NetworkError = QNetworkReply::OperationCanceledError;
        return false;
    }

    if (m_pNetworkReply->error() != QNetworkReply::NoError) {
        qDebug() << "NetReply Error : " << m_pNetworkReply->errorString();
        m_NetworkError = m_pNetworkReply->error();
        return false;
    }

    m_NetworkError = m_pNetworkReply->error();
    m_nResponse    = m_pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_strResponse  = m_pNetworkReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    if (getNetworkError() != QNetworkReply::NoError) {
        qDebug() << "NetReply Error : " << getNetworkError();
        m_NetworkError = m_pNetworkReply->error();
        return false;
    }

    m_NetworkError = response2error(m_nResponse);
    return (getResponseCode() == 200 || getResponseCode() == 206);
}

/*!
 * \brief MyUploader::close()
 */
void MyUploader::close()
{
    if (m_pNetworkReply != NULL) {
        m_pNetworkReply->close();
	delete m_pNetworkReply;
        m_pNetworkReply = NULL;
    }
}

/*!
 * \brief MyUploader::read()
 */
QByteArray MyUploader::read(qint64 maxlen)
{
    QByteArray data;
    if (isGuiThread()) {
        QMetaObject::invokeMethod(this, "slotRead", Qt::QueuedConnection,
                                  Q_ARG(void *, &m_qEventLoop),
                                  Q_ARG(QByteArray *, &data),
                                  Q_ARG(qint64, maxlen),
                                  Q_ARG(READMODE, MODE_READ));
        m_qEventLoop.exec();
    } else {
        QMetaObject::invokeMethod(this, "slotRead", Qt::BlockingQueuedConnection,
                                  Q_ARG(void *, NULL),
                                  Q_ARG(QByteArray *, &data),
                                  Q_ARG(qint64, maxlen),
                                  Q_ARG(READMODE, MODE_READ));
    }

    return data;
}

/*!
 * \brief MyUploader::readAll()
 */
QByteArray MyUploader::readAll()
{
    QByteArray data;

    if (isGuiThread()) {
        QMetaObject::invokeMethod(this, "slotRead", Qt::QueuedConnection,
                                  Q_ARG(void *, &m_qEventLoop),
                                  Q_ARG(QByteArray *, &data),
                                  Q_ARG(qint64, 0),
                                  Q_ARG(READMODE, MODE_READALL));
        m_qEventLoop.exec();
    } else {
        QMetaObject::invokeMethod(this, "slotRead", Qt::BlockingQueuedConnection,
                                  Q_ARG(void *, NULL),
                                  Q_ARG(QByteArray *, &data),
                                  Q_ARG(qint64, 0),
                                  Q_ARG(READMODE, MODE_READALL));
    }

    return data;  // Please note that it's perfectly safe to wait on the return Q_ARG, as we wait for the invokeMethod call to complete.
}

/*!
 * \brief MyUploader::readLine()
 */
QByteArray MyUploader::readLine(qint64 maxlen /*= 0*/)
{
    QByteArray data;

    if (isGuiThread()) {
        QMetaObject::invokeMethod(this, "slotRead", Qt::QueuedConnection,
                                  Q_ARG(void *, &m_qEventLoop),
                                  Q_ARG(QByteArray *, &data),
                                  Q_ARG(qint64, maxlen),
                                  Q_ARG(READMODE, MODE_READLINE));
        m_qEventLoop.exec();
    } else {
        QMetaObject::invokeMethod(this, "slotRead", Qt::BlockingQueuedConnection,
                                  Q_ARG(void *, NULL),
                                  Q_ARG(QByteArray *, &data),
                                  Q_ARG(qint64, maxlen),
                                  Q_ARG(READMODE, MODE_READLINE));
    }

    return data;  // Please note that it's perfectly safe to wait on the return Q_ARG, as we wait for the invokeMethod call to complete.
}

/*!
 * \brief MyUploader::slotRead()
 */
void MyUploader::slotRead(void *pLoop, QByteArray *data, qint64 maxlen, READMODE eReadMode)
{
    readData(data, maxlen, eReadMode);

    if (pLoop != NULL) {
        ((QEventLoop*)pLoop)->wakeUp();
        QMetaObject::invokeMethod((QEventLoop*)pLoop, "quit", Qt::QueuedConnection);
    }
}

/*!
 * \brief MyUploader::readData()
 */
void MyUploader::readData(QByteArray *data, qint64 maxlen, READMODE eReadMode)
{
    data->clear();

    if (m_NetworkError != QNetworkReply::NoError) {
        return;
    }

    m_bWaitTimeout = false;

    try {
        qint64 nSize = 0;

        if (m_pNetworkReply == NULL) {
            qDebug() << "FATAL ERROR! Connect Cancelled";
            m_NetworkError = QNetworkReply::OperationCanceledError;
            throw (qint64)-1;
        }

        while ((nSize = m_pNetworkReply->size()) < maxlen && !isFinished()) {
            if (!waitForResponse(m_nReadTimeout)) {
                qDebug() << "FATAL ERROR! Buffer read timed out";
                if (m_pNetworkReply != NULL) {
                    m_pNetworkReply->close();
                }
                m_NetworkError = QNetworkReply::TimeoutError;
                throw (qint64)-1;
            }
        }

        if (m_pNetworkReply == NULL)
        {
            qDebug() << "FATAL ERROR! Download cancelled";
            m_NetworkError = QNetworkReply::OperationCanceledError;
            throw (qint64)-1;
        }

        switch (eReadMode) {
        case MODE_READLINE:
            *data = m_pNetworkReply->readLine(maxlen);
            break;
        case MODE_READ:
            *data = m_pNetworkReply->read(maxlen);
            break;
        case MODE_READALL:
            *data = m_pNetworkReply->readAll();
            break;
        default:
            qDebug() << "FATAL ERROR! Invalid parameter";
            throw (qint64)-1;
            break;
        }
    } catch (qint64) {
        data->clear();
    }
}

/*!
 * \brief MyUploader::waitForConnect()
 */
bool MyUploader::waitForConnect(int nTimeout, QNetworkAccessManager *pManager)
{
    QTimer *pTimer = NULL;
    QEventLoop eventLoop;
    bool bWaitTimeout = false;

    m_bWaitTimeout = false;

    // initialize timer
    if (nTimeout > 0) {
        pTimer = new QTimer;

        connect(pTimer, SIGNAL(timeout()), this, SLOT(slotWaitTimeout()));
        pTimer->setSingleShot(true);
        pTimer->start(nTimeout);

        connect(this, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    }

    // wait on QNetManager reply here
    connect(pManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // wait for the reply
    if (m_pNetworkReply != NULL) {
        connect(m_pNetworkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    //	connect(m_pNetworkReply, SIGNAL(readyRead()), &eventLoop, SLOT(quit()));
    }
    eventLoop.exec();

    // release timer
    if (pTimer != NULL) {
        pTimer->stop();
        delete pTimer;
        pTimer = NULL;
    }

    bWaitTimeout = m_bWaitTimeout;
    m_bWaitTimeout = false;

    return !bWaitTimeout;
}

/*!
 * \brief MyUploader::waitForResponse()
 */
bool MyUploader::waitForResponse(int nTimeout)
{
    QEventLoop eventLoop;
    QTimer *timer = NULL;
    bool bWaitTimeout = false;

    // clear flag of timeout
    m_bWaitTimeout = false;

    if (nTimeout > 0) {
        timer = new QTimer;

        connect(timer, SIGNAL(timeout()), this, SLOT(slotWaitTimeout()));
        timer->setSingleShot(true);
        timer->start(nTimeout);

        connect(this, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    }

    if (m_pNetworkReply != NULL) {
        connect(m_pNetworkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        connect(m_pNetworkReply, SIGNAL(readyRead()), &eventLoop, SLOT(quit()));
    }
    eventLoop.exec();

    if (timer != NULL) {
	timer->stop();
	delete timer;
	timer = NULL;
    }

    bWaitTimeout = m_bWaitTimeout;
    m_bWaitTimeout = false;

    return !bWaitTimeout;
}

/*!
 * \brief MyUploader::getLogType()
 */
QString MyUploader::getLogType(QString log)
{
    // TODO
    return log;
}

/*!
 * \brief MyUploader::getLogTime()
 */
QString MyUploader::getLogTime(QString log)
{
    // TODO
    return log;
}

/*!
 * \brief MyUploader::getLogData()
 */
QString MyUploader::getLogData(QString log)
{
    // TODO
    return log;
}


/*!
 * \brief MyUploader::getServer(OPENMODE mode)
 */
QUrl MyUploader::getServer(OPENMODE mode)
{
//	QString url = "";

    return QUrl();
}

/*!
 * \brief MyUploader::getDevID()
 */
QString MyUploader::getDevID()
{
	// TODO
    return QString("%1").arg(struCnfg.MachineIDInfo);
}

/*!
 * \brief MyUploader::getVendorID()
 */
QString MyUploader::getVendorID()
{
    // TODO
    return myString.sVendor;
}

/*!
 * \brief MyUploader::getProductID()
 */
QString MyUploader::getProductID()
{
    // TODO
//    return myString.sProduct;
    return QString("%1").arg(struCnfg.MachineIDInfo);
}

/*!
 * \brief MyUploader::httpRequestPost()
 */
bool MyUploader::httpRequestPost(const QUrl &url, const QByteArray &data)
{
    // clear any parameters
    clear();

    // config request
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "MeyerMessage");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Post Url  : " << url.toString();
    qDebug() << "Post Data : " << data;
    QNetworkReply *pNetworkReply = m_pNetworkManager->post(request, data);
    if (pNetworkReply == NULL) {
			return false;
    }

    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(slotFinished()));
    connect(pNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)), 
			   SLOT(slotError(QNetworkReply::NetworkError)));

    if(!waitForConnect(m_nOpenTimeout, m_pNetworkManager)) {
	qDebug() << "Post Timeout";
	return false;
    }

    // get reply content
    return true;
}

/*!
 * \brief MyUploader::httpRequestGet()
 */
bool MyUploader::httpRequestGet(const QUrl &url)
{

    /* set url */
    setUrl(url);

    /* send request (get method) */
    return openUrl();
}

/*!
 * \brief MyUploader::isUploaded()
 */
bool MyUploader::isUploaded(QString log)
{
    bool bIsUploaded = false;

    QString uploadFile = QString("%1.post").arg(LOG_FILE_NAME);
    QFile file(uploadFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    QString all = in.readAll();
    if (all.contains(log)) {
        bIsUploaded = true;
    }
    file.close();

    return bIsUploaded;
}

/*!
 * \brief MyUploader::markLog()
 */
bool MyUploader::markLog(QString log)
{
    QString uploadFile = QString("%1.post").arg(LOG_FILE_NAME);
    QFile file(uploadFile);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    if (!file.isSequential()) {
        in.seek(file.size());
#ifdef Q_OS_WIN
        in << log << "\r\n";
#else
        in << log << "\n";
#endif
    }

    file.close();
    return true;
}

/*!
 * \brief MyUploader::encodeLog()
 */
QByteArray MyUploader::encodeLog(QString log)
{
    QString logVendor = this->getVendorID();
    QString logPid    = this->getProductID();
    QString logData   = log;

    /* set post data */
    JsonObject data;
    data["sid"]     = logPid;
    data["content"] = logData;

    return QJson::serialize(data);
}

/*!
 * \brief MyUploader::postLog()
 */
bool MyUploader::postLog(QString log)
{
    QByteArray data;

    // url
    QUrl url = getServer(OPEN_LOG);

    // json
    data = encodeLog(log);

    // send request
    return httpRequestPost(url, data);
}

/*!
 * \brief MyUploader:uploadLog
 * \param
 */
bool MyUploader::uploadLog()
{
    bool bSuccess = false;
    QFile file;

    /* if raw log not exists, exit */
    QString rawLog = QString(LOG_FILE_NAME);
    if (!QFileInfo(rawLog).exists()) {
	    return false;
    }

    /* backup raw log to tmp file */
    QString tmpLog = QString("/tmp/%1").arg(QFileInfo(LOG_FILE_NAME).fileName());
    if (QFileInfo(tmpLog).exists()) {
        file.remove(tmpLog);
    }

    if (!file.copy(rawLog, tmpLog)) {
	    return false;
    }

    /* open tmp log file */
    file.setFileName(tmpLog);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
	    return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
	// get log line by line
        QString log = in.readLine();

	// if null, skip it
        if (log.isNull()) {
            continue;
	}

	// if uploaded, skip it
        if (isUploaded(log)) {
            continue;
	}

        // if post fail, skip it
        if (!postLog(log)) {
            continue;
	}

        // if post success, mark it
        if (!markLog(log)) {
            continue;
	}
    }

    // if upload all the log data
    if (in.atEnd()) {
        bSuccess = true;
    }

    /* delete tmp file */
    file.close();
    file.remove();

    return bSuccess;
}

JsonObject MyUploader::getMatPoint()
{
    JsonObject mat_point;

    //TODO - 传送所有相机
    JsonArray mat_num;
#if DEF_VERSION_STATISTIC == 1
    for (int i = 0; i < 8; i++) {
#else
    for (int i = 0; i < 4; i++) {
#endif
	mat_num << struGsh.struStatisticInfo.materialStatistic.at(i);
    }
    mat_point["data"] = mat_num;

    return mat_point;
}

JsonObject MyUploader::getBadPoint()
{
    JsonArray bad_num;
#if DEF_VERSION_STATISTIC == 1
    for (int i = 8; i < struGsh.struStatisticInfo.materialStatistic.size(); i++) {
#else
    for (int i = 4; i < struGsh.struStatisticInfo.materialStatistic.size(); i++) {
#endif
	bad_num << struGsh.struStatisticInfo.materialStatistic.at(i);
    }

    JsonObject bad_point;
    bad_point["arith_total"] = ARITHMETIC_TOTAL;
    bad_point["data"] = bad_num;

    return bad_point;
}

JsonObject MyUploader::getEjectTimes()
{
    JsonArray eject_num;
    for (int i = 0, len = 1; i < struGsh.struStatisticInfo.ejectStatistic.size(); i += len) {
	eject_num << struGsh.struStatisticInfo.ejectStatistic.at(i);
    }

    JsonObject eject_times;
    eject_times["ej_total"] = struCnfg.nEjectorsPerChute;
    eject_times["data"] = eject_num;

    return eject_times;
}

JsonObject MyUploader::getHistogram()
{
    JsonArray histo_num;
    for (int i = 0, len = 1; i < struGsh.struStatisticInfo.histogramStatistic.size(); i += len) {
	histo_num << struGsh.struStatisticInfo.histogramStatistic.at(i);
    }

    JsonObject range;
    range["min"] = 0;
    range["max"] = 255;

    JsonObject histogram;
    histogram["range"]= range;
    histogram["data"] = histo_num;
    return histogram;
}

JsonObject MyUploader::getBgInfo()
{

    JsonArray bg_data_before;
    for (int i = 0; i < struGsh.struStatisticInfo.bgBeforeWipeStatistic.size(); i++) {
	bg_data_before << struGsh.struStatisticInfo.bgBeforeWipeStatistic.at(i);
    }

    JsonArray bg_data_after;
    for (int i = 0; i < struGsh.struStatisticInfo.bgAfterWipeStatistic.size(); i++) {
	bg_data_after << struGsh.struStatisticInfo.bgAfterWipeStatistic.at(i);
    }

    JsonObject bg_data;
    bg_data["before"] = bg_data_before;
    bg_data["after"]   = bg_data_after;

    JsonObject bg_info;
    bg_info["pixel"] = struCnfc.nSensorPixelLevel[struGsh.nLevel];
    bg_info["data"] = bg_data;

    return bg_info;
}

JsonObject MyUploader::getSystemState()
{
    JsonObject system;
    // 系统开机时间

    // 系统运行时间
    system["this_run"] = struGsh.nCounter;
    system["total_run"] = struCnfg.nCounter;

    // 系统供料状态
    system["feed_stat"] = struGsh.bStatFeed;
    system["eject_stat"] = struGsh.bStatEjectFpga;

    // 供料量设置
    JsonArray feeder;
    for (int i = 0; i < MAX_FEED; i++) {
	feeder << struCnfp.struGroupCtrl[0].nFeeder[i];
    }
    system["feed_set"] = feeder;

    return system;
}

JsonObject MyUploader::getDescription()
{
    JsonObject desc;
	// 所在层信息
    desc["level_total"] = struCnfg.nLevelTotal;
    desc["level_id"] = struCnfg.nCurCapLevel;
	desc["view_total"] = struCnfg.struLevelInfo[struCnfg.nCurCapLevel].nViewTotal;
	// 所在组信息
    desc["unit_total"] = struCnfg.struLevelInfo[struCnfg.nCurCapLevel].nUnitLevelTotal;;
    desc["unit_id"] = struCnfg.nCurCapUnit;

    return desc;
}

QByteArray MyUploader::encodeInfo()
{
    JsonObject info;
    // 设备ID 
    info["sid"] = getProductID();

    // 物料点数
    info["mat_point"] = getMatPoint();

    // 坏点个数 
    info["bad_point"] = getBadPoint();

    // 吹气次数
    info["eject_times"] = getEjectTimes();

    // 校正数据 
    info["histogram"] = getHistogram();

    // 背景信息
    info["bg_info"] = getBgInfo();

    return QJson::serialize(info);
}

QByteArray MyUploader::encodeStatistic()
{
    JsonObject data;
    // 设备ID
    data["sid"] = getProductID();

    JsonObject statistic;
    // 统计版本
    statistic["version"] = DEF_VERSION_STATISTIC;
	// 统计描述
    statistic["description"] = getDescription();
    // 物料点数 
    statistic["mat_point"] = getMatPoint();
    // 坏点个数 
    statistic["bad_point"] = getBadPoint();
    // 吹气次数
    statistic["eject_times"] = getEjectTimes();
    // 信号数据
    statistic["histogram"] = getHistogram();
    // 背景信息 
    statistic["bg_info"] = getBgInfo();
    // 统计数据
    data["content"] = statistic;

    // 系统状态
    data["system_state"] = getSystemState();

    return QJson::serialize(data);
}

/*!
 * \brief MyUploader::uploadStatistic()
 */
bool MyUploader::uploadStatistic()
{
    // url
    QUrl url = getServer(OPEN_STATISTIC);

    // data
    QByteArray data = encodeStatistic();

    // post
    return httpRequestPost(url, data);
}

JsonArray MyUploader::getAlarm()
{
/* 报警状态检测 */
    JsonArray alarm;
    /* 气压报警 */
    JsonObject pressure_alarm;
    pressure_alarm["type"] = ALARM_ID_PRESSURE;
    pressure_alarm["id"]   = 0; 
    pressure_alarm["state"] = struGsh.bAlarmPressure;
    alarm << pressure_alarm;

    /* 风机报警 */
    JsonObject blower_alarm;
    blower_alarm["type"] = ALARM_ID_BLOWER;
    blower_alarm["id"]   = 0; 
    blower_alarm["state"] = struGsh.bAlarmBlower;
    alarm << blower_alarm;

    /* 灯控报警 */
    JsonObject lamp_alarm;
    lamp_alarm["type"] = ALARM_ID_LAMP;
    lamp_alarm["id"]   = 0; 
    lamp_alarm["state"] = struGsh.bAlarmLamp;
    alarm << lamp_alarm;

    /* 皮带报警 */
    JsonObject belt_alarm;
    belt_alarm["type"] = ALARM_ID_BELT;
    belt_alarm["id"]   = 0; 
    belt_alarm["state"] = struGsh.bStatBelt[0];
    alarm << belt_alarm;

    belt_alarm.clear();
    belt_alarm["type"] = ALARM_ID_BELT;
    belt_alarm["id"]   = 1; 
    belt_alarm["state"] = struGsh.bStatBelt[1];
    alarm << belt_alarm;

    /* 料位报警 */
    JsonObject level_alarm;
    level_alarm["type"] = ALARM_ID_LEVEL;
    level_alarm["id"]   = 0; 
    level_alarm["state"] = struGsh.nAlarmLevel;
    alarm << level_alarm;

    /* 通信报警 */
    JsonObject comm_alarm;
    comm_alarm["type"] = ALARM_ID_COMM;
    comm_alarm["id"]   = 0; 
    comm_alarm["state"] = struGsh.nCommStat;
    alarm << comm_alarm;

    return alarm;
}

QByteArray MyUploader::encodeAlarm()
{
    JsonObject data;

    // 报警设备
    data["sid"] = getProductID();
    // 报警内容
    data["content"] = getAlarm();

    return QJson::serialize(data);
}

/*!
 * \brief MyUploader::uploadAlarm()
 */
bool MyUploader::uploadAlarm()
{
    /* url */
    QUrl url = getServer(OPEN_ALARM);

    /* data */
    QByteArray data = encodeAlarm();

    /* post */
    return httpRequestPost(url, data);
}
