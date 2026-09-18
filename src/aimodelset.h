#ifndef AIMODELSET_H
#define AIMODELSET_H


#include <QWidget>
#include <QFileDialog>
#include <QStringList>
#include <QThread>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMetaObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>
#include <QRect>
#include <QColor>
#include <QVector>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QCheckBox>
#include <QSet>
#include <QTimer>
#include "httptool.h"
#include "sftpworker.h"
#include "merge_dialog.h"
#include "sortertypes.h"

// ══════════════════════════════════════════════════════════════════
// 路径配置（编译期宏，可用 -D 覆盖；独立工程从 globalparams/aicommunicate 缺省）
// ══════════════════════════════════════════════════════════════════
#ifndef APP_PATH
#define APP_PATH "./"
#endif

// 模型下载保存目录：训练完成后模型文件(.bin/.json)下载到本地的目录，默认 /ftp/model/
#ifndef LOCAL_MODEL_PATH
#define LOCAL_MODEL_PATH "/ftp/model/"
#endif

// 本地图片目录：导入图片时的默认打开目录，默认 ./img/
#ifndef LOCAL_IMG_PATH
#define LOCAL_IMG_PATH  APP_PATH "img/"
#endif



namespace Ui {
class AiModelSet;
}

// 接口路径（保留为常量，仅 URL 前缀和 SFTP host/port/user/pass 改为成员变量）
const QString API_TRAIN_CREATE = "/api/cloud/train/create";   // 创建目录

const QString API_TRAIN_START = "/api/cloud/train/start";    // 训练启动
const QString API_TRAIN_QUERY = "/api/cloud/train/query";    // 训练查询
const QString API_MODEL_DOWNLOAD = "/api/cloud/download_file/";                      // 模型下载（路径拼接模型名称）
const QString API_CONFIG_ARCH = "/api/cloud/config/arch";    // 查询硬件架构（snpe→.dlc，其他→.bin）

// ══════════════════════════════════════════════════════════════════
// 仿真板卡配置（AI 推理 UDP 通信）
// ══════════════════════════════════════════════════════════════════
const QString BOARD_HOST = "192.168.0.12";    // 板卡 AI 仿真目标 IP
const int     BOARD_PORT = 9193;               // 板卡 AI 仿真目标 UDP 端口
const QString BOARD_SFTP_HOST = "192.168.0.12"; // 板卡 SFTP 地址（上传模型）
const QString BOARD_SFTP_USER = "root";
const QString BOARD_SFTP_PASS = "linaro";
const int     BOARD_SFTP_PORT = 22;

// 响应错误码（根据接口文档扩展）
const int RESPONSE_SUCCESS_CODE = 0;  // 假设 0 表示成功

// 训练查询响应数据结构（匹配接口返回字段）
struct TrainQueryResponse {
    int code;          // 错误码
    QString message;       // 错误描述
    int progress;          // 任务进度（百分比）
    int number;            // 0:训练中，其他：前面的任务个数
    QString model_name;         // 模型名称（progress=100 时有效）
    QString model_dir;          // 模型远程目录（/ftp/xxx/model_dir/时间戳），用于 SFTP 下载
    bool isSuccess() const { return code == RESPONSE_SUCCESS_CODE; }
};

// 训练启动响应数据结构（匹配接口返回字段）
struct TrainStartResponse {
    int code;          // 错误码
    QString message;       // 错误描述
    bool isSuccess() const { return code == RESPONSE_SUCCESS_CODE; }
};

// 标注类型枚举（最多支持 10 类，枚举值 = classId，TYPE_NONE 为特殊值）
enum class AnnotationType {
    TYPE_0 = 0,  // 绿色
    TYPE_1 = 1,  // 红色
    TYPE_2 = 2,  // 蓝色
    TYPE_3 = 3,  // 黄色
    TYPE_4 = 4,  // 橙色
    TYPE_5 = 5,  // 品红
    TYPE_6 = 6,  // 紫色
    TYPE_7 = 7,  // 青色
    TYPE_8 = 8,  // 粉色
    TYPE_9 = 9,  // 棕色
    TYPE_NONE = 99 // 未选中
};

// 标注模式枚举（内部状态，控制鼠标行为 + 按钮互斥）
enum class AnnotaionMode {
    MODE_NONE,      // 空闲（一键标注为瞬时触发，不作为一种模式）
    MODE_SELECT,    // 点击选择标注（点击前景框改类别）
    MODE_DRAW       // 拉框标注（手动画框）
};

// 标注数据结构
struct AnnotationData {
    AnnotationType type;       // 标注类型
    QRect rect;                // 矩形边框（x, y, width, height）
    QColor color;              // 边框颜色
    QString label;             // 类型名称
    int classId = 0;           // 类别 ID（从 txt 解析，和 pred cls_id 对比用）

    // 计算中心坐标
    QPointF center() const {
        return QPointF(rect.x() + rect.width()/2.0, rect.y() + rect.height()/2.0);
    }

    // 获取边框大小（宽高）
    QSize size() const {
        return rect.size();
    }
};

class ModelApi : public QObject
{
    Q_OBJECT
public:
    explicit ModelApi(QObject *parent = nullptr);
    ~ModelApi();

    void createDirTrain(const QString& taskId);


    // 1. 启动模型训练（POST /api/cloud/train/start）
    void startTrain(const QString& taskId);

    // 2. 查询训练进度（POST /api/cloud/train/query）
    void queryTrainProgress(const QString& taskId);

    // 3. 下载模型（GET /模型名称）
    void downloadModel(const QString& modelName, const QString& savePath);

    // 4. 下载模型（GET /模型名称Json）
    void downloadModelJson(const QString& modelJson, const QString& savePath);

    // 5. 查询硬件架构（GET /api/cloud/config/arch，决定下载 .bin 还是 .dlc）
    void queryArch();
    
    // 设置 HTTP 超时时间（默认 30 秒）
    void setHttpTimeout(int timeoutMs);

    // 设置训练服务器 base URL（如 "http://192.168.0.11:5000"）
    void setBaseUrl(const QString& baseUrl) { m_baseUrl = baseUrl; }
    QString baseUrl() const { return m_baseUrl; }

    // 设置模型远程目录（训练返回的 model_dir，SFTP 下载用）
    void setModelRemoteDir(const QString& dir) { m_remoteModelDir = dir; }

signals:
    void createDirTrainResult(const TrainStartResponse& response);
    // 训练启动结果信号
    void startTrainResult(const TrainStartResponse& response);
    // 训练查询结果信号
    void queryTrainResult(const TrainQueryResponse& response);
    // 模型下载进度信号（已下载字节数、总字节数）
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // 模型下载完成信号（是否成功、错误信息）
    void downloadFinished(bool success, const QString& errorMsg);

    // 模型Json下载进度信号（已下载字节数、总字节数）
    void downloadJsonProgress(qint64 bytesReceived, qint64 bytesTotal);
    // 模型Json下载完成信号（是否成功、错误信息）
    void downloadJsonFinished(bool success, const QString& errorMsg);

    // 硬件架构查询结果信号（arch: "snpe" / "x5" 等）
    void archResult(const QString& arch);

    // 网络错误信号（错误信息）
    void networkError(const QString& errorMsg);

    private slots:
        // HTTP 请求成功回调（训练启动/查询）
        void onHttpRequestSuccess(const QByteArray& responseData, int statusCode);
        // HTTP 请求失败回调
        void onHttpRequestFailed(const QString& errorMsg, int errorCode);
        // 下载进度回调
        void onHttpDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
//        // 下载完成回调（内部使用）
//        void onHttpDownloadFinished(bool success, const QString& errorMsg);
private:
    // 解析训练启动响应
    TrainStartResponse parseStartTrainResponse(const QByteArray& data);
    // 解析训练查询响应
    TrainQueryResponse parseQueryTrainResponse(const QByteArray& data);
    // 解析硬件架构查询响应（返回 arch 字符串）
    QString parseArchResponse(const QByteArray& data);

private:
    HttpTool* m_httpTool;       // HTTP 工具类
    QString m_baseUrl = "http://192.168.0.11:5000";  // 训练服务器 base URL（默认值，可动态配置）
    QString m_currentTaskId;    // 当前请求的任务 ID（用于匹配响应）
    QString m_remoteModelDir;   // 训练返回的 model_dir，SFTP 下载用
    enum class RequestType {    // 当前请求类型（用于区分响应解析）
        None,
        StartTrain,
        QueryTrain,
        DownloadModel,
        DownloadJson,
        CreateDirTrain,
        QueryArch
    };
    RequestType m_currentRequestType;  // 当前请求类型
};

// 图片扫描 Worker（纯 QObject，moveToThread 到子线程执行，避免子类化 QThread）
class ImageScanWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImageScanWorker(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    // 在子线程中同步扫描路径（不会卡 UI 线程）
    void doScan(const QString& dirPath)
    {
        qWarning() << "[FLOW]" << QDateTime::currentMSecsSinceEpoch() << "thr=" << (quintptr)QThread::currentThreadId() << "doScan 入口 dirPath=" << dirPath;
        QStringList imagePaths;
        QDir dir(dirPath);
        if (!dir.exists()) {
            emit scanFinished(imagePaths);
            return;
        }
        QDirIterator it(dir.path(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
        static const QStringList kSupported = {"png", "jpg", "jpeg", "bmp", "gif", "tiff", "webp", "svg"};
        int count = 0;
        while (it.hasNext() && !QThread::currentThread()->isInterruptionRequested()) {
            QFileInfo fi(it.next());
            if (kSupported.contains(fi.suffix().toLower())) {
                imagePaths << fi.absoluteFilePath();
                if ((++count & 0x3f) == 0)  // 每 64 张发一次进度，不刷屏
                    emit scanProgress(count);
            }
        }
        qWarning() << "[FLOW]" << QDateTime::currentMSecsSinceEpoch() << "thr=" << (quintptr)QThread::currentThreadId() << "doScan 完成 数量=" << imagePaths.size();
        emit scanFinished(imagePaths);
    }

signals:
    void scanFinished(const QStringList& imagePaths);
    void scanProgress(int count);
};


class AiModelSet : public QWidget
{
    Q_OBJECT

public:
    explicit AiModelSet(QWidget *parent = 0);
    ~AiModelSet();

    // 停止并回收上一次的扫描线程（防重复点击崩溃）
    void cleanupScanThread();

    // 核心函数：将标注转换为 YOLO 格式并保存为 .txt 文件
    //void saveYoloAnnotation(const QString& imgPath, const QVector<AnnotationData>& annotations);

    void saveMyAnnotation(const QString& imgPath);
    void LoadMyAnnotation(const QString& imgPath);

    
    void prepareTrain();
    
   // 辅助函数：classId ↔ AnnotationType（枚举值直接等于 classId）
   int classIdFromAnnotType(AnnotationType type) { return type == AnnotationType::TYPE_NONE ? -1 : static_cast<int>(type); }
   AnnotationType annotTypeFromClassId(int classId) { return (classId >= 0 && classId <= 9) ? static_cast<AnnotationType>(classId) : AnnotationType::TYPE_NONE; }

   // 颜色表（用户指定的 10 类颜色）
   static QVector<QColor> getClassColorTable();

   // 动态创建 class checkbox（数量由模型类别数决定，names 可选：json 里读的类别名）
   void setupClassCheckBoxes(int count, const QStringList& names = QStringList());

   //void saveImg(const QString& imgPath);
   //void saveClass(const QString& imgPath);
    
private:
    Ui::AiModelSet *ui;
    int modelCategoryNum;
    QString modelName;
    QString downLoadModelName;
    QString m_modelRemoteDir;        // 训练服务器上模型的远程目录（SFTP get 用）
    QString m_modelArch = "x5";     // 硬件架构（默认 x5；snpe 时下载 .dlc）
    QString m_expectedModelMd5;     // 从 JSON 里解析出来的 md5，下载完模型后校验用

    QString m_currentDir;

    QThread* m_scanThread = nullptr;    // 扫描线程（子线程，不操作 UI）
    ImageScanWorker* m_scanWorker = nullptr;  // 被 moveToThread 的 Worker
    QStringList m_allImagePaths;    // 所有图片路径

    int m_currentPage = 1;          // 当前页码
    const int m_pageSize = 10;      // 每页显示数量
    int m_totalPages = 0;           // 总页数

    int m_currentImg = 1;          // 当前图片张数
    int m_totalImg = 0;           // 总张数

    ModelApi* m_modelApi;  // 模型接口实例
    QString m_currentTaskId;  // 当前训练任务 ID

    // ── 训练状态 ──────────────────────────────────────────────────────
    bool m_trainingBusy = false;       // 训练流程中（防重入）
    QTimer* m_pollTimer = nullptr;     // 训练进度轮询定时器
    int m_pollFailCount = 0;           // 连续查询失败次数

    // SFTP（用 libssh2 的 SftpWorker，moveToThread 到子线程）
    SftpWorker* m_sftpWorker = nullptr;   // SFTP Worker 对象（子线程执行）
    QThread*    m_sftpThread = nullptr;   // SFTP 后台线程
    int  m_sftpUploadPhase = 0;           // 0=未开始 1=image完成 2=label完成 3=classes完成
    QString m_sftpLocalImgDir, m_sftpLocalLblDir, m_sftpClassesFile;
    QString m_sftpRemoteImgDir, m_sftpRemoteLblDir, m_sftpRemoteRootDir;

    // ── 训练服务器配置（可动态修改，arm 版本保存到 JSON）─────────────
    QString m_trainServerIp   = "192.168.0.11";
    int     m_trainHttpPort   = 5000;
    int     m_trainSftpPort   = 22;
    QString m_trainSftpUser   = "root";
    QString m_trainSftpPass   = "duyi";

    // ── 训练服务器 JSON 配置文件路径（arm 部署时指向 /opt/app/userdata/cnf/trainserver.json）
    QString trainServerJsonPath() const;
    // 加载训练服务器配置（arm 版本从 JSON 读，上位机/开发版用默认值或历史值）
    void loadTrainServerConfig();
    // 保存训练服务器配置（仅 arm 版本写入 JSON）
    void saveTrainServerConfig();
    // 静态：是否 arm 版本（通过宏 __arm__ / Q_PROCESSOR_ARCH 等判断）
    static bool isArmBuild();
    // 检查训练服务器是否已配置（有有效 IP+HTTP Port），未配置则弹框提示
    bool ensureTrainServerConfigured();

    // ── 训练流程回调（私有 slot，由 HTTP 信号触发） ────────────────────

    /** [0] /train/create 返回：connect → mkdir_p → moveToThread → 启动 SFTP 上传线程 */
    void onCreateDirTrainResult(const TrainStartResponse& response);
    /** [2] /train/query 返回：更新进度，完成后下载模型 */
    void onQueryTrainResult(const TrainQueryResponse& response);
    /** [3] /train/start 返回：启动 QTimer 轮询 progress */
    void onStartTrainResult(const TrainStartResponse& response);
    /** [7] SFTP 上传阶段串联：image → label → classes → startTrain */
    void onSftpUploadCompleted(bool success);
    /** 轮询定时器触发：每 5 秒打一次 /train/query */
    void onPollTrainProgress();

    QVector<AnnotationData> m_annotations;  // 所有标注数据
    AnnotationType m_currentAnnotType;      // 当前标注类型
    bool m_isDrawing;                       // 是否正在绘制
    QPoint m_drawStartPos;                  // 绘制起点
    QPoint m_drawEndPos;                    // 绘制终点
    int m_selectedAnnotIndex;               // 选中标注索引（-1 无选中）
    QVector<QVector<AnnotationData>> m_annotUndoStack;  // 标注撤销栈（最多缓存 3 个快照）
    QString m_currentImagePath;             // 当前预览图片路径
    QPixmap m_currentPixmap;                // 当前预览图片（用于缩放后绘图）

    QVector<QString> m_train_lists;  // 所有标注数据
    QSet<int> m_activate_class_ids; // 实际出现的标签

    QVector<QRect> m_fg_rects; //
    bool m_show_fg_rects = false; // 是否显示前景矩形

    // ── 前景像素过滤（areaThresholdAnnoPushButton）─────────────────────
    bool m_show_fg_pixel_count = false;      // 是否显示前景像素计数
    QVector<int> m_fg_pixel_counts;          // 与 m_fg_rects 一一对应的前景像素数
    QVector<int> m_emulate_pixel_counts;     // 与 m_emulateObjInfos 一一对应的前景像素数
    QVector<int> m_annot_pixel_counts;       // 与 m_annotations 一一对应的前景像素数
    QImage m_fgMaskQImageCached;             // 前景掩码缓存（单通道 Format_Grayscale8，paintEvent 用来只涂前景像素区域）

    // ── 仿真（AI 推理板卡） ──────────────────────────────────────────
    bool m_emulating = false;                      // 仿真状态（从 pred txt 读推理框显示）
    QVector<ObjInfo> m_emulateObjInfos;            // 仿真返回的目标列表（cls_id/x/y/w/h）
    QString m_lastEmulateModelName;                // 上次跑仿真用的模型名（用于判断是否需要清 pred 缓存）
    bool m_vsAnnoMode = false;                     // 仿真vs标注对比模式
    QStringList m_origAllImagePaths;               // vsAnno 筛选前的原始列表（用于 toggle off 恢复）

    // ── 背景均值（bg_ 开头的图片集合） ──────────────────────────────────
    // 导入图片目录时自动从所有 bg_ 图片提取，OpenCV 默认 BGR 顺序
    // 用于 getFgRects 里 absdiff 做前景背景二值化
    int m_bgMeanBGR[3] = {128, 128, 128};  // 默认中灰（B, G, R）
    bool m_hasBgMean = false;                // 是否已计算过 bg 均值
    int m_bgCount = 0;                       // bg_ 图片数量（导入时校验 >= 2）

    QVector<QCheckBox*> m_classCheckBoxes;  // 动态创建的类别 checkbox（对应 classId 下标）
    QStringList m_classNames;               // 类别名（checkbox 显示文字）
    QWidget* m_cbContainer = nullptr;       // checkbox 的 container widget

    AnnotaionMode m_anno_mode = AnnotaionMode::MODE_NONE; // 当前标注模式

    // 加载并显示指定图片（延迟到事件循环下一拍调用）
    void loadFirstImage(const QString& path);

    // 辅助：标注文件路径（和图片同名不同后缀 .txt）
    QString annotationFilePath(const QString& imgPath) const;

    // 辅助：图片在 imgLabel 中的坐标 ↔ 图片实际坐标（带缩放/居中偏移）
    QPoint widgetToImgCoord(const QPoint& widgetPos);
    QPoint imgToWidgetCoord(const QPoint& imgPos);


    // 辅助函数
    QColor getAnnotColorByType(AnnotationType type);  // 获取标注颜色
    QString getAnnotLabelByType(AnnotationType type); // 获取标注名称
    int hitTestAnnotation(const QPoint& pos);         // 检查是否点击标注
    QRect getNormalizedRect(const QPoint& start, const QPoint& end); // 规范化矩形（确保宽高为正）

    // 标注撤销：保存当前标注状态到栈（最多 3 个），并清空栈
    void pushAnnotUndo();
    void clearAnnotUndo();


    QRect m_pixmapDrawRect;  // 图片在 widget 中显示的实际区域
    double m_scaleX = 1.0;
    double m_scaleY = 1.0;

public:
    void updateWidget();
    void updateCategoryChenkBox();
    void updatePageInfo();    // 更新页码信息（如 "第1页/共5页"）
    void clearImageList();    // 清空图片列表
    void updateSelectImgInfo(); // 更新当前选中信息

    void getFgRects();
    void removeNestedBoxes(QVector<QRect>& rects);  // 移除被大框包含的小框，仅留最外层
    void showTip(const QString &msg, bool red = false);  // 提示（red=true 用红色）

public slots:
    void onSetBackBtnClicked();      // 响应返回按钮按下事件
    void onModelSelPushButtonClicked();  // 加载模型（弹出 QDialog 列 /ftp/models/*.bin）
    void onImportImgPushButtonClicked();
    void onMergeDirsClicked();            // 合并标注目录对话框（mergeImgPushButton）
    void onScanFinished(const QStringList& imagePaths);  // 扫描完成后初始化分页
    void showPage(int page);  // 显示指定页码
    void prevPage();          // 上一页
    void nextPage();          // 下一页
    void gotoPage();          // 从 m_curPagelineEdit 读取页码跳转（回车触发）
    void onImageItemClicked(QListWidgetItem* item);  // 图片项点击事件
    void onModelTrainPushButtonClicked();
    void onModelNewPushButtonClicked();  // 新建模型按钮 → 弹出模型配置对话框
    void onTrainServerCfgPushButtonClicked();  // 训练服务器配置按钮 → 弹出配置对话框（带 HTTP/SFTP 测试）

    void onShowFgRectsBtnClicked();

    void onAreaThresholdAnnoPushButtonClicked();  // 前景像素过滤按钮
    void computeFgPixelCounts();                 // 辅助：计算当前图片所有前景框/仿真框的前景像素数

    void onValidImgPushButtonClicked();  // 仿真按钮（从 pred 读或远程推理一次）
    void onBatchValidImgPushButtonClicked();  // 批量仿真按钮
    void onValidvsAnnoImgPushButtonClicked();  // 仿真vs标注按钮：对比 pred txt 和人工标注，错标用虚线框+色块标记

    /** 辅助：保存推理结果到 pred子目录 的 pred_{imgname}.txt
     *  格式：cls_id x1 y1 w h score （每行一个目标，和人工标注格式一致）
     */
    void saveEmulateResultToFile(const QString& imgPath, const QVector<ObjInfo>& objs);

    /** 辅助：读取 pred子目录 的 pred_{imgname}.txt 到 m_emulateObjInfos
     *  @return true 文件存在且读取成功
     */
    bool loadEmulateResultFromFile(const QString& imgPath, QVector<ObjInfo>& outObjs);

    /** 辅助：确保板卡上已有当前模型（.bin + .json）
     *  逻辑：stat 板卡 /ftp/model/xxx.bin → MD5 比对 → 不一致则 SFTP 上传
     *  @return true 板卡已准备好，false 失败
     */
    bool ensureBoardModelUploaded(const QString& modelName);

    /** 辅助：向板卡发一次 UDP 仿真请求，解析返回结果，更新 m_emulateObjInfos + update()
     *  不检查任何前置条件（调用者自己保证：模型已选中、已加载、板卡 MD5 已通过）
     *  @return true 成功获取并解析到结果，false 任何一步失败（内部已 showTip）
     */
    bool runEmulateOnce();

    // 四个模式按钮（互斥，setCheckable）
    void onOneKeyAnnoBtnClicked();  // oneKeyAnnoPushButton
    void onSelectAnnoBtnClicked();  // selectAnnoPushButton
    void onDrawAnnoBtnClicked();    // drawAnnoPushButton

    void onClassCheckBoxClicked();  // 通用类别 checkbox 点击
    void onClearAnnotBtnClicked();  // 清除所有标注
    void onDeleteAnnotBtnClicked(); // 删除选中标注
    void onBackLastAnnoBtnClicked(); // 退回上一步标注

    void addTrainListPushButtonPressed();
    void delTrainListPushButtonPressed();
    void addAllLabeledTrainListPushButtonPressed(); // 一键添加所有有标注文件的图片到训练集
    void refreshThumbnails();   // 刷新当前页缩略图绿框（训练集变更后调用）
    void updateImgLabelBorder(); // 根据当前图片是否在训练集，设置 imgLabel 的 QSS 绿框

    QRect processROI(const QImage& image, const QRect& roi,
                     int threshold, int minArea);


protected:
    // 重写绘图事件（绘制标注矩形）
    void paintEvent(QPaintEvent *event) override;
    // 重写鼠标按下事件（开始绘制标注）
    void mousePressEvent(QMouseEvent *event) override;
    // 重写鼠标移动事件（调整标注大小）
    void mouseMoveEvent(QMouseEvent *event) override;
    // 重写鼠标松开事件（完成标注）
    void mouseReleaseEvent(QMouseEvent *event) override;


signals:
    void backToHomePageSig();       // 返回上一级信号

};

#endif // AIMODELSET_H
