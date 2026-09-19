#include <opencv2/opencv.hpp>

#include "aimodelset.h"
#include "ui_aimodelset.h"
#include "cmdudpmanager.h"
#include "cmdworker.h"
#include "sftp_client.h"
#include "unilog.h"
#include <queue>
#include <iostream>
#include <sstream>

// ── unilog LOG_*_STM 依赖：允许 Qt 类型直接通过 << 输出 ──
#ifndef AIMODELSET_QT_STREAM_FIX
#define AIMODELSET_QT_STREAM_FIX
#include <sstream>
inline std::ostream& operator<<(std::ostream& os, const QString& s) {
    return os << s.toStdString();
}
inline std::ostream& operator<<(std::ostream& os, const QByteArray& b) {
    return os << b.toStdString();
}
inline std::ostream& operator<<(std::ostream& os, const QStringList& l) {
    return os << l.join(", ").toStdString();
}
#endif

#include <QDirIterator>
#include <QListWidget>
#include <QListWidgetItem>
#include <QImageReader>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>
#include <QHostAddress>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include "common/myinputmethod.h"
#include "common/myinputpanel.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <algorithm>   // std::sort

// ── IoU 辅助：两个矩形的交并比 ──
static double calcIoU(const QRect& a, const QRect& b)
{
    QRect inter = a.intersected(b);
    if (inter.width() <= 0 || inter.height() <= 0) return 0.0;
    double interArea = inter.width() * inter.height();
    double unionArea = (double)a.width() * a.height() + (double)b.width() * b.height() - interArea;
    return unionArea > 0 ? interArea / unionArea : 0.0;
}

// ── 辅助：判断一张图片是否存在"标注和仿真 cls_id 不一致"的情况 ──
// 读取 {imgDir}/pred/pred_{name}.txt + {imgDir}/{name}.txt（人工标注）
// 返回 true 表示有错标
static bool hasMismatch(const QString& imgPath)
{
    QFileInfo fi(imgPath);
    QString baseDir = fi.absolutePath();
    QString baseName = fi.completeBaseName();

    // 1. pred txt
    QFile predFile(baseDir + "/pred/pred_" + baseName + ".txt");
    if (!predFile.exists()) return false;
    QVector<ObjInfo> predObjs;
    {
        predFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream ts(&predFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine().trimmed();
            if (line.isEmpty()) continue;
            QStringList parts = line.split(' ', QString::SkipEmptyParts);
            if (parts.size() < 6) continue;
            ObjInfo o;
            o.cls_id_ = parts[0].toInt();
            o.x_ = parts[1].toInt(); o.y_ = parts[2].toInt();
            o.w_ = parts[3].toInt(); o.h_ = parts[4].toInt();
            o.score_ = parts[5].toFloat();
            predObjs.append(o);
        }
    }
    if (predObjs.isEmpty()) return false;

    // 2. 人工标注 txt
    QFile annoFile(baseDir + "/" + baseName + ".txt");
    if (!annoFile.exists()) return false;
    QVector<QPair<int, QRect>> annoList;  // classId + rect
    {
        annoFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream ts(&annoFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine().trimmed();
            if (line.isEmpty()) continue;
            QStringList parts = line.split(' ', QString::SkipEmptyParts);
            if (parts.size() != 5) continue;
            int cid = parts[0].toInt();
            QRect r(parts[1].toInt(), parts[2].toInt(), parts[3].toInt(), parts[4].toInt());
            annoList.append(qMakePair(cid, r));
        }
    }
    if (annoList.isEmpty()) return false;

    // 3. IoU 匹配 + cls_id 对比
    for (const auto& ap : annoList) {
        int annoCls = ap.first;
        QRect annoRect = ap.second;
        double bestIoU = 0; int bestCls = -1;
        for (const ObjInfo& p : predObjs) {
            QRect pr(p.x_, p.y_, p.w_, p.h_);
            double iou = calcIoU(annoRect, pr);
            if (iou > bestIoU) { bestIoU = iou; bestCls = p.cls_id_; }
        }
        if (bestIoU > 0.3 && bestCls != annoCls) return true;  // 找到一个错标就返回
    }
    return false;
}

// ── FLOW 日志宏（走 unilog WARN 级别）：毫秒时间戳 + 线程ID，定位阻塞点 ──
#define FLOW(tag) LOG_WARN_STM("[FLOW]" << QDateTime::currentMSecsSinceEpoch() \
    << "thr=" << (quintptr)QThread::currentThreadId() << tag)

//#include <opencv2/opencv.hpp>

// 将字节数转为人类可读大小（如 "5.23 MB"、"1.50 GB"），用于下载进度提示
static QString formatFileSize(qint64 bytes)
{
    if (bytes < 0) return "0 B";
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;
    double v = static_cast<double>(bytes);
    if (v >= GB) return QString("%1 GB").arg(v / GB, 0, 'f', 2);
    if (v >= MB) return QString("%1 MB").arg(v / MB, 0, 'f', 2);
    if (v >= KB) return QString("%1 KB").arg(v / KB, 0, 'f', 2);
    return QString("%1 B").arg(bytes);
}


AiModelSet::AiModelSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AiModelSet)
{
    ui->setupUi(this);
    // 强制对齐到 parent 左上角，避免 stackedWidget layout 偏移
    move(0, 0);
    modelCategoryNum = 0;  // ⭐ 默认 0，页面刚进来不显示类别按钮；新建/加载模型后才设置

    // ⭐ 开启鼠标追踪：让 mouseMoveEvent 在不按任何键时也能触发（悬停提示前景像素数）
    setMouseTracking(true);
    ui->imgLabel->setMouseTracking(true);
    // imgLabel 上的鼠标移动事件转发给父级（AiModelSet::mouseMoveEvent）
    ui->imgLabel->installEventFilter(this);

    // 登录进来不显示 checkbox，等用户新建或加载模型后再显示
    // (m_cbContainer 初始为 nullptr, setupClassCheckBoxes 首次调用时才创建)

    // ── m_imageList 自适应 2行×5列 ──────────────────────────────────────
    ui->m_imageList->setViewMode(QListView::IconMode);
    ui->m_imageList->setWrapping(true);                   // 自动换行
    ui->m_imageList->setMovement(QListWidget::Static);    // 禁止拖拽
    ui->m_imageList->setSpacing(8);                        // 间距

    // 隐藏滚动条
    ui->m_imageList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->m_imageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 根据 .ui 里 m_imageList 的实际尺寸自适应：5 列 × 2 行
    const int COL = 5;
    const int ROW = 2;
    const int PAD_X = 12;  // 列表左右 padding
    const int PAD_Y = 10;  // 列表上下 padding

    int listW = ui->m_imageList->width();
    int listH = ui->m_imageList->height();

    int gridW = (listW - PAD_X * 2) / COL;   // 每格宽度
    int gridH = (listH - PAD_Y * 2) / ROW;   // 每格高度
    int iconW = gridW - ui->m_imageList->spacing();  // 图标宽度
    int iconH = gridH - 24;                          // 图标高度（24px 给文件名）
    if (iconH < 20) iconH = 20;  // 最低保底

    ui->m_imageList->setGridSize(QSize(gridW, gridH));
    ui->m_imageList->setIconSize(QSize(iconW, iconH));

    // 初始化模型接口
    m_modelApi = new ModelApi(this);
    m_modelApi->setHttpTimeout(5000);  // 训练查询/创建/启动超时 5 秒（超过基本可以判定服务挂了）
    
    // 加载训练服务器配置（arm 版本从 JSON 读，开发版用默认值）
    loadTrainServerConfig();
    // 同步到 ModelApi（用成员变量构建 base URL）
    QString baseUrl = QString("http://%1:%2").arg(m_trainServerIp).arg(m_trainHttpPort);
    m_modelApi->setBaseUrl(baseUrl);
    LOG_INFO_STM("训练服务器配置：" << baseUrl << " SFTP=" << m_trainServerIp << ":" << m_trainSftpPort);

    // 初始化 SFTP Worker（libssh2 封装，moveToThread 到子线程）
    // 注意：不能传 parent（this），否则 moveToThread 会失败——Qt 规定有 parent 的 QObject 禁止 moveToThread
    m_sftpWorker = new SftpWorker(m_trainServerIp, m_trainSftpUser, m_trainSftpPass,
                                  m_trainSftpPort);
    SftpClient::init_sftp_lib();  // libssh2 全局一次

    connect(ui->backPushButton, SIGNAL(pressed()), this, SLOT(onSetBackBtnClicked()));
    connect(ui->modelSelPushButton, SIGNAL(pressed()), this, SLOT(onModelSelPushButtonClicked()));
    connect(ui->importImgPushButton, SIGNAL(clicked()), this, SLOT(onImportImgPushButtonClicked()));
    connect(ui->mergeImgPushButton, &QPushButton::clicked, this, &AiModelSet::onMergeDirsClicked);
    connect(ui->m_prevBtn, SIGNAL(clicked()), this, SLOT(prevPage()));
    connect(ui->m_nextBtn, SIGNAL(clicked()), this, SLOT(nextPage()));

    // 页码跳转：只允许输入 1-999 的数字，回车触发 gotoPage
    ui->m_curPagelineEdit->setValidator(new QIntValidator(1, 999, this));
    connect(ui->m_curPagelineEdit, &QLineEdit::returnPressed, this, &AiModelSet::gotoPage);

    // ── 前景目标按钮（独立 toggle，不参与互斥组）──────────────────
    ui->fgPushButton->setCheckable(true);
    connect(ui->fgPushButton, &QPushButton::clicked, this, &AiModelSet::onShowFgRectsBtnClicked);

    // ── 前景像素过滤按钮（toggle，显示每个框的前景像素个数）──────────
    ui->areaThresholdAnnoPushButton->setCheckable(true);
    connect(ui->areaThresholdAnnoPushButton, &QPushButton::clicked, this, &AiModelSet::onAreaThresholdAnnoPushButtonClicked);

    // ── 校验图片按钮（toggle，触发完整仿真流程）───────────────────
    ui->validImgPushButton->setCheckable(true);
    connect(ui->validImgPushButton, &QPushButton::clicked, this, &AiModelSet::onValidImgPushButtonClicked);

    ui->batchValidImgPushButton->setCheckable(true);
    connect(ui->batchValidImgPushButton, &QPushButton::clicked, this, &AiModelSet::onBatchValidImgPushButtonClicked);

    ui->validvsAnnoImgPushButton->setCheckable(true);
    connect(ui->validvsAnnoImgPushButton, &QPushButton::clicked, this, &AiModelSet::onValidvsAnnoImgPushButtonClicked);

    // ═══ 阈值 LineEdit：改完回车/失焦自动重算前景框 + 像素计数 ═══
    auto onThresholdChanged = [this]() {
        if (!m_currentImagePath.isEmpty()) {
            getFgRects();
            if (m_show_fg_pixel_count) computeFgPixelCounts();  // ⭐ 像素过滤开着也同步重算
            if (m_show_fg_rects || m_show_fg_pixel_count) update();
            else LOG_DEBUG_STM("[阈值] 已重算，但前景/像素过滤显示未开启");
        }
    };
    connect(ui->m_areaThresholdlineEdit, &QLineEdit::textChanged, this, onThresholdChanged);
    connect(ui->m_colorDiffThresholdlineEdit, &QLineEdit::textChanged, this, onThresholdChanged);

    // ⭐ 板卡上点击阈值输入框弹纯数字面板（myInputPanel = 项目内专用数字面板）
    {
        struct KbFilter : QObject {
            QString t;
            bool onlyDigits;
            explicit KbFilter(const QString &tt, bool digits, QObject *p) : QObject(p), t(tt), onlyDigits(digits) {}
            bool eventFilter(QObject *o, QEvent *e) override {
                if (e->type() == QEvent::MouseButtonPress) {
                    QLineEdit *le = qobject_cast<QLineEdit*>(o);
                    if (le) {
                        if (onlyDigits) {
                            // 与 mainwidget 里 operation level 密码同模式：栈上构造、不传 parent、intType=纯数字不遮罩
                            myInputPanel kb(intType, 1, 9999, le->text().toInt());
                            kb.setTitle(t);
                            if (kb.exec() == QDialog::Accepted) {
                                le->setText(QString::number((int)kb.getValue()));
                            }
                        } else {
                            myInputMethod kb(t, le->text());
                            if (kb.exec() == QDialog::Accepted) {
                                le->setText(kb.getText());
                            }
                        }
                        return true;
                    }
                }
                return QObject::eventFilter(o, e);
            }
        };
        ui->m_areaThresholdlineEdit->setFocusPolicy(Qt::NoFocus);
        ui->m_areaThresholdlineEdit->setValidator(new QIntValidator(1, 9999, this));
        ui->m_areaThresholdlineEdit->installEventFilter(new KbFilter("面积阈值", true, this));
        ui->m_colorDiffThresholdlineEdit->setFocusPolicy(Qt::NoFocus);
        ui->m_colorDiffThresholdlineEdit->setValidator(new QIntValidator(1, 9999, this));
        ui->m_colorDiffThresholdlineEdit->installEventFilter(new KbFilter("色差阈值", true, this));
    }

    // draw 和 selcet 模式是互斥的
    // 选中态样式（:checked）与 fgPushButton 一致，按下后有明显选中视觉
    const QString modeBtnQss = QStringLiteral(
        "QPushButton { text-align: center; outline: none; background-color: lightcyan;"
        " border-color: #afafaf; border-width: 2px; border-style: solid;"
        " border-radius: 5px; padding: 2px; }"
        "QPushButton:pressed { padding-left: 5px; padding-top: 5px; background-color: lightgrey; }"
        "QPushButton:checked  { background-color: #00bcd4; color: white;"
        " border-color: #0097a7; border-width: 3px; padding: 2px; }");
    // 一键标注 = 瞬时触发，不 setCheckable、不保持按下态
    ui->selectAnnoPushButton->setCheckable(true);
    ui->drawAnnoPushButton->setCheckable(true);
    ui->selectAnnoPushButton->setStyleSheet(modeBtnQss);
    ui->drawAnnoPushButton->setStyleSheet(modeBtnQss);
    connect(ui->oneKeyAnnoPushButton, &QPushButton::clicked, this, &AiModelSet::onOneKeyAnnoBtnClicked);
    connect(ui->selectAnnoPushButton, &QPushButton::clicked, this, &AiModelSet::onSelectAnnoBtnClicked);
    connect(ui->drawAnnoPushButton,   &QPushButton::clicked, this, &AiModelSet::onDrawAnnoBtnClicked);

    // 新增：绑定图片项点击信号（点击后触发预览）
    connect(ui->m_imageList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onImageItemClicked(QListWidgetItem*)));
    connect(ui->modelTrainPushButton, SIGNAL(pressed()), this, SLOT(onModelTrainPushButtonClicked()));
    connect(ui->modelNewPushButton, SIGNAL(pressed()), this, SLOT(onModelNewPushButtonClicked()));
    connect(ui->trainServerCfgPushButton, &QPushButton::clicked, this, &AiModelSet::onTrainServerCfgPushButtonClicked);

    connect(ui->addTrainListPushButton, SIGNAL(pressed()), this, SLOT(addTrainListPushButtonPressed()));
    connect(ui->delTrainListPushButton, SIGNAL(pressed()), this, SLOT(delTrainListPushButtonPressed()));
    connect(ui->delTrainListAllPushButton, &QPushButton::pressed, this, &AiModelSet::delTrainListAllPushButtonPressed);
    connect(ui->addAllLabeledTrainListPushButton, SIGNAL(pressed()), this, SLOT(addAllLabeledTrainListPushButtonPressed()));

    // ═══════════════════════════════════════════════════════════════════════
    // 训练流程信号槽链：
    //   onModelTrainPushButtonClicked
    //     → createDirTrain (HTTP POST /train/create)
    //        → SFTP 上传 raw/image + raw/label + classes.txt
    //           → startTrain (HTTP POST /train/start)
    //              → QTimer 每 5s 查 queryTrainProgress
    //                 → progress==100 → downloadModel + downloadModelJson
    // ═══════════════════════════════════════════════════════════════════════

    // [0] 创建远程目录 → moveToThread → worker.connect() → mkdir_p → 依次上传 → startTrain
    //
    // ── 信号槽机制回顾（Qt5 新式函数指针写法） ───────────────────────────────
    // connect(发送者, 信号, 接收者, 槽) 四个参数：
    //   1. m_modelApi                     —— 信号的发送者（发射 signal 的对象）
    //   2. &ModelApi::createDirTrainResult —— 要监听的信号（带类名限定，编译期校验）
    //   3. this                           —— 接收者（槽函数所在的对象，即当前 AiModelSet）
    //   4. &AiModelSet::onCreateDirTrainResult —— 槽函数（信号触发时要执行的方法）
    //
    // 触发时机：当 m_modelApi 内部调用 emit createDirTrainResult(response) 时，
    //   Qt 会自动调用 this->onCreateDirTrainResult(response)，参数原样传过来。
    //
    // 签名匹配：信号与槽的参数类型必须兼容（槽参数可以比信号少，但类型要对应）。
    //   本例双方都是 const TrainStartResponse&，完全一致。
    //
    // 线程安全：默认 AutoConnection —— 同线程直接调用（Direct），跨线程走事件队列（Queued）。
    //   m_modelApi 和 this 都在主线程，所以这里是同步直接调用。
    //
    // 生命周期：接收者 this 析构时，Qt 会自动断开这条连接，不会产生悬空调用。
    connect(m_modelApi, &ModelApi::createDirTrainResult, this, &AiModelSet::onCreateDirTrainResult);

    // [全局] HTTP 网络错误兜底处理（createDirTrain/startTrain/queryTrain/arch 查询等环节的 ConnectionRefused/Timeout 等）
    // 下载阶段（DownloadModel/DownloadJson）由 downloadFinished/downloadJsonFinished 独立处理，不走这里
    connect(m_modelApi, &ModelApi::networkError, this, [this](const QString& errMsg) {
        // ⭐ 轮询阶段的网络波动：允许重试 3 次（15 秒超时 × 3 = 45 秒足够判断）
        if (m_pollTimer && m_pollTimer->isActive()) {
            m_pollFailCount++;
            showTip(QString("训练服务连接波动（%1/3）：%2").arg(m_pollFailCount).arg(errMsg), true);
            if (m_pollFailCount >= 3) {
                m_pollTimer->stop();
                resetTrainingState();
                showTip("训练服务连接连续失败 3 次，已停止轮询", true);
            }
            return;
        }
        // 非轮询阶段（createDir/startTrain/arch 查询等），立即终止
        showTip(errMsg, true);
        resetTrainingState();
        LOG_DEBUG_STM("[训练流程] networkError 兜底：m_trainingBusy 已重置");
    });

    // [0.1] SftpWorker 单文件上传进度 → showTip
    // m_sftpUploadPhase 语义：0=图片上传中，1=标签上传中，2=classes 上传中
    connect(m_sftpWorker, &SftpWorker::progressChanged, this, [this](int current, int total) {
        QString phaseLabel = (m_sftpUploadPhase == 0) ? "图片上传"
                           : (m_sftpUploadPhase == 1) ? "标签上传"
                           : "classes 上传";
        showTip(QString("%1 %2|%3").arg(phaseLabel).arg(current).arg(total));
    });
    connect(m_sftpWorker, &SftpWorker::fileUploadCompleted, this,
        [this](const QString& localFile, const QString& remoteFile, bool success) {
        if (!success) {
            QFileInfo fi(localFile);
            showTip(QString("上传失败: %1").arg(fi.fileName()), true);
        }
    });
    connect(m_sftpWorker, &SftpWorker::error, this, [this](const QString& msg) {
        showTip(QString("SFTP 错误：%1").arg(msg), true);
    });

    // [0.2] SftpWorker 一批上传完成 → 串联下一个（image → label → classes → startTrain）
    connect(m_sftpWorker, &SftpWorker::allUploadCompleted, this, &AiModelSet::onSftpUploadCompleted);
    // [0.3] 上传后验证：remoteList 完成回调
    connect(m_sftpWorker, &SftpWorker::remoteListCompleted, this, &AiModelSet::onSftpRemoteListCompleted);

    // [1] 训练启动成功 → 启动 QTimer 轮询进度（每 5 秒一次）
    connect(m_modelApi, &ModelApi::startTrainResult, this, [this](const TrainStartResponse& response) {
        if (!response.isSuccess()) {
            showTip(QString("训练启动失败：%1（错误码：%2）").arg(response.message).arg(response.code), true);
            resetTrainingState();
            return;
        }
        showTip(QString("训练任务已提交，开始查询进度..."));
        m_pollFailCount = 0;
        m_pollTimer->start();  // 每 5s 触发 queryTrainProgress
    });

    // [2] 查询进度结果
    connect(m_modelApi, &ModelApi::queryTrainResult, this, &AiModelSet::onQueryTrainResult);

    // [3] 下载进度反馈
    connect(m_modelApi, &ModelApi::downloadProgress, this, [this](qint64 received, qint64 total) {
        if (total <= 0) return;
        int p = (received * 100) / total;
        showTip(QString("模型下载中：%1%（%2/%3）").arg(p).arg(formatFileSize(received)).arg(formatFileSize(total)));
    });

    // [3.5] 硬件架构查询结果 → 先下载 JSON（取 md5），再下载 .bin/.dlc 并校验
    // ═══ 关键修复：用 QTimer::singleShot(0, ...) 延迟触发 ═══
    // 原因：archResult 是由 HttpTool::requestSuccess 同步 emit 的，
    // 而 ModelApi 对 requestSuccess 有两个 DirectConnection connect：
    //   connect #1: onHttpRequestSuccess → 处理 archResult → 同步进入本 handler
    //   connect #2: lambda 检查 m_currentRequestType 是否为 DownloadJson
    // 如果本 handler 里同步调用 downloadModelJson 把 m_currentRequestType
    // 改成 DownloadJson，那 connect #2 就会误判 JSON 下载成功（其实还没开始），
    // 导致真正的 JSON reply 还没收到数据就被后续 bin 下载 cancel 掉。
    // 延迟到下一个事件循环 tick，让两个 connect 全部执行完再改 m_currentRequestType。
    connect(m_modelApi, &ModelApi::archResult, this, [this](const QString& arch) {
        m_modelArch = arch;
        QString savePath = QString(LOCAL_MODEL_PATH) + downLoadModelName + ".json";
        LOG_DEBUG_STM("[下载流程] archResult 触发：arch=" << arch << "downLoadModelName=" << downLoadModelName << "savePath=" << savePath);
        showTip(QString("硬件架构：%1，先下载 JSON：%2.json").arg(arch).arg(downLoadModelName));
        QTimer::singleShot(0, this, [this, savePath]() {
            m_modelApi->downloadModelJson(this->downLoadModelName + ".json", savePath);
        });
    });

    // [4] JSON 下载完成 → 读 JSON 取 md5 → 下载 .bin/.dlc
    connect(m_modelApi, &ModelApi::downloadJsonFinished, this, [this](bool success, const QString& errorMsg) {
        LOG_DEBUG_STM("[下载流程] downloadJsonFinished 触发：success=" << success << "errorMsg=" << errorMsg << "downLoadModelName=" << downLoadModelName);
        if (!success) {
            showTip(QString("JSON 下载失败：%1").arg(errorMsg), true);
            resetTrainingState();
            return;
        }

        // 读 JSON 取 md5（模型文件名里写进去，供 .bin 下载完成后校验）
        QString jsonPath = QString(LOCAL_MODEL_PATH) + downLoadModelName + ".json";
        LOG_DEBUG_STM("[下载流程] 尝试读取 JSON：" << jsonPath << "文件存在=" << QFile::exists(jsonPath) << "文件大小=" << (QFile::exists(jsonPath) ? QFileInfo(jsonPath).size() : 0));
        QFile jsonFile(jsonPath);
        QString expectedMd5;
        if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray raw = jsonFile.readAll();
            jsonFile.close();
            LOG_DEBUG_STM("[下载流程] JSON 原始内容（前 300 字节）：" << raw.left(300));
            QJsonParseError parseErr;
            QJsonDocument doc = QJsonDocument::fromJson(raw, &parseErr);
            if (parseErr.error != QJsonParseError::NoError) {
                LOG_WARN_STM("[下载流程] JSON 解析错误：" << parseErr.errorString() << "offset=" << parseErr.offset);
            }
            QJsonObject obj = doc.object();
            LOG_DEBUG_STM("[下载流程] JSON object keys=" << obj.keys());
            // 常见字段：md5 / file_md5 / model_md5 / bin_md5
            for (const QString& key : {"md5", "file_md5", "model_md5", "bin_md5", "dlc_md5"}) {
                if (obj.contains(key)) { expectedMd5 = obj.value(key).toString(); break; }
            }
            LOG_DEBUG_STM("[下载流程] JSON 解析完成，expectedMd5=" << expectedMd5);
        } else {
            LOG_WARN_STM("[下载流程] JSON 文件读失败" << jsonFile.errorString() << "路径=" << jsonPath);
        }
        m_expectedModelMd5 = expectedMd5;

        // 开始下载模型文件
        QString ext = (m_modelArch == "snpe") ? "dlc" : "bin";
        QString fileName = downLoadModelName + "." + ext;
        QString savePath = QString(LOCAL_MODEL_PATH) + fileName;
        showTip(QString("JSON 已下载，开始下载模型：%1").arg(fileName));
        QTimer::singleShot(0, this, [this, fileName, savePath]() {
            m_modelApi->downloadModel(fileName, savePath);
        });
    });

    // [5] 模型下载完成 → md5 校验 → 训练流程结束
    connect(m_modelApi, &ModelApi::downloadFinished, this, [this](bool success, const QString& errorMsg) {
        if (!success) {
            showTip(QString("模型下载失败：%1").arg(errorMsg), true);
            resetTrainingState();
            return;
        }

        // 做 md5 校验
        QString ext = (m_modelArch == "snpe") ? "dlc" : "bin";
        QString modelPath = QString(LOCAL_MODEL_PATH) + downLoadModelName + "." + ext;
        QString actualMd5;
        {
            QFile f(modelPath);
            if (f.open(QIODevice::ReadOnly)) {
                actualMd5 = QString::fromLatin1(QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5).toHex());
                f.close();
            }
        }
        LOG_DEBUG_STM("[下载流程] md5 校验：expected=" << m_expectedModelMd5 << "actual=" << actualMd5);

        if (!m_expectedModelMd5.isEmpty() && !actualMd5.isEmpty()
            && !m_expectedModelMd5.compare(actualMd5, Qt::CaseInsensitive)) {
            showTip(QString("模型下载完成且校验通过（MD5=%1）").arg(actualMd5));
        } else if (!m_expectedModelMd5.isEmpty()) {
            showTip(QString("✗ 模型 MD5 校验失败！expected=%1 actual=%2")
                    .arg(m_expectedModelMd5).arg(actualMd5), true);
            resetTrainingState();
            return;
        } else {
            // JSON 里没 md5 字段，跳过校验（正常，先让流程走通）
            LOG_DEBUG_STM("[下载流程] JSON 里没 md5 字段，跳过校验");
            showTip(QString("模型下载完成（JSON 无 md5 字段，跳过校验）"));
        }

        resetTrainingState();
    });


   m_currentAnnotType = AnnotationType::TYPE_NONE;
   m_isDrawing = false;
   m_selectedAnnotIndex = -1;

   // 登录进来不创建 checkbox，等用户点"新建模型"或"加载模型"时再创建

   connect(ui->erasePushButton, &QPushButton::clicked, this, &AiModelSet::onDeleteAnnotBtnClicked);

   // ── 退回上一步标注（独立工程专属） ─────────────────────────────────
   connect(ui->backLastAnnoPushButton, &QPushButton::clicked, this, &AiModelSet::onBackLastAnnoBtnClicked);
   ui->backLastAnnoPushButton->setEnabled(false);  // 初始无历史，按钮置灰

}

// [0] 创建远程目录 → moveToThread → worker.connect() → mkdir_p → 依次上传 → startTrain
void AiModelSet::onCreateDirTrainResult(const TrainStartResponse& response)
{
    qInfo() << "[SFTP] ⭐ onCreateDirTrainResult：success=" << response.isSuccess()
            << "msg=" << response.message << "code=" << response.code;
    if (!response.isSuccess()) {
        showTip(QString("创建远程目录失败：%1（错误码：%2）").arg(response.message).arg(response.code), true);
        resetTrainingState();
        return;
    }
    showTip("远程目录已创建，准备 SFTP 上传...");

    // 构造路径（存到成员变量，供后续信号槽使用）
    QString tmpDir = QDir(QCoreApplication::applicationDirPath()).filePath("tmp");
    m_sftpLocalImgDir  = QDir(tmpDir).filePath("image");
    m_sftpLocalLblDir  = QDir(tmpDir).filePath("label");
    m_sftpClassesFile  = QDir(tmpDir).filePath("classes.txt");
    m_sftpRemoteRootDir = QString("/ftp/%1/raw").arg(m_currentTaskId);
    m_sftpRemoteImgDir  = m_sftpRemoteRootDir + "/image";
    m_sftpRemoteLblDir  = m_sftpRemoteRootDir + "/label";
    m_sftpUploadPhase = 0;

    // 1. 清理旧 SFTP 线程（worker 保留，在主线程重新 connect）
    qInfo() << "[SFTP] ⭐ step1: cleanupSftpResources()";
    cleanupSftpResources();
    
    // 2. 主线程 connect（服务器端会在 startTrain 时自动准备好 /ftp/{taskId}/raw/ 目录结构）
    qInfo() << "[SFTP] ⭐ step2: m_sftpWorker->connect()";
    if (!m_sftpWorker->connect()) {
        showTip(QString("SFTP connect 失败：%1:%2").arg(m_trainServerIp).arg(m_trainSftpPort), true);
        resetTrainingState();
        return;
    }
    showTip("SFTP 已连接，开始上传训练数据...");

    // 3. moveToThread + 启动上传（session 已建立，moveToThread 后继续复用）
    m_sftpThread = new QThread(this);
    m_sftpWorker->moveToThread(m_sftpThread);
    qInfo() << "[SFTP] ⭐ step3: moveToThread done, starting thread";

    connect(m_sftpThread, &QThread::started, this, [this]() {
        qInfo() << "[SFTP] ⭐ thread started → invokeMethod onUploadLocalDir("
                << m_sftpLocalImgDir << ")";
        QMetaObject::invokeMethod(m_sftpWorker, "onUploadLocalDir", Qt::QueuedConnection,
                                  Q_ARG(QString, m_sftpLocalImgDir),
                                  Q_ARG(QString, m_sftpRemoteImgDir));
    });
    connect(m_sftpThread, &QThread::finished, this, [this]() {
        if (m_sftpWorker) m_sftpWorker->moveToThread(this->thread());
    });

    m_sftpThread->start();
}

// [0.2] SftpWorker 一批上传完成 → 验证 → 串联下一个（image → label → classes → startTrain）
void AiModelSet::onSftpUploadCompleted(bool success)
{
    qInfo() << "[SFTP] ⭐ onSftpUploadCompleted：phase=" << m_sftpUploadPhase
            << "success=" << success
            << "lastMethod=" << m_sftpLastUploadMethod;
    LOG_DEBUG_STM("[SFTP] onSftpUploadCompleted：phase=" << m_sftpUploadPhase << " success=" << success);
    if (!success) {
        showTip("SFTP 上传出现错误，继续验证...", true);
    }

    // 图片/标签/classes 每个阶段上传完后都做验证
    m_sftpVerifyRetry = 0;  // 重置重试计数
    QString remoteDir;
    QString phaseName;
    QString uploadMethod;      // "onUploadLocalDir" 或 "onUploadFiles"
    QString uploadArg1;        // 第一个参数（本地目录 or 本地文件）
    QString uploadArg2;        // 第二个参数（远程目录）

    if (m_sftpUploadPhase == 0) {
        remoteDir = m_sftpRemoteImgDir;
        phaseName = "图片";
        uploadMethod = "onUploadLocalDir";
        uploadArg1 = m_sftpLocalImgDir;
        uploadArg2 = m_sftpRemoteImgDir;
    } else if (m_sftpUploadPhase == 1) {
        remoteDir = m_sftpRemoteLblDir;
        phaseName = "标签";
        uploadMethod = "onUploadLocalDir";
        uploadArg1 = m_sftpLocalLblDir;
        uploadArg2 = m_sftpRemoteLblDir;
    } else if (m_sftpUploadPhase == 2) {
        remoteDir = m_sftpRemoteRootDir;
        phaseName = "classes.txt";
        uploadMethod = "onUploadFiles";
        uploadArg1 = m_sftpClassesFile;
        uploadArg2 = m_sftpRemoteRootDir;
    } else {
        goto TRAIN_START;
    }

    m_sftpLastUploadMethod = uploadMethod;
    m_sftpLastUploadArg1 = uploadArg1;
    m_sftpLastUploadArg2 = uploadArg2;
    m_sftpVerifyPhase = m_sftpUploadPhase;

    // 发起 remoteList 验证
    m_sftpVerifying = true;
    showTip(QString("%1上传完成，正在验证...").arg(phaseName));
    LOG_DEBUG_STM("[SFTP] 开始验证 phase=" << m_sftpUploadPhase << " remoteDir=" << remoteDir);
    QMetaObject::invokeMethod(m_sftpWorker, "onRemoteList", Qt::QueuedConnection,
                              Q_ARG(QString, remoteDir));
    return;

TRAIN_START:
    // ── 全部上传验证通过 → disconnect + quit + startTrain ──
    showTip("训练数据上传验证通过，正在启动远程训练...");
    QMetaObject::invokeMethod(m_sftpWorker, "disconnect", Qt::QueuedConnection);
    QTimer::singleShot(0, m_sftpThread, [this]() {
        if (m_sftpThread) m_sftpThread->quit();
    });
    m_modelApi->startTrain(m_currentTaskId);
}

// [0.3] 上传后验证回调：remoteList 返回远程文件列表 → 对比本地数量
void AiModelSet::onSftpRemoteListCompleted(const QStringList& remoteFiles)
{
    if (!m_sftpVerifying) return;
    m_sftpVerifying = false;

    // 算本地期望文件数
    int localCount = 0;
    if (m_sftpVerifyPhase == 0 || m_sftpVerifyPhase == 1) {
        localCount = QDir(m_sftpVerifyPhase == 0 ? m_sftpLocalImgDir : m_sftpLocalLblDir)
                         .entryInfoList(QDir::Files).size();
    } else if (m_sftpVerifyPhase == 2) {
        localCount = QFileInfo(m_sftpClassesFile).exists() ? 1 : 0;
    }

    int remoteCount = remoteFiles.size();
    QString phaseName = (m_sftpVerifyPhase == 0) ? "图片"
                       : (m_sftpVerifyPhase == 1) ? "标签"
                                                  : "classes.txt";

    LOG_DEBUG_STM("[SFTP] 验证 phase=" << m_sftpVerifyPhase
                  << " localCount=" << localCount << " remoteCount=" << remoteCount
                  << " retry=" << m_sftpVerifyRetry);

    if (remoteCount >= localCount && localCount > 0) {
        // ✅ 验证通过，进入下一 phase
        showTip(QString("%1验证通过（%2/%3）").arg(phaseName).arg(remoteCount).arg(localCount));
        m_sftpUploadPhase++;
        // 递归触发下一个阶段（直接调 onSftpUploadCompleted，它会根据 phase 决定是验证还是 startTrain）
        onSftpUploadCompleted(true);
        return;
    }

    // 远程文件数 < 本地期望 → 重试上传（最多 3 次）
    if (m_sftpVerifyRetry < 3) {
        m_sftpVerifyRetry++;
        showTip(QString("%1验证失败（远程%2 < 本地%3），重试上传 (%4/3)...")
                    .arg(phaseName).arg(remoteCount).arg(localCount).arg(m_sftpVerifyRetry), true);
        if (m_sftpLastUploadMethod == "onUploadLocalDir") {
            QMetaObject::invokeMethod(m_sftpWorker, "onUploadLocalDir", Qt::QueuedConnection,
                                      Q_ARG(QString, m_sftpLastUploadArg1),
                                      Q_ARG(QString, m_sftpLastUploadArg2));
        } else {
            QMetaObject::invokeMethod(m_sftpWorker, "onUploadFiles", Qt::QueuedConnection,
                                      Q_ARG(QStringList, QStringList{m_sftpLastUploadArg1}),
                                      Q_ARG(QString, m_sftpLastUploadArg2));
        }
        return;
    }

    // ❌ 3 次重试都失败 → 报错 + 终止
    showTip(QString("%1上传验证失败（远程%2 < 本地%3），重试 3 次仍不通过，终止训练")
                .arg(phaseName).arg(remoteCount).arg(localCount), true);
    resetTrainingState();
}

// ⭐ 统一的训练状态重置：所有错误/成功结束都调用
void AiModelSet::resetTrainingState()
{
    LOG_DEBUG_STM("[训练流程] resetTrainingState() 被调用");
    // 1. 标志位
    m_trainingBusy = false;
    m_sftpUploadPhase = 0;
    m_sftpVerifyRetry = 0;
    m_sftpVerifyPhase = 0;
    m_sftpVerifying = false;
    m_sftpLastUploadMethod.clear();
    m_sftpLastUploadArg1.clear();
    m_sftpLastUploadArg2.clear();
    // 2. 轮询定时器
    if (m_pollTimer) m_pollTimer->stop();
    m_pollFailCount = 0;
    // 3. SFTP 资源彻底清理（阻塞等待线程退出，确保无悬垂指针）
    cleanupSftpResources();
}

// ⭐ 彻底清理 SFTP 线程（worker 保留，prepareTrain 里复用重连）
void AiModelSet::cleanupSftpResources()
{
    if (!m_sftpThread) return;
    LOG_DEBUG_STM("[训练流程] cleanupSftpResources()  thread="
                  << (m_sftpThread ? "running" : "null"));

    // ⭐ 先阻塞 disconnect（BlockingQueuedConnection，立即在子线程执行），确保 socket 正常关闭
    //    如果用 QueuedConnection，disconnect 会投到子线程事件队列，
    //    紧接着 quit() 把线程退了，disconnect 可能没跑到 → socket 残留 TIME_WAIT
    if (m_sftpWorker && m_sftpThread->isRunning()) {
        bool ok = QMetaObject::invokeMethod(m_sftpWorker, "disconnect",
                                            Qt::BlockingQueuedConnection);
        LOG_DEBUG_STM("[训练流程] blocking disconnect result=" << ok);
    }

    // 通知线程退出（此时 disconnect 已执行完毕）
    m_sftpThread->quit();
    if (!m_sftpThread->wait(2000)) {
        LOG_WARN_STM("[训练流程] cleanupSftpResources 线程未正常退出，terminate");
        m_sftpThread->terminate();
        m_sftpThread->wait(500);
    }
    // 把 worker 移回主线程（在删除 thread 之前）
    if (m_sftpWorker) {
        m_sftpWorker->moveToThread(QThread::currentThread());
    }
    delete m_sftpThread;
    m_sftpThread = nullptr;
}

// [2] 查询进度结果
void AiModelSet::onQueryTrainResult(const TrainQueryResponse& response)
{
    // ⭐ task_id 匹配：只处理当前正在训练的任务响应，忽略旧任务/其他任务的异步返回
    if (!response.task_id.isEmpty() && response.task_id != m_currentTaskId) {
        LOG_DEBUG_STM("[轮询] 忽略非当前任务响应：response.task_id=" << response.task_id
                     << " m_currentTaskId=" << m_currentTaskId);
        return;
    }

    if (!response.isSuccess()) {
        m_pollTimer->stop();   // 服务端已返回错误，立即停止轮询
        resetTrainingState();
        showTip(QString("训练服务错误：%1（错误码：%2）").arg(response.message).arg(response.code), true);
        return;
    }
    m_pollFailCount = 0;

    // 排队 / 训练中 / 完成 三种状态
    QString stateText;
    if (response.number > 0) {
        stateText = QString("排队中（前面还有 %1 个任务）").arg(response.number);
    } else if (response.progress >= 100) {
        stateText = "训练完成";
    } else {
        stateText = "训练中";
    }

    // ⭐ 训练过程中如果服务端返回了 message（可能是错误/警告/状态），一并显示到 tipLabel
    QString tipMsg = QString("训练进度：%1% | %2").arg(response.progress).arg(stateText);
    if (!response.message.isEmpty() && response.progress < 100) {
        tipMsg += QString(" | %1").arg(response.message);
    }
    showTip(tipMsg);

    // 进度 100% → 停止轮询 + 开始下载
    if (response.progress >= 100 && !response.model_name.isEmpty()) {
        m_pollTimer->stop();
        // 提取模型名 stem（去路径、去扩展名），后续用于拼 .bin/.dlc/.json 下载文件名
        downLoadModelName = QFileInfo(response.model_name).completeBaseName();
        m_modelRemoteDir = response.model_dir;   // 保存 SFTP 远程目录
        m_modelApi->setModelRemoteDir(response.model_dir);  // 传给 ModelApi 用于 SFTP get
        showTip(QString("训练完成！开始下载模型：%1").arg(response.model_name));

        // 先查询硬件架构，决定下载 .bin（x5）还是 .dlc（snpe）
        m_modelApi->queryArch();
    }
}

// ── 动态创建类别按钮 ──────────────────────────────────────────────────────────
// 固定区域: x=30, y=30, w=640, h=40
// 按钮宽度固定 24px，在区域内居中展开；底部跟计数 label
void AiModelSet::setupClassCheckBoxes(int count, const QStringList& names)
{
    if (m_cbContainer) {
        delete m_cbContainer;
        m_cbContainer = nullptr;
    }
    if (m_classBtnGroup) {
        delete m_classBtnGroup;
        m_classBtnGroup = nullptr;
    }

    // ⭐ 没加载数据 / 新建模型时 count<=0 → 不创建按钮
    if (count <= 0) {
        LOG_DEBUG_STM("🟡 setupClassCheckBoxes: count<=0, 跳过创建");
        return;
    }
    if (count > 9) count = 9;

    auto colorTable = getClassColorTable();

    // 存类别名
    m_classNames = names;
    if (m_classNames.size() < count) {
        for (int i = m_classNames.size(); i < count; ++i) {
            m_classNames.append(QString("%1").arg(i));
        }
    }

    const int AREA_X = 30, AREA_Y = 35, AREA_W = 640, AREA_H = 60;  // ⭐ 垂直中心对齐 trainServerCfgPushButton(y=30,h=41 → 中心50.5)
    const int BTN_FIXED_W = 48;
    const int BTN_FIXED_H = 24;
    const int FIXED_SPACING = 6;  // 按钮间水平间距
    const int BTN_LABEL_GAP = 5;  // 按钮行和计数行之间的垂直间距

    m_cbContainer = new QWidget(this);
    m_cbContainer->setObjectName("classButtonContainer");
    m_cbContainer->setGeometry(AREA_X, AREA_Y, AREA_W, AREA_H);

    m_classBtnGroup = new QButtonGroup(this);
    m_classBtnGroup->setExclusive(true);

    // ⭐ 容器用 QVBoxLayout：上面按钮一行，下面计数一行
    // 不设 stretch，两行自然堆叠，垂直间距完全由 spacing 控制
    auto *vbox = new QVBoxLayout(m_cbContainer);
    vbox->setContentsMargins(0, 4, 0, 4);  // 上下各 4px margin
    vbox->setSpacing(BTN_LABEL_GAP);

    // ── 第一行：按钮 ──
    auto *btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 0, 0, 0);
    btnRow->setSpacing(FIXED_SPACING);
    btnRow->addStretch(1);  // 左 stretch 居中

    // ── 第二行：计数 label ──
    auto *countRow = new QHBoxLayout();
    countRow->setContentsMargins(0, 0, 0, 0);
    countRow->setSpacing(FIXED_SPACING);  // 和按钮行同 spacing → label 对齐按钮
    countRow->addStretch(1);  // 左 stretch 居中

    m_classButtons.clear();
    m_classCountLabels.clear();

    LOG_DEBUG_STM("🟢 setupClassCheckBoxes: count=" << count);

    for (int i = 0; i < count; ++i) {
        QColor color = (i < colorTable.size()) ? colorTable[i] : QColor(Qt::black);
        QString label = (i < m_classNames.size()) ? m_classNames[i] : QString("%1").arg(i);

        // 互斥按钮（checkable），固定 48x24
        auto* btn = new QPushButton(label, m_cbContainer);
        btn->setCheckable(true);
        btn->setFixedSize(BTN_FIXED_W, BTN_FIXED_H);
        btn->setFont(QFont("HarmonyOS Sans Medium", 8, QFont::Bold));
        btn->setCursor(Qt::PointingHandCursor);

        QString colorStr = QString("rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue());
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; color: white; border: 2px solid transparent; border-radius: 3px; }"
            "QPushButton:checked { border: 2px solid #333333; }"
        ).arg(colorStr));

        m_classBtnGroup->addButton(btn, i);
        connect(btn, &QPushButton::clicked, this, &AiModelSet::onClassButtonClicked);

        // 计数 label，固定 48x18（和按钮同宽 → 完美对齐，高度足够容纳 8pt 文字）
        auto* countLbl = new QLabel("0", m_cbContainer);
        countLbl->setAlignment(Qt::AlignCenter);
        countLbl->setFixedSize(BTN_FIXED_W, 18);
        countLbl->setStyleSheet("color: #888; font-size: 9pt;");

        btnRow->addWidget(btn);
        countRow->addWidget(countLbl);
        m_classButtons.append(btn);
        m_classCountLabels.append(countLbl);
    }

    btnRow->addStretch(1);    // 右 stretch 居中
    countRow->addStretch(1);  // 右 stretch 居中

    vbox->addLayout(btnRow);
    vbox->addLayout(countRow);

    m_cbContainer->show();
    updateClassAnnotCounts();
}

// 通用类别按钮点击槽
void AiModelSet::onClassButtonClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int classId = m_classBtnGroup->id(btn);
    if (classId < 0) return;

    QString className = (classId < m_classNames.size()) ? m_classNames[classId] : QString("%1").arg(classId);
    auto colorTable = getClassColorTable();
    QString colorName = (classId < colorTable.size())
        ? QString("(%1,%2,%3)").arg(colorTable[classId].red()).arg(colorTable[classId].green()).arg(colorTable[classId].blue())
        : "";

    // QButtonGroup exclusive 模式自动互斥，不需要手动取消其他按钮
    m_currentAnnotType = annotTypeFromClassId(classId);
    showTip(QString("当前标注类型：%1 %2").arg(className).arg(colorName));
}

// 刷新每个类别按钮下的标注计数（统计所有训练图片的标签，非当前图片）
void AiModelSet::updateClassAnnotCounts()
{
    // 先清零
    for (auto* lbl : m_classCountLabels) {
        lbl->setText("0");
        lbl->setStyleSheet("color: #888; font-size: 9pt;");
    }

    // ⭐ 统计所有训练图片的标签（不是当前图片）
    // 规则：当前图片用内存 m_annotations（可能尚未保存），其余图片读同名 txt
    QMap<int, int> counts;
    for (const QString& imgPath : m_train_lists) {
        if (imgPath == m_currentImagePath) {
            // 当前图片：用内存标注，避免未保存导致计数遗漏
            for (const auto& a : m_annotations) {
                counts[static_cast<int>(a.type)]++;
            }
            continue;
        }
        // 其他图片：读取同名 label txt
        QFileInfo fi(imgPath);
        QString txtPath = fi.absolutePath() + "/" + fi.completeBaseName() + ".txt";
        QFile f(txtPath);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.isEmpty()) continue;
            bool ok = false;
            int cid = parts[0].toInt(&ok);
            if (ok) counts[cid]++;
        }
        f.close();
    }

    // 更新 label
    for (int i = 0; i < m_classCountLabels.size(); ++i) {
        int n = counts.value(i, 0);
        auto* lbl = m_classCountLabels[i];
        lbl->setText(QString::number(n));
        if (n > 0) {
            lbl->setStyleSheet("color: #000; font-size: 9pt; font-weight: bold;");
        }
    }
}

// 清除所有标注
void AiModelSet::onClearAnnotBtnClicked()
{
    pushAnnotUndo();
    m_annotations.clear();
    m_selectedAnnotIndex = -1;
    ui->annotationInfoEdit->clear();
    updateClassAnnotCounts();
    update(); // 刷新绘图
}

// ── 标注模式按钮（select / draw 互斥）──────────────────────────────────
// 辅助：清除另一个按钮的 checked 状态（避免触发其 signal）
static void _clearOtherBtn(QPushButton* other) {
    other->blockSignals(true);
    other->setChecked(false);
    other->blockSignals(false);
}

// 提示信息（red=true 用红色文字，否则恢复默认色）
void AiModelSet::showTip(const QString &msg, bool red) {
    // ⭐ 显式清除/设置颜色，避免上一条红色残留
    ui->tipLabel->setStyleSheet(red ? "color: red;" : "color: black;");
    ui->tipLabel->setText(msg);
}

void AiModelSet::onOneKeyAnnoBtnClicked()
{
    // ⭐ 一次性触发：点击立即执行，不保持按下态，也不改变 m_anno_mode
    if (m_currentAnnotType == AnnotationType::TYPE_NONE) {
        showTip("请先选一个类别 checkbox", true);
        return;
    }
    if (!m_currentImagePath.isEmpty()) {
        // 每次都重算，确保阈值调整后生效
        getFgRects();
    }
    if (m_fg_rects.isEmpty()) {
        showTip("未检测到前景框，无法一键标注", true);
        return;
    }
    // 所有前景框 → annotation，类别 = 当前 checkbox
    pushAnnotUndo();
    for (const QRect& fg : m_fg_rects) {
        AnnotationData a;
        a.type  = m_currentAnnotType;
        a.rect  = fg;
        a.color = getAnnotColorByType(m_currentAnnotType);
        a.label = getAnnotLabelByType(m_currentAnnotType);
        m_annotations.append(a);
        m_activate_class_ids.insert(classIdFromAnnotType(a.type));
    }
    showTip(QString("一键标注完成，共 %1 个框（类别 %2）")
        .arg(m_fg_rects.size()).arg(getAnnotLabelByType(m_currentAnnotType)));
    updateClassAnnotCounts();
    update();
}

void AiModelSet::onSelectAnnoBtnClicked()
{
    bool isActive = ui->selectAnnoPushButton->isChecked();
    if (isActive && m_currentAnnotType == AnnotationType::TYPE_NONE) {
        // 没选类别：回弹按钮 + 红色提示
        ui->selectAnnoPushButton->setChecked(false);
        m_anno_mode = AnnotaionMode::MODE_NONE;
        showTip("请先选择类别 checkbox", true);
        return;
    }
    if (isActive) _clearOtherBtn(ui->drawAnnoPushButton);  // 选中 select 时取消 draw
    m_anno_mode = isActive ? AnnotaionMode::MODE_SELECT : AnnotaionMode::MODE_NONE;
    if (isActive) showTip("点击前景框 → 标记为当前类别");
}

void AiModelSet::onDrawAnnoBtnClicked()
{
    bool isActive = ui->drawAnnoPushButton->isChecked();
    if (isActive && m_currentAnnotType == AnnotationType::TYPE_NONE) {
        // 没选类别：回弹按钮 + 红色提示
        ui->drawAnnoPushButton->setChecked(false);
        m_anno_mode = AnnotaionMode::MODE_NONE;
        showTip("请先选择类别 checkbox", true);
        return;
    }
    if (isActive) _clearOtherBtn(ui->selectAnnoPushButton);  // 选中 draw 时取消 select
    m_anno_mode = isActive ? AnnotaionMode::MODE_DRAW : AnnotaionMode::MODE_NONE;
    if (isActive) showTip("按住鼠标左键拖动 → 手动画框");
}


QRect AiModelSet::processROI(const QImage& img, const QRect& roi, int blueThres = 80, int minArea = 50)
{
    if (img.isNull() || !roi.isValid())
        return roi;

    QRect r = roi.intersected(img.rect());
    if (!r.isValid())
        return roi;

    int w = img.width();
    int h = img.height();

    // ----------------- 1. ROI 内前景中心点（种子点） -----------------
    int sumX = 0, sumY = 0, count = 0;
    for (int y = r.top(); y < r.bottom(); y++) {
        for (int x = r.left(); x < r.right(); x++) {
            if (qBlue(img.pixel(x, y)) < blueThres) {
                sumX += x;
                sumY += y;
                count++;
            }
        }
    }

    if (count == 0)
        return roi;  // ROI 内没有前景

    QPoint seed(sumX / count, sumY / count);

    // ----------------- 2. 扩展探索区域（ROI × 3，中心在 ROI 中心） -----------------
    int newW = r.width() * 3;
    int newH = r.height() * 3;
    QPoint center = r.center();
    QRect explore(center.x() - newW / 2, center.y() - newH / 2, newW, newH);
    explore = explore.intersected(img.rect());

    // ----------------- 3. Flood Fill (8 邻域) -----------------
    QVector<QVector<bool>> visited(h, QVector<bool>(w, false));
    QVector<QPoint> stack;
    stack.push_back(seed);
    visited[seed.y()][seed.x()] = true;

    int minX = seed.x(), maxX = seed.x();
    int minY = seed.y(), maxY = seed.y();
    int area = 0;

    const int dx[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    const int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};

    while (!stack.isEmpty()) {
        QPoint p = stack.takeLast();
        int x = p.x();
        int y = p.y();

        if (!explore.contains(p))
            continue;

        if (qBlue(img.pixel(x, y)) >= blueThres)
            continue; // 背景像素停止扩散

        // 更新 bounding box
        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
        area++;

        // 8 邻域扩散
        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            QPoint np(nx, ny);

            if (!img.rect().contains(np))
                continue;
            if (!explore.contains(np))
                continue;
            if (visited[ny][nx])
                continue;

            visited[ny][nx] = true;
            stack.push_back(np);
        }
    }

    if (area < minArea)
        return QRect(); // 面积太小，忽略

    return QRect(QPoint(minX, minY), QPoint(maxX, maxY));
}


// 删除选中标注
void AiModelSet::onDeleteAnnotBtnClicked()
{
    if (m_selectedAnnotIndex >= 0 && m_selectedAnnotIndex < m_annotations.size()) {
        pushAnnotUndo();
        m_annotations.removeAt(m_selectedAnnotIndex);
        m_selectedAnnotIndex = -1;
        ui->annotationInfoEdit->clear();
        updateClassAnnotCounts();
        update();
    } else {
        showTip("请先选中标注", true);
    }
}

// ── 标注撤销（缓存最多 3 个快照） ───────────────────────────────────────
void AiModelSet::pushAnnotUndo()
{
    const int kMaxUndoDepth = 3;  // 最多缓存 3 次操作
    if (m_annotUndoStack.size() >= kMaxUndoDepth) {
        m_annotUndoStack.removeFirst();  // 超出上限，丢弃最早的一次
    }
    m_annotUndoStack.append(m_annotations);
    ui->backLastAnnoPushButton->setEnabled(true);
}

void AiModelSet::clearAnnotUndo()
{
    m_annotUndoStack.clear();
    ui->backLastAnnoPushButton->setEnabled(false);
}

void AiModelSet::onBackLastAnnoBtnClicked()
{
    if (m_annotUndoStack.isEmpty()) {
        ui->backLastAnnoPushButton->setEnabled(false);  // 没有可退回的历史
        return;
    }
    m_annotations = m_annotUndoStack.takeLast();  // 恢复上一步标注状态
    m_selectedAnnotIndex = -1;
    ui->annotationInfoEdit->clear();
    update();

    if (m_annotUndoStack.isEmpty()) {
        ui->backLastAnnoPushButton->setEnabled(false);  // 历史退回完 → 置灰
    }
}


// ── 10 类颜色表（用户指定，索引 = classId） ─────────────────────────────────
QVector<QColor> AiModelSet::getClassColorTable()
{
    static const QVector<QColor> table = {
        QColor(0,   255, 0),    // 0: 绿色
        QColor(255, 0,   0),    // 1: 红色
        QColor(0,   0,   255),  // 2: 蓝色
        QColor(255, 255, 0),    // 3: 黄色
        QColor(255, 165, 0),    // 4: 橙色
        QColor(255, 0,   255),  // 5: 品红
        QColor(128, 0,   128),  // 6: 紫色
        QColor(0,   255, 255),  // 7: 青色
        QColor(255, 192, 203),  // 8: 粉色
        QColor(139, 69,  19),   // 9: 棕色
    };
    return table;
}

// 根据类型获取标注颜色
QColor AiModelSet::getAnnotColorByType(AnnotationType type)
{
    int classId = classIdFromAnnotType(type);
    if (classId < 0) return Qt::black;
    auto table = getClassColorTable();
    return (classId < table.size()) ? table[classId] : Qt::black;
}

// 根据类型获取标注名称
QString AiModelSet::getAnnotLabelByType(AnnotationType type)
{
    int classId = classIdFromAnnotType(type);
    if (classId < 0) return "未知类型";
    return QString("类型%1").arg(classId);
}

// 规范化矩形（确保左上角为起点，宽高为正）
QRect AiModelSet::getNormalizedRect(const QPoint& start, const QPoint& end)
{
    int x = qMin(start.x(), end.x());
    int y = qMin(start.y(), end.y());
    int width = qAbs(end.x() - start.x());
    int height = qAbs(end.y() - start.y());
    return QRect(x, y, width, height);
}

// 检查鼠标是否点击标注（返回索引，-1 无选中）
int AiModelSet::hitTestAnnotation(const QPoint& imageRelativePos)
{
    // 从后往前遍历（后画的框在上层，优先命中可见的框）
    for (int i = m_annotations.size() - 1; i >= 0; --i) {
        const QRect& annotRect = m_annotations[i].rect;
        // 扩大点击范围（图片内坐标）
        QRect expandRect = annotRect.adjusted(-10, -10, 10, 10); // 触摸适配，扩大10px
        if (expandRect.contains(imageRelativePos)) {
            return i;
        }
    }
    return -1;
}


// 鼠标按下：开始绘制或选中标注
void AiModelSet::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;

    if (m_currentPixmap.isNull()) {
        showTip("请先选择图片", true);
        return;
    }

    // ── 公共：算图片坐标 ──────────────────────────────────────────
    QPoint labelPos = ui->imgLabel->mapFromParent(event->pos());
    QPixmap scaledPixmap = ui->imgLabel->pixmap()->copy();
    if (scaledPixmap.isNull()) return;
    int offsetX = (ui->imgLabel->width()  - scaledPixmap.width())  / 2;
    int offsetY = (ui->imgLabel->height() - scaledPixmap.height()) / 2;
    QRect imageDisplayRect(offsetX, offsetY, scaledPixmap.width(), scaledPixmap.height());
    if (!imageDisplayRect.contains(labelPos)) {
        showTip("请在图片内操作", true);
        return;
    }
    QPoint imageRelativePos = labelPos - QPoint(offsetX, offsetY);

    switch (m_anno_mode) {

    // ── MODE_SELECT：点框改类别 + 切换为选中态 ──────────────────
    case AnnotaionMode::MODE_SELECT: {
        if (m_currentAnnotType == AnnotationType::TYPE_NONE) {
            showTip("请先选择类别 checkbox", true);
            return;
        }
        // ① 点中已有 annotation → 改类别/颜色 + 设为选中（实线）
        int hitAnnotIdx = hitTestAnnotation(imageRelativePos);
        if (hitAnnotIdx != -1) {
            m_selectedAnnotIndex = hitAnnotIdx;
            pushAnnotUndo();
            AnnotationData& annot = m_annotations[hitAnnotIdx];
            annot.type  = m_currentAnnotType;
            annot.color = getAnnotColorByType(m_currentAnnotType);
            annot.label = getAnnotLabelByType(m_currentAnnotType);
            m_activate_class_ids.insert(classIdFromAnnotType(m_currentAnnotType));
            ui->annotationInfoEdit->setText(
                QString("选中标注 %1: x=%2 y=%3 w=%4 h=%5")
                .arg(annot.label).arg(annot.rect.x()).arg(annot.rect.y())
                .arg(annot.rect.width()).arg(annot.rect.height()));
            showTip(QString("已改为类别 %1").arg(annot.label));
            update();
            return;  // ⭐ 点中已有 annotation 就不再找前景框
        }
        // ② 没点中 annotation → 找前景框变成新标注
        if (!m_show_fg_rects || m_fg_rects.isEmpty()) {
            showTip("请先点「前景目标」显示前景", true);
            return;
        }
        int hitIdx = -1;
        for (int i = 0; i < m_fg_rects.size(); ++i) {
            if (m_fg_rects[i].adjusted(-10, -10, 10, 10).contains(imageRelativePos)) {
                hitIdx = i;
                break;
            }
        }
        if (hitIdx == -1) {
            showTip("请点击一个前景框或已有标注", true);
            return;
        }
        AnnotationData a;
        a.type  = m_currentAnnotType;
        a.rect  = m_fg_rects[hitIdx];
        a.color = getAnnotColorByType(m_currentAnnotType);
        a.label = getAnnotLabelByType(m_currentAnnotType);
        pushAnnotUndo();
        m_annotations.append(a);
        m_activate_class_ids.insert(classIdFromAnnotType(a.type));
        m_selectedAnnotIndex = m_annotations.size() - 1;  // ⭐ 新建后自动选中
        showTip(QString("点击标注：类别 %1").arg(a.label));
        updateClassAnnotCounts();
        update();
        return;
    }

    // ── MODE_DRAW：手动拉框 ────────────────────────────────────────
    case AnnotaionMode::MODE_DRAW: {
        if (m_currentAnnotType == AnnotationType::TYPE_NONE) {
            showTip("请先选一个类别 checkbox", true);
            return;
        }
        m_selectedAnnotIndex = hitTestAnnotation(imageRelativePos);
        if (m_selectedAnnotIndex != -1) {
            const AnnotationData& annot = m_annotations[m_selectedAnnotIndex];
            ui->annotationInfoEdit->setText(
                QString("选中标注：%1  x=%2 y=%3 w=%4 h=%5")
                .arg(annot.label).arg(annot.rect.x()).arg(annot.rect.y())
                .arg(annot.rect.width()).arg(annot.rect.height()));
            update();
            return;
        }
        m_isDrawing = true;
        m_drawStartPos = imageRelativePos;
        m_drawEndPos = imageRelativePos;
        return;
    }

    // ── MODE_NONE / MODE_ONEKEY：空闲态，仅允许点击选中已有标注 ──
    default: {
        m_selectedAnnotIndex = hitTestAnnotation(imageRelativePos);
        if (m_selectedAnnotIndex != -1) {
            const AnnotationData& annot = m_annotations[m_selectedAnnotIndex];
            ui->annotationInfoEdit->setText(
                QString("选中标注：%1  x=%2 y=%3 w=%4 h=%5")
                .arg(annot.label).arg(annot.rect.x()).arg(annot.rect.y())
                .arg(annot.rect.width()).arg(annot.rect.height()));
        } else {
            m_selectedAnnotIndex = -1;
            ui->annotationInfoEdit->clear();
        }
        update();
        return;
    }
    } // switch
}

// 鼠标移动：调整标注大小
void AiModelSet::mouseMoveEvent(QMouseEvent *event)
{
    // ── 悬停提示：像素过滤开启时，显示当前悬停框的前景像素数 ──
    if (!m_isDrawing && m_show_fg_pixel_count && !m_currentPixmap.isNull()) {
        QPoint labelPos = ui->imgLabel->mapFromParent(event->pos());
        QPixmap scaledPixmap = ui->imgLabel->pixmap()->isNull() ? m_currentPixmap : ui->imgLabel->pixmap()->copy();
        if (!scaledPixmap.isNull()) {
            int offsetX = (ui->imgLabel->width() - scaledPixmap.width()) / 2;
            int offsetY = (ui->imgLabel->height() - scaledPixmap.height()) / 2;
            QPoint imgPos = labelPos - QPoint(offsetX, offsetY);

            // 检查标注框
            for (int i = 0; i < m_annotations.size(); ++i) {
                if (m_annotations[i].rect.contains(imgPos)) {
                    int cnt = (i < m_annot_pixel_counts.size()) ? m_annot_pixel_counts[i] : 0;
                    int thresh = 50;
                    bool ok = true;
                    int t = ui->m_areaThresholdlineEdit->text().toInt(&ok);
                    if (ok && t > 0) thresh = t;
                    showTip(QString("标注[%1] 前景像素=%2 %3")
                        .arg(m_annotations[i].label).arg(cnt)
                        .arg(cnt >= thresh ? "≥阈值 ✓" : "<阈值 ✗"));
                    return;
                }
            }
            // 检查前景框
            for (int i = 0; i < m_fg_rects.size(); ++i) {
                if (m_fg_rects[i].contains(imgPos)) {
                    int cnt = (i < m_fg_pixel_counts.size()) ? m_fg_pixel_counts[i] : 0;
                    int thresh = 50;
                    bool ok = true;
                    int t = ui->m_areaThresholdlineEdit->text().toInt(&ok);
                    if (ok && t > 0) thresh = t;
                    showTip(QString("前景框[%1] 前景像素=%2 %3")
                        .arg(i).arg(cnt)
                        .arg(cnt >= thresh ? "≥阈值 ✓" : "<阈值 ✗"));
                    return;
                }
            }
            // 检查预测/仿真框
            for (int i = 0; i < m_emulateObjInfos.size(); ++i) {
                const ObjInfo& oi = m_emulateObjInfos[i];
                QRect r(static_cast<int>(oi.x_), static_cast<int>(oi.y_),
                        static_cast<int>(oi.w_), static_cast<int>(oi.h_));
                if (r.contains(imgPos)) {
                    int cnt = (i < m_emulate_pixel_counts.size()) ? m_emulate_pixel_counts[i] : 0;
                    int thresh = 50;
                    bool ok = true;
                    int t = ui->m_areaThresholdlineEdit->text().toInt(&ok);
                    if (ok && t > 0) thresh = t;
                    showTip(QString("预测框[%1] 前景像素=%2 %3")
                        .arg(oi.cls_id_).arg(cnt)
                        .arg(cnt >= thresh ? "≥阈值 ✓" : "<阈值 ✗"));
                    return;
                }
            }
        }
    }

    if (!m_isDrawing) return;
    LOG_INFO_STM("Begin to draw move");
    // 1. 鼠标坐标 → imgLabel 控件坐标 → 图片内相对坐标
       QPoint labelPos = ui->imgLabel->mapFromParent(event->pos());
       QPixmap scaledPixmap = ui->imgLabel->pixmap()->copy();
       if (scaledPixmap.isNull()) return;
       int offsetX = (ui->imgLabel->width() - scaledPixmap.width()) / 2;
       int offsetY = (ui->imgLabel->height() - scaledPixmap.height()) / 2;
       QRect imageDisplayRect(offsetX, offsetY, scaledPixmap.width(), scaledPixmap.height());

       // 2. 限制移动范围在图片内
       QPoint imageRelativePos = labelPos - QPoint(offsetX, offsetY);
       imageRelativePos.setX(qMin(qMax(imageRelativePos.x(), 0), scaledPixmap.width()));
       imageRelativePos.setY(qMin(qMax(imageRelativePos.y(), 0), scaledPixmap.height()));

       m_drawEndPos = imageRelativePos;
       update(); // 刷新绘图
}

// 鼠标松开：完成标注
void AiModelSet::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!m_isDrawing) return;

    m_isDrawing = false;
    QRect newAnnotRect = getNormalizedRect(m_drawStartPos, m_drawEndPos);

    // MODE_DRAW 下拉框后自动用边缘提取拟合（可选，用户原逻辑保留）
    if (m_anno_mode == AnnotaionMode::MODE_DRAW) {
        QImage image = m_currentPixmap.toImage();  // ✅ 必须用原图，不能用 imgLabel 上已画框的合成图
    FLOW("getFgRects toImage后");
        QRect roi(m_drawStartPos, m_drawEndPos);
        QRect fitted = processROI(image, roi, 80, 50);
        if (fitted.width() > 10 && fitted.height() > 10) {
            newAnnotRect = fitted;
        }
    }
    
   // 过滤过小矩形
   if (newAnnotRect.width() < 10 || newAnnotRect.height() < 10) {
       showTip("标注矩形过小，请重新绘制", true);
       return;
   }

   // 创建标注（坐标是图片内相对坐标）
   AnnotationData newAnnot;
   newAnnot.type = m_currentAnnotType;
   newAnnot.rect = newAnnotRect; // 关键：存储的是图片内的坐标，而非控件坐标
   newAnnot.color = getAnnotColorByType(m_currentAnnotType);
   newAnnot.label = getAnnotLabelByType(m_currentAnnotType);

   pushAnnotUndo();
   m_annotations.append(newAnnot);
   // ⭐ 新画的框自动选中
   m_selectedAnnotIndex = m_annotations.size() - 1;

   m_activate_class_ids.insert(classIdFromAnnotType(newAnnot.type)); // 更新实际出现的标签
   QString info = QString("标注完成：类型：%1; 边框：x=%2, y=%3, 宽=%4, 高=%5; 中心：x=%6, y=%7")
       .arg(newAnnot.label)
       .arg(newAnnot.rect.x()).arg(newAnnot.rect.y()).arg(newAnnot.rect.width()).arg(newAnnot.rect.height())
       .arg(newAnnot.center().x()).arg(newAnnot.center().y());
   ui->annotationInfoEdit->setText(info);

   //打印所有标注
//    for (const AnnotationData& annot : m_annotations) {
//        qDebug() << "Annotation:" << annot.label << "Rect:" << annot.rect;
//    }


   updateClassAnnotCounts();
   update();
}

// ═══════════════════════════════════════════════════════════
// 事件过滤器：转发 imgLabel 的鼠标移动事件到 mouseMoveEvent
// 因为 imgLabel 是子控件，AiModelSet::mouseMoveEvent 默认收不到它上面的悬停事件
// ═══════════════════════════════════════════════════════════
bool AiModelSet::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->imgLabel && event->type() == QEvent::MouseMove) {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        // 把 imgLabel 坐标转成 AiModelSet 父级坐标，调用自身 mouseMoveEvent
        QPoint parentPos = ui->imgLabel->mapToParent(me->pos());
        QMouseEvent newEvent(QEvent::MouseMove, parentPos, me->globalPos(),
                              me->button(), me->buttons(), me->modifiers());
        mouseMoveEvent(&newEvent);
    }
    return QWidget::eventFilter(watched, event);
}

void AiModelSet::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event); // 必须调用父类方法，确保图片正常显示

    // ⭐ 辅助 lambda：在 painter 上只涂 ROI 区域内的前景像素（半透红）
    auto tintFgPixels = [&](QPainter& painter, const QRect& roiRect) {
        if (m_fgMaskQImageCached.isNull()) return;

        QRect roi = roiRect.intersected(QRect(0, 0, m_fgMaskQImageCached.width(),
                                                    m_fgMaskQImageCached.height()));
        if (roi.width() <= 0 || roi.height() <= 0) return;

        QImage roiMask = m_fgMaskQImageCached.copy(roi);  // Format_Grayscale8
        int fgCnt = 0;
        for (int y = 0; y < roi.height(); y++) {
            const uchar* s = roiMask.scanLine(y);
            for (int x = 0; x < roi.width(); x++)
                if (s[x] > 0) fgCnt++;
        }
        if (fgCnt == 0) return;

        // 生成带 alpha 的红色 tint 图
        QImage tintImg(roi.width(), roi.height(), QImage::Format_ARGB32);
        tintImg.fill(Qt::transparent);

        for (int y = 0; y < roi.height(); y++) {
            const uchar* src = roiMask.scanLine(y);
            QRgb* dst = (QRgb*)tintImg.scanLine(y);
            for (int x = 0; x < roi.width(); x++) {
                int alpha = src[x] * 80 / 255;  // 0→0, 255→80
                dst[x] = qRgba(255, 0, 0, alpha);
            }
        }
        painter.drawImage(roi.x(), roi.y(), tintImg);
    };

    QPixmap tmpPixmap = m_currentPixmap.copy(); // 复制原图
    bool needUpdatePixmap = false;

    // ═══ 仿真vs标注对比模式 ═══
    if (m_vsAnnoMode) {
        // 对比 pred txt 和人工标注，错标的标注框画虚线 + 外面贴一个 AI 预测类别的小色块
        QPainter painter(&tmpPixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        // 没 pred → 什么都不画
        QVector<ObjInfo> predObjs;
        loadEmulateResultFromFile(m_currentImagePath, predObjs);
        if (!predObjs.isEmpty() && !m_annotations.isEmpty()) {
            // 对每个人工标注，找 IoU 最大的 AI 预测框
            for (const AnnotationData& anno : m_annotations) {
                int annoClsId = anno.classId;
                // 遍历找匹配
                int bestIdx = -1;
                double bestIoU = 0.0;
                for (int p = 0; p < predObjs.size(); p++) {
                    const ObjInfo& pred = predObjs[p];
                    QRect predRect(pred.x_, pred.y_, pred.w_, pred.h_);
                    double iou = calcIoU(anno.rect, predRect);
                    if (iou > bestIoU) { bestIoU = iou; bestIdx = p; }
                }
                // 找到匹配且 cls_id 不同 → 错标
                if (bestIdx >= 0 && bestIoU > 0.3 && predObjs[bestIdx].cls_id_ != annoClsId) {
                    // 画虚线标注框（颜色 = 标注类别的颜色）
                    QColor annoColor = getClassColorTable().value(annoClsId, QColor(255, 0, 0));
                    QPen dashPen(annoColor, 2, Qt::DashLine);
                    painter.setPen(dashPen);
                    painter.setBrush(Qt::NoBrush);
                    painter.drawRect(anno.rect);

                    // AI 预测类别的小色块（直接放在标注框中心）
                    QColor predColor = getClassColorTable().value(predObjs[bestIdx].cls_id_, QColor(255, 0, 0));
                    const int sq = 16;
                    int sqX = anno.rect.center().x() - sq / 2;
                    int sqY = anno.rect.center().y() - sq / 2;
                    painter.fillRect(sqX, sqY, sq, sq, predColor);
                }
            }
            needUpdatePixmap = true;
        }
    } else if (m_emulating) {
        // ═══ 仿真模式：只画仿真框，跳过人工标注和前景框 ═══
        if (!m_emulateObjInfos.isEmpty()) {
            QPainter painter(&tmpPixmap);
            painter.setRenderHint(QPainter::Antialiasing);

            // 提前读面积阈值（避免每个循环重复 fromText）
            int areaThresh = 50;
            if (m_show_fg_pixel_count) {
                bool ok = true;
                int t = ui->m_areaThresholdlineEdit->text().toInt(&ok);
                if (ok && t > 0) areaThresh = t;
            }

            for (int oi = 0; oi < m_emulateObjInfos.size(); ++oi) {
                const ObjInfo& obj = m_emulateObjInfos[oi];
                QColor color = getClassColorTable().value(obj.cls_id_, QColor(255, 0, 0));

                // ⭐ 面积过滤粗细：前景像素数 ≥ 阈值 → 粗框(5)，否则 → 细框(2)
                int penWidth = 3;
                if (m_show_fg_pixel_count && oi < m_emulate_pixel_counts.size()) {
                    penWidth = (m_emulate_pixel_counts[oi] >= areaThresh) ? 5 : 2;
                }

                QPen pen(color, penWidth, Qt::SolidLine);
                painter.setPen(pen);
                QRect r(obj.x_, obj.y_, obj.w_, obj.h_);
                painter.drawRect(r);

                // ⭐ 像素面积过滤：满足条件的框内前景像素区域填半透红色
                if (m_show_fg_pixel_count && oi < m_emulate_pixel_counts.size()
                    && m_emulate_pixel_counts[oi] >= areaThresh) {
                    tintFgPixels(painter, r);
                }
            }
            needUpdatePixmap = true;
        }
    } else {
        // ═══ 非仿真模式：原来的绘制逻辑 ═══

        // 1. 绘制正在创建的临时标注（拖拽过程中）
        if (m_isDrawing && !tmpPixmap.isNull()) {
            QPainter painter(&tmpPixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            QRect drawRect = getNormalizedRect(m_drawStartPos, m_drawEndPos);
            QPen pen(getAnnotColorByType(m_currentAnnotType), 2, Qt::DashLine);
            painter.setPen(pen);
            painter.drawRect(drawRect);
            painter.setPen(Qt::black);
            painter.drawText(drawRect.topLeft() + QPoint(5, 15), "绘制中...");
            needUpdatePixmap = true;
        }

        // 2. 绘制所有标注（选中的先跳过，最后单独画选中态）
        if (!tmpPixmap.isNull() && !m_annotations.isEmpty()) {
            QPainter painter(&tmpPixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            
            // 提前读面积阈值
            int areaThresh = 50;
            if (m_show_fg_pixel_count) {
                bool ok = true;
                int t = ui->m_areaThresholdlineEdit->text().toInt(&ok);
                if (ok && t > 0) areaThresh = t;
            }

            for (int i = 0; i < m_annotations.size(); ++i) {
                if (i == m_selectedAnnotIndex) continue;
                const AnnotationData& annot = m_annotations[i];

                // ⭐ 面积过滤粗细：开过滤时按像素数区分
                int penWidth = 2;
                if (m_show_fg_pixel_count && i < m_annot_pixel_counts.size()) {
                    penWidth = (m_annot_pixel_counts[i] >= areaThresh) ? 3 : 1;
                }
                QPen pen(getAnnotColorByType(annot.type), penWidth, Qt::DashLine);
                painter.setPen(pen);
                painter.drawRect(annot.rect);

                // ⭐ 像素面积过滤：满足条件的框内前景像素区域填半透红色
                if (m_show_fg_pixel_count && i < m_annot_pixel_counts.size()
                    && m_annot_pixel_counts[i] >= areaThresh) {
                    tintFgPixels(painter, annot.rect);
                }
            }
            needUpdatePixmap = true;
        }

        // 3. 选中框：实线 + 加粗（选中态视觉优先），也加红色像素数字
        if (m_selectedAnnotIndex >= 0 && m_selectedAnnotIndex < m_annotations.size()) {
            const AnnotationData& annot = m_annotations[m_selectedAnnotIndex];
            QPainter painter(&tmpPixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            QPen pen(getAnnotColorByType(annot.type), 5, Qt::SolidLine);
            painter.setPen(pen);
            painter.drawRect(annot.rect);

            // ⭐ 像素面积过滤：选中框满足条件也填半透红色
            int areaThresh = 50;
            if (m_show_fg_pixel_count) {
                bool ok = true;
                int t = ui->m_areaThresholdlineEdit->text().toInt(&ok);
                if (ok && t > 0) areaThresh = t;
            }
            if (m_show_fg_pixel_count && m_selectedAnnotIndex < m_annot_pixel_counts.size()
                && m_annot_pixel_counts[m_selectedAnnotIndex] >= areaThresh) {
                tintFgPixels(painter, annot.rect);
            }
            needUpdatePixmap = true;
        }

        // 4. 前景框（默认黑虚线，开面积过滤后按像素数区分粗细）
        if (m_show_fg_rects && !m_fg_rects.isEmpty()) {
            QPainter painter(&tmpPixmap);
            painter.setRenderHint(QPainter::Antialiasing);

            int areaThresh = 50;
            if (m_show_fg_pixel_count) {
                bool ok = true;
                int t = ui->m_areaThresholdlineEdit->text().toInt(&ok);
                if (ok && t > 0) areaThresh = t;
            }

            for (int i = 0; i < m_fg_rects.size(); ++i) {
                const QRect &rc = m_fg_rects[i];
                bool overlapped = false;
                for (const AnnotationData& a : m_annotations) {
                    if (a.rect.intersects(rc)) { overlapped = true; break; }
                }
                if (overlapped) continue;

                // ⭐ 面积过滤粗细：前景像素数 ≥ 阈值 → 粗框(3)，否则 → 细框(1)
                int penWidth = 2;
                if (m_show_fg_pixel_count && i < m_fg_pixel_counts.size()) {
                    penWidth = (m_fg_pixel_counts[i] >= areaThresh) ? 3 : 1;
                }
                QPen pen(Qt::black, penWidth, Qt::DashLine);
                painter.setPen(pen);
                painter.drawRect(rc);

                // ⭐ 像素面积过滤：满足条件的前景框内前景像素区域填半透红色
                if (m_show_fg_pixel_count && i < m_fg_pixel_counts.size()
                    && m_fg_pixel_counts[i] >= areaThresh) {
                    tintFgPixels(painter, rc);
                }
            }
            needUpdatePixmap = true;
        }
    }

    if (needUpdatePixmap) {
        ui->imgLabel->setPixmap(tmpPixmap);
    }
}


// ⭐ 统一设置 imgLabel 训练集绿框（QSS border，比 QPainter 画在主窗口更可靠）
void AiModelSet::updateImgLabelBorder()
{
    if (!m_currentImagePath.isEmpty()
        && m_train_lists.contains(m_currentImagePath))
    {
        
        ui->imgLabel->setStyleSheet(
            "QLabel { border: 3px solid rgb(0, 220, 0); background: transparent; }"
        );
    } else {
        ui->imgLabel->setStyleSheet("QLabel { border: none; }");
    }
}






// classIdFromAnnotType / annotTypeFromClassId 已改为 .h 内联的 classIdFromAnnotType / annotTypeFromClassId

void AiModelSet::getFgRects()
{
    FLOW("getFgRects 入口");
    if (ui->imgLabel->pixmap() == nullptr || ui->imgLabel->pixmap()->isNull()) {
        LOG_WARN_STM("⚠️ getFgRects: 没有图片，跳过");
        return;
    }

    // 从 UI 读取阈值（默认值参照 Python label_page.py 的 _extract_edges）
    bool ok1 = true, ok2 = true;
    int areaThresh = ui->m_areaThresholdlineEdit->text().toInt(&ok1);
    int colorDiffThresh = ui->m_colorDiffThresholdlineEdit->text().toInt(&ok2);
    if (!ok1 || areaThresh <= 0) areaThresh = 50;     // UI 默认 50（像素面积）
    if (!ok2 || colorDiffThresh <= 0) colorDiffThresh = 30;  // Python 默认 30（色差）
    FLOW("getFgRects 阈值 areaThresh=" << areaThresh << "colorDiffThresh=" << colorDiffThresh);

    QImage image = m_currentPixmap.toImage();  // ✅ 必须用原图，不能用 imgLabel 上已画框的合成图

    // QImage(RGB) → cv::Mat(BGR)，OpenCV 默认 BGR
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    image = image.convertToFormat(QImage::Format_BGR888);
    cv::Mat img(image.height(), image.width(), CV_8UC3,
                image.bits(), image.bytesPerLine());
    img = img.clone();
#else
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat img(image.height(), image.width(), CV_8UC3,
                image.bits(), image.bytesPerLine());
    img = img.clone();
    cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
#endif
    FLOW("getFgRects 转 Mat后");


    // ═══ 核心：absdiff 背景均值方案（和 Python label_page.py 一致） ═══
    cv::Mat processMat;
    if (m_hasBgMean) {
        cv::Mat bgMat(img.size(), img.type(),
                      cv::Scalar(m_bgMeanBGR[0], m_bgMeanBGR[1], m_bgMeanBGR[2]));
        cv::Mat diff;
        cv::absdiff(img, bgMat, diff);
        cv::Mat diffGray;
        cv::cvtColor(diff, diffGray, cv::COLOR_BGR2GRAY);

        cv::Mat fgMask;
        cv::threshold(diffGray, fgMask, colorDiffThresh, 255, cv::THRESH_BINARY);

        cv::Mat kernel3x3 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::morphologyEx(fgMask, processMat, cv::MORPH_CLOSE, kernel3x3);
        FLOW("getFgRects absdiff→threshold→close后");
    } else {
        // 没有 bg 均值 → 直接用灰度图（回退）
        cv::cvtColor(img, processMat, cv::COLOR_BGR2GRAY);
        LOG_DEBUG_STM("[getFgRects] m_hasBgMean=false，使用纯灰度回退");
    }

    // Canny 边缘提取（参数和 Python 一致：50, 150）
    cv::Mat edges;
    cv::Canny(processMat, edges, 50, 150);
    FLOW("getFgRects Canny后");

    // 膨胀连接断裂边缘（kernel 2x2，和 Python 一致）
    cv::Mat kernel2x2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::dilate(edges, edges, kernel2x2, cv::Point(-1, -1), 1);
    FLOW("getFgRects dilate后");

    // findContours（RETR_EXTERNAL 只取最外层，CHAIN_APPROX_SIMPLE）
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    FLOW("getFgRects findContours后 数量=" << contours.size());

    // 筛选轮廓：像素面积 >= areaThresh
    m_fg_rects.clear();
    LOG_DEBUG_STM("\n═══════════════ [getFgRects] 筛选前所有轮廓 ═══════════════");
    LOG_DEBUG_STM("总轮廓数=" << contours.size() << " 面积阈值=" << areaThresh << " 图像尺寸=" << img.cols << "x" << img.rows);
    int idx = 0;
    for (const auto& cnt : contours) {
        cv::Rect r = cv::boundingRect(cnt);
        int area = r.width * r.height;
        QString reason;
        bool passed = area >= areaThresh;
        if (passed) {
            // 左边缘细长竖条过滤：x<2 且 h≈图像高度 且 w<4 → 黑边
            if (r.x < 2 && r.height >= img.rows - 2 && r.width < 4) {
                passed = false;
                reason = "SKIP:左边缘竖条";
            }
        } else {
            reason = "SKIP:面积";
        }
        LOG_DEBUG_STM(QString("  [%1] x=%2 y=%3 w=%4 h=%5 area=%6  %7") .arg(idx++).arg(r.x).arg(r.y).arg(r.width).arg(r.height).arg(area) .arg(passed ? "PASS" : reason));
        if (passed) m_fg_rects.append(QRect(r.x, r.y, r.width, r.height));
    }
    LOG_DEBUG_STM("─────────────── 筛选后保留" << m_fg_rects.size() << "个 ───────────────");
    for (int i = 0; i < m_fg_rects.size(); i++) {
        const QRect& r = m_fg_rects[i];
        LOG_DEBUG_STM(QString("  [%1] x=%2 y=%3 w=%4 h=%5  area=%6  center=(%7,%8)  right=%9  bottom=%10") .arg(i).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()) .arg(r.width() * r.height()) .arg(r.center().x()).arg(r.center().y()) .arg(r.right()).arg(r.bottom()));
    }

    // 去重嵌套框：大框包含小框时仅保留大框（和 Python _remove_nested_bboxes 一致）
    removeNestedBoxes(m_fg_rects);
    FLOW("getFgRects 去重后 数量=" << m_fg_rects.size());

    LOG_DEBUG_STM("\n═══════════════ [getFgRects] 去重后最终前景框 ═══════════════");
    LOG_DEBUG_STM("最终框数量=" << m_fg_rects.size());
    for (int i = 0; i < m_fg_rects.size(); i++) {
        const QRect& r = m_fg_rects[i];
        LOG_DEBUG_STM(QString("  [%1] x=%2 y=%3 w=%4 h=%5  area=%6  center=(%7,%8)  right=%9  bottom=%10") .arg(i).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()) .arg(r.width() * r.height()) .arg(r.center().x()).arg(r.center().y()) .arg(r.right()).arg(r.bottom()));
    }
    LOG_DEBUG_STM("══════════════════════════════════════════════════════════════════\n");
}

// ═══ removeNestedBoxes：移除被大框完全包含的小框，仅保留最外层框 ═══
// 对应 Python label_page.py 的 _remove_nested_bboxes()
void AiModelSet::removeNestedBoxes(QVector<QRect>& rects)
{
    if (rects.size() < 2) return;

    // 按面积降序排列，大框优先
    std::sort(rects.begin(), rects.end(), [](const QRect& a, const QRect& b) {
        return a.width() * a.height() > b.width() * b.height();
    });

    QVector<QRect> kept;
    for (const QRect& candidate : rects) {
        bool nested = false;
        for (const QRect& outer : kept) {
            // 检查 candidate 是否被 outer 完全包含
            if (outer.x() <= candidate.x() &&
                outer.y() <= candidate.y() &&
                outer.right()  >= candidate.right() &&
                outer.bottom() >= candidate.bottom()) {
                nested = true;
                break;
            }
        }
        if (!nested) kept.append(candidate);
    }
    rects = kept;
}


void AiModelSet::addTrainListPushButtonPressed(){
    // 空值校验：未加载图片时不添加
    if (m_currentImagePath.isEmpty()) {
        showTip("请先加载图片", true);
        return;
    }
    // 去重：已在训练集则跳过
    if (m_train_lists.contains(m_currentImagePath)) {
        return;
    }
    m_train_lists.append(m_currentImagePath);
    // ⭐ 刷新缩略图 + 更新大图 QSS 绿框
    refreshThumbnails();
    updateImgLabelBorder();
    updateClassAnnotCounts();  // ⭐ 加入训练集后刷新全局类别计数
}

void AiModelSet::delTrainListPushButtonPressed(){
    //从m_train_lists中删除m_currentImagePath
    m_train_lists.removeAll(m_currentImagePath);
    // ⭐ 刷新缩略图 + 更新大图 QSS 绿框
    refreshThumbnails();
    updateImgLabelBorder();
    updateClassAnnotCounts();  // ⭐ 移出训练集后刷新全局类别计数
}

// ⭐ 全部删除：清空训练集，所有绿框消失
void AiModelSet::delTrainListAllPushButtonPressed()
{
    if (m_train_lists.isEmpty()) {
        showTip("训练集已为空", true);
        return;
    }
    int n = m_train_lists.size();
    m_train_lists.clear();
    m_train_lists.squeeze();
    refreshThumbnails();     // 缩略图四周绿框消失
    updateImgLabelBorder();  // 大图绿框消失
    updateClassAnnotCounts();  // ⭐ 清空训练集后刷新全局类别计数
    showTip(QString("已从训练集移除 %1 张图片").arg(n));
}

// ⭐ 一键添加所有「有标注文件」的图片到训练集（自动去重）
void AiModelSet::addAllLabeledTrainListPushButtonPressed()
{
    if (m_allImagePaths.isEmpty()) {
        showTip("请先导入图片", true);
        return;
    }

    int added = 0;
    int skipped = 0;
    for (const QString& imgPath : m_allImagePaths) {
        QFileInfo fi(imgPath);
        QString txtPath = fi.absolutePath() + "/" + fi.completeBaseName() + ".txt";
        QFile txtFile(txtPath);

        // 标注文件存在且非空
        if (!txtFile.exists()) { skipped++; continue; }
        if (txtFile.size() == 0) { skipped++; continue; }

        // 去重：已在训练集则跳过
        if (m_train_lists.contains(imgPath)) { skipped++; continue; }

        m_train_lists.append(imgPath);
        added++;
    }

    refreshThumbnails();
    updateImgLabelBorder();
    updateClassAnnotCounts();  // ⭐ 批量加入后刷新全局类别计数

    showTip(
        QString("批量添加完成：新增 %1 张，跳过 %2 张")
            .arg(added).arg(skipped)
    );
}

// ⭐ 刷新当前页所有缩略图的绿框（训练集状态变更后调用）
void AiModelSet::refreshThumbnails()
{
    if (!ui || !ui->m_imageList) return;

    int thumbW = ui->m_imageList->iconSize().width();
    int thumbH = ui->m_imageList->iconSize().height();
    if (thumbW <= 0) thumbW = 150;
    if (thumbH <= 0) thumbH = 120;

    for (int i = 0; i < ui->m_imageList->count(); ++i) {
        QListWidgetItem* item = ui->m_imageList->item(i);
        if (!item) continue;
        QString imagePath = item->data(Qt::UserRole).toString();
        if (imagePath.isEmpty()) continue;

        QImageReader reader(imagePath);
        reader.setScaledSize(QSize(thumbW, thumbH));
        QImage thumb = reader.read();
        if (thumb.isNull()) continue;

        // 训练集 → 画 4px 绿框；非训练集 → 纯缩略图
        if (m_train_lists.contains(imagePath)) {
            QPainter p(&thumb);
            QPen pen(Qt::green, 4);
            p.setPen(pen);
            p.drawRect(thumb.rect().adjusted(2, 2, -2, -2));
            p.end();
        }
        item->setIcon(QIcon(QPixmap::fromImage(thumb)));
    }
}


AiModelSet::~AiModelSet()
{
    cleanupScanThread();

    // 清理 SFTP 线程与 worker（worker 无 parent，需手动释放）
    if (m_sftpThread && m_sftpThread->isRunning()) {
        m_sftpThread->quit();
        m_sftpThread->wait(3000);
    }
    if (m_sftpWorker) {
        // 确保 worker 回到主线程再删除，避免跨线程 delete
        if (m_sftpWorker->thread() != QThread::currentThread()) {
            m_sftpWorker->moveToThread(QThread::currentThread());
        }
        delete m_sftpWorker;
        m_sftpWorker = nullptr;
    }
    if (m_sftpThread) {
        delete m_sftpThread;
        m_sftpThread = nullptr;
    }

    FLOW("import cleanup后");
    delete ui;
}

void AiModelSet::cleanupScanThread()
{
    if (!m_scanThread) return;
    // 不 wait：绝不阻塞 UI 线程（否则第二次点导入 QFileDialog 都弹不出来）
    m_scanThread->requestInterruption();
    m_scanThread->quit();
    m_scanThread->deleteLater();
    m_scanWorker->deleteLater();
    m_scanWorker = nullptr;
    m_scanThread = nullptr;
}

void AiModelSet::updateWidget(){
    updateCategoryChenkBox();
}

void AiModelSet::updateCategoryChenkBox(){
    // ⭐ 没加载模型时 modelCategoryNum==0 → 跳过（setupClassCheckBoxes 里有 count<=0 guard）
    setupClassCheckBoxes(modelCategoryNum, m_classNames);
    m_currentAnnotType = AnnotationType::TYPE_NONE;
}

void AiModelSet::onSetBackBtnClicked(){
    emit backToHomePageSig();
}

// ── 模型加载：弹出 QDialog，列出 LOCAL_MODEL_PATH 下所有 .bin 文件，按时间倒序 ──────
void AiModelSet::onModelSelPushButtonClicked()
{
    // 预加载路径（优先编译宏，否则默认 LOCAL_MODEL_PATH）

    QString dirPath = QString(LOCAL_MODEL_PATH);

    qDebug() << "[MODEL] dirPath=" << dirPath
             << "exists=" << QDir(dirPath).exists();

    // 目录不存在则自动创建
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(dirPath);
    }

    QDialog dlg(this);
    dlg.setWindowTitle("加载模型");
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.resize(700, 400);
    dlg.setMinimumWidth(600);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);

    QLabel *tip = new QLabel(QString("模型目录: %1").arg(dirPath), &dlg);
    tip->setStyleSheet("color: gray; font-size: 8pt;");
    mainLayout->addWidget(tip);

    QListWidget *listWidget = new QListWidget(&dlg);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(listWidget, 1);

    // 扫描 .bin 文件 + 手动按修改时间倒序（最新在前）
    QStringList filters; filters << "*.bin";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    std::sort(files.begin(), files.end(), [](const QFileInfo& a, const QFileInfo& b) {
        return a.lastModified() > b.lastModified();  // 时间大（新）的排前面
    });

    // 填充列表 + 存 .bin 名（去掉后缀）
    QStringList binNames;
    for (const auto &fi : files) {
        binNames.append(fi.completeBaseName());
        auto *item = new QListWidgetItem(fi.completeBaseName(), listWidget);
        item->setData(Qt::UserRole, fi.filePath());   // 存完整路径
        item->setToolTip(QString("%1  |  %2").arg(fi.filePath())
                         .arg(fi.lastModified().toString("yyyy-MM-dd HH:mm:ss")));
    }

    if (binNames.isEmpty()) {
        new QListWidgetItem("(该目录下没有 .bin 模型文件)", listWidget);
    } else {
        // 默认选中第一个模型
        listWidget->setCurrentRow(0);
    }

    // 底部按钮
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    QPushButton *okBtn = new QPushButton("确定", &dlg);
    QPushButton *cancelBtn = new QPushButton("取消", &dlg);
    btnRow->addWidget(okBtn);
    btnRow->addWidget(cancelBtn);
    mainLayout->addLayout(btnRow);

    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    // 执行
    if (dlg.exec() == QDialog::Accepted) {
        QListWidgetItem *item = listWidget->currentItem();
        if (item && !binNames.isEmpty()) {
            modelName = item->text();   // 文件名 (去 .bin)
            ui->modelNameLabel->setText(modelName);

            // 尝试读取同名 .json 解析类别信息
            QString binPath = item->data(Qt::UserRole).toString();
            QString jsonPath = binPath;
            jsonPath.chop(4);  // 去掉 .bin
            jsonPath += ".json";

            bool jsonOk = false;
            QStringList classNames;
            int classCount = 0;

            QFile jsonFile(jsonPath);
            if (jsonFile.open(QIODevice::ReadOnly)) {
                QByteArray data = jsonFile.readAll();
                jsonFile.close();
                QJsonParseError err;
                QJsonDocument doc = QJsonDocument::fromJson(data, &err);
                if (err.error == QJsonParseError::NoError && doc.isObject()) {
                    QJsonArray arr = doc.object().value("class").toArray();
                    classCount = arr.size();
                    // 统一用 0..N-1 作为标签，忽略 json 里的 name（保持和新建模型一致）
                    for (int i = 0; i < classCount; ++i) {
                        classNames.append(QString("%1").arg(i));
                    }
                    jsonOk = true;
                    LOG_DEBUG_STM("🟡 从 json 读到 class:" << classCount << "names:" << classNames);
                }
            }

            if (jsonOk && classCount > 0) {
                modelCategoryNum = classCount;
                setupClassCheckBoxes(classCount, classNames);
                showTip(QString("加载模型: %1  (%2 类)").arg(modelName).arg(classCount));
            } else {
                // json 读不到，默认 1 类
                modelCategoryNum = 1;
                m_classNames.clear();
                setupClassCheckBoxes(1);
                showTip(QString("加载模型: %1 (json 读取失败, 默认 1 类)").arg(modelName), true);
                LOG_WARN_STM("⚠️ json 读取失败:" << jsonPath);
            }
        }
    }
}

// onOkPushButtonClicked / onModelNameLineEditClicked / onModelsize1/2CheckBoxClicked
// 已删除（modeInfowidget UI 已移除，相关逻辑迁移到 onModelNewPushButtonClicked 的 QDialog）

// 删除导入对话框里当前选中的目录（供“删除”按钮使用）
static void removeDirInFileDialog(QFileDialog *dlg)
{
    if (!dlg) return;

    const QString sel = dlg->selectedFiles().value(0);
    if (sel.isEmpty() || !QFileInfo(sel).isDir()) {
        QMessageBox::warning(dlg, "提示", "请先在列表中选择要删除的目录");
        return;
    }
    // 保护图片根目录，避免误删整个数据目录
    if (QDir(sel) == QDir(QString(LOCAL_IMG_PATH))) {
        QMessageBox::warning(dlg, "提示", "不能删除图片根目录：\n" + sel);
        return;
    }
    if (QMessageBox::question(dlg, "删除目录",
            QString("确定删除该目录及其全部内容吗？此操作不可恢复！\n\n%1").arg(sel),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    const QFileInfo fi(sel);
    if (QDir(sel).removeRecursively()) {
        LOG_INFO_STM("🗑️ 已删除目录:" << sel);
        dlg->setDirectory(fi.absolutePath());   // 回到父目录并刷新列表
    } else {
        LOG_WARN_STM("⚠️ 删除目录失败:" << sel);
        QMessageBox::warning(dlg, "删除失败", "无法删除目录：\n" + sel);
    }
}

void AiModelSet::onImportImgPushButtonClicked(){
    FLOW("import 入口");
    FLOW("import 弹框前");
    // 确保初始目录存在（否则 QFileDialog 会 fallback 到程序工作目录）
    const QString initDir = QString(LOCAL_IMG_PATH);
    QDir().mkpath(initDir);
    qDebug() << "[IMPORT] LOCAL_IMG_PATH=" << initDir
             << "exists=" << QDir(initDir).exists()
             << "AA_DontUseNativeDialogs="
             << QCoreApplication::testAttribute(Qt::AA_DontUseNativeDialogs);

    QFileDialog dlg(this, "选择图片文件夹", initDir);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);
    dlg.setOption(QFileDialog::DontResolveSymlinks, true);
    // 板卡上 GTK3 原生对话框无 transient parent 会卡死，必须走 Qt 自绘对话框
    dlg.setOption(QFileDialog::DontUseNativeDialog, true);
    // 默认打开 LOCAL_IMG_PATH（部分平台会忽略构造参数，这里再显式设置一次）
    dlg.setDirectory(initDir);

    // 缩短 Directory 右边的编辑框 + 顶部 Look in 下拉框
    if (QLineEdit *fileNameEdit = dlg.findChild<QLineEdit*>("fileNameEdit"))
        fileNameEdit->setMaximumWidth(350);
    if (QComboBox *lookCombo = dlg.findChild<QComboBox*>("lookInCombo"))
        lookCombo->setMaximumWidth(280);

    // 在 buttonBox 里加 Delete 但立刻 hide + 从 buttonBox 脱出来，
    // 然后手动定位到 Choose(AcceptedRole) 按钮的正左边，
    // 完全绕过 theme 对 buttonBox 内部布局的竖排行为
    // ⭐ 调试：探测 QFileDialog 自绘对话框的 buttonBox 结构
    QDialogButtonBox *box = dlg.findChild<QDialogButtonBox*>("buttonBox");
    qDebug() << "[DBG] buttonBox=" << box;

    if (!box) {
        qWarning() << "[DBG] buttonBox not found!";
    } else {
        // 找到 AcceptRole 的那个按钮（就是 Choose）
        QPushButton *chooseBtn = nullptr;
        for (QAbstractButton *ab : box->buttons()) {
            QPushButton *btn = qobject_cast<QPushButton*>(ab);
            if (!btn) continue;
            qDebug() << "[DBG] box button:" << btn->text()
                     << "role=" << box->buttonRole(btn);
            if (box->buttonRole(btn) == QDialogButtonBox::AcceptRole)
                chooseBtn = btn;
        }

        QPushButton *delBtn = box->addButton("Delete", QDialogButtonBox::DestructiveRole);
        qDebug() << "[DBG] delBtn=" << delBtn << "chooseBtn=" << chooseBtn;
        // 从 buttonBox 脱出来挂到 dlg 顶层，绕过 theme 的竖排布局
        delBtn->setParent(&dlg);

        QPushButton *chooseRef = chooseBtn;
        QMetaObject::invokeMethod(&dlg, [delBtn, chooseRef, &dlg]() {
            if (!chooseRef) { qWarning() << "[DBG] chooseRef null"; return; }
            delBtn->setFixedSize(chooseRef->size());
            // ⭐ 关键：chooseRef 的坐标是相对 buttonBox 的，必须 mapTo dlg 才能用于 delBtn
            const QPoint p = chooseRef->mapTo(&dlg, QPoint(0, 0));
            delBtn->move(p.x() - delBtn->width() - 6, p.y());
            delBtn->raise();
            delBtn->show();

            // ⭐ Delete 是浮在 dlg 顶层的独立控件，不参与 grid 布局，
            // Directory 编辑框不会自动让位 → 把它的右端收到 Delete 左侧，避免被遮挡
            if (QLineEdit *edit = dlg.findChild<QLineEdit*>("fileNameEdit")) {
                const QPoint ep = edit->mapTo(&dlg, QPoint(0, 0));
                const int w = delBtn->x() - 6 - ep.x();
                if (w > 60) edit->setMaximumWidth(w);
            }

            qDebug() << "[DBG] delBtn ->" << delBtn->geometry()
                     << "choose(mapTo dlg) ->" << p
                     << "visible=" << delBtn->isVisible();
        }, Qt::QueuedConnection);

        connect(delBtn, &QPushButton::clicked, &dlg, [&dlg]() {
            removeDirInFileDialog(&dlg);
        });
    }

    if (dlg.exec() != QDialog::Accepted) return;
    const QString dirPath = dlg.selectedFiles().value(0);
    FLOW("import 弹框后 dirPath=" << dirPath);
    if (dirPath.isEmpty()) return;
    m_currentDir = dirPath;
    showTip("扫描中...");
    
    // 先干净地停掉上一次的扫描（如果有）
    cleanupScanThread();
    FLOW("import cleanup后");

    // ── 标准 Qt Worker 模式：QThread + moveToThread ────────────────────
    m_scanThread = new QThread(this);
    m_scanWorker = new ImageScanWorker();
    m_scanWorker->moveToThread(m_scanThread);

    // 扫描进度 + 完成信号（Worker 子线程 emit，UI 线程 QueuedConnection 接收）
    connect(m_scanWorker, &ImageScanWorker::scanProgress,
            this, [this](int count) {
                showTip(QString("扫描中：已找到 %1 张").arg(count));
            }, Qt::QueuedConnection);

    connect(m_scanWorker, &ImageScanWorker::scanFinished,
            this, &AiModelSet::onScanFinished, Qt::QueuedConnection);

    // 扫描完成后自动退出线程（让 cleanupScanThread 的 wait 能干净返回）
    connect(m_scanWorker, &ImageScanWorker::scanFinished,
            m_scanThread, &QThread::quit);

    // 启动线程 + 异步投递扫描任务
    m_scanThread->start();
    FLOW("import 线程已启动");
    QMetaObject::invokeMethod(m_scanWorker, "doScan", Qt::QueuedConnection,
                              Q_ARG(QString, dirPath));
}

// ── 合并标注目录对话框入口 ─────────────────────────────────────────────────
void AiModelSet::onMergeDirsClicked()
{
    MergeDialog dlg(this);
    dlg.exec();
}

// ── 图片文件名排序辅助 ──────────────────────────────────────────────────────
// 前缀优先级：bg_ < img_ < 其他
static int imageFilePrefixRank(const QString& fn)
{
    if (fn.startsWith("bg_")) return 0;
    if (fn.startsWith("img_")) return 1;
    return 2;
}

// 解析文件名中的相机序号 x、图像序号 xxxx、可选源目录后缀 @n（格式：prefix_x_xxxx[@n].ext）
// 例：img_01_0001.jpg      → cam=1, seq=1,  src=-1（无 @）
//     img_01_0001@2.jpg    → cam=1, seq=1,  src=2
//     bg_02_0003@1.jpg     → cam=2, seq=3,  src=1
static bool parseImageNums(const QString& fn, int& cam, int& seq, int& src)
{
    int u1 = fn.indexOf('_');
    if (u1 < 0) return false;
    int u2 = fn.indexOf('_', u1 + 1);
    if (u2 < 0) return false;
    // 去掉后缀
    QString rest = fn.mid(u2 + 1);
    int dot = rest.lastIndexOf('.');
    if (dot >= 0) rest = rest.left(dot);
    // 去掉 @源序号（如果有）
    int at = rest.indexOf('@');
    src = -1;  // 默认无 @
    if (at >= 0) {
        src = rest.mid(at + 1).toInt();
        rest = rest.left(at);
    }
    cam = fn.mid(u1 + 1, u2 - u1 - 1).toInt();
    seq = rest.toInt();
    return true;
}

// 图片文件排序比较器：bg_ 前缀在前，组内优先级 → 图像序号 seq → 相机序号 cam → 源目录 src
// 这样合并后的文件名 img_01_0001@1.jpg 和 img_01_0001@2.jpg 会排在一起，按 @序号 区分
static bool compareImageFileNames(const QString& a, const QString& b)
{
    QString na = QFileInfo(a).fileName();
    QString nb = QFileInfo(b).fileName();

    int ra = imageFilePrefixRank(na);
    int rb = imageFilePrefixRank(nb);
    if (ra != rb) return ra < rb;

    int camA = 0, seqA = 0, srcA = -1;
    int camB = 0, seqB = 0, srcB = -1;
    bool okA = parseImageNums(na, camA, seqA, srcA);
    bool okB = parseImageNums(nb, camB, seqB, srcB);

    // 解析失败的放最后
    if (!okA && !okB) return na < nb;
    if (!okA) return false;
    if (!okB) return true;

    if (seqA != seqB) return seqA < seqB;
    if (camA != camB) return camA < camB;
    return srcA < srcB;  // @源序号 最后比较
}

void AiModelSet::onScanFinished(const QStringList& imagePaths)
{
    FLOW("scanFinished 入口 数量=" << imagePaths.size());

    // ⭐ 排序：bg_ 开头 → img_ 开头；每组内按相机序号 x → 图像序号 xxxx 升序
    QStringList allPaths = imagePaths;
    std::sort(allPaths.begin(), allPaths.end(), compareImageFileNames);

    // ═══ 分离 bg_ 和 img_：bg_ 不进 imglist，但存到 m_bgPaths 供训练时一并提交 ═══
    m_bgPaths.clear();
    QStringList imgOnlyPaths;
    for (const QString& p : allPaths) {
        if (QFileInfo(p).fileName().startsWith("bg_")) {
            m_bgPaths.append(p);
        } else {
            imgOnlyPaths.append(p);
        }
    }
    m_allImagePaths = imgOnlyPaths;   // imglist 只显示 img_ 开头的

    // ═══ bg_ 图片校验 + 背景均值提取 ═══
    // 用户要求：bg_ 开头图片 >= 2 张，否则警告并取消导入
    m_bgCount = m_bgPaths.size();
    FLOW("bg_ 图片数量=" << m_bgCount);

    if (m_bgCount < 2) {
        // 警告：bg_ 图片不足 2 张
        showTip(QString("导入目录必须包含 2 张以上 bg_ 开头的背景图像（当前 %1 张）").arg(m_bgCount), true);
        QMessageBox::warning(this, "警告",
            QString("导入目录必须包含 2 张以上 bg_ 开头的背景图像\n当前目录仅有 %1 张 bg_ 图片").arg(m_bgCount));
        // 清空数据 + 返回
        m_allImagePaths.clear();
        m_bgPaths.clear();
        showTip("bg_ 图片不足，导入取消");
        ui->m_prevBtn->setEnabled(false);
        ui->m_nextBtn->setEnabled(false);
        clearImageList();
        m_hasBgMean = false;
        m_bgCount = 0;
        return;
    }

    // 计算 bg_ 图片均值（参考 Python label_page.py 的 load_directories 逻辑）
    // Python 用 cv2.mean(img)[:3] 取 BGR，再转 RGB 存。C++ 直接用 BGR 存（OpenCV 默认）
    double bgSumB = 0, bgSumG = 0, bgSumR = 0;
    int bgValid = 0;
    for (const QString& path : m_bgPaths) {
        cv::Mat img = cv::imread(path.toStdString(), cv::IMREAD_COLOR);
        if (img.empty()) {
            LOG_WARN_STM("[bg均值] 跳过无法读取的文件:" << path);
            continue;
        }
        cv::Scalar mean = cv::mean(img);  // BGR 顺序
        bgSumB += mean[0];
        bgSumG += mean[1];
        bgSumR += mean[2];
        bgValid++;
    }
    if (bgValid > 0) {
        m_bgMeanBGR[0] = int(bgSumB / bgValid + 0.5);
        m_bgMeanBGR[1] = int(bgSumG / bgValid + 0.5);
        m_bgMeanBGR[2] = int(bgSumR / bgValid + 0.5);
        m_hasBgMean = true;
        LOG_DEBUG_STM("[bg均值] 已计算，共" << bgValid << "张，BGR=(" << m_bgMeanBGR[0] << "," << m_bgMeanBGR[1] << "," << m_bgMeanBGR[2] << ")");
    } else {
        m_bgMeanBGR[0] = m_bgMeanBGR[1] = m_bgMeanBGR[2] = 128;  // 默认中灰
        m_hasBgMean = false;
        LOG_WARN_STM("[bg均值] 所有 bg_ 图片都读取失败，使用默认中灰");
    }
    // ═══ bg 校验/均值 结束 ═══

    m_currentPage = 1;  // 重置为第1页

    // 计算总页数
    m_totalPages = (m_allImagePaths.size() + m_pageSize - 1) / m_pageSize;

    // 更新状态和按钮状态
    if (m_allImagePaths.isEmpty()) {
        showTip("未找到支持的图片文件", true);

        ui->m_prevBtn->setEnabled(false);
        ui->m_nextBtn->setEnabled(false);
        clearImageList();
    } else {
        showTip(QString("共找到 %1 张图片").arg(m_allImagePaths.size()));
        ui->m_prevBtn->setEnabled(false);  // 第1页禁用上一页
        ui->m_nextBtn->setEnabled(m_totalPages > 1);  // 超过1页才启用下一页
        showPage(m_currentPage);  // 显示第1页
    }

    updatePageInfo();  // 更新页码信息
}

void AiModelSet::showPage(int page)
{
    FLOW("showPage 入口 page=" << page);
    if (m_allImagePaths.isEmpty() || page < 1 || page > m_totalPages) return;

    clearImageList();  // 清空当前列表
    FLOW("showPage clear后");

    // 计算当前页的图片索引范围
    int startIdx = (page - 1) * m_pageSize;
    int endIdx = qMin(startIdx + m_pageSize, m_allImagePaths.size());

    // 目标缩略图尺寸（从 .ui + cpp 自适应配置动态取）
    int thumbW = ui->m_imageList->iconSize().width();
    int thumbH = ui->m_imageList->iconSize().height();
    if (thumbW <= 0) thumbW = 150;
    if (thumbH <= 0) thumbH = 120;

    // 添加当前页的图片
    for (int i = startIdx; i < endIdx; ++i) {
        QString imagePath = m_allImagePaths[i];
        QFileInfo fileInfo(imagePath);

        QListWidgetItem* item = new QListWidgetItem(ui->m_imageList);
        item->setText(fileInfo.fileName());
        item->setData(Qt::UserRole, imagePath);

        // 缩略图：QImageReader::setScaledSize 让解码器直接出小图，避免解码原图
        QImageReader reader(imagePath);
        reader.setScaledSize(QSize(thumbW, thumbH));
        QImage thumb = reader.read();
        if (!thumb.isNull()) {
            // 已在训练集 → 缩略图四周加绿框标识
            if (m_train_lists.contains(imagePath)) {
                QPainter p(&thumb);
                QPen pen(Qt::green, 4);       // ⭐ 加粗到 4px，小图更清晰
                p.setPen(pen);
                p.drawRect(thumb.rect().adjusted(2, 2, -2, -2));
                p.end();
            }
            item->setIcon(QIcon(QPixmap::fromImage(thumb)));
        } else {
            item->setText(fileInfo.fileName() + " (加载失败)");
        }
    }
    FLOW("showPage 缩略图加载完成");
    ui->m_imageList->setCurrentRow(0);
    m_currentImg = 1;
    m_currentPage = page;

    updatePageInfo();
    updateSelectImgInfo();


    // 更新按钮状态
    ui->m_prevBtn->setEnabled(m_currentPage > 1);
    ui->m_nextBtn->setEnabled(m_currentPage < m_totalPages);

    // ⭐ 自动显示第一张图片：延迟到事件循环下一拍，先让缩略图渲染完
    if (ui->m_imageList->count() > 0) {
        QListWidgetItem* firstItem = ui->m_imageList->item(0);
        QString path = firstItem->data(Qt::UserRole).toString();
        QTimer::singleShot(0, this, [this, path]() { loadFirstImage(path); });
    }
}

// ⭐ 加载并显示指定图片（保存上一张标注 → 加载新标注 → 显示原图）
void AiModelSet::loadFirstImage(const QString& path)
{
    FLOW("lambda 入口 加载第一张原图");
    QString prevPath = m_currentImagePath;
    if (!prevPath.isEmpty()) saveMyAnnotation(prevPath);
    m_currentImagePath = path;
    LoadMyAnnotation(path);
    QImage image(path);
    FLOW("lambda QImage解码后");
    if (!image.isNull()) {
        m_currentPixmap = QPixmap::fromImage(image).copy();
        ui->imgLabel->setPixmap(m_currentPixmap);
    }
    m_selectedAnnotIndex = -1;
    ui->annotationInfoEdit->clear();
    updateImgLabelBorder();  // ⭐ 训练集状态切换绿框

    // 仿真状态下，翻页后也需要从 pred txt 加载对应图片的推理框
    if (m_emulating) {
        m_emulateObjInfos.clear();
        loadEmulateResultFromFile(path, m_emulateObjInfos);  // 没有 pred 就保持空
    }

    // 前景框：翻页切换图片必须重算，否则 m_fg_rects 保留上一张图的数据
    if (m_show_fg_rects) {
        getFgRects();
    } else {
        m_fg_rects.clear();  // 即使前景显示没开，也清掉上一张的残留框
    }

    // ⭐ 前景像素过滤：翻页必须重算
    m_fg_pixel_counts.clear();
    m_emulate_pixel_counts.clear();
    m_annot_pixel_counts.clear();
    if (m_show_fg_pixel_count) computeFgPixelCounts();

    updateClassAnnotCounts();  // ⭐ 切换图片后刷新全局类别计数
    update();
}

void AiModelSet::updatePageInfo()
{
    ui->m_pageInfoLabel->setText(QString("第%1页/共%2页").arg(m_currentPage).arg(m_totalPages));
    ui->m_curPagelineEdit->setText(QString("%1").arg(m_currentPage));

}

void AiModelSet::updateSelectImgInfo()
{
    if(m_currentPage<m_totalPages){
        m_totalImg = m_pageSize;
    }else{
        m_totalImg = m_allImagePaths.size()%((m_totalPages-1)*m_pageSize);
    }

    ui->m_curPageInfoLabel->setText(QString("当前第%1张/共%2张").arg(m_currentImg).arg(m_totalImg));

}

void AiModelSet::clearImageList()
{
   ui->m_imageList->clear();
}

void AiModelSet::prevPage()
{
    if (m_currentPage > 1) {
        m_currentPage = m_currentPage - 1;
        showPage(m_currentPage);
    }else{
        ui->m_nextBtn->setEnabled(false);
    }
}

void AiModelSet::nextPage()
{
    if (m_currentPage < m_totalPages) {
        m_currentPage = m_currentPage + 1;
        showPage(m_currentPage);
    }else{
        ui->m_prevBtn->setEnabled(false);
    }
}

void AiModelSet::gotoPage()
{
    bool ok = false;
    int page = ui->m_curPagelineEdit->text().toInt(&ok);
    if (!ok) {
        showTip("请输入有效页码", true);
        ui->m_curPagelineEdit->setText(QString("%1").arg(m_currentPage));
        return;
    }
    if (m_totalPages <= 0) {
        showTip("暂无图片，无法跳转", true);
        ui->m_curPagelineEdit->setText(QString("%1").arg(m_currentPage));
        return;
    }
    if (page < 1) page = 1;
    if (page > m_totalPages) page = m_totalPages;
    ui->m_curPagelineEdit->setText(QString("%1").arg(page));  // 修正回合法值
    if (page != m_currentPage) {
        m_currentPage = page;
        showPage(m_currentPage);
    }
}

void AiModelSet::saveMyAnnotation(const QString& imgPath)
{
    LOG_DEBUG_STM("m_currentImagePath "<<imgPath);
    QFileInfo imgFileInfo(imgPath);
    QString imgFileName = imgFileInfo.completeBaseName(); // 示例：H:/images/xxx.jpg → "xxx"
    QString txtFileName = imgFileName + ".txt"; // 文件名与图片相同："xxx.txt"
    QString txtPath = imgFileInfo.absolutePath() + "/" + txtFileName; // 拼接路径：H:/labels/xxx.txt
    QFile txtFile(txtPath);
    if (!txtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showTip(QString("保存标注失败：无法打开文件 %1").arg(txtPath), true);
        return;
    }
    QTextStream out(&txtFile);
    out.setRealNumberPrecision(3); // 数值保留 3 位小数（可调整）
    for (const auto& annot : m_annotations) {
        // a. 获取标注框的原始坐标（基于原始图片，而非缩放后）
        int bboxX = annot.rect.x();      // 原始图片中矩形左上角 x
        int bboxY = annot.rect.y();      // 原始图片中矩形左上角 y
        int bboxW = annot.rect.width();  // 原始图片中矩形宽度
        int bboxH = annot.rect.height(); // 原始图片中矩形高度

        int classId = classIdFromAnnotType(annot.type); // 或从 annot 中存储 class_id
        // e. 写入文件（格式：class_id x_center y_center width height）
        out << classId << " "
            << bboxX << " "
            << bboxY << " "
            << bboxW << " "
            << bboxH << "\n";
    }
    txtFile.close();
    showTip(QString("标注已保存：%1").arg(txtPath));
    m_annotations.clear();
}

void AiModelSet::LoadMyAnnotation(const QString& imgPath)
{
    m_annotations.clear();
    clearAnnotUndo();  // 切换图片 → 丢弃上一张的撤销历史
    LOG_DEBUG_STM("load txt " << imgPath);
    QFileInfo imgFileInfo(imgPath);
    
    QString imgFileName = imgFileInfo.completeBaseName(); 
    QString txtFileName = imgFileName + ".txt"; 
    QString txtPath = imgFileInfo.absolutePath() + "/" + txtFileName;

    QFile txtFile(txtPath);
    if (!txtFile.exists()) {
        showTip(QString("标注文件不存在：%1").arg(txtPath), true);
        return;
    }

    if (!txtFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        showTip(QString("无法打开标注文件：%1").arg(txtPath), true);
        return;
    }

    QTextStream in(&txtFile);

    m_annotations.clear(); // 清空旧标注
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        // YOLO 格式：class x_center y_center w h
        QStringList parts = line.split(' ');
        if (parts.size() != 5) continue;

        int classId = parts[0].toInt();
        int bboxX = parts[1].toInt();
        int bboxY = parts[2].toInt();
        int bboxW = parts[3].toInt();
        int bboxH = parts[4].toInt();

        QRect rect(bboxX, bboxY, bboxW, bboxH);

        AnnotationData annot;
        annot.type = annotTypeFromClassId(classId); // 映射回类型
        annot.rect = rect;
        annot.classId = classId;                    // 记住原始 classId，对比用

        m_annotations.push_back(annot);
    }

    txtFile.close();
    showTip(QString("标注已加载：%1").arg(txtPath));
    updateClassAnnotCounts();

}




// 图片项点击后，在 previewLabel 中显示原图
void AiModelSet::onImageItemClicked(QListWidgetItem* item)
{
    FLOW("onImageItemClicked 入口");

    LOG_DEBUG_STM("save anno:"<<m_currentImagePath);
    saveMyAnnotation(m_currentImagePath);

    if (!item) return;  // 防止空指针

    // 1. 获取选中图片的完整路径（之前存储在 Qt::UserRole 中）
    m_currentImagePath  = item->data(Qt::UserRole).toString();
    LOG_DEBUG_STM("load anno: "<<m_currentImagePath);
    LoadMyAnnotation(m_currentImagePath);



    if (m_currentImagePath.isEmpty()) {
        showTip("图片路径无效", true);
        return;
    }

    // 2. 加载原图（支持大图片，避免内存溢出）
    QImage image(m_currentImagePath);
    if (image.isNull()) {
        showTip("图片加载失败", true);
        return;
    }

    showTip(m_currentImagePath);

    // 3. 优化显示：让图片适应 previewLabel 大小，保持比例（不拉伸）
    QPixmap pixmap = QPixmap::fromImage(image);
    // scaled 函数参数：目标大小、保持比例、平滑缩放
    // m_currentPixmap = pixmap.scaled(
    //     QSize(1024, 64),  // 适应标签大小
    //     Qt::IgnoreAspectRatio,       // 保持宽高比
    //     Qt::SmoothTransformation   // 平滑缩放（高清）
    // );
    m_currentPixmap = pixmap.copy();

    m_currentImg = ui->m_imageList->row(item)+1;
    updateSelectImgInfo();

    // 4. 显示图片（清空之前的文字）
    ui->imgLabel->setPixmap(m_currentPixmap);
    updateImgLabelBorder();  // ⭐ 训练集状态切换绿框

    // 只有开启「前景显示」功能后，切换图片才计算前景框
    if (m_show_fg_rects) {
        getFgRects();
    }

    // ⭐ 前景像素过滤：翻页必须重算
    m_fg_pixel_counts.clear();
    m_emulate_pixel_counts.clear();
    m_annot_pixel_counts.clear();
    m_fgMaskQImageCached = QImage();
    if (m_show_fg_pixel_count) {
        if (m_fg_rects.isEmpty()) getFgRects();  // 确保有前景框
        computeFgPixelCounts();
    }
    
    // 仿真状态下，切图自动触发 UDP 仿真
    // 先尝试从 pred txt 读取缓存结果，没有再调用板卡
    if (m_emulating) {
        m_emulateObjInfos.clear();
        if (loadEmulateResultFromFile(m_currentImagePath, m_emulateObjInfos)) {
            // 有缓存 → 直接显示
            update();
        } else {
            // 无缓存 → 先画干净图，再请求板卡
            update();
            QApplication::processEvents();
            runEmulateOnce();
        }
    }

    //qDebug()<<"m_activate_class_ids "<<m_activate_class_ids;

//    ui->imgLabel->setText("");  // 清除初始提示文字

    // 清空标注选中状态和信息显示
    m_selectedAnnotIndex = -1;
    ui->annotationInfoEdit->clear();
//    update(); // 刷新绘图
}

bool AiModelSet::prepareTrain()
{
        // 1. 使用程序目录下的 tmp
        QString tmpDir = QDir(QCoreApplication::applicationDirPath()).filePath("tmp");
        QDir dir;


        // 2. 清空 tmp 目录或创建
        if (QDir(tmpDir).exists()) {
            QDir tmpDirObj(tmpDir);
            tmpDirObj.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
            QFileInfoList entries = tmpDirObj.entryInfoList();
            for (const QFileInfo& entry : entries) {
                if (entry.isDir())
                    QDir(entry.absoluteFilePath()).removeRecursively();
                else
                    QFile::remove(entry.absoluteFilePath());
            }
            LOG_DEBUG_STM("Cleared existing tmp directory:" << tmpDir);
        } else {
            dir.mkpath(tmpDir);
            LOG_DEBUG_STM("Created tmp directory:" << tmpDir);
        }

        // 3. 创建 image 和 label 子目录
        QString imageDir = QDir(tmpDir).filePath("image");
        QString labelDir = QDir(tmpDir).filePath("label");
        dir.mkpath(imageDir);
        dir.mkpath(labelDir);
        LOG_DEBUG_STM("Created subdirectories:" << imageDir << labelDir);

        // 4. 复制图片和标注，收集 class_id
        QSet<int> classIds;
        for (const QString& imgPath : m_train_lists) {
            QFileInfo imgFileInfo(imgPath);
            if (!imgFileInfo.exists()) {
                LOG_WARN_STM("Image file not found:" << imgPath);
                continue;
            }

            // 复制图片
            QString destImagePath = QDir(imageDir).filePath(imgFileInfo.fileName());
            QFile::remove(destImagePath);
            if (QFile::copy(imgPath, destImagePath))
                LOG_DEBUG_STM("Copied image:" << imgPath << "->" << destImagePath);
            else
                LOG_WARN_STM("Failed to copy image:" << imgPath << "->" << destImagePath);

            // 复制并清洗标注文件（过滤空行/无效行，防止服务器端 parse_yolo_file 因空行 IndexError）
            // 标注格式：class_id x1 y1 w h（全整数绝对坐标）
            QString txtFileName = imgFileInfo.completeBaseName() + ".txt";
            QString txtPath = imgFileInfo.absolutePath() + "/" + txtFileName;
            QFileInfo txtFileInfo(txtPath);
            if (txtFileInfo.exists()) {
                QString destLabelPath = QDir(labelDir).filePath(txtFileName);

                QFile srcTxt(txtPath);
                if (srcTxt.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QStringList validLines;
                    QTextStream in(&srcTxt);
                    while (!in.atEnd()) {
                        QString line = in.readLine().trimmed();
                        if (line.isEmpty()) continue;
                        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                        if (parts.size() < 5) continue;
                        bool ok0, ok1, ok2, ok3, ok4;
                        int classId = parts[0].toInt(&ok0);
                        parts[1].toInt(&ok1); parts[2].toInt(&ok2);
                        parts[3].toInt(&ok3); parts[4].toInt(&ok4);
                        if (!(ok0 && ok1 && ok2 && ok3 && ok4)) continue;

                        validLines.append(line);
                        classIds.insert(classId);
                    }
                    srcTxt.close();

                    // 重写清洗后的 label 文件（空文件或只有空行的不写入）
                    QFile::remove(destLabelPath);
                    if (!validLines.isEmpty()) {
                        QFile dstTxt(destLabelPath);
                        if (dstTxt.open(QIODevice::WriteOnly | QIODevice::Text)) {
                            QTextStream out(&dstTxt);
                            for (const QString& vl : validLines)
                                out << vl << "\n";
                            dstTxt.close();
                        }
                    }
                    LOG_DEBUG_STM("Label cleaned:" << txtPath << "->" << destLabelPath
                                                           << "(" << validLines.size() << " valid lines)");
                } else {
                    LOG_WARN_STM("Failed to open label for cleaning:" << txtPath);
                }
            } else {
                LOG_WARN_STM("Label file not found for image:" << imgPath);
            }
        }

        // ═══ 4.1 一并提交 bg_ 背景图片（复制到 tmp/image/，并创建同名空 .txt 到 label/） ═══
        for (const QString& bgPath : m_bgPaths) {
            QFileInfo bgFileInfo(bgPath);
            if (!bgFileInfo.exists()) {
                LOG_WARN_STM("[bg] Image file not found:" << bgPath);
                continue;
            }
            QString destBgPath = QDir(imageDir).filePath(bgFileInfo.fileName());
            QFile::remove(destBgPath);
            if (QFile::copy(bgPath, destBgPath))
                LOG_DEBUG_STM("[bg] Copied:" << bgPath << "->" << destBgPath);
            else
                LOG_WARN_STM("[bg] Failed to copy:" << bgPath);

            // 创建同名空 .txt（背景图无标注，但训练框架要求 image/ 和 label/ 一一对应）
            QString bgTxtName = bgFileInfo.completeBaseName() + ".txt";
            QString destBgTxtPath = QDir(labelDir).filePath(bgTxtName);
            QFile::remove(destBgTxtPath);
            QFile f(destBgTxtPath);
            if (f.open(QIODevice::WriteOnly)) f.close();  // 创建空文件
        }
        LOG_DEBUG_STM("[bg] prepareTrain: 已追加" << m_bgPaths.size() << "张 bg_ 图片 + 空 txt");

        // 5. 生成 classes.txt
        QString classesFile = QDir(tmpDir).filePath("classes.txt");
        QFile f(classesFile);
        QList<int> sortedIds = classIds.values();
        std::sort(sortedIds.begin(), sortedIds.end());

        // ⭐ 类别校验：扫描出来的 classIds 必须覆盖 [0, modelCategoryNum-1]
        //    缺了 → 弹告警让用户确认（可能是漏标了某个类别的图片）
        QSet<int> expectedIds;
        for (int i = 0; i < modelCategoryNum; ++i) expectedIds.insert(i);
        QSet<int> scannedSet;
        for (int id : sortedIds) scannedSet.insert(id);

        // 缺的（expected - scanned）
        QList<int> missing;
        for (int i = 0; i < modelCategoryNum; ++i)
            if (!scannedSet.contains(i)) missing.append(i);

        // 多余的（scanned - expected）
        QList<int> extra;
        for (int id : sortedIds)
            if (!expectedIds.contains(id)) extra.append(id);

        if (!missing.isEmpty() || !extra.isEmpty()) {
            QStringList ms, es;
            for (int i : missing) ms << QString::number(i);
            for (int i : extra) es << QString::number(i);

            QString warnMsg = QString(
                "当前模型类别数为 %1（0 ~ %2），但标注中扫描到的类别不匹配：\n\n"
            ).arg(modelCategoryNum).arg(modelCategoryNum - 1);

            if (!missing.isEmpty())
                warnMsg += QString("❌ 缺少类别：%1\n").arg(ms.join(", "));
            if (!extra.isEmpty())
                warnMsg += QString("⚠️  存在超范围类别：%1\n").arg(es.join(", "));

            QMessageBox::critical(this, tr("类别不匹配，无法提交训练"), warnMsg);
            showTip("类别不匹配，请检查标注文件后重试", true);
            return false;
        }

        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            for (int id : sortedIds) {
                out << id << "\n";
            }
            f.close();
            LOG_DEBUG_STM("Generated classes.txt:" << classesFile << " count=" << sortedIds.size());
        } else {
            LOG_WARN_STM("Failed to create classes.txt:" << classesFile);
        }

        LOG_DEBUG_STM("prepareTrain completed. Images:" << imageDir << "Labels:" << labelDir << "Classes file:" << classesFile);

        return true;
}

void AiModelSet::onShowFgRectsBtnClicked()
{
    // ⭐ 没有图片时禁用
    if (m_currentPixmap.isNull()) {
        ui->fgPushButton->setChecked(false);
        showTip("请先加载图片", true);
        return;
    }
    bool willShow = ui->fgPushButton->isChecked();
    // 按下（要显示前景）但还没选类别 → 回弹 + 红色提示
    if (willShow && m_currentAnnotType == AnnotationType::TYPE_NONE) {
        ui->fgPushButton->setChecked(false);
        showTip("请先选择类别 checkbox", true);
        return;
    }
    m_show_fg_rects = willShow;

    if (willShow) {
        getFgRects();     // ⭐ 立即对当前图片计算前景框
        showTip(QString("前景目标显示中（%1 个框）").arg(m_fg_rects.size()));
    } else {
        m_fg_rects.clear();  // 关闭时清空残留
    }
    update();
}

// ═══════════════════════════════════════════════════════════
// 前景像素过滤按钮（areaThresholdAnnoPushButton）
// 切换 m_show_fg_pixel_count，计算并缓存每个前景框 / 仿真框内的前景像素个数
// 集成 AiSorter/foreground 的 countForegroundPixels 算法：
//   absdiff(img, bg_mean) → threshold(colorDiffThresh) → 前景掩码 → countNonZero(ROI)
// ═══════════════════════════════════════════════════════════
void AiModelSet::onAreaThresholdAnnoPushButtonClicked()
{
    if (m_currentPixmap.isNull()) {
        ui->areaThresholdAnnoPushButton->setChecked(false);
        showTip("请先加载图片", true);
        return;
    }
    m_show_fg_pixel_count = ui->areaThresholdAnnoPushButton->isChecked();

    if (m_show_fg_pixel_count) {
        // 先确保前景框已算好（前景按钮没开也没关系，这里独立算）
        if (m_fg_rects.isEmpty()) getFgRects();
        computeFgPixelCounts();
        int total = m_fg_pixel_counts.size() + m_emulate_pixel_counts.size();
        showTip(QString("前景像素过滤开启（%1 个框已统计）").arg(total));
    } else {
        m_fg_pixel_counts.clear();
        m_emulate_pixel_counts.clear();
    }
    update();
}

// ═══════════════════════════════════════════════════════════
// 核心：严格按 AiSorter ForegroundExtractor::countForegroundPixels2 实现
//   absdiff(img, scalar(bg_b_, bg_g_, bg_r_)) → split → 每通道平方 → 相加得 dist_sq
//   threshold(dist_sq > thresh^2) → 二值掩码
// ═══════════════════════════════════════════════════════════
void AiModelSet::computeFgPixelCounts()
{
    m_fg_pixel_counts.clear();
    m_emulate_pixel_counts.clear();
    m_annot_pixel_counts.clear();
    m_fgMaskQImageCached = QImage();

    if (m_currentPixmap.isNull()) return;
    if (!m_hasBgMean) {
        for (int i = 0; i < m_fg_rects.size(); i++) m_fg_pixel_counts.append(0);
        for (int i = 0; i < m_emulateObjInfos.size(); i++) m_emulate_pixel_counts.append(0);
        for (int i = 0; i < m_annotations.size(); i++) m_annot_pixel_counts.append(0);
        return;
    }

    // 从 UI 读取阈值
    bool ok = true;
    double colorThreshold = ui->m_colorDiffThresholdlineEdit->text().toDouble(&ok);
    if (!ok || colorThreshold <= 0) colorThreshold = 40.0;
    int areaThresh = 50;
    bool ok2 = true;
    int t = ui->m_areaThresholdlineEdit->text().toInt(&ok2);
    if (ok2 && t > 0) areaThresh = t;

    // QImage → cv::Mat（BGR）
    QImage image = m_currentPixmap.toImage();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    image = image.convertToFormat(QImage::Format_BGR888);
    cv::Mat img(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine());
    img = img.clone();
#else
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat img(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine());
    img = img.clone();
    cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
#endif

    // ⭐ 严格按 countForegroundPixels2 生成全图前景掩码
    cv::Mat fgMask;
    {
        cv::Mat bgMat(img.size(), img.type(),
                      cv::Scalar(m_bgMeanBGR[0], m_bgMeanBGR[1], m_bgMeanBGR[2]));
        cv::Mat diff;
        cv::absdiff(img, bgMat, diff);

        std::vector<cv::Mat> channels(3);
        cv::split(diff, channels);

        // ⭐ 显式转换 CV_32F，避免隐式类型问题
        cv::Mat ch0f, ch1f, ch2f;
        channels[0].convertTo(ch0f, CV_32F);
        channels[1].convertTo(ch1f, CV_32F);
        channels[2].convertTo(ch2f, CV_32F);

        cv::Mat dist_sq(img.size(), CV_32F, cv::Scalar(0));
        cv::add(ch0f.mul(ch0f), ch1f.mul(ch1f), dist_sq);
        cv::add(dist_sq, ch2f.mul(ch2f), dist_sq);

        double threshold_sq = colorThreshold * colorThreshold;

        // ⭐ threshold 输出跟随 src(CV_32F)，必须 convertTo 成 CV_8U
        cv::Mat mask_32f;
        cv::threshold(dist_sq, mask_32f, threshold_sq, 255.0, cv::THRESH_BINARY);
        mask_32f.convertTo(fgMask, CV_8U);
    }

    // 缓存 QImage 版本供 paintEvent 涂色
    // ⭐ 安全逐行 copy：QImage 和 cv::Mat 的 bytesPerLine 对齐可能不同
    QImage maskImg(fgMask.cols, fgMask.rows, QImage::Format_Grayscale8);
    maskImg.fill(Qt::black);
    for (int y = 0; y < fgMask.rows; y++) {
        const uchar* src = fgMask.ptr<uchar>(y);
        uchar* dst = maskImg.scanLine(y);
        memcpy(dst, src, fgMask.cols);
    }
    m_fgMaskQImageCached = maskImg.copy();

    // ── 对每个前景框计数 ──
    for (int i = 0; i < m_fg_rects.size(); ++i) {
        const QRect& rc = m_fg_rects[i];
        cv::Rect roi(std::max(0, rc.x()), std::max(0, rc.y()),
                     std::min(fgMask.cols - rc.x(), rc.width()),
                     std::min(fgMask.rows - rc.y(), rc.height()));
        int cnt = 0;
        if (roi.width > 0 && roi.height > 0)
            cnt = cv::countNonZero(fgMask(roi));
        m_fg_pixel_counts.append(cnt);
    }

    // ── 对每个仿真框计数 ──
    for (const ObjInfo& obj : m_emulateObjInfos) {
        int ox = static_cast<int>(obj.x_);
        int oy = static_cast<int>(obj.y_);
        int ow = static_cast<int>(obj.w_);
        int oh = static_cast<int>(obj.h_);
        cv::Rect roi(std::max(0, ox), std::max(0, oy),
                     std::min(fgMask.cols - ox, ow),
                     std::min(fgMask.rows - oy, oh));
        if (roi.width > 0 && roi.height > 0)
            m_emulate_pixel_counts.append(cv::countNonZero(fgMask(roi)));
        else
            m_emulate_pixel_counts.append(0);
    }

    // ── 对每个标注框计数 ──
    for (const AnnotationData& annot : m_annotations) {
        int ax = std::max(0, annot.rect.x());
        int ay = std::max(0, annot.rect.y());
        int aw = annot.rect.width();
        int ah = annot.rect.height();
        cv::Rect roi(ax, ay,
                     std::min(fgMask.cols - ax, aw),
                     std::min(fgMask.rows - ay, ah));
        if (roi.width > 0 && roi.height > 0)
            m_annot_pixel_counts.append(cv::countNonZero(fgMask(roi)));
        else
            m_annot_pixel_counts.append(0);
    }
}

// ═══════════════════════════════════════════════════════════
// 本地 MD5 计算（文件级）
// ═══════════════════════════════════════════════════════════
static QString localMd5(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return "";
    QCryptographicHash h(QCryptographicHash::Md5);
    h.addData(&f);
    return QString(h.result().toHex());
}

// ═══════════════════════════════════════════════════════════
// 板卡模型检查 + 自动上传（SFTP stat + SSH exec md5sum）
// 返回 true 表示板卡已有匹配模型，可以进入仿真
// ═══════════════════════════════════════════════════════════
bool AiModelSet::ensureBoardModelUploaded(const QString& modelName)
{
    QString localBin   = QString(LOCAL_MODEL_PATH) + modelName + ".bin";
    QString localJson  = QString(LOCAL_MODEL_PATH) + modelName + ".json";
    QString boardDir   = "/ftp/model";
    QString remoteBin  = boardDir + "/" + modelName + ".bin";
    QString remoteJson = boardDir + "/" + modelName + ".json";

    // 前置：本地必须有 .bin
    if (!QFile::exists(localBin)) {
        showTip(QString("本地模型文件不存在: %1").arg(localBin), true);
        return false;
    }

    showTip(QString("连接板卡检查模型 %1 ...").arg(modelName));
    QApplication::processEvents();

    // 1. 连接板卡 SFTP
    SftpClient cli(BOARD_SFTP_HOST.toStdString(), BOARD_SFTP_PORT,
                   BOARD_SFTP_USER.toStdString(), BOARD_SFTP_PASS.toStdString());
    if (!cli.connect()) {
        showTip(QString("板卡 SFTP 连接失败 %1:%2").arg(BOARD_SFTP_HOST).arg(BOARD_SFTP_PORT), true);
        return false;
    }

    // 2. 确保目录存在
    cli.mkdir_p(boardDir.toStdString());

    // 3. 检查 .bin 是否存在 + 本地 MD5
    QString localMd5Hex = localMd5(localBin);
    bool binNeedUpload = false;

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    if (!cli.stat_file(remoteBin.toStdString(), attrs)) {
        // 板卡没有 .bin → 必须上传
        binNeedUpload = true;
        LOG_DEBUG_STM("[板卡模型] 板卡不存在:" << remoteBin);
    } else {
        // 存在 → 跑 md5sum 比对
        QString boardMd5 = QString::fromStdString(cli.exec(
            QString("md5sum %1").arg(remoteBin).toStdString()));
        // md5sum 输出格式: "abc123...  /path/to/file"  取前 32 字符
        boardMd5 = boardMd5.left(32);
        if (boardMd5.compare(localMd5Hex, Qt::CaseInsensitive) != 0) {
            binNeedUpload = true;
            LOG_DEBUG_STM("[板卡模型] .bin MD5 不一致 local=" << localMd5Hex << "board=" << boardMd5);
        } else {
            LOG_DEBUG_STM("[板卡模型] .bin MD5 一致:" << localMd5Hex);
        }
    }

    // 4. .json 处理（可选，存在则也比对/上传）
    bool localHasJson = QFile::exists(localJson);
    bool jsonNeedUpload = false;
    if (localHasJson) {
        if (!cli.stat_file(remoteJson.toStdString(), attrs)) {
            jsonNeedUpload = true;
        } else {
            QString localJsonMd5 = localMd5(localJson);
            QString boardJsonMd5 = QString::fromStdString(cli.exec(
                QString("md5sum %1").arg(remoteJson).toStdString())).left(32);
            if (boardJsonMd5.compare(localJsonMd5, Qt::CaseInsensitive) != 0) {
                jsonNeedUpload = true;
            }
        }
    }

    // 5. 按需上传
    if (binNeedUpload || jsonNeedUpload) {
        showTip(QString("板卡模型不匹配，上传中 %1 ...").arg(modelName));
        QApplication::processEvents();

        if (binNeedUpload) {
            if (!cli.upload(localBin.toStdString(), remoteBin.toStdString())) {
                showTip(QString("板卡上传 .bin 失败: %1").arg(localBin), true);
                cli.disconnect();
                return false;
            }
            LOG_DEBUG_STM("[板卡模型] .bin 上传成功");
        }
        if (jsonNeedUpload && localHasJson) {
            if (!cli.upload(localJson.toStdString(), remoteJson.toStdString())) {
                showTip(QString("板卡上传 .json 失败: %1").arg(localJson), true);
                cli.disconnect();
                return false;
            }
            LOG_DEBUG_STM("[板卡模型] .json 上传成功");
        }
        showTip(QString("板卡模型 %1 上传完成").arg(modelName));
    } else {
        showTip(QString("板卡已有匹配模型 %1").arg(modelName));
    }

    cli.disconnect();
    return true;
}

// ═══════════════════════════════════════════════════════════
// 单次板卡 UDP 仿真推理（公共函数，按钮/切图都调）
// 不做任何前置条件检查（调用者保证）
// ═══════════════════════════════════════════════════════════
bool AiModelSet::runEmulateOnce()
{
    QString modelBinName = modelName + ".bin";
    QFileInfo imgFi(m_currentImagePath);
    QString imgFileName = imgFi.fileName();

    // ═══ 先把当前图片 SFTP 上传到板卡的仿真目录 ═══
    QString boardImgDir = "/ftp/emulate";
    showTip(QString("上传图片到板卡：%1 → %2 ...").arg(imgFileName).arg(boardImgDir));
    QApplication::processEvents();
    {
        SftpClient cli(BOARD_HOST.toStdString(), 22, "root", "linaro");
        if (!cli.connect()) {
            showTip(QString("板卡 SFTP 连接失败 %1:22").arg(BOARD_HOST), true);
            return false;
        }
        cli.mkdir_p(boardImgDir.toStdString());
        QString remotePath = boardImgDir + "/" + imgFileName;
        if (!cli.upload(m_currentImagePath.toStdString(), remotePath.toStdString())) {
            showTip(QString("图片上传到板卡失败：%1").arg(remotePath), true);
            cli.disconnect();
            return false;
        }
        cli.disconnect();
    }

    // ═══ 先 ModelApply 加载模型到板卡内存 ═══
    {
        ModelApply applyInfo;
        applyInfo.model_name_ = modelBinName;
        QByteArray applyReq = cmdworker::ModelApplyRequest(applyInfo);
        QByteArray applyResp;
        showTip(QString("加载模型到板卡：%1 ...").arg(modelBinName));
        QApplication::processEvents();
        bool applyOk = CmdUdpManager::instance().onSendCommand(
            QHostAddress(BOARD_HOST), BOARD_PORT, applyReq, applyResp, 3000);
        if (!applyOk) {
            showTip("模型加载 UDP 通信失败", true);
            return false;
        }
        CmdPackage applyPkg;
        cmdworker::ParseCmdPkg(applyResp, applyPkg);
        if (applyPkg.cmd_data_.isEmpty() || applyPkg.cmd_data_.at(0) != 1) {
            showTip(QString("板卡模型加载失败 code=%1").arg(applyPkg.cmd_data_.isEmpty() ? -1 : applyPkg.cmd_data_.at(0)), true);
            return false;
        }
    }

    // 发送仿真 UDP 请求（img_name_ 用纯文件名！板卡自己去 /ftp/emulate/ 找）
    EmulateParam info;
    info.model_name_ = modelBinName;
    info.img_name_   = imgFileName;

    QByteArray request = cmdworker::EmulateParamRequest(info);
    QByteArray response;

    showTip(QString("仿真中：model=%1, img=%2 ...").arg(modelBinName).arg(imgFileName));
    QApplication::processEvents();

    // UDP 发送到板卡 192.168.0.12:9193（3s 超时）
    bool ok = CmdUdpManager::instance().onSendCommand(
        QHostAddress(BOARD_HOST),
        BOARD_PORT,
        request,
        response,
        3000);

    if (!ok) {
        showTip(QString("仿真通信失败：板卡 %1:%2 无响应").arg(BOARD_HOST).arg(BOARD_PORT), true);
        return false;
    }

    // 解析响应
    CmdPackage cmdPkg;
    ok = cmdworker::ParseCmdPkg(response, cmdPkg);
    if (!ok) {
        showTip("仿真响应解析失败", true);
        return false;
    }

    EmulateResInfo resInfo = cmdworker::EmulateParamResponse(cmdPkg);
    if (resInfo.code_ != 1) {
        showTip(QString("仿真执行失败（板卡返回 code=%1，可能图片/模型路径问题）").arg(resInfo.code_), true);
        return false;
    }

    // 存结果 + 重绘
    m_emulateObjInfos = resInfo.obj_infos_;
    saveEmulateResultToFile(m_currentImagePath, m_emulateObjInfos);  // 保存 pred txt
    showTip(QString("仿真完成：检测到 %1 个目标").arg(m_emulateObjInfos.size()));
    update();
    return true;
}

// ═══════════════════════════════════════════════════════════
// 保存/读取 pred_{imgname}.txt 文件（放在图片目录的 pred/ 子目录）
// 格式：cls_id x1 y1 w h score （每行一个目标）
// ═══════════════════════════════════════════════════════════
void AiModelSet::saveEmulateResultToFile(const QString& imgPath, const QVector<ObjInfo>& objs)
{
    QFileInfo fi(imgPath);
    QString predDir  = fi.absolutePath() + "/pred";
    QString predFile = predDir + "/pred_" + fi.completeBaseName() + ".txt";

    QDir().mkpath(predDir);
    QFile f(predFile);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_WARN_STM("保存 pred txt 失败：" << predFile);
        return;
    }
    QTextStream ts(&f);
    for (const ObjInfo& obj : objs) {
        ts << obj.cls_id_ << " " << obj.x_ << " " << obj.y_ << " "
           << obj.w_ << " " << obj.h_ << " " << obj.score_ << "\n";
    }
    f.close();
}

bool AiModelSet::loadEmulateResultFromFile(const QString& imgPath, QVector<ObjInfo>& outObjs)
{
    QFileInfo fi(imgPath);
    QString predFile = fi.absolutePath() + "/pred/pred_" + fi.completeBaseName() + ".txt";
    QFile f(predFile);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    outObjs.clear();
    QTextStream ts(&f);
    while (!ts.atEnd()) {
        QString line = ts.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(' ', QString::SkipEmptyParts);
        if (parts.size() < 6) continue;
        ObjInfo obj;
        obj.cls_id_ = parts[0].toInt();
        obj.x_      = parts[1].toInt();
        obj.y_      = parts[2].toInt();
        obj.w_      = parts[3].toInt();
        obj.h_      = parts[4].toInt();
        obj.score_  = parts[5].toFloat();
        outObjs.append(obj);
    }
    return !outObjs.isEmpty();
}

// ═══════════════════════════════════════════════════════════
// 仿真按钮：进入仿真态
//   · 当前图 有 pred txt → 直接读显示（不远程调用）
//   · 当前图 无 pred txt → 远程调一次板卡推理 → 保存 pred → 显示
//   · 切图时：有 pred 读 pred；无 pred 远程推理一次 → 保存 pred
// 和批量仿真按钮互斥
// ═══════════════════════════════════════════════════════════
void AiModelSet::onValidImgPushButtonClicked()
{
    bool willShow = ui->validImgPushButton->isChecked();
    if (!willShow) {
        m_emulating = false;
        m_emulateObjInfos.clear();
        showTip("仿真查看已关闭");
        update();
        return;
    }

    // 和批量仿真按钮互斥
    ui->batchValidImgPushButton->setChecked(false);
    ui->validvsAnnoImgPushButton->setChecked(false);
    m_vsAnnoMode = false;

    // 从 vsAnno 筛选模式切换过来 → 恢复原始图片列表
    if (!m_origAllImagePaths.isEmpty()) {
        m_allImagePaths = m_origAllImagePaths;
        m_origAllImagePaths.clear();
        m_totalPages = (m_allImagePaths.size() + m_pageSize - 1) / m_pageSize;
        m_currentPage = 1;
        showPage(1);
    }

    // ① 模型必须选中
    if (modelName.isEmpty()) {
        ui->validImgPushButton->setChecked(false);
        QMessageBox::warning(this, tr("提示"), tr("请先在模型加载中选择仿真模型！"));
        return;
    }
    // ② 图片必须加载
    if (m_currentPixmap.isNull()) {
        ui->validImgPushButton->setChecked(false);
        showTip("请先加载图片", true);
        return;
    }

    // ③ 板卡模型 MD5 校验 + 自动上传（只做一次）
    if (!ensureBoardModelUploaded(modelName)) {
        ui->validImgPushButton->setChecked(false);
        return;
    }

    // ④ 切换模型后先清掉旧 pred 目录（避免旧模型的缓存结果）
    if (modelName != m_lastEmulateModelName) {
        QFileInfo fi(m_currentImagePath);
        QString predDir = fi.absolutePath() + "/pred";
        QDir(predDir).removeRecursively();
        m_lastEmulateModelName = modelName;
    }

    // ⑤ 进入仿真态
    m_emulating = true;
    m_emulateObjInfos.clear();

    // ⑤ 当前图：有 pred 读 pred；无 pred 远程推理一次 + 保存 pred
    if (!loadEmulateResultFromFile(m_currentImagePath, m_emulateObjInfos)) {
        runEmulateOnce();  // 内部会保存 pred txt
    }
    update();
}

// ═══════════════════════════════════════════════════════════
// 批量仿真按钮：遍历当前目录所有图片，保存 pred txt
// UI 阻塞，tipLabel 显示进度
// ═══════════════════════════════════════════════════════════
void AiModelSet::onBatchValidImgPushButtonClicked()
{
    bool willShow = ui->batchValidImgPushButton->isChecked();
    if (!willShow) {
        m_emulating = false;
        m_emulateObjInfos.clear();
        update();
        return;
    }

    // 互斥：关掉其他两个按钮的选中态
    ui->validImgPushButton->setChecked(false);
    ui->validvsAnnoImgPushButton->setChecked(false);
    m_vsAnnoMode = false;

    // 从 vsAnno 筛选模式切换过来 → 恢复原始图片列表
    if (!m_origAllImagePaths.isEmpty()) {
        m_allImagePaths = m_origAllImagePaths;
        m_origAllImagePaths.clear();
        m_totalPages = (m_allImagePaths.size() + m_pageSize - 1) / m_pageSize;
        m_currentPage = 1;
        showPage(1);
    }

    // ① 模型必须选中
    if (modelName.isEmpty()) {
        ui->batchValidImgPushButton->setChecked(false);
        QMessageBox::warning(this, tr("提示"), tr("请先在模型加载中选择仿真模型！"));
        return;
    }
    
    // ② 当前目录必须有图（用 m_allImagePaths 取全量，不是 UI 上分页显示的那几条）
    int total = m_allImagePaths.size();
    if (total == 0) {
        ui->batchValidImgPushButton->setChecked(false);
        showTip("图片列表为空，请先导入图片目录", true);
        return;
    }

    // ③ 板卡模型 MD5 校验 + 自动上传（只做一次）
    if (!ensureBoardModelUploaded(modelName)) {
        ui->batchValidImgPushButton->setChecked(false);
        return;
    }

    // ④ 切换模型后先清掉旧 pred 目录（避免旧模型的缓存结果）
    if (modelName != m_lastEmulateModelName && !m_allImagePaths.isEmpty()) {
        QFileInfo fi(m_allImagePaths.first());
        QString predDir = fi.absolutePath() + "/pred";
        QDir(predDir).removeRecursively();
        m_lastEmulateModelName = modelName;
    }

    // ⑤ 禁用所有按钮（UI 阻塞），保存 enable 状态在 finally 里恢复
    QList<QPushButton*> allBtns = findChildren<QPushButton*>();
    for (auto* b : allBtns) b->setEnabled(false);
    ui->batchValidImgPushButton->setEnabled(true);  // 允许用户再次点击关闭

    // ⑤ 单次板卡连接复用（SFTP upload 每张图片）
    SftpClient cli(BOARD_HOST.toStdString(), 22, "root", "linaro");
    if (!cli.connect()) {
        showTip(QString("板卡 SFTP 连接失败 %1:22").arg(BOARD_HOST), true);
        ui->batchValidImgPushButton->setChecked(false);
        for (auto* b : allBtns) b->setEnabled(true);
        return;
    }
    QString boardImgDir = "/ftp/emulate";
    cli.mkdir_p(boardImgDir.toStdString());

    // ⑥ ModelApply（只做一次，加载模型到板卡内存）
    QString modelBinName = modelName + ".bin";
    {
        ModelApply applyInfo;
        applyInfo.model_name_ = modelBinName;
        QByteArray applyReq = cmdworker::ModelApplyRequest(applyInfo);
        QByteArray applyResp;
        CmdUdpManager::instance().onSendCommand(
            QHostAddress(BOARD_HOST), BOARD_PORT, applyReq, applyResp, 3000);
    }

    int successCount = 0;
    int failCount = 0;

    // ⑦ 遍历所有图片（m_allImagePaths = 整个目录全量，不是 UI 分页那几条）
    for (int i = 0; i < total; i++) {
        QString imgPath = m_allImagePaths.at(i);
        if (imgPath.isEmpty()) { failCount++; continue; }

        QFileInfo fi(imgPath);
        QString imgFileName = fi.fileName();

        // 进度显示
        showTip(QString("批量仿真 %1/%2（%3%） %4")
                    .arg(i+1).arg(total)
                    .arg(int((i+1)*100.0/total))
                    .arg(imgFileName));
        QApplication::processEvents();

        // 7a. SFTP upload 图片
        QString remotePath = boardImgDir + "/" + imgFileName;
        if (!cli.upload(imgPath.toStdString(), remotePath.toStdString())) {
            LOG_WARN_STM("批量仿真 SFTP 上传失败：" << imgFileName);
            failCount++; continue;
        }

        // 7b. Emulate UDP 请求（img_name 用纯文件名！）
        EmulateParam info;
        info.model_name_ = modelBinName;
        info.img_name_   = imgFileName;
        QByteArray request  = cmdworker::EmulateParamRequest(info);
        QByteArray response;
        bool ok = CmdUdpManager::instance().onSendCommand(
            QHostAddress(BOARD_HOST), BOARD_PORT, request, response, 3000);

        if (!ok) { failCount++; continue; }

        CmdPackage cmdPkg;
        if (!cmdworker::ParseCmdPkg(response, cmdPkg)) { failCount++; continue; }

        EmulateResInfo resInfo = cmdworker::EmulateParamResponse(cmdPkg);
        if (resInfo.code_ != 1) { failCount++; continue; }

        // 7c. 保存 pred txt（即使无目标也保存空文件）
        saveEmulateResultToFile(imgPath, resInfo.obj_infos_);
        successCount++;

        // 点击的是当前图 → 同步更新 m_emulateObjInfos + 重绘
        if (m_currentImagePath == imgPath) {
            m_emulateObjInfos = resInfo.obj_infos_;
            update();
        }
    }

    cli.disconnect();

    // ⑧ 恢复 UI + 自动进入仿真查看态（从 pred txt 读）
    ui->batchValidImgPushButton->setChecked(true);   // 批量按钮保持选中
    ui->batchValidImgPushButton->setEnabled(true);
    ui->validImgPushButton->setChecked(false);         // 互斥：仿真按钮取消选中
    for (auto* b : allBtns) b->setEnabled(true);

    // ⑨ 切换到仿真查看态（只读 pred txt）
    m_emulating = true;
    m_emulateObjInfos.clear();
    loadEmulateResultFromFile(m_currentImagePath, m_emulateObjInfos);
    update();

    showTip(QString("批量仿真完成：成功 %1 / 总计 %2（失败 %3）").arg(successCount).arg(total).arg(failCount)
                + QString("   [仿真查看模式：%1 按钮开启中]").arg(ui->batchValidImgPushButton->text()));
}

// ═══════════════════════════════════════════════════════════
// 仿真vs标注按钮：对比 pred txt 和人工标注
//   · pred 存在 + 标注存在 → IoU 匹配 + cls_id 不同 → 错标：虚线框 + AI 类别小色块
//   · 互斥：和仿真按钮、批量仿真按钮三选一
// ═══════════════════════════════════════════════════════════
void AiModelSet::onValidvsAnnoImgPushButtonClicked()
{
    bool willShow = ui->validvsAnnoImgPushButton->isChecked();
    if (!willShow) {
        m_vsAnnoMode = false;
        // 取消筛选 → 恢复显示全部图片
        if (!m_origAllImagePaths.isEmpty()) {
            m_allImagePaths = m_origAllImagePaths;
            m_origAllImagePaths.clear();
            m_totalPages = (m_allImagePaths.size() + m_pageSize - 1) / m_pageSize;
            m_currentPage = 1;
            showPage(1);
        }
        update();
        return;
    }

    // 和其他按钮互斥
    ui->validImgPushButton->setChecked(false);
    ui->batchValidImgPushButton->setChecked(false);
    m_emulating = false;
    m_emulateObjInfos.clear();

    // ① 遍历全目录，筛选出有错标的图片
    showTip("筛选错标图片中...");
    QApplication::processEvents();
    QStringList mismatchList;
    int checked = 0;
    int total = m_allImagePaths.size();
    for (const QString& path : m_allImagePaths) {
        checked++;
        if (hasMismatch(path)) mismatchList.append(path);
        if (checked % 20 == 0) {
            showTip(QString("筛选错标图片中... %1/%2（%3 张错标）")
                        .arg(checked).arg(total).arg(mismatchList.size()));
            QApplication::processEvents();
        }
    }
    showTip(QString("筛选完成：%1 张图片有错标 / 总计 %2 张").arg(mismatchList.size()).arg(total));

    // ② 没找到错标 → 弹框提示，取消按钮
    if (mismatchList.isEmpty()) {
        ui->validvsAnnoImgPushButton->setChecked(false);
        QMessageBox::information(this, tr("提示"),
            tr("全部 %1 张图片标注和仿真结果一致，没有错标！").arg(total));
        return;
    }

    // ③ 保存原始列表 + 替换成筛选后的列表 + 刷新 UI
    m_origAllImagePaths = m_allImagePaths;
    m_allImagePaths = mismatchList;
    m_totalPages = (m_allImagePaths.size() + m_pageSize - 1) / m_pageSize;
    m_currentPage = 1;
    showPage(1);

    // ④ 进入对比模式
    m_vsAnnoMode = true;
    update();
}



void AiModelSet::onModelTrainPushButtonClicked(){
    // 先确保训练服务器已配置（未配置会弹对话框让用户填）
    if (!ensureTrainServerConfigured()) {
        return;
    }
    // 防重入：训练正在进行时 → 主动查询进度（不提交新训练）
    if (m_trainingBusy) {
        if (!m_currentTaskId.isEmpty()) {
            showTip("正在训练，查询最新进度...");
            m_modelApi->queryTrainProgress(m_currentTaskId);
        } else {
            showTip("训练流程正在进行中，请等待完成");
        }
        return;
    }

    // 空训练集校验
    if (m_train_lists.isEmpty()) {
        showTip("训练集为空，请先添加训练图片", true);
        return;
    }
    if (m_train_lists.size() <= 20) {
        showTip(QString("训练图片数量不足（当前 %1 张，需超过 20 张，不含背景图）").arg(m_train_lists.size()), true);
        return;
    }
    if (modelName.isEmpty()) {
        showTip("模型名为空，请先创建或加载模型", true);
        return;
    }

    // 1. 准备训练数据（本地 tmp 目录）
    showTip(QString("准备训练数据（%1 张图片）...").arg(m_train_lists.size()));
    if (!prepareTrain()) {
        // 类别校验没通过且用户点了 No，已经弹过告警了
        return;
    }

    // 1.5 校验 prepareTrain 产物（防止 tmp 里空文件就开始 SFTP）
    QString tmpDir = QDir(QCoreApplication::applicationDirPath()).filePath("tmp");
    int realImgs = QDir(QDir(tmpDir).filePath("image")).entryInfoList(QDir::Files).size();
    int realLbls = QDir(QDir(tmpDir).filePath("label")).entryInfoList(QDir::Files).size();
    if (realImgs == 0) {
        showTip("prepareTrain 失败：tmp/image 为空，请检查训练图片路径", true);
        return;
    }
    showTip(QString("训练数据准备完成：%1 张图片 + %2 个标签").arg(realImgs).arg(realLbls));

    // 3. 初始化轮询定时器（第一次进的时候创建）
    if (!m_pollTimer) {
        m_pollTimer = new QTimer(this);
        m_pollTimer->setInterval(5000);
        connect(m_pollTimer, &QTimer::timeout, this, [this]() {
            if (m_currentTaskId.isEmpty()) return;
            m_modelApi->queryTrainProgress(m_currentTaskId);
        });
    }

    // 3. 发起远程创建任务目录
    ++m_tryTimes;   // 内部计数，不影响 task_id
    m_currentTaskId = modelName;
    m_trainingBusy = true;
    m_pollFailCount = 0;
    showTip(QString("正在创建远程训练任务目录... 任务ID：%1").arg(m_currentTaskId));
    m_modelApi->createDirTrain(m_currentTaskId);
}

void AiModelSet::onModelNewPushButtonClicked()
{
    // ── 创建对话框 ──
    QDialog dlg(this);
    dlg.setWindowTitle("新建模型");
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setMinimumWidth(480);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);

    // ── 第一栏: 模型名称 ──
    QLabel *nameLabel = new QLabel("模型名称:", &dlg);
    QLineEdit *nameEdit = new QLineEdit(&dlg);
    nameEdit->setPlaceholderText("仅允许英文字母、数字、下划线 _");
    nameEdit->setFocusPolicy(Qt::NoFocus);  // ⭐ 阻止系统虚拟键盘自动弹出，只用 myInputMethod
    // ⭐ 板卡上点击 QLineEdit 弹软键盘
    struct KbFilter : QObject {
        QString title;
        explicit KbFilter(const QString &t, QObject *p = nullptr) : QObject(p), title(t) {}
        bool eventFilter(QObject *o, QEvent *e) override {
            if (e->type() == QEvent::MouseButtonPress) {
                QLineEdit *le = qobject_cast<QLineEdit*>(o);
                if (le) {
                    myInputMethod kb(title, le->text());
                    if (kb.exec() == QDialog::Accepted) le->setText(kb.getText());
                    return true;
                }
            }
            return QObject::eventFilter(o, e);
        }
    };
    nameEdit->installEventFilter(new KbFilter("新建模型", &dlg));

    // 正则: 英文字母/数字/下划线，不允许中文和其他特殊符号
    QRegularExpression reModelName("^[A-Za-z0-9_]+$");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(reModelName, &dlg);
    nameEdit->setValidator(validator);
    if (!modelName.isEmpty()) nameEdit->setText(modelName);

    QHBoxLayout *nameRow = new QHBoxLayout();
    nameRow->addWidget(nameLabel);
    nameRow->addWidget(nameEdit, 1);
    mainLayout->addLayout(nameRow);

    // ── 第二栏: 模型类别数量 (9 个 checkbox, 单选) ──
    QLabel *countLabel = new QLabel("类别数量:", &dlg);
    QHBoxLayout *countRow = new QHBoxLayout();
    countRow->addWidget(countLabel);

    QButtonGroup *countGroup = new QButtonGroup(&dlg);
    countGroup->setExclusive(true);  // 单选

    for (int i = 1; i <= 9; i++) {
        QCheckBox *cb = new QCheckBox(QString::number(i), &dlg);
        cb->setProperty("value", i);
        countGroup->addButton(cb, i);
        countRow->addWidget(cb);
    }
    // 默认选中当前 modelCategoryNum (范围 1~9)
    if (modelCategoryNum >= 1 && modelCategoryNum <= 9) {
        QAbstractButton *btn = countGroup->button(modelCategoryNum);
        if (btn) btn->setChecked(true);
    } else {
        countGroup->button(1)->setChecked(true);
    }
    countRow->addStretch(1);
    mainLayout->addLayout(countRow);

    // ── 确定 / 取消 ──
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    QPushButton *okBtn = new QPushButton("确定", &dlg);
    QPushButton *cancelBtn = new QPushButton("取消", &dlg);
    btnRow->addWidget(okBtn);
    btnRow->addWidget(cancelBtn);
    mainLayout->addLayout(btnRow);

    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(okBtn, &QPushButton::clicked, &dlg, [&]() {
        // 校验模型名称不能为空
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            nameEdit->setStyleSheet("QLineEdit { border: 2px solid red; }");
            return;
        }
        dlg.accept();
    });

    // 执行对话框
    if (dlg.exec() == QDialog::Accepted) {
        // 赋值
        modelName = nameEdit->text().trimmed();
        modelCategoryNum = countGroup->checkedId();

        // 更新 UI 上的 modelNameLabel
        ui->modelNameLabel->setText(modelName);

        // 更新 tipLabel 提示
        showTip(QString("新建模型: %1, 类别数: %2").arg(modelName).arg(modelCategoryNum));

        // 🔧 新建模型 → 清空 json 残留的类别名，让 setupClassCheckBoxes 自动按 0..N-1 生成
        m_classNames.clear();

        // 🔧 关键：重建动态 checkbox，数量跟 modelCategoryNum 对齐
        updateCategoryChenkBox();
    }
}

// ═══════════════════════════════════════════════════════════
// 训练服务器配置相关实现
// ═══════════════════════════════════════════════════════════

// 静态：判断当前编译目标是否 arm
bool AiModelSet::isArmBuild()
{
#if defined(__arm__) || defined(__aarch64__) || defined(Q_PROCESSOR_ARM)
    return true;
#else
    return false;
#endif
}

// JSON 配置文件路径（arm → /opt/app/userdata/cnf/trainserver.json，开发版 → 程序目录）
QString AiModelSet::trainServerJsonPath() const
{
    if (isArmBuild()) {
        return "/opt/app/userdata/cnf/trainserver.json";
    }
    return QDir(QCoreApplication::applicationDirPath()).filePath("trainserver.json");
}

// 加载训练服务器配置
void AiModelSet::loadTrainServerConfig()
{
    QString path = trainServerJsonPath();
    QFile f(path);
    if (!f.exists()) {
        LOG_INFO_STM("[TrainServerCfg] 配置文件不存在：" << path << "，使用默认值");
        return;
    }
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARN_STM("[TrainServerCfg] 配置文件读失败：" << path);
        return;
    }
    QByteArray data = f.readAll();
    f.close();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        LOG_WARN_STM("[TrainServerCfg] JSON 解析错误：" << err.errorString());
        return;
    }
    QJsonObject obj = doc.object();
    m_trainServerIp   = obj.value("host").toString(m_trainServerIp);
    m_trainHttpPort   = obj.value("http_port").toInt(m_trainHttpPort);
    m_trainSftpPort   = obj.value("sftp_port").toInt(m_trainSftpPort);
    m_trainSftpUser   = obj.value("sftp_user").toString(m_trainSftpUser);
    m_trainSftpPass   = obj.value("sftp_pass").toString(m_trainSftpPass);

    LOG_INFO_STM("[TrainServerCfg] 已加载：host=" << m_trainServerIp
                 << " http_port=" << m_trainHttpPort
                 << " sftp_port=" << m_trainSftpPort);
}

// 保存训练服务器配置
void AiModelSet::saveTrainServerConfig()
{
    QString path = trainServerJsonPath();
    QJsonObject obj;
    obj["host"]       = m_trainServerIp;
    obj["http_port"]  = m_trainHttpPort;
    obj["sftp_port"]  = m_trainSftpPort;
    obj["sftp_user"]  = m_trainSftpUser;
    obj["sftp_pass"]  = m_trainSftpPass;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    // arm 版本需要确保目录存在
    QFileInfo fi(path);
    QDir().mkpath(fi.absolutePath());

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_WARN_STM("[TrainServerCfg] 保存失败：" << path << " " << f.errorString());
        return;
    }
    f.write(data);
    f.close();
    LOG_INFO_STM("[TrainServerCfg] 已保存到：" << path);
}

// 检查训练服务器是否已配置有效，未配置则弹配置对话框
bool AiModelSet::ensureTrainServerConfigured()
{
    bool valid = !m_trainServerIp.isEmpty() && m_trainHttpPort > 0 && m_trainHttpPort < 65536;
    if (valid) return true;

    // arm 版也尝试再读一次文件（可能用户之前手动放了 JSON）
    if (isArmBuild()) {
        loadTrainServerConfig();
        valid = !m_trainServerIp.isEmpty() && m_trainHttpPort > 0;
        if (valid) {
            // 重新同步到 ModelApi
            m_modelApi->setBaseUrl(QString("http://%1:%2").arg(m_trainServerIp).arg(m_trainHttpPort));
            return true;
        }
    }

    // 弹配置对话框
    QMessageBox::information(this, "提示",
        "训练服务器尚未配置，请先配置训练服务器参数");
    onTrainServerCfgPushButtonClicked();
    // 如果用户取消了对话框，valid 还是 false，但我们没法知道他取消了
    // 简单处理：再检查一次（如果他确定了会同步到成员变量）
    return !m_trainServerIp.isEmpty() && m_trainHttpPort > 0;
}

// ── 训练服务器配置对话框 ──
void AiModelSet::onTrainServerCfgPushButtonClicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle("训练服务器配置");
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setMinimumWidth(480);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);

    // ── 3 栏输入 ──
    QFormLayout *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setSpacing(8);

    QLineEdit *ipEdit   = new QLineEdit(&dlg);
    QLineEdit *httpPortEdit = new QLineEdit(&dlg);
    QLineEdit *sftpPortEdit = new QLineEdit(&dlg);
    QLineEdit *userEdit = new QLineEdit(&dlg);
    QLineEdit *passEdit = new QLineEdit(&dlg);
    passEdit->setEchoMode(QLineEdit::Password);
    
    // ⭐ 板卡上点击 QLineEdit 弹软键盘（小 helper）
    auto installKb = [&](QLineEdit *le, const QString &title) {
        le->setFocusPolicy(Qt::NoFocus);  // ⭐ 阻止系统虚拟键盘
        struct KbFilter : QObject {
            QString t;
            explicit KbFilter(const QString &tt, QObject *p) : QObject(p), t(tt) {}
            bool eventFilter(QObject *o, QEvent *e) override {
                if (e->type() == QEvent::MouseButtonPress) {
                    QLineEdit *le2 = qobject_cast<QLineEdit*>(o);
                    if (le2) {
                        myInputMethod kb(t, le2->text());
                        if (kb.exec() == QDialog::Accepted) le2->setText(kb.getText());
                        return true;
                    }
                }
                return QObject::eventFilter(o, e);
            }
        };
        le->installEventFilter(new KbFilter(title, &dlg));
    };
    installKb(ipEdit,        "训练服务器 IP");
    installKb(httpPortEdit,  "HTTP 端口");
    installKb(sftpPortEdit,  "SFTP 端口");
    installKb(userEdit,      "SFTP 用户名");
    installKb(passEdit,      "SFTP 密码");

    // 填入当前值
    ipEdit->setText(m_trainServerIp);
    httpPortEdit->setText(QString::number(m_trainHttpPort));
    sftpPortEdit->setText(QString::number(m_trainSftpPort));
    userEdit->setText(m_trainSftpUser);
    passEdit->setText(m_trainSftpPass);

    httpPortEdit->setValidator(new QIntValidator(1, 65535, &dlg));
    sftpPortEdit->setValidator(new QIntValidator(1, 65535, &dlg));

    form->addRow("服务器 IP:", ipEdit);
    form->addRow("HTTP 端口:", httpPortEdit);
    form->addRow("SFTP 端口:", sftpPortEdit);
    form->addRow("SFTP 用户名:", userEdit);
    form->addRow("SFTP 密码:", passEdit);
    mainLayout->addLayout(form);

    // ── 2 个测试按钮 ──
    QHBoxLayout *testRow = new QHBoxLayout();
    QPushButton *testHttpBtn  = new QPushButton("测试 HTTP 连接", &dlg);
    QPushButton *testSftpBtn  = new QPushButton("测试 SFTP 连接", &dlg);
    QLabel *testResultLabel = new QLabel("", &dlg);
    testResultLabel->setStyleSheet("color: gray; font-size: 8pt;");
    testResultLabel->setWordWrap(true);

    testRow->addWidget(testHttpBtn);
    testRow->addWidget(testSftpBtn);
    testRow->addStretch(1);
    mainLayout->addLayout(testRow);
    mainLayout->addWidget(testResultLabel);

    // HTTP 测试：POST /api/cloud/train/query {id: "test"}，5s 超时
    connect(testHttpBtn, &QPushButton::clicked, [&]() {
        QString ip = ipEdit->text().trimmed();
        int port = httpPortEdit->text().toInt();
        if (ip.isEmpty() || port <= 0) {
            testResultLabel->setText("请先填写有效的 IP 和 HTTP 端口");
            testResultLabel->setStyleSheet("color: red; font-size: 8pt;");
            return;
        }
        testHttpBtn->setEnabled(false);
        testResultLabel->setStyleSheet("color: gray; font-size: 8pt;");
        testResultLabel->setText(QString("正在测试 HTTP 连接 %1:%2 ...").arg(ip).arg(port));
        QApplication::processEvents();

        // 用临时 HttpTool 同步测试（短超时）
        // ⚠️ 同步发送：直接用 QNetworkAccessManager 更简单可控
        QNetworkAccessManager manager;
        QNetworkRequest req(QUrl(QString("http://%1:%2%3")
            .arg(ip).arg(port).arg(API_TRAIN_QUERY)));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QNetworkReply *reply = manager.post(req, QByteArray("{\"id\":\"test\"}"));
        // 同步等 5s
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        timer.start(5000);
        loop.exec();

        if (!timer.isActive()) {
            // 超时
            reply->abort();
            testResultLabel->setStyleSheet("color: red; font-size: 8pt;");
            testResultLabel->setText(QString("HTTP 连接失败：5 秒超时（%1:%2）").arg(ip).arg(port));
        } else if (reply->error() != QNetworkReply::NoError) {
            testResultLabel->setStyleSheet("color: red; font-size: 8pt;");
            testResultLabel->setText(QString("HTTP 连接失败：%1").arg(reply->errorString()));
        } else {
            int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            testResultLabel->setStyleSheet("color: green; font-size: 8pt;");
            testResultLabel->setText(QString("HTTP 连接成功（HTTP %1）").arg(code));
        }
        reply->deleteLater();
        testHttpBtn->setEnabled(true);
    });

    // SFTP 测试：尝试 connect + disconnect
    connect(testSftpBtn, &QPushButton::clicked, [&]() {
        QString ip = ipEdit->text().trimmed();
        int port = sftpPortEdit->text().toInt();
        QString user = userEdit->text().trimmed();
        QString pass = passEdit->text();
        if (ip.isEmpty() || port <= 0) {
            testResultLabel->setText("请先填写有效的 IP 和 SFTP 端口");
            testResultLabel->setStyleSheet("color: red; font-size: 8pt;");
            return;
        }
        testSftpBtn->setEnabled(false);
        testResultLabel->setStyleSheet("color: gray; font-size: 8pt;");
        testResultLabel->setText(QString("正在测试 SFTP 连接 %1:%2 ...").arg(ip).arg(port));
        QApplication::processEvents();

        SftpClient cli(ip.toStdString(), port, user.toStdString(), pass.toStdString());
        if (cli.connect()) {
            cli.disconnect();
            testResultLabel->setStyleSheet("color: green; font-size: 8pt;");
            testResultLabel->setText(QString("SFTP 连接成功（%1@%2:%3）").arg(user).arg(ip).arg(port));
        } else {
            testResultLabel->setStyleSheet("color: red; font-size: 8pt;");
            testResultLabel->setText(QString("SFTP 连接失败：%1:%2").arg(ip).arg(port));
        }
        testSftpBtn->setEnabled(true);
    });

    // ── OK / Cancel ──
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    QPushButton *okBtn = new QPushButton("确定", &dlg);
    QPushButton *cancelBtn = new QPushButton("取消", &dlg);
    btnRow->addWidget(okBtn);
    btnRow->addWidget(cancelBtn);
    mainLayout->addLayout(btnRow);

    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(okBtn, &QPushButton::clicked, [&]() {
        QString ip = ipEdit->text().trimmed();
        int httpPort = httpPortEdit->text().toInt();
        int sftpPort = sftpPortEdit->text().toInt();
        if (ip.isEmpty() || httpPort <= 0 || sftpPort <= 0) {
            QMessageBox::warning(&dlg, "输入错误", "请填写有效的 IP 和端口");
            return;
        }
        dlg.accept();
    });

    if (dlg.exec() == QDialog::Accepted) {
        m_trainServerIp = ipEdit->text().trimmed();
        m_trainHttpPort = httpPortEdit->text().toInt();
        m_trainSftpPort = sftpPortEdit->text().toInt();
        m_trainSftpUser = userEdit->text().trimmed();
        m_trainSftpPass = passEdit->text();

        // 同步到 ModelApi
        QString baseUrl = QString("http://%1:%2").arg(m_trainServerIp).arg(m_trainHttpPort);
        m_modelApi->setBaseUrl(baseUrl);

        // 重建 SftpWorker（安全退出旧线程 → delete worker → 用新配置创建）
        if (m_sftpThread && m_sftpThread->isRunning()) {
            m_sftpThread->quit();
            m_sftpThread->wait(2000);
            delete m_sftpThread;
            m_sftpThread = nullptr;
        }
        if (m_sftpWorker) {
            if (m_sftpWorker->thread() != QThread::currentThread()) {
                m_sftpWorker->moveToThread(QThread::currentThread());
            }
            delete m_sftpWorker;
            m_sftpWorker = nullptr;
        }
        m_sftpWorker = new SftpWorker(m_trainServerIp, m_trainSftpUser, m_trainSftpPass, m_trainSftpPort);

        // arm 版本保存到 JSON
        saveTrainServerConfig();

        showTip(QString("训练服务器已更新：HTTP %1，SFTP %2:%3")
                    .arg(baseUrl).arg(m_trainServerIp).arg(m_trainSftpPort));
    }
}

// -------------------------- ImageScanWorker 实现 --------------------------
// (内联在 .h 里，这里无需额外代码)

// -------------------------- ModelApi 实现 --------------------------
ModelApi::ModelApi(QObject *parent)
    : QObject(parent)
    , m_currentRequestType(RequestType::None)
{
    m_httpTool = new HttpTool(this);
    m_httpTool->setTimeout(30000);  // 默认 30 秒超时

    // 绑定 HTTP 信号
    connect(m_httpTool, &HttpTool::requestSuccess, this, &ModelApi::onHttpRequestSuccess);
    connect(m_httpTool, &HttpTool::requestFailed, this, &ModelApi::onHttpRequestFailed);
    connect(m_httpTool, &HttpTool::downloadProgress, this, &ModelApi::onHttpDownloadProgress);

    // 自定义下载完成信号（通过 lambda 转发）
    connect(m_httpTool, &HttpTool::requestSuccess, this, [this](const QByteArray&, int) {
        LOG_DEBUG_STM("[DOWNLOAD] requestSuccess 转发 lambda，当前 m_currentRequestType=" << static_cast<int>(m_currentRequestType));
        if (m_currentRequestType == RequestType::DownloadModel) {
            LOG_DEBUG_STM("[DOWNLOAD] 命中 DownloadModel → emit downloadFinished(true)");
            emit downloadFinished(true, "");
            m_currentRequestType = RequestType::None;
        }
        if (m_currentRequestType == RequestType::DownloadJson) {
            LOG_DEBUG_STM("[DOWNLOAD] 命中 DownloadJson → emit downloadJsonFinished(true)");
            emit downloadJsonFinished(true, "");
            m_currentRequestType = RequestType::None;
        }
    });
    connect(m_httpTool, &HttpTool::requestFailed, this, [this](const QString& errorMsg, int) {
        LOG_DEBUG_STM("[DOWNLOAD] requestFailed 转发 lambda，错误=" << errorMsg << "，当前 m_currentRequestType=" << static_cast<int>(m_currentRequestType));
        if (m_currentRequestType == RequestType::DownloadModel) {
            LOG_DEBUG_STM("[DOWNLOAD] 命中 DownloadModel → emit downloadFinished(false)");
            emit downloadFinished(false, errorMsg);
            m_currentRequestType = RequestType::None;
        }
        if (m_currentRequestType == RequestType::DownloadJson) {
            LOG_DEBUG_STM("[DOWNLOAD] 命中 DownloadJson → emit downloadJsonFinished(false)");
            emit downloadJsonFinished(false, errorMsg);
            m_currentRequestType = RequestType::None;
        }
    });
}

ModelApi::~ModelApi()
{
    delete m_httpTool;
}

// 设置 HTTP 超时时间
void ModelApi::setHttpTimeout(int timeoutMs)
{
    m_httpTool->setTimeout(timeoutMs);
}

void ModelApi:: createDirTrain(const QString& taskId)
{
    if (taskId.isEmpty()) {
        emit networkError("任务 ID 不能为空");
        return;
    }
    
    // 构建请求 URL
    QString url = m_baseUrl + API_TRAIN_CREATE;

    // 构建请求体（协议要求：{"id": "任务唯一id"}）
    QJsonObject requestBody;
    requestBody["id"] = taskId;
    QByteArray jsonData = QJsonDocument(requestBody).toJson(QJsonDocument::Compact);

    // 记录当前请求类型和任务 ID
    m_currentRequestType = RequestType::CreateDirTrain;
    m_currentTaskId = taskId;

    // 发起 POST JSON 请求（协议要求 POST 方法，请求体为 JSON）
    m_httpTool->sendRequest(
        url,
        HttpRequestType::POST_JSON,  // 匹配 JSON 格式
        QMap<QString, QString>(),    // 无表单参数
        QString(jsonData)            // JSON 数据
    );

    LOG_DEBUG_STM("[模型目录建立] 发起请求：URL=" << url << "，任务ID=" << taskId << "，请求体=" << QString(jsonData));
}




// 1. 启动模型训练（严格匹配协议）
void ModelApi::startTrain(const QString& taskId)
{

    if (taskId.isEmpty()) {
        emit networkError("任务 ID 不能为空");
        return;
    }

    // 构建请求 URL
    QString url = m_baseUrl + API_TRAIN_START;

    // 构建请求体（协议要求：{"id": "任务唯一id"}）
    QJsonObject requestBody;
    requestBody["id"] = taskId;
    QByteArray jsonData = QJsonDocument(requestBody).toJson(QJsonDocument::Compact);

    // 记录当前请求类型和任务 ID
    m_currentRequestType = RequestType::StartTrain;
    m_currentTaskId = taskId;

    // 发起 POST JSON 请求（协议要求 POST 方法，请求体为 JSON）
    m_httpTool->sendRequest(
        url,
        HttpRequestType::POST_JSON,  // 匹配 JSON 格式
        QMap<QString, QString>(),    // 无表单参数
        QString(jsonData)            // JSON 数据
    );

    LOG_DEBUG_STM("[模型训练启动] 发起请求：URL=" << url << "，任务ID=" << taskId << "，请求体=" << QString(jsonData));
}

// 2. 查询训练进度（严格匹配协议）
void ModelApi::queryTrainProgress(const QString& taskId)
{
    if (taskId.isEmpty()) {
        emit networkError("任务 ID 不能为空");
        return;
    }

    // 构建请求 URL
    QString url = m_baseUrl + API_TRAIN_QUERY;

    // 构建请求体（协议要求：{"id": "任务唯一id"}）
    QJsonObject requestBody;
    requestBody["id"] = taskId;
    QByteArray jsonData = QJsonDocument(requestBody).toJson(QJsonDocument::Compact);

    // 记录当前请求类型和任务 ID
    m_currentRequestType = RequestType::QueryTrain;
    m_currentTaskId = taskId;

    // 发起 POST JSON 请求（协议要求 POST 方法，请求体为 JSON）
    m_httpTool->sendRequest(
        url,
        HttpRequestType::POST_JSON,
        QMap<QString, QString>(),
        QString(jsonData)
    );

    LOG_DEBUG_STM("[训练进度查询] 发起请求：URL=" << url << "，任务ID=" << taskId << "，请求体=" << QString(jsonData));
}

// 查询硬件架构（GET /api/cloud/config/arch，决定下载 .bin 还是 .dlc）
void ModelApi::queryArch()
{
    QString url = m_baseUrl + API_CONFIG_ARCH;

    m_currentRequestType = RequestType::QueryArch;

    // 发起 GET 请求（无请求体）
    m_httpTool->sendRequest(url, HttpRequestType::GET);

    LOG_DEBUG_STM("[硬件架构查询] 发起请求：URL=" << url);
}

// 3. 下载模型（HTTP GET）
void ModelApi::downloadModel(const QString& modelName, const QString& savePath)
{
    LOG_DEBUG_STM("[DOWNLOAD] ModelApi::downloadModel 入口：modelName=" << modelName << "savePath=" << savePath);
    m_httpTool->setTimeout(120000);  // 下载大文件给 120 秒超时
    if (modelName.isEmpty()) {
        emit networkError("模型名称不能为空");
        return;
    }
    if (savePath.isEmpty()) {
        emit networkError("保存路径不能为空");
        return;
    }

    // 构建请求 URL
    QString url = m_baseUrl + API_MODEL_DOWNLOAD + modelName;

    // 记录当前请求类型
    m_currentRequestType = RequestType::DownloadModel;

    // 发起 GET 请求下载文件
    m_httpTool->downloadFile(url, savePath);

    LOG_DEBUG_STM("[模型下载] 发起请求：URL=" << url << "，保存路径=" << savePath);
}

void ModelApi::downloadModelJson(const QString& modelJson, const QString& savePath){
    LOG_DEBUG_STM("[DOWNLOAD] ModelApi::downloadModelJson 入口：modelJson=" << modelJson << "savePath=" << savePath);
    if (modelJson.isEmpty()) {
        emit networkError("模型Json不能为空");
        return;
    }
    if (savePath.isEmpty()) {
        emit networkError("保存路径不能为空");
        return;
    }

    // 构建请求 URL
    QString url = m_baseUrl + API_MODEL_DOWNLOAD + modelJson;

    // 记录当前请求类型
    m_currentRequestType = RequestType::DownloadJson;

    // 发起 GET 请求下载文件
    m_httpTool->downloadFile(url, savePath);

    LOG_DEBUG_STM("[模型JSON下载] 发起请求：URL=" << url << "，保存路径=" << savePath);
}


// HTTP 请求成功回调（训练启动/查询）
void ModelApi::onHttpRequestSuccess(const QByteArray& responseData, int statusCode)
{
    // 下载场景（模型文件 / 模型 JSON）由构造函数中的 lambda 独立转发为
    // downloadFinished / downloadJsonFinished，此处跳过，避免提前清空 m_currentRequestType
    if (m_currentRequestType == RequestType::DownloadModel ||
        m_currentRequestType == RequestType::DownloadJson) {
        return;
    }

    LOG_DEBUG_STM("[HTTP 成功] 状态码=" << statusCode << "，响应数据=" << QString(responseData));

    // 严格检查 HTTP 状态码（协议要求 200 正常返回）
    if (statusCode != 200) {
        emit networkError(QString("HTTP 状态码异常：%1").arg(statusCode));
        m_currentRequestType = RequestType::None;
        return;
    }
 
    // 记录进入时的类型，emit 过程中可能触发下游新请求（如 archResult → downloadModel）
    // 只有类型没变时才清 None，避免覆盖下游新请求的类型
    const RequestType enteredType = m_currentRequestType;

    // 根据请求类型解析响应
    switch (m_currentRequestType) {
    case RequestType::StartTrain: {
        TrainStartResponse response = parseStartTrainResponse(responseData);
        emit startTrainResult(response);
        break;
    }
    case RequestType::QueryTrain: {
        TrainQueryResponse response = parseQueryTrainResponse(responseData);
        emit queryTrainResult(response);
        break;
    }
    case RequestType::CreateDirTrain: {
        TrainStartResponse response = parseStartTrainResponse(responseData);
        emit createDirTrainResult(response);
        break;
    }
    case RequestType::QueryArch: {
        QString arch = parseArchResponse(responseData);
        emit archResult(arch);
        break;
    }
    default:
        break;
    }

    // 仅当类型仍为本次请求的类型时才清 None，避免覆盖下游同步发起的新请求类型
    if (m_currentRequestType == enteredType) {
        m_currentRequestType = RequestType::None;
    }
}

// HTTP 请求失败回调
void ModelApi::onHttpRequestFailed(const QString& errorMsg, int errorCode)
{
    // 下载场景（模型文件 / 模型 JSON）由构造函数中的 lambda 独立转发，此处跳过，避免提前清空 m_currentRequestType
    if (m_currentRequestType == RequestType::DownloadModel ||
        m_currentRequestType == RequestType::DownloadJson) {
        return;
    }

    QString err = QString("[HTTP 失败] 错误码=%1，错误信息=%2").arg(errorCode).arg(errorMsg);
    LOG_DEBUG_STM(err);
    emit networkError(err);
    m_currentRequestType = RequestType::None;
}

// 下载进度回调
void ModelApi::onHttpDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(m_currentRequestType == RequestType::DownloadModel){
        emit downloadProgress(bytesReceived, bytesTotal);
    }
    if(m_currentRequestType == RequestType::DownloadJson){
        emit downloadJsonProgress(bytesReceived, bytesTotal);
    }
}

// 解析训练启动响应（严格匹配协议返回字段）
TrainStartResponse ModelApi::parseStartTrainResponse(const QByteArray& data)
{
    TrainStartResponse response;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);

    // JSON 解析失败
    if (jsonError.error != QJsonParseError::NoError || !doc.isObject()) {
        response.code = -1;
        response.message = QString("响应解析失败：%1").arg(jsonError.errorString());
        return response;
    }

    QJsonObject obj = doc.object();
    // 解析字段（严格匹配协议中的 key：code、message）
    response.code = obj.contains("code") ? obj["code"].toInt() : -1;
    response.message = obj.contains("message") ? obj["message"].toString() : "未知错误";

    return response;
}

// 解析训练查询响应（严格匹配协议返回字段）
TrainQueryResponse ModelApi::parseQueryTrainResponse(const QByteArray& data)
{
    TrainQueryResponse response;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);

    // JSON 解析失败
    if (jsonError.error != QJsonParseError::NoError || !doc.isObject()) {
        response.code = -1;
        response.message = QString("响应解析失败：%1").arg(jsonError.errorString());
        response.progress = 0;
        response.number = -1;
        return response;
    }

    QJsonObject obj = doc.object();
    // 解析字段（严格匹配协议中的 key：code、message、progress、number、model）
    response.task_id = obj.contains("task_id") ? obj["task_id"].toString() : "";
    response.code = obj.contains("code") ? obj["code"].toInt() : -1;
    response.message = obj.contains("message") ? obj["message"].toString() : "未知错误";
    response.progress = obj.contains("progress") ? obj["progress"].toInt() : 0;
    response.number = obj.contains("number") ? obj["number"].toInt() : -1;
    response.model_name = obj.contains("model_name") ? obj["model_name"].toString() : "";
    response.model_dir = obj.contains("model_dir") ? obj["model_dir"].toString() : "";

    return response;
}

// 解析硬件架构查询响应（返回 arch 字符串，解析失败兜底 "x5"）
QString ModelApi::parseArchResponse(const QByteArray& data)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error != QJsonParseError::NoError || !doc.isObject()) {
        return "x5";
    }
    QJsonObject obj = doc.object();
    QString arch = obj.contains("arch") ? obj["arch"].toString() : "x5";
    return arch.isEmpty() ? "x5" : arch;
}
