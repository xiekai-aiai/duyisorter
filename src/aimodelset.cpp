#include <opencv2/opencv.hpp>

#include "aimodelset.h"
#include "ui_aimodelset.h"
#include <queue>

//#include <opencv2/opencv.hpp>


AiModelSet::AiModelSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AiModelSet)
{
    ui->setupUi(this);
    modelCategoryNum = 2;
    ui->modeInfowidget->setWindowModality(Qt::ApplicationModal);
    ui->modeInfowidget->setWindowFlags(Qt::FramelessWindowHint);
    QPalette palette;
    QPixmap pixmap1(":/res/png/messageBack.png");
    pixmap1 = pixmap1.scaled(700,431);

    palette.setBrush(backgroundRole(),QBrush(pixmap1));
    ui->modeInfowidget->setPalette(palette);
    ui->modeInfowidget->setAutoFillBackground(true);
    ui->modelsize1CheckBox->setChecked(true);
    ui->modelsize2CheckBox->setChecked(false);

//    ui->m_imageList = new QListWidget(this);
     // 1. 基础配置（图标模式、缩略图大小、间距）
    ui->m_imageList->setViewMode(QListWidget::IconMode);
    ui->m_imageList->setIconSize(QSize(160, 20));  // 图片缩略图大小（可根据需求调整）
    ui->m_imageList->setSpacing(15);                // 图片间距（可调整）
//    ui->m_imageList->setLayoutMode(QListWidget::Grid);  // 强制网格布局

    // 2. 隐藏滚动条（2行5列刚好显示10张，无需滚动）
    ui->m_imageList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏垂直滚动条
    ui->m_imageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条

    // 3. 关键：设置网格大小（每个图片项的固定尺寸）
    // 宽度 = 缩略图宽度 + 间距（确保每个项宽度一致）
    // 高度 = 缩略图高度 + 30（30px 预留文件名显示空间）
    QSize gridSize(
        ui->m_imageList->iconSize().width() + ui->m_imageList->spacing(),
        ui->m_imageList->iconSize().height() + 30
    );
    ui->m_imageList->setGridSize(gridSize);  // 核心：通过网格大小统一项尺寸

    // 4. 固定列表宽高（强制 2行5列）
    int colCount = 5;  // 5列
    int rowCount = 2;  // 2行
    // 列表宽度 = 5列 × 网格宽度（一行刚好放5个，第6个自动换行）
    ui->m_imageList->setMinimumWidth(colCount * gridSize.width()+40);
    ui->m_imageList->setMaximumWidth(colCount * gridSize.width()+40);
    // 列表高度 = 2行 × 网格高度（刚好显示2行，不超出）
    ui->m_imageList->setMinimumHeight(rowCount * gridSize.height() + 10);  // +10 上下边距
    ui->m_imageList->setMaximumHeight(rowCount * gridSize.height() + 10);

    // 5. 水平居中（优化显示效果，可选）
//    ui->m_imageList->setA(Qt::AlignHCenter);

    // 初始化模型接口
    m_modelApi = new ModelApi(this);
    m_modelApi->setHttpTimeout(60000);  // 训练相关接口超时设为 60 秒

    connect(ui->backPushButton, SIGNAL(pressed()), this, SLOT(onSetBackBtnClicked()));
    connect(ui->modelSelPushButton, SIGNAL(pressed()), this, SLOT(onModelSelPushButtonClicked()));
    connect(ui->okPushButton, SIGNAL(pressed()), this, SLOT(onOkPushButtonClicked()));
    connect(ui->modelNameLineEdit, SIGNAL(selectionChanged()), this, SLOT(onModelNameLineEditClicked()));
    connect(ui->modelsize1CheckBox, SIGNAL(clicked()), this, SLOT(onModelsize1CheckBoxClicked()));
    connect(ui->modelsize2CheckBox, SIGNAL(clicked()), this, SLOT(onModelsize2CheckBoxClicked()));
    connect(ui->importImgPushButton, SIGNAL(clicked()), this, SLOT(onImportImgPushButtonClicked()));
    connect(ui->m_prevBtn, SIGNAL(clicked()), this, SLOT(prevPage()));
    connect(ui->m_nextBtn, SIGNAL(clicked()), this, SLOT(nextPage()));

    connect(ui->modelTrainPushButton_2, SIGNAL(clicked()), this, SLOT(onManualAnnoBtnClicked()));
    connect(ui->modelTrainPushButton_3, SIGNAL(clicked()), this, SLOT(onAutoAnnoBtnClicked()));

    connect(ui->fgPushButton, SIGNAL(clicked()), this, SLOT(onShowFgRectsBtnClicked()));
    connect(ui->selectPushButton, SIGNAL(clicked()), this, SLOT(onSelectModeBtnClicked()));


    // 新增：绑定图片项点击信号（点击后触发预览）
    connect(ui->m_imageList, &QListWidget::itemClicked, this, &AiModelSet::onImageItemClicked);
    connect(ui->modelTrainPushButton, SIGNAL(pressed()), this, SLOT(onModelTrainPushButtonClicked()));

    connect(ui->addTrainListPushButton, SIGNAL(pressed()), this, SLOT(addTrainListPushButtonPressed()));
    connect(ui->delTrainListPushButton, SIGNAL(pressed()), this, SLOT(delTrainListPushButtonPressed()));

    m_currentTaskId=QString("123-2-20251129113124");
    modelName = m_currentTaskId;

    // 0. 创建训练目录信号
    connect(m_modelApi, &ModelApi::createDirTrainResult, this, [this](const TrainStartResponse& response) {
       if (response.isSuccess()) {
           ui->tipLabel->setText(QString("创建训练目录成功！任务ID：%1").arg(m_currentTaskId));
           m_modelApi->startTrain(m_currentTaskId);
       } else {
           ui->tipLabel->setText(QString("创建训练目录失败：%1（错误码：%2）").arg(response.message).arg(response.code));
       }
    });

// 1. 绑定训练启动结果信号
   connect(m_modelApi, &ModelApi::startTrainResult, this, [this](const TrainStartResponse& response) {
       if (response.isSuccess()) {
           ui->tipLabel->setText(QString("训练任务启动成功！任务ID：%1").arg(m_currentTaskId));
           // 启动成功后，定期查询进度（例如每 5 秒查询一次）
           QTimer::singleShot(5000, this, [this]() {
               m_modelApi->queryTrainProgress(m_currentTaskId);
           });
       } else {
           ui->tipLabel->setText(QString("训练任务启动失败：%1（错误码：%2）").arg(response.message).arg(response.code));
       }
   });

   // 2. 绑定训练查询结果信号
   connect(m_modelApi, &ModelApi::queryTrainResult, this, [this](const TrainQueryResponse& response) {
       if (!response.isSuccess()) {
           ui->tipLabel->setText(QString("进度查询失败：%1（错误码：%2）").arg(response.message).arg(response.code));
           // 重试查询（最多重试 3 次）
           static int retryCount = 0;
           if (retryCount < 3) {
               QTimer::singleShot(3000, this, [this]() {
                   m_modelApi->queryTrainProgress(m_currentTaskId);
               });
               retryCount++;
           }
           return;
       }

       // 查询成功，更新进度
       ui->tipLabel->setText(QString("训练进度：%1% | 状态：%2").arg(response.progress).arg(
           response.number == 0 ? "训练中" : QString("排队中（前面还有%1个任务）").arg(response.number)
       ));

       // 进度 100% 时，自动下载模型
       if (response.progress == 100 && !response.model_name.isEmpty()) {

           ui->tipLabel->setText(QString("训练完成！开始下载模型：%1").arg(response.model_name));
           // 下载模型（保存到本地路径）
           QString savePath = QString(LOCAL_MODEL_PATH)+QString("%1").arg(response.model_name);
           m_modelApi->downloadModel(response.model_name, savePath);
           QStringList parts = response.model_name.split('.');
           if (!parts.isEmpty()) { // 检查是否至少有一个元素，以防万一文件名中没有点号或为空字符串
               downLoadModelName = parts[0]; // 获取去掉扩展名后的文件名
           }
       } else if (response.progress < 100) {
           // 未完成，继续查询（每 5 秒一次）
           QTimer::singleShot(5000, this, [this]() {
               m_modelApi->queryTrainProgress(m_currentTaskId);
           });
       }
   });

   // 3. 绑定下载进度信号
   connect(m_modelApi, &ModelApi::downloadProgress, this, [this](qint64 received, qint64 total) {
       if (total <= 0) return;
       int progress = (received * 100) / total;
       ui->tipLabel->setText(QString("模型下载中：%1%").arg(progress));
   });

   // 4. 绑定下载完成信号
   connect(m_modelApi, &ModelApi::downloadFinished, this, [this](bool success, const QString& errorMsg) {
       if (success) {
           ui->tipLabel->setText("模型下载完成！");
           //开始下载modelJson
           QString savePath = QString(LOCAL_MODEL_PATH)+QString("%1%2").arg(downLoadModelName).arg(QString(".json"));
           m_modelApi->downloadModelJson(downLoadModelName, savePath);
           ui->tipLabel->setText(QString("模型下载完成！开始下载模型Json文件：%1%2").arg(downLoadModelName).arg(QString(".json")));
       } else {
           ui->tipLabel->setText(QString("模型下载失败：%1").arg(errorMsg));
       }
   });

   // 5. 绑定网络错误信号
   connect(m_modelApi, &ModelApi::networkError, this, [this](const QString& errorMsg) {
       ui->tipLabel->setText(QString("网络错误：%1").arg(errorMsg));
   });

   // 6. 绑定Json下载进度信号
   connect(m_modelApi, &ModelApi::downloadJsonProgress, this, [this](qint64 received, qint64 total) {
       if (total <= 0) return;
       int progress = (received * 100) / total;
       ui->tipLabel->setText(QString("模型Json下载中：%1%").arg(progress));
   });

   // 7. 绑定Json下载完成信号
   connect(m_modelApi, &ModelApi::downloadJsonFinished, this, [this](bool success, const QString& errorMsg) {
       if (success) {
           ui->tipLabel->setText("模型Json下载完成！");
       } else {
           ui->tipLabel->setText(QString("模型下载失败：%1").arg(errorMsg));
       }
   });


   m_currentAnnotType = AnnotationType::TYPE_NONE;
   m_isDrawing = false;
   m_selectedAnnotIndex = -1;

   // 绑定标注操作按钮（UI 设计器中添加的按钮）
    connect(ui->a_checkBox, &QCheckBox::clicked, this, &AiModelSet::onType1BtnClicked);
    connect(ui->b_checkBox, &QCheckBox::clicked, this, &AiModelSet::onType2BtnClicked);
    connect(ui->c_checkBox, &QCheckBox::clicked, this, &AiModelSet::onType3BtnClicked);
    // connect(ui->clearAnnotBtn, &QPushButton::clicked, this, &AiModelSet::onClearAnnotBtnClicked);
    connect(ui->erasePushButton, &QPushButton::clicked, this, &AiModelSet::onDeleteAnnotBtnClicked);

}


// 切换到第一类标注（红色）
void AiModelSet::onType1BtnClicked()
{
    if(ui->a_checkBox->isChecked()){
        m_currentAnnotType = AnnotationType::TYPE_1;
        ui->tipLabel->setText("当前标注类型：类型1（红色）");
        ui->b_checkBox->setChecked(false);
        ui->c_checkBox->setChecked(false);
    }else{
        ui->tipLabel->setText("取消标注类型：类型1（红色）");
        ui->a_checkBox->setChecked(false);
        m_currentAnnotType = AnnotationType::TYPE_NONE;
    }
}

// 切换到第二类标注（绿色）
void AiModelSet::onType2BtnClicked()
{
    if(ui->b_checkBox->isChecked()){
        m_currentAnnotType = AnnotationType::TYPE_2;
        ui->tipLabel->setText("当前标注类型：类型2（绿色)");
        ui->a_checkBox->setChecked(false);
        ui->c_checkBox->setChecked(false);
    }else{
        ui->tipLabel->setText("取消标注类型：类型2（绿色)");
        ui->b_checkBox->setChecked(false);
        m_currentAnnotType = AnnotationType::TYPE_NONE;
    }

}

// 切换到第三类标注（蓝色）
void AiModelSet::onType3BtnClicked()
{
    if(ui->c_checkBox->isChecked()){
        m_currentAnnotType = AnnotationType::TYPE_3;
        ui->tipLabel->setText("当前标注类型：类型3（蓝色）");
        ui->a_checkBox->setChecked(false);
        ui->b_checkBox->setChecked(false);
    }else{
        ui->tipLabel->setText("取消标注类型：类型3（蓝色）");
        ui->c_checkBox->setChecked(false);
        m_currentAnnotType = AnnotationType::TYPE_NONE;
    }
}

// 清除所有标注
void AiModelSet::onClearAnnotBtnClicked()
{
    m_annotations.clear();
    m_selectedAnnotIndex = -1;
    ui->annotationInfoEdit->clear();
    update(); // 刷新绘图
}

// Manual Anno
void AiModelSet::onManualAnnoBtnClicked()
{
    m_anno_mode = AnnotaionMode::Manual;
}
void AiModelSet::onAutoAnnoBtnClicked()
{
    m_anno_mode = AnnotaionMode::Auto;
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
        m_annotations.removeAt(m_selectedAnnotIndex);
        m_selectedAnnotIndex = -1;
        ui->annotationInfoEdit->clear();
        update();
    } else {
        ui->tipLabel->setText("请先选中标注");
    }
}


// 根据类型获取标注颜色
QColor AiModelSet::getAnnotColorByType(AnnotationType type)
{
    switch (type) 
    {
        case AnnotationType::TYPE_1: return Qt::red;    // 第一类：红色
        case AnnotationType::TYPE_2: return Qt::green;  // 第二类：绿色
        case AnnotationType::TYPE_3: return Qt::blue;   // 第三类：蓝色
        default: return Qt::black;
    }
}

// 根据类型获取标注名称
QString AiModelSet::getAnnotLabelByType(AnnotationType type)
{
    switch (type) 
    {
        case AnnotationType::TYPE_1: return "类型1";
        case AnnotationType::TYPE_2: return "类型2";
        case AnnotationType::TYPE_3: return "类型3";
        default: return "未知类型";
    }
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
    // 遍历所有标注（存储的是图片内坐标）
    for (int i = 0; i < m_annotations.size(); ++i) {
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
    
    if (m_select_mode && m_show_fg_rects && !m_fg_rects.isEmpty())
    {
        if (m_currentAnnotType == AnnotationType::TYPE_NONE) {
            ui->tipLabel->setText("请先选择标注类型");
            return;
        }
    
        // 1️⃣ 父窗口坐标 → QLabel
        QPoint labelPos = ui->imgLabel->mapFrom(this, event->pos());
    
        // 2️⃣ QLabel 坐标 → 图片坐标
        QPixmap pix = *ui->imgLabel->pixmap();
        int offsetX = (ui->imgLabel->width()  - pix.width())  / 2;
        int offsetY = (ui->imgLabel->height() - pix.height()) / 2;
        QPoint imgPos = labelPos - QPoint(offsetX, offsetY);
    
        int select_idx = -1;
    
        for (int i = 0; i < m_fg_rects.size(); ++i)
        {
            QRect expandRect = m_fg_rects[i].adjusted(-10, -10, 10, 10);
            if (expandRect.contains(imgPos)) {
                select_idx = i;
                break;
            }
        }
    
        if (select_idx == -1) {
            ui->tipLabel->setText("请选择一个前景区域");
            return;
        }
    
        AnnotationData newAnnot;
        newAnnot.type  = m_currentAnnotType;
        newAnnot.rect  = m_fg_rects[select_idx];   // 图片坐标 ✅
        newAnnot.color = getAnnotColorByType(m_currentAnnotType);
        newAnnot.label = getAnnotLabelByType(m_currentAnnotType);
    
        m_annotations.append(newAnnot);
    }


    std::cout<<"Begin to draw"<<std::endl;
    // if (event->button() != Qt::LeftButton) return;
    if (m_currentPixmap.isNull()) { // 无预览图片时不允许标注
        ui->tipLabel->setText("请先选择图片");
        return;
    }

    if(m_currentAnnotType == AnnotationType::TYPE_NONE){
        ui->tipLabel->setText("请先选择标注类型");
        return;
    }

    // 1. 鼠标坐标 → imgLabel 控件局部坐标（相对于 imgLabel 左上角）
    QPoint labelPos = ui->imgLabel->mapFromParent(event->pos());

    //qDebug() << "labelPos:" << labelPos;

    // 2. imgLabel 控件坐标 → 图片实际显示区域的坐标（关键！处理图片居中偏移）
    QPixmap scaledPixmap = ui->imgLabel->pixmap()->copy();
    if (scaledPixmap.isNull()) return;

    // 计算图片在 imgLabel 中的居中偏移（图片保持比例缩放后，左右/上下空白区的偏移）
    int offsetX = (ui->imgLabel->width() - scaledPixmap.width()) / 2;
    int offsetY = (ui->imgLabel->height() - scaledPixmap.height()) / 2;

    // 3. 过滤图片区域外的点击（只允许在图片内绘制）
    QRect imageDisplayRect(offsetX, offsetY, scaledPixmap.width(), scaledPixmap.height());
    //qDebug() << "imageDisplayRect:" << imageDisplayRect;

    if (!imageDisplayRect.contains(labelPos)) {
        ui->tipLabel->setText("请在图片内绘制标注");
        return;
    }

    // 4. 最终坐标：图片内的相对坐标（相对于图片左上角，而非 imgLabel 控件）
    QPoint imageRelativePos = labelPos - QPoint(offsetX, offsetY);

    // --- 后续逻辑用 imageRelativePos 替代原来的 labelPos ---
    // 优先检测标注选中
    m_selectedAnnotIndex = hitTestAnnotation(imageRelativePos); // 注意：hitTestAnnotation 也要修改坐标
    if (m_selectedAnnotIndex != -1) {
        const AnnotationData& annot = m_annotations[m_selectedAnnotIndex];
        QString info = QString("选中标注：类型：%1;边框：x=%2, y=%3, 宽=%4, 高=%5; 中心：x=%6, y=%7")
            .arg(annot.label)
            .arg(annot.rect.x()).arg(annot.rect.y()).arg(annot.rect.width()).arg(annot.rect.height())
            .arg(annot.center().x()).arg(annot.center().y());
        ui->annotationInfoEdit->setText(info);
        update();
        return;
    }

    // 开始绘制新标注（记录图片内的起点坐标）
    m_isDrawing = true;
    m_drawStartPos = imageRelativePos; // 关键：存储图片内的相对坐标
    m_drawEndPos = m_drawStartPos;
}

// 鼠标移动：调整标注大小
void AiModelSet::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isDrawing) return;
    std::cout<<"Begin to draw move"<<std::endl;
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
//    if (!m_isDrawing || event->button() != Qt::LeftButton) return;
    if (!m_isDrawing) return;
    QImage image = ui->imgLabel->pixmap()->toImage();

    m_isDrawing = false;
    QRect newAnnotRect = getNormalizedRect(m_drawStartPos, m_drawEndPos);

    if(m_anno_mode == AnnotaionMode::Auto)
    {
        qDebug() << "Auto Anno";
        QRect roi(m_drawStartPos, m_drawEndPos);
        newAnnotRect = processROI(image, roi, 80, 50);
        qDebug()<<"roi:"<<roi<<"newAnnotRect:"<<newAnnotRect;
    }

   // 过滤过小矩形
   if (newAnnotRect.width() < 10 || newAnnotRect.height() < 10) {
       ui->tipLabel->setText("标注矩形过小，请重新绘制");
       return;
   }

   // 创建标注（坐标是图片内相对坐标）
   AnnotationData newAnnot;
   newAnnot.type = m_currentAnnotType;
   newAnnot.rect = newAnnotRect; // 关键：存储的是图片内的坐标，而非控件坐标
   newAnnot.color = getAnnotColorByType(m_currentAnnotType);
   newAnnot.label = getAnnotLabelByType(m_currentAnnotType);

   m_annotations.append(newAnnot);
   m_activate_class_ids.insert(getClassIdFromAnnotType(newAnnot.type)); // 更新实际出现的标签
   QString info = QString("标注完成：类型：%1; 边框：x=%2, y=%3, 宽=%4, 高=%5; 中心：x=%6, y=%7")
       .arg(newAnnot.label)
       .arg(newAnnot.rect.x()).arg(newAnnot.rect.y()).arg(newAnnot.rect.width()).arg(newAnnot.rect.height())
       .arg(newAnnot.center().x()).arg(newAnnot.center().y());
   ui->annotationInfoEdit->setText(info);

   //打印所有标注
//    for (const AnnotationData& annot : m_annotations) {
//        qDebug() << "Annotation:" << annot.label << "Rect:" << annot.rect;
//    }


   update();
}

void AiModelSet::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event); // 必须调用父类方法，确保图片正常显示

    // 无需绘制：标注已永久画在 m_annotatedPixmap 上，QLabel 会自动显示
    // 1. 绘制正在创建的临时标注（拖拽过程中）
    QPixmap tmpPixmap = m_currentPixmap.copy(); // 复制原图
    if (m_isDrawing && !ui->imgLabel->pixmap()->isNull()) {
        
        QPainter painter(&tmpPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
    
        QRect drawRect = getNormalizedRect(m_drawStartPos, m_drawEndPos);
    
        QPen pen(getAnnotColorByType(m_currentAnnotType), 2, Qt::DashLine);
        painter.setPen(pen);
        painter.drawRect(drawRect);
        
        painter.setPen(Qt::black);
        painter.drawText(drawRect.topLeft() + QPoint(5, 15), "绘制中...");
    
        ui->imgLabel->setPixmap(tmpPixmap); // 更新 QLabel
    }

    if (m_train_lists.contains(m_currentImagePath)) {

        QPainter painter(&tmpPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        // 绿色粗框
        QPen pen(Qt::green, 2);
        painter.setPen(pen);
        // 绘制整个图像外边框
        painter.drawRect(tmpPixmap.rect().adjusted(1, 1, -1, -1));
    }

    // 2. 绘制所有标注
    //qDebug() << "Annotation 数量" << m_annotations.size();
    if(ui->imgLabel->pixmap())
    {
        QPainter painter(&tmpPixmap);
        for (const AnnotationData& annot : m_annotations) {
            QPen pen(getAnnotColorByType(annot.type), 2, Qt::DashLine);
            painter.setPen(pen);
            painter.drawRect(annot.rect);
        }
        ui->imgLabel->setPixmap(tmpPixmap); // 更新 QLabel
        
    }

    if (m_selectedAnnotIndex != -1 && !ui->imgLabel->pixmap()->isNull()) {
        
        QPainter painter(&tmpPixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        const AnnotationData& annot = m_annotations[m_selectedAnnotIndex];
        QPixmap scaledPixmap = ui->imgLabel->pixmap()->copy();
        int offsetX = (ui->imgLabel->width() - scaledPixmap.width()) / 2;
        int offsetY = (ui->imgLabel->height() - scaledPixmap.height()) / 2;

        // 转换选中标注坐标
        QPoint labelTopLeft = annot.rect.topLeft() + QPoint(offsetX, offsetY);
        QPoint labelBottomRight = annot.rect.bottomRight() + QPoint(offsetX, offsetY);
        //QPoint parentTopLeft = ui->imgLabel->mapToParent(labelTopLeft);
        //QPoint parentBottomRight = ui->imgLabel->mapToParent(labelBottomRight);
        QRect parentRect(labelTopLeft, labelBottomRight);

        // 选中样式（加粗+不同颜色，突出显示）
        QPen pen(Qt::yellow, 4); // 黄色加粗边框
        painter.setPen(pen);
        painter.drawRect(parentRect);

        // 选中提示文字
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(labelTopLeft + QPoint(5, 30), "已选中");
    }

    if(m_show_fg_rects && !ui->imgLabel->pixmap()->isNull())
    {
        QPainter painter(&tmpPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
    
        QPen pen(Qt::gray);
        pen.setWidth(1);
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        
        for (const QRect &rc : m_fg_rects)
        {
            painter.drawRect(rc);
        }
    
        ui->imgLabel->setPixmap(tmpPixmap);
    }

}






// 辅助函数：标注类型 → class_id（根据你的实际类型映射调整）
int AiModelSet::getClassIdFromAnnotType(AnnotationType type)
{
    switch (type) {
        case AnnotationType::TYPE_1: return 0;
        case AnnotationType::TYPE_2: return 1;
        case AnnotationType::TYPE_3: return 2;
        default: return 0; // 默认 class_id=0
    }
}

AnnotationType AiModelSet::getAnnotTypeFromClassId(int classId)
{
    switch (classId) {
        case 0: return AnnotationType::TYPE_1;
        case 1: return AnnotationType::TYPE_2;
        case 2: return AnnotationType::TYPE_3;
        default: return AnnotationType::TYPE_NONE;
    }
}

void AiModelSet::getFgRects()
{
    QImage image = ui->imgLabel->pixmap()->toImage()
    .convertToFormat(QImage::Format_BGR888);

    cv::Mat img(image.height(), image.width(), CV_8UC3,
    image.bits(), image.bytesPerLine());
    img = img.clone();   // ★ 关键

    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
    float wh_ratio_thresh = 4.0;
    // 蓝色背景
    cv::Scalar lower_blue(90, 50, 50);
    cv::Scalar upper_blue(130, 255, 255);

    cv::Mat mask_bg, mask_fg;
    cv::inRange(hsv, lower_blue, upper_blue, mask_bg);
    cv::bitwise_not(mask_bg, mask_fg);

    // Canny
    cv::Mat edges;
    cv::Canny(mask_fg, edges, 50, 150);
    cv::imwrite("mask.jpg", mask_fg);

    cv::Mat mask_combined;
    cv::bitwise_or(mask_fg, edges, mask_combined);

    // 轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask_combined, contours,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 初步框
    int min_area = 50;
    int max_area = 2000;
    std::vector<cv::Rect> rects_pixel;
    m_fg_rects.clear();
    for (auto& cnt : contours) {
        double area = cv::contourArea(cnt);
        if (area < min_area || area > max_area)
            continue;

        cv::Rect r = cv::boundingRect(cnt);
        float ratio = std::max((float)r.width / r.height,
                               (float)r.height / r.width);
        if (ratio > wh_ratio_thresh)
            continue;
        
        m_fg_rects.append(QRect(r.x, r.y, r.width, r.height));
        qDebug() << "Rect:" << r.x<<" " << r.y<<" " << r.width<<" " << r.height;
    }

    //qDebug() << "m_fg_rects:" << m_fg_rects;
    

}

// void AiModelSet::saveImg(const QString& imgPath){
//     QFileInfo imgFileInfo(imgPath);
//     QDir targetDir(imgFileInfo.absolutePath()+"/"+modelName+"/image/");
//     if (!targetDir.exists()) {
//         // 尝试创建目录（mkpath 支持创建多级目录，如 "D:/a/b/c"）
//         bool createSuccess = targetDir.mkpath(".");
//         if (createSuccess) {
//             qInfo() << "目录创建成功：" << targetDir.path();
//         } else {
//             qInfo() << "目录创建失败：" << targetDir.path();
//             return;
//         }
//     }

//     QString imgFileName = imgFileInfo.baseName(); // 示例：H:/images/xxx.jpg → "xxx"
//     QString imgSaveFileName = imgFileName + ".jpg"; // 文件名与图片相同："xxx.txt"
//     QString imgSavePath = targetDir.filePath(imgSaveFileName); // 拼接路径：H:/labels/xxx.txt


//     const QPixmap* annotatedPixmap = ui->imgLabel->pixmap();
//     // 保存为 JPG（质量 95，0-100 可调），若需透明背景可改为 PNG 格式
//     bool saveSuccess = annotatedPixmap->save(imgSavePath, "JPG", 95);

//     if (saveSuccess) {
//       ui->tipLabel->setText(QString("标注图片保存成功：%1").arg(imgSavePath));
//     } else {
//       ui->tipLabel->setText(QString("保存标注图片失败：无法写入文件 %1").arg(imgSavePath));
//     }
// }


// void AiModelSet::saveClass(const QString& imgPath){
//     QFileInfo imgFileInfo(imgPath);
//     QDir targetDir(imgFileInfo.absolutePath()+"/"+modelName+"/");
//     if (!targetDir.exists()) {
//         // 尝试创建目录（mkpath 支持创建多级目录，如 "D:/a/b/c"）
//         bool createSuccess = targetDir.mkpath(".");
//         if (createSuccess) {
//             qInfo() << "目录创建成功：" << targetDir.path();
//         } else {
//             qInfo() << "目录创建失败：" << targetDir.path();
//             return;
//         }
//     }
//     QString txtPath = targetDir.filePath("classes.txt"); // 拼接路径：H:/labels/xxx.txt
//     QFile txtFile(txtPath);
//     if (!txtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
//         ui->tipLabel->setText(QString("保存标注失败：无法打开文件 %1").arg(txtPath));
//         return;
//     }

//     QTextStream out(&txtFile);
//     for(int i=0; i<modelCategoryNum; i++){
//         out << i << "\n";
//     }
//     txtFile.close();
//     ui->tipLabel->setText(QString("classes.txt已保存：%1").arg(txtPath));
// }



void AiModelSet::addTrainListPushButtonPressed(){
    m_train_lists.append(m_currentImagePath);
}

void AiModelSet::delTrainListPushButtonPressed(){

    //从m_train_lists中删除m_currentImagePath
    m_train_lists.removeAll(m_currentImagePath);
}


AiModelSet::~AiModelSet()
{
    delete ui;
}

void AiModelSet::updateWidget(){
    ui->modelNameLineEdit->setText(modelName);
    updateCategoryChenkBox();
    ui->modeInfowidget->hide();
}

void AiModelSet::updateCategoryChenkBox(){
    switch(modelCategoryNum){
    case 2:
        ui->a_checkBox->setHidden(false);
        ui->b_checkBox->setHidden(false);
        ui->c_checkBox->setHidden(true);
        break;
    case 3:
        ui->a_checkBox->setHidden(false);
        ui->b_checkBox->setHidden(false);
        ui->c_checkBox->setHidden(false);
        break;
    default:
        ui->a_checkBox->setHidden(false);
        ui->b_checkBox->setHidden(false);
        ui->c_checkBox->setHidden(true);
        break;
    }
}

void AiModelSet::onSetBackBtnClicked(){
    emit backToHomePageSig();
}

void AiModelSet::onModelSelPushButtonClicked(){
    ui->modeInfowidget->show();
}
void AiModelSet::onOkPushButtonClicked(){
    QString str =ui->modelNameLineEdit->toPlainText();
    if(str.isNull() || str.isEmpty() ){
        QMessageBox::critical(this,"模型名称校验","模型名称为空");
        return;
    }
    modelName =QString("%1-%2-%3").arg(ui->modelNameLineEdit->toPlainText()).arg(QString::number(modelCategoryNum)).arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
    ui->modelNameLabel->setText(modelName);
    updateCategoryChenkBox();
    ui->modeInfowidget->hide();
}



void AiModelSet::onModelNameLineEditClicked(){
    QString str =ui->modelNameLineEdit->toPlainText();
    myInputMethod *input = new myInputMethod(myLan.input, str);
    if (input->exec() == QDialog::Accepted) {
        str = input->getText();
        ui->modelNameLineEdit->setText(str);
    }
}

void AiModelSet::onModelsize1CheckBoxClicked(){
    if(ui->modelsize1CheckBox->isChecked()){
        ui->modelsize2CheckBox->setChecked(false);
        modelCategoryNum = 2;
    }else{
        ui->modelsize2CheckBox->setChecked(true);
        modelCategoryNum = 3;
    }
}

void AiModelSet::onModelsize2CheckBoxClicked(){
    if(ui->modelsize2CheckBox->isChecked()){
        ui->modelsize1CheckBox->setChecked(false);
        modelCategoryNum = 3;
    }else{
        ui->modelsize1CheckBox->setChecked(true);
        modelCategoryNum = 2;
    }
}

void AiModelSet::onImportImgPushButtonClicked(){
    // 打开文件夹选择对话框
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        "选择图片文件夹",
        LOCAL_IMG_PATH,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    if (dirPath.isEmpty()) return;
    m_currentDir = dirPath;
    m_scanThread = new ImageScanThread(dirPath, this);
    // 停止当前扫描（如果正在进行）
   if (m_scanThread && m_scanThread->isRunning()) {
       m_scanThread->stopScan();
       m_scanThread->wait();
       delete m_scanThread;
   }

   connect(m_scanThread, &ImageScanThread::scanFinished, this, &AiModelSet::onScanFinished,
           Qt::QueuedConnection);
   connect(m_scanThread, &ImageScanThread::scanProgress, this,[this](int count) {
       ui->tipLabel->setText(QString("扫描中：已找到 %1 张图片").arg(count));
   },
   Qt::QueuedConnection);
   m_scanThread->start();

}

void AiModelSet::onScanFinished(const QStringList& imagePaths)
{

    m_allImagePaths = imagePaths;
    m_currentPage = 1;  // 重置为第1页

    // 计算总页数
    m_totalPages = (m_allImagePaths.size() + m_pageSize - 1) / m_pageSize;

    // 更新状态和按钮状态
    if (m_allImagePaths.isEmpty()) {
        ui->tipLabel->setText("未找到支持的图片文件");

        ui->m_prevBtn->setEnabled(false);
        ui->m_nextBtn->setEnabled(false);
        clearImageList();
    } else {
        ui->tipLabel->setText(QString("共找到 %1 张图片").arg(m_allImagePaths.size()));
        ui->m_prevBtn->setEnabled(false);  // 第1页禁用上一页
        ui->m_nextBtn->setEnabled(m_totalPages > 1);  // 超过1页才启用下一页
        showPage(m_currentPage);  // 显示第1页
    }

    updatePageInfo();  // 更新页码信息
}

void AiModelSet::showPage(int page)
{
    if (m_allImagePaths.isEmpty() || page < 1 || page > m_totalPages) return;

    clearImageList();  // 清空当前列表

    // 计算当前页的图片索引范围
    int startIdx = (page - 1) * m_pageSize;
    int endIdx = qMin(startIdx + m_pageSize, m_allImagePaths.size());

    // 添加当前页的图片
    for (int i = startIdx; i < endIdx; ++i) {
        QString imagePath = m_allImagePaths[i];
        QFileInfo fileInfo(imagePath);

        // 创建列表项
        QListWidgetItem* item = new QListWidgetItem(ui->m_imageList);
        item->setText(fileInfo.fileName());  // 显示文件名
        item->setData(Qt::UserRole, imagePath);  // 存储完整路径

        // 加载缩略图
        QImage image(imagePath);
        if (!image.isNull()) {
            QPixmap pixmap = QPixmap::fromImage(
                image.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
            item->setIcon(QIcon(pixmap));
        } else {
            item->setText("图片加载失败");
        }
    }
    //默认选中第一个
    ui->m_imageList->setCurrentRow(0);
    m_currentImg = 1;
    m_currentPage = page;

    updatePageInfo();
    updateSelectImgInfo();


    // 更新按钮状态
    ui->m_prevBtn->setEnabled(m_currentPage > 1);
    ui->m_nextBtn->setEnabled(m_currentPage < m_totalPages);
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

void AiModelSet::saveMyAnnotation(const QString& imgPath)
{
    qDebug()<<"m_currentImagePath "<<imgPath;
    QFileInfo imgFileInfo(imgPath);
    QString imgFileName = imgFileInfo.completeBaseName(); // 示例：H:/images/xxx.jpg → "xxx"
    QString txtFileName = imgFileName + ".txt"; // 文件名与图片相同："xxx.txt"
    QString txtPath = imgFileInfo.absolutePath() + "/" + txtFileName; // 拼接路径：H:/labels/xxx.txt
    QFile txtFile(txtPath);
    if (!txtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->tipLabel->setText(QString("保存标注失败：无法打开文件 %1").arg(txtPath));
        return;
    }
    QTextStream out(&txtFile);
    out.setRealNumberPrecision(3); // 数值保留 5 位小数（可调整）
    int imgWidth = ui->imgLabel->width();
    int imgHeight = ui->imgLabel->height();
    for (const auto& annot : m_annotations) {
        // a. 获取标注框的原始坐标（基于原始图片，而非缩放后）
        int bboxX = annot.rect.x();      // 原始图片中矩形左上角 x
        int bboxY = annot.rect.y();      // 原始图片中矩形左上角 y
        int bboxW = annot.rect.width();  // 原始图片中矩形宽度
        int bboxH = annot.rect.height(); // 原始图片中矩形高度

        int classId = getClassIdFromAnnotType(annot.type); // 或从 annot 中存储 class_id
        // e. 写入文件（格式：class_id x_center y_center width height）
        out << classId << " "
            << bboxX << " "
            << bboxY << " "
            << bboxW << " "
            << bboxH << "\n";
    }
    txtFile.close();
    ui->tipLabel->setText(QString("标注已保存：%1").arg(txtPath));
    m_annotations.clear();
}

void AiModelSet::LoadMyAnnotation(const QString& imgPath)
{
    m_annotations.clear();
    qDebug() << "load txt " << imgPath;
    QFileInfo imgFileInfo(imgPath);
    
    QString imgFileName = imgFileInfo.completeBaseName(); 
    QString txtFileName = imgFileName + ".txt"; 
    QString txtPath = imgFileInfo.absolutePath() + "/" + txtFileName;

    QFile txtFile(txtPath);
    if (!txtFile.exists()) {
        ui->tipLabel->setText(QString("标注文件不存在：%1").arg(txtPath));
        return;
    }

    if (!txtFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->tipLabel->setText(QString("无法打开标注文件：%1").arg(txtPath));
        return;
    }

    QTextStream in(&txtFile);

    // 获取显示图像尺寸（注意：必须与保存时一致）
    int imgWidth  = ui->imgLabel->width();
    int imgHeight = ui->imgLabel->height();

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
        annot.type = getAnnotTypeFromClassId(classId); // 映射回类型
        annot.rect = rect;

        m_annotations.push_back(annot);
    }

    txtFile.close();
    ui->tipLabel->setText(QString("标注已加载：%1").arg(txtPath));

}




// 图片项点击后，在 previewLabel 中显示原图
void AiModelSet::onImageItemClicked(QListWidgetItem* item)
{

    qDebug()<<"save anno:"<<m_currentImagePath;
    saveMyAnnotation(m_currentImagePath);

    if (!item) return;  // 防止空指针

    // 1. 获取选中图片的完整路径（之前存储在 Qt::UserRole 中）
    m_currentImagePath  = item->data(Qt::UserRole).toString();
    qDebug()<<"load anno: "<<m_currentImagePath;
    LoadMyAnnotation(m_currentImagePath);



    if (m_currentImagePath.isEmpty()) {
        ui->tipLabel->setText("图片路径无效");
        return;
    }

    // 2. 加载原图（支持大图片，避免内存溢出）
    QImage image(m_currentImagePath);
    if (image.isNull()) {
        ui->tipLabel->setText("图片加载失败");
        return;
    }

    ui->tipLabel->setText(m_currentImagePath);

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

    getFgRects();
    //qDebug()<<"m_activate_class_ids "<<m_activate_class_ids;

//    ui->imgLabel->setText("");  // 清除初始提示文字

    // 清空标注选中状态和信息显示
    m_selectedAnnotIndex = -1;
    ui->annotationInfoEdit->clear();
//    update(); // 刷新绘图
}

void AiModelSet::prepareTrain()
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
            qDebug() << "Cleared existing tmp directory:" << tmpDir;
        } else {
            dir.mkpath(tmpDir);
            qDebug() << "Created tmp directory:" << tmpDir;
        }

        // 3. 创建 image 和 label 子目录
        QString imageDir = QDir(tmpDir).filePath("image");
        QString labelDir = QDir(tmpDir).filePath("label");
        dir.mkpath(imageDir);
        dir.mkpath(labelDir);
        qDebug() << "Created subdirectories:" << imageDir << labelDir;

        // 4. 复制图片和标注，收集 class_id
        QSet<int> classIds;
        for (const QString& imgPath : m_train_lists) {
            QFileInfo imgFileInfo(imgPath);
            if (!imgFileInfo.exists()) {
                qWarning() << "Image file not found:" << imgPath;
                continue;
            }

            // 复制图片
            QString destImagePath = QDir(imageDir).filePath(imgFileInfo.fileName());
            QFile::remove(destImagePath);
            if (QFile::copy(imgPath, destImagePath))
                qDebug() << "Copied image:" << imgPath << "->" << destImagePath;
            else
                qWarning() << "Failed to copy image:" << imgPath << "->" << destImagePath;

            // 复制对应标注文件
            QString txtFileName = imgFileInfo.completeBaseName() + ".txt";
            QString txtPath = imgFileInfo.absolutePath() + "/" + txtFileName;
            QFileInfo txtFileInfo(txtPath);
            if (txtFileInfo.exists()) {
                QString destLabelPath = QDir(labelDir).filePath(txtFileName);
                QFile::remove(destLabelPath);
                if (QFile::copy(txtPath, destLabelPath))
                    qDebug() << "Copied label:" << txtPath << "->" << destLabelPath;
                else
                    qWarning() << "Failed to copy label:" << txtPath << "->" << destLabelPath;

                // 读取标注文件收集 class_id
                QFile txtFile(txtPath);
                if (txtFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&txtFile);
                    while (!in.atEnd()) {
                        QString line = in.readLine().trimmed();
                        if (line.isEmpty()) continue;
                        QStringList parts = line.split(" ");
                        if (!parts.isEmpty()) {
                            bool ok = false;
                            int classId = parts[0].toInt(&ok);
                            if (ok) classIds.insert(classId);
                        }
                    }
                    txtFile.close();
                }
            } else {
                qWarning() << "Label file not found for image:" << imgPath;
            }
        }

        // 5. 生成 classes.txt
        QString classesFile = QDir(tmpDir).filePath("classes.txt");
        QFile f(classesFile);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            QList<int> sortedIds = classIds.toList();
            std::sort(sortedIds.begin(), sortedIds.end());
            for (int id : sortedIds) {
                out << id << "\n";
            }
            f.close();
            qDebug() << "Generated classes.txt:" << classesFile;
        } else {
            qWarning() << "Failed to create classes.txt:" << classesFile;
        }

        qDebug() << "prepareTrain completed. Images:" << imageDir
                << "Labels:" << labelDir
                << "Classes file:" << classesFile;


}

void AiModelSet::onShowFgRectsBtnClicked()
{
    if(m_show_fg_rects)
    {
        ui->fgPushButton->setText("显示背景");
        m_show_fg_rects = false;
    }
    else{
        ui->fgPushButton->setText("隐藏背景");
        m_show_fg_rects = true;
    }
    //m_show_fg_rects != m_show_fg_rects;
    qDebug()<<"show fg rects:"<<m_show_fg_rects;
}

void AiModelSet::onSelectModeBtnClicked()
{
    if(m_select_mode)
    {   
        
        m_select_mode = false;
    }
    else{
        
        m_select_mode = true;
    }
    //m_show_fg_rects != m_show_fg_rects;
    qDebug()<<"m_select_mode:"<<m_select_mode;
}



void AiModelSet::onModelTrainPushButtonClicked(){
    prepareTrain();
    m_currentTaskId = QUuid::createUuid().toString();
    if (modelName.isEmpty()) {
        ui->tipLabel->setText("模型名为空");
        return;
    }
    m_currentTaskId=modelName;
    m_modelApi->createDirTrain(modelName);
    ui->tipLabel->setText(QString("正在启动训练任务... 任务ID：%1").arg(m_currentTaskId));
}

// -------------------------- ImageScanThread 实现 --------------------------
ImageScanThread::ImageScanThread(const QString& dirPath, QObject* parent)
    : QThread(parent), m_dirPath(dirPath), m_stopFlag(false)
{
}

void ImageScanThread::run()
{
    m_stopFlag = false;
    QStringList imagePaths;
    QDir dir(m_dirPath);

    if (!dir.exists()) {
        emit scanFinished(imagePaths);
        return;
    }

    // 递归扫描文件夹（包括子文件夹）
    QDirIterator it(dir.path(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext() && !m_stopFlag) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        // 检查是否为支持的图片格式（不区分大小写）
        if (m_supportedFormats.contains(fileInfo.suffix().toLower())) {
            imagePaths << filePath;
            emit scanProgress(imagePaths.size());  // 发送进度
        }
    }

    emit scanFinished(imagePaths);  // 扫描完成，发送结果
}

void ImageScanThread::stopScan()
{
    m_stopFlag = true;
}

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
        if (m_currentRequestType == RequestType::DownloadModel) {
            emit downloadFinished(true, "");
            m_currentRequestType = RequestType::None;
        }
        if (m_currentRequestType == RequestType::DownloadJson) {
            emit downloadJsonFinished(true, "");
            m_currentRequestType = RequestType::None;
        }
    });
    connect(m_httpTool, &HttpTool::requestFailed, this, [this](const QString& errorMsg, int) {
        if (m_currentRequestType == RequestType::DownloadModel) {
            emit downloadFinished(false, errorMsg);
            m_currentRequestType = RequestType::None;
        }
        if (m_currentRequestType == RequestType::DownloadJson) {
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
    QString url = BASE_URL + API_TRAIN_CREATE;

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

    qDebug() << "[模型目录建立] 发起请求：URL=" << url << "，任务ID=" << taskId << "，请求体=" << QString(jsonData);
}




// 1. 启动模型训练（严格匹配协议）
void ModelApi::startTrain(const QString& taskId)
{

    if (taskId.isEmpty()) {
        emit networkError("任务 ID 不能为空");
        return;
    }

    // 构建请求 URL
    QString url = BASE_URL + API_TRAIN_START;

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

    qDebug() << "[模型训练启动] 发起请求：URL=" << url << "，任务ID=" << taskId << "，请求体=" << QString(jsonData);
}

// 2. 查询训练进度（严格匹配协议）
void ModelApi::queryTrainProgress(const QString& taskId)
{
    if (taskId.isEmpty()) {
        emit networkError("任务 ID 不能为空");
        return;
    }

    // 构建请求 URL
    QString url = BASE_URL + API_TRAIN_QUERY;

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

    qDebug() << "[训练进度查询] 发起请求：URL=" << url << "，任务ID=" << taskId << "，请求体=" << QString(jsonData);
}

// 3. 下载模型（严格匹配协议）
void ModelApi::downloadModel(const QString& modelName, const QString& savePath)
{
    if (modelName.isEmpty()) {
        emit networkError("模型名称不能为空");
        return;
    }
    if (savePath.isEmpty()) {
        emit networkError("保存路径不能为空");
        return;
    }

    // 构建请求 URL（协议要求：GET /模型名称）
    QString url = BASE_URL + API_MODEL_DOWNLOAD + modelName;

    // 记录当前请求类型
    m_currentRequestType = RequestType::DownloadModel;

    // 发起 GET 请求下载文件（协议要求 GET 方法，无请求体）
    m_httpTool->downloadFile(url, savePath);

    qDebug() << "[模型下载] 发起请求：URL=" << url << "，保存路径=" << savePath;
}

void ModelApi::downloadModelJson(const QString& modelJson, const QString& savePath){
    if (modelJson.isEmpty()) {
        emit networkError("模型Json不能为空");
        return;
    }
    if (savePath.isEmpty()) {
        emit networkError("保存路径不能为空");
        return;
    }

    // 构建请求 URL（协议要求：GET /模型名称）
    QString url = BASE_URL + API_MODEL_DOWNLOAD + modelJson;

    // 记录当前请求类型
    m_currentRequestType = RequestType::DownloadJson;

    // 发起 GET 请求下载文件（协议要求 GET 方法，无请求体）
    m_httpTool->downloadFile(url, savePath);

    qDebug() << "[模型JSON下载] 发起请求：URL=" << url << "，保存路径=" << savePath;
}


// HTTP 请求成功回调（训练启动/查询）
void ModelApi::onHttpRequestSuccess(const QByteArray& responseData, int statusCode)
{
    qDebug() << "[HTTP 成功] 状态码=" << statusCode << "，响应数据=" << QString(responseData);

    // 严格检查 HTTP 状态码（协议要求 200 正常返回）
    if (statusCode != 200) {
        emit networkError(QString("HTTP 状态码异常：%1").arg(statusCode));
        m_currentRequestType = RequestType::None;
        return;
    }

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
    default:
        break;
    }

    m_currentRequestType = RequestType::None;
}

// HTTP 请求失败回调
void ModelApi::onHttpRequestFailed(const QString& errorMsg, int errorCode)
{
    QString err = QString("[HTTP 失败] 错误码=%1，错误信息=%2").arg(errorCode).arg(errorMsg);
    qDebug() << err;
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
    response.code = obj.contains("code") ? obj["code"].toInt() : -1;
    response.message = obj.contains("message") ? obj["message"].toString() : "未知错误";
    response.progress = obj.contains("progress") ? obj["progress"].toInt() : 0;
    response.number = obj.contains("number") ? obj["number"].toInt() : -1;
    response.model_name = obj.contains("model_name") ? obj["model_name"].toString() : "";

    return response;
}
