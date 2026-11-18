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
#include <QEventLoop>
#include <QPointer>
#include "unilog.h"
#include "sqlitemgr.h"
#include "configmgr.h"
#include "loadingwindow.h"
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
        font.setFamily("Noto Sans SC");
        font.setWeight(QFont::Medium);

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

    // ⭐ 全局禁用平台原生对话框（板卡上 GTK3 原生文件对话框无 transient parent，
    // 会导致 Gtk-Message 警告并卡死；且原生对话框会忽略 QFileDialog 的默认目录设置）。
    // 必须在 QApplication 构造前设置。
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs);

    myApplication a(argc, argv);

    // 初始化日志库
    LOG_INIT(CNF_LOG_CFG);

    LOG_INFO_STM("=========================Start Sorter=========================");

    // 初始化数据库
    SQLiteMgr::Instance().Init();

    // 初始化AI配置
    ConfigMgr::Instance().Init();

    // 注册 Noto Sans SC 字体（aimodelset.ui 和全局 getFont() 都依赖它）
    // 不注册会导致 Qt 回退默认字体 → 控件 label 尺寸变大 → UI 整体下移
    static const char* kFonts[] = {
        ":/res/Noto_Sans_SC/static/NotoSansSC-Regular.ttf",
        ":/res/Noto_Sans_SC/static/NotoSansSC-Medium.ttf",
        ":/res/Noto_Sans_SC/static/NotoSansSC-Bold.ttf",
    };
    for (auto f : kFonts) QFontDatabase::addApplicationFont(f);

//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_OPERATOR;

    //! 启动画面：底板 + 光环动画 + 5 组检测进度
    LoadingWindow *splash = new LoadingWindow;
    splash->showFullScreen();
    // 先渲染几帧，避免 initAll() 阻塞时启动页完全冻结
    for (int i = 0; i < 5; ++i)
    {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    //! 开机检测状态上报 → 启动页
    //! splash 在开机完成后会被 deleteLater() 销毁, 而自检列表页点击"刷新"
    //! 会再次触发 startComSlt() → bootStage() → initStageChanged 信号.
    //! 这里必须用 QPointer 守护, 否则回调访问已析构的 splash 会直接崩溃.
    QPointer<LoadingWindow> splashGuard(splash);
    QObject::connect(&myFlow, &GlobalFlow::initStageChanged, &a,
                     [splashGuard](int stage, int status, const QString &detail) {
                         if (splashGuard)
                         {
                             splashGuard->onStageChanged(stage, status, detail);
                         }
                     });
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

   //! 全局输入框样式：深色底 + 1px 钢青描边，悬停/聚焦切换亮青描边（九宫格切片 4px）
   a.setStyleSheet(
       "QLineEdit {"
       "  border-width: 4px 4px 4px 4px;"
       "  border-style: solid;"
       "  border-color: transparent;"
       "  border-image: url(:/res/png2/lineedit/lineedit_normal.png) 4 4 4 4 stretch stretch;"
       "  background: transparent;"
       "  color: #E8F6FF;"
       "  font-size: 16px;"
       "  padding: 0px 6px;"
       "  selection-background-color: #1E4A5E;"
       "  selection-color: #FFFFFF;"
       "}"
       "QLineEdit:hover {"
       "  border-image: url(:/res/png2/lineedit/lineedit_hover.png) 4 4 4 4 stretch stretch;"
       "}"
       "QLineEdit:focus {"
       "  border-image: url(:/res/png2/lineedit/lineedit_focus.png) 4 4 4 4 stretch stretch;"
       "}"
       "QLineEdit:disabled {"
       "  border-image: url(:/res/png2/lineedit/lineedit_disabled.png) 4 4 4 4 stretch stretch;"
       "  color: #5A7686;"
       "}");



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

    //! 5 组检测全部结束 → 收尾：先 show 主窗口，再关启动页，消除窗口间隙
    bool bootFinished = false;
    QObject::connect(&myFlow, &GlobalFlow::initStageChanged, &a,
                     [&](int stage, int status, const QString &detail) {
                         Q_UNUSED(detail);
                         if (stage != BOOT_STAGE_HW || bootFinished)
                         {
                             return;
                         }
                         if (status != BOOT_STATUS_OK
                             && status != BOOT_STATUS_ERROR
                             && status != BOOT_STATUS_SKIPPED)
                         {
                             return;
                         }
                         bootFinished = true;

                         // 正常 400ms 收尾；异常停留 2.5s 让用户看清红段与错误行
                         const int delay = (status == BOOT_STATUS_OK) ? 400 : 2500;
                         QTimer::singleShot(delay, &a, [&w, splash]() {
                             // ⭐ 先 show 主窗口 → 让 X11 立即把 MainWidget 映射到屏幕
                             //    再 hide splash → 彻底消除「窗口间隙」露出桌面背景的问题
                             w.showFullScreen();
                             QCoreApplication::processEvents();

                             splash->close();
                             splash->deleteLater();
                         });
                     });

//    w.show();

    // 进度条达到100%时，移除开机图
//    label->hide();
//    progressBar->hide();

//    label->hide();

    return a.exec();
}
