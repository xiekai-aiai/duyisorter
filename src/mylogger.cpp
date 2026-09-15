#include "mylogger.h"
#include "qdir.h"
#define LOG_BUF_SIZE (4096)

MyLogger *myLog;

/* MyLogger日志信息,单条大小限定为4096字节*/
MyLogger::MyLogger(QObject *parent) :
    QObject(parent)
{
    Log4Qt::LogManager::version();

    myLog = Log4Qt::Logger::logger(QLatin1String("MyLog")); // 自定义log

    configMyLogger();

    qtLog = Log4Qt::Logger::logger(QLatin1String("Qt"));    // 根log

    configQtLogger();                                       // Qt log
}

MyLogger::~MyLogger()
{
}

/* 此日志服务设置函数仅设置了通用的日志类， 若再添加其他的日志的服务需另行配置 */
void MyLogger::configMyLogger()
{
    myLog->removeAllAppenders();
    myLog->setLevel(Log4Qt::Level::ALL_INT);
    Log4Qt::RollingFileAppender *fileAppender = new Log4Qt::RollingFileAppender();  //文件大小达到指定尺寸产生一个新文件
    fileAppender->setName("FileAppender");
    fileAppender->setEncoding(QTextCodec::codecForName("UTF-8"));
    QDir dir(QString("%1%2").arg(APP_PATH).arg("/userdata"));
    if(!dir.exists()){
        dir.mkdir(QString("%1%2").arg(APP_PATH).arg("/userdata"));
    }
    QFile qFile;
    QString fpath = QString("%1").arg(LOG_FILE_NAME);
    qFile.setFileName(fpath);
    qFile.open(QIODevice::Append);
    QFileInfo fileInfo(qFile);
    fileAppender->setFile(fileInfo.filePath());                                     //文件路径
    fileAppender->setAppendFile(true);
    fileAppender->setMaximumFileSize(MAX_LEN_LOGFILE_SIZE);                         //文件尺寸
    fileAppender->setMaxBackupIndex(1);
    Log4Qt::PatternLayout *pLayout = new Log4Qt::PatternLayout();                   //自定义格式
    pLayout->setConversionPattern("%-d{yyyy-MM-dd HH:mm:ss} %m%n");
    fileAppender->setLayout(pLayout);
    fileAppender->setImmediateFlush(true);
    fileAppender->setThreshold(Log4Qt::Level::INFO_INT);                            //存储级别
    fileAppender->activateOptions();                                                //激活设置
    myLog->addAppender(fileAppender);

#if DEBUG_OUTPUT > 0
    Log4Qt::ConsoleAppender *consoleAppender = new Log4Qt::ConsoleAppender();   //控制台显示
    consoleAppender->setName("consoleAppender");
    consoleAppender->setEncoding(QTextCodec::codecForName("GBK"));
    consoleAppender->setTarget(Log4Qt::ConsoleAppender::STDOUT_TARGET);         //输出至终端
    Log4Qt::PatternLayout *pConsLayout = new Log4Qt::PatternLayout();
    pConsLayout->setConversionPattern("%r [%p] %m%n");
    consoleAppender->setLayout(pConsLayout);                                    //使用文件布局的形式显示
    consoleAppender->setImmediateFlush(true);
    consoleAppender->setThreshold(Log4Qt::Level::ALL_INT);                      //显示级别
    consoleAppender->activateOptions();
    myLog->addAppender(consoleAppender);
#endif
}

void MyLogger::configQtLogger()
{

}

void MyLogger::info(int type, const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap,fmt);
    vsnprintf(buf,LOG_BUF_SIZE,fmt,ap);
    va_end(ap);

    myLog->log(Log4Qt::Level::INFO_INT,QString("[%1] %2").arg(type).arg(buf));
}

void MyLogger::info(int type, QString qStr)
{
    myLog->log(Log4Qt::Level::INFO_INT,QString("[%1] %2").arg(type).arg(qStr));
}

void MyLogger::debug(const char *fmt, ...)
{
#if DEBUG_OUTPUT > 0
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap,fmt);
    vsnprintf(buf,LOG_BUF_SIZE,fmt,ap);
    va_end(ap);

    myLog->log(Log4Qt::Level::DEBUG_INT,buf);
#endif
}

void MyLogger::debug(QString qStr)
{
#if DEBUG_OUTPUT > 0
    myLog->log(Log4Qt::Level::DEBUG_INT,qStr);
#endif
}

void MyLogger::warn(const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap,fmt);
    vsnprintf(buf,LOG_BUF_SIZE,fmt,ap);
    va_end(ap);

    myLog->log(Log4Qt::Level::WARN_INT,buf);
}

void MyLogger::warn(QString qStr)
{
    myLog->log(Log4Qt::Level::WARN_INT,qStr);
}

void MyLogger::error(const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap,fmt);
    vsnprintf(buf,LOG_BUF_SIZE,fmt,ap);
    va_end(ap);

    myLog->log(Log4Qt::Level::ERROR_INT,buf);
}

void MyLogger::error(QString qStr)
{
    myLog->log(Log4Qt::Level::ERROR_INT,qStr);
}

void MyLogger::fatal(const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap,fmt);
    vsnprintf(buf,LOG_BUF_SIZE,fmt,ap);
    va_end(ap);

    myLog->log(Log4Qt::Level::FATAL_INT,buf);
}

void MyLogger::fatal(QString qStr)
{
    myLog->log(Log4Qt::Level::FATAL_INT,qStr);
}

void MyLogger::trace(const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap,fmt);
    vsnprintf(buf,LOG_BUF_SIZE,fmt,ap);
    va_end(ap);

    myLog->log(Log4Qt::Level::TRACE_INT,buf);
}

void MyLogger::trace(QString qStr)
{
    myLog->log(Log4Qt::Level::TRACE_INT,qStr);
}

/* PrintLog可以打印某logger的日志信息,单条大小限定为4096字节*/
void MyLogger::PrintLog(Log4Qt::Logger *logger, int prio, const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap,fmt);
    vsnprintf(buf,LOG_BUF_SIZE,fmt,ap);
    va_end(ap);

    PrintLog(logger, prio, QString::fromUtf8(buf));
}

void MyLogger::PrintLog(Log4Qt::Logger *logger, int prio, QString qStr)
{
    switch(prio) {
    case Log4Qt::Level::TRACE_INT:
        logger->trace(qStr);
        break;
    case Log4Qt::Level::DEBUG_INT:
        logger->debug(qStr);
        break;
    case Log4Qt::Level::INFO_INT:
        logger->info(qStr);
        break;
    case Log4Qt::Level::WARN_INT:
        logger->warn(qStr);
        break;
    case Log4Qt::Level::ERROR_INT:
        logger->error(qStr);
        break;
    case Log4Qt::Level::FATAL_INT:
        logger->fatal(qStr);
        break;
    default:
        break;
    }
}
