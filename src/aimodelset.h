#ifndef AIMODELSET_H
#define AIMODELSET_H


#include <QWidget>
#include <QFileDialog>
#include <QStringList>
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>
#include <QRect>
#include <QColor>
#include <QVector>
#include "globalparams.h"
#include "globalflow.h"
#include "httptool.h"
#include "aismainwidget.h"



namespace Ui {
class AiModelSet;
}

// 接口基础 URL（根据实际服务器地址修改）
const QString BASE_URL = "http://192.168.1.114:5000";

// 接口路径
const QString API_TRAIN_CREATE = "/api/cloud/train/create";   // 创建目录

const QString API_TRAIN_START = "/api/cloud/train/start";    // 训练启动
const QString API_TRAIN_QUERY = "/api/cloud/train/query";    // 训练查询
const QString API_MODEL_DOWNLOAD = "/api/cloud/download_file/";                      // 模型下载（路径拼接模型名称）

// 响应错误码（根据接口文档扩展）
const int RESPONSE_SUCCESS_CODE = 0;  // 假设 0 表示成功

// 训练查询响应数据结构（匹配接口返回字段）
struct TrainQueryResponse {
    int code;          // 错误码
    QString message;       // 错误描述
    int progress;          // 任务进度（百分比）
    int number;            // 0:训练中，其他：前面的任务个数
    QString model_name;         // 模型名称（progress=100 时有效）
    bool isSuccess() const { return code == RESPONSE_SUCCESS_CODE; }
};

// 训练启动响应数据结构（匹配接口返回字段）
struct TrainStartResponse {
    int code;          // 错误码
    QString message;       // 错误描述
    bool isSuccess() const { return code == RESPONSE_SUCCESS_CODE; }
};

// 标注类型枚举（三类）
enum class AnnotationType {
    TYPE_1,  // 第一类（红色）
    TYPE_2,  // 第二类（绿色）
    TYPE_3,   // 第三类（蓝色）
    TYPE_NONE // 未选中

};

enum class AnnotaionMode {
    Manual,
    Auto
};

// 标注数据结构
struct AnnotationData {
    AnnotationType type;       // 标注类型
    QRect rect;                // 矩形边框（x, y, width, height）
    QColor color;              // 边框颜色
    QString label;             // 类型名称

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

    // 设置 HTTP 超时时间（默认 30 秒）
    void setHttpTimeout(int timeoutMs);

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

private:
    HttpTool* m_httpTool;       // HTTP 工具类
    QString m_currentTaskId;    // 当前请求的任务 ID（用于匹配响应）
    enum class RequestType {    // 当前请求类型（用于区分响应解析）
        None,
        StartTrain,
        QueryTrain,
        DownloadModel,
        DownloadJson,
        CreateDirTrain
    };
    RequestType m_currentRequestType;  // 当前请求类型
};

// 图片扫描线程（后台扫描，避免UI卡顿）
class ImageScanThread : public QThread
{
    Q_OBJECT
public:
    explicit ImageScanThread(const QString& dirPath, QObject* parent = nullptr);
    void run() override;
    void stopScan();

signals:
    void scanFinished(const QStringList& imagePaths);  // 扫描完成信号
    void scanProgress(int count);                     // 扫描进度信号

private:
    QString m_dirPath;
    bool m_stopFlag;
    QStringList m_supportedFormats = {"png", "jpg", "jpeg", "bmp", "gif", "tiff", "webp", "svg"};
};


class AiModelSet : public QWidget
{
    Q_OBJECT

public:
    explicit AiModelSet(QWidget *parent = 0);
    ~AiModelSet();

    // 核心函数：将标注转换为 YOLO 格式并保存为 .txt 文件
    //void saveYoloAnnotation(const QString& imgPath, const QVector<AnnotationData>& annotations);

    void saveMyAnnotation(const QString& imgPath);
    void LoadMyAnnotation(const QString& imgPath);

    
    void prepareTrain();
    
   // 辅助函数：获取标注类型对应的 class_id
   int getClassIdFromAnnotType(AnnotationType type);
   AnnotationType getAnnotTypeFromClassId(int classId);

   //void saveImg(const QString& imgPath);
   //void saveClass(const QString& imgPath);
    
private:
    Ui::AiModelSet *ui;
    int modelCategoryNum;
    QString modelName;
    QString downLoadModelName;

    QString m_currentDir;

    ImageScanThread* m_scanThread;  // 扫描线程
    QStringList m_allImagePaths;    // 所有图片路径

    int m_currentPage = 1;          // 当前页码
    const int m_pageSize = 10;      // 每页显示数量
    int m_totalPages = 0;           // 总页数

    int m_currentImg = 1;          // 当前图片张数
    int m_totalImg = 0;           // 总张数

    ModelApi* m_modelApi;  // 模型接口实例
    QString m_currentTaskId;  // 当前训练任务 ID

    QVector<AnnotationData> m_annotations;  // 所有标注数据
    AnnotationType m_currentAnnotType;      // 当前标注类型
    bool m_isDrawing;                       // 是否正在绘制
    QPoint m_drawStartPos;                  // 绘制起点
    QPoint m_drawEndPos;                    // 绘制终点
    int m_selectedAnnotIndex;               // 选中标注索引（-1 无选中）
    QString m_currentImagePath;             // 当前预览图片路径
    QPixmap m_currentPixmap;                // 当前预览图片（用于缩放后绘图）

    QVector<QString> m_train_lists;  // 所有标注数据
    QSet<int> m_activate_class_ids; // 实际出现的标签

    QVector<QRect> m_fg_rects; // 
    bool m_show_fg_rects = false; // 是否显示前景矩形
    bool m_select_mode = false;


    // 辅助函数
    QColor getAnnotColorByType(AnnotationType type);  // 获取标注颜色
    QString getAnnotLabelByType(AnnotationType type); // 获取标注名称
    int hitTestAnnotation(const QPoint& pos);         // 检查是否点击标注
    QRect getNormalizedRect(const QPoint& start, const QPoint& end); // 规范化矩形（确保宽高为正）


    QRect m_pixmapDrawRect;  // 图片在 widget 中显示的实际区域
    double m_scaleX = 1.0;
    double m_scaleY = 1.0;

    AnnotaionMode m_anno_mode = AnnotaionMode::Manual;

public:
    void updateWidget();
    void updateCategoryChenkBox();
    void updatePageInfo();    // 更新页码信息（如 "第1页/共5页"）
    void clearImageList();    // 清空图片列表
    void updateSelectImgInfo(); // 更新当前选中信息

    void getFgRects();

public slots:
    void onSetBackBtnClicked();      // 响应返回按钮按下事件
    void onModelSelPushButtonClicked();
    void onOkPushButtonClicked();
    void onModelNameLineEditClicked();
    void onModelsize1CheckBoxClicked();
    void onModelsize2CheckBoxClicked();
    void onImportImgPushButtonClicked();
    void onScanFinished(const QStringList& imagePaths);  // 扫描完成后初始化分页
    void showPage(int page);  // 显示指定页码
    void prevPage();          // 上一页
    void nextPage();          // 下一页
    void onImageItemClicked(QListWidgetItem* item);  // 图片项点击事件
    void onModelTrainPushButtonClicked();

    void onShowFgRectsBtnClicked();
    void onSelectModeBtnClicked();

    void onType1BtnClicked();    // 切换到第一类标注
    void onType2BtnClicked();    // 切换到第二类标注
    void onType3BtnClicked();    // 切换到第三类标注
    void onClearAnnotBtnClicked();// 清除所有标注
    void onDeleteAnnotBtnClicked();// 删除选中标注

    void onManualAnnoBtnClicked();
    void onAutoAnnoBtnClicked();

    void addTrainListPushButtonPressed();
    void delTrainListPushButtonPressed();

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
