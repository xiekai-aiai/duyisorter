/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        main.cpp
 * \brief       主函数文件
 * \date        2015.01.14
 */
#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QtGui>
#include <QProcess>
#include <QSettings>
#include <QProgressBar>
#include <QMovie>
//#include "aes.h"

/*!
 * \brief 根据语言获取字体
 * \return 字体
 */
QFont getFont(void)
{
    QFont font;

    if(struCnfg.nLang == LANG_THAILAND) {
        font.setFamily("Leelawadee");
    } else if(struCnfg.nLang == LANG_FARSIE
              || struCnfg.nLang == LANG_UYGHUR
              || struCnfg.nLang == LANG_ARABICE ) {
        font.setFamily("Tahoma");
    } else if (struCnfg.nLang == LANG_BENGALIE) {
        font.setFamily("Vrinda");
    } else {
        font.setFamily("HarmonyOS Sans Medium");
        font.setWeight(63);

        // 将字体细节全部引入
//        int s0 = QFontDatabase::addApplicationFont(":/res/fonts/HarmonyOS_Sans_SC_Black.ttf");
//        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(s0);
//        qDebug()<<"fontfamilies:"<<fontFamilies;

//        // 设置全局
//        QString  fontName = QFontDatabase::applicationFontFamilies(s0).at(0);
//        qDebug()<<"fontName:"<<fontName;
//        font = QFont("HarmonyOS",12);
    }

    return font;
}

/*!
 * \brief 设置开机图片
 */
void setLogo(QLabel *label)
{
    if (struGsh.bIsMC) {
        //! MC定制机型
        label->setPixmap(QPixmap(":/res/png/Vendor_Milltec.png").scaled(label->size(),
                                                                 Qt::KeepAspectRatio,
                                                                 Qt::SmoothTransformation));
        label->show();
    } else if (strcmp(struCnfe.sTitle, "")) {
        //! 其它定制机型
        QString str = str.fromLatin1(struCnfe.sTitle);
        label->setText(str);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(g_style1);

        QFont fontLbl;
        fontLbl.setPixelSize(80);
        label->setFont(fontLbl);

        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::blue);
        label->setPalette(pe);
        label->show();
    } else {
        //! 通用机型
        label->setPixmap(QPixmap(":/res/png/pic9.png").scaled(LCD_WIDTH, LCD_HEIGHT-80,
                                                                 Qt::IgnoreAspectRatio,
                                                                 Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignLeft);
//        label->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏;
        label->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);

        label->setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT-80);
    }
}

int createParaInfoTable(){
    int result = 0;
    //查询数据表是否存在
    QSqlQuery sql_query;
    if(!qDataBaseInfo.isTableExist(sql_query,"paraInfo")){
        QString  create_sql = "CREATE TABLE paraInfo ("
                    "paraName  VARCHAR (200) PRIMARY KEY,"
                    "paraValue INTEGER)";
        qDataBaseInfo.createTable(sql_query,create_sql);
        result = 1;
    }else{
         result = 0;
    }
    return result;
}

int createModeInfoTable(){
    int result = 0;
    //查询数据表是否存在
    QSqlQuery sql_query;
    if(!qDataBaseInfo.isTableExist(sql_query,"modelInfo")){
        QString  create_sql = "CREATE TABLE modelInfo ("
                    "modelId   VARCHAR (200) PRIMARY KEY,"
                    "modelName VARCHAR (100),"
                    "isApply   CHAR(1),"
                    "chgTime   DATE)";
        qDataBaseInfo.createTable(sql_query,create_sql);
        result = 1;
    }else{
         result = 0;
    }
    return result;
}

int createModeParaInfoTable(){
    int result = 0;
    //查询数据表是否存在
    QSqlQuery sql_query;
    if(!qDataBaseInfo.isTableExist(sql_query,"modelParaInfo")){
        QString  create_sql = "CREATE TABLE modelParaInfo ("
                    "modelId   VARCHAR(200),"
                    "id VARCHAR(100),"
                    "zhName   CHAR(100),"
                    "enName   CHAR(100),"
                    "levelTotal   INT(2),"
                    "identifyGroupTotal   INT(2),"
                    "threshold   CHAR(100),"
                    "isApply   CHAR(1),"
                    "chgTime   DATE,"
                    "PRIMARY KEY (modelId, id, levelTotal, identifyGroupTotal))";
        qDataBaseInfo.createTable(sql_query,create_sql);
        result = 1;
    }else{
         result = 0;
    }
    return result;
}

int createSysTable(){
//    QDataBase qDataBase;
//    QSqlDatabase database = qDataBase.initQDatabase();
//    qDataBaseInfo.openQDatabase(database);
    int result = 0;
    //查询数据表是否存在
    QSqlQuery sql_query;
    if(!qDataBaseInfo.isTableExist(sql_query,"sfio")){
        QString  create_sql = "CREATE TABLE sfio ("
                    "machineNo  VARCHAR (20) PRIMARY KEY,"
                    "encyStatus CHAR (2),"
                    "encyCode  VARCHAR (50),"
                    "encyDays VARCHAR (5),"
                    "encyTime   DATE)";
        qDataBaseInfo.createTable(sql_query,create_sql);
        result = 1;
    }else{
//         qDebug()<<"table  sfio exist";
         result = 0;
    }
//    qDebug()<<"yes"<<qDataBaseInfo.checkIsTableDataExist(sql_query,"sfio");
    return result;
}

int createCodeTable(){
//    QDataBase qDataBase;
//    QSqlDatabase database = qDataBase.initQDatabase();
//    qDataBaseInfo.openQDatabase(database);
    int result = 0;
    //查询数据表是否存在
    QSqlQuery sql_query;
    if(!qDataBaseInfo.isTableExist(sql_query,"encycodeio")){
        QString  create_sql = "CREATE TABLE encycodeio ("
                "machineNo VARCHAR (20),"
                "encyCode  VARCHAR (50),"
                "encyTime  DATE)";
        qDataBaseInfo.createTable(sql_query,create_sql);
        result = 1;
    }else{
//         qDebug()<<"table  encycodeio exist";
         result = 0;
    }
    return result;
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_UNIX
    QSettings setting(CNF_MY, QSettings::NativeFormat);
    printf("\n\nstart run %s_%s_R%s\t\n\n",
            setting.value("Version/MYAPP_NAME", "6SXZ-300F_XS").toString().toLocal8Bit().constData(),
            setting.value("Version/MYAPP_VERSION", "V2.00PRE").toString().toLocal8Bit().constData(),
            setting.value("Version/MYAPP_SVN", "1864").toString().toLocal8Bit().constData());
    fflush(stdout);
#endif

    myApplication a(argc, argv);

//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_OPERATOR;

    QLabel *label = new QLabel;
    label->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    label->setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT);
    label->setWindowOpacity(1);
    QMovie* movie=new QMovie(":/res/png/Loading.gif");

    label->setMovie(movie);//Label添加动图
    movie->start();//启动动图
    label->setScaledContents(true);//自适应
    label->show();
//    // 创建一个进度条
//   QProgressBar *progressBar = new QProgressBar();
//   QString s1 = "QProgressBar {\
//       border: 2px solid grey;\
//       border-radius: 5px;\
//       text-align: center;\
//       color:#260000;\
//   }";

//   QString s2 = "QProgressBar::chunk {\
//       background-color: #05B8CC;\
//       width: 20px;\
//       margin: 0.5px;\
//   }";


   //#ifdef Q_OS_UNIX
       //! 设置开机图片
//       setLogo(label);
   //#endif

//   progressBar->setStyleSheet(s1+s2);
//   progressBar->setGeometry(0, LCD_HEIGHT-80, LCD_WIDTH, 80);
////   progressBar->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
//   progressBar->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
//   progressBar->setRange(0, 100); // 设置进度条范围
//   progressBar->setValue(0); // 设置初始进度值
//   progressBar->setFormat(QString("System Loading ....")+QString(" %p%"));
//   progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
////   progressBar->setTextVisible(false);

   //! 读取配置文件
   myFlow.initAll();

   //! 设置字体
   a.setFont(getFont());


//   for (int i = 0; i <= 100; i=i+10) {
//       if(i==10){
//            label->show();
//            progressBar->show();
//       }
//       progressBar->setValue(i); // 更新进度
//       QCoreApplication::processEvents(); // 确保界面更新
//       // 模拟耗时操作
//        myFlow.msleep(400);
//   }

    MainWidget w;
    createSysTable();
    createCodeTable();
    createModeInfoTable();
    createModeParaInfoTable();
    createParaInfoTable();

// 设置定时器，在一段时间后关闭启动画面并显示主窗口
    QTimer::singleShot(2000, [&]() {
        // 停止动图播放
        movie->stop();
        // 释放QMovie对象的内存
        delete movie;
        // 关闭启动画面
        label->close();
        // 显示主窗口
        w.show();
    });

//    w.show();

    // 进度条达到100%时，移除开机图
//    label->hide();
//    progressBar->hide();

//    label->hide();

    return a.exec();
}
