# Tasks：顶部运行态实时状态栏

## 已完成（代码已落盘）

- [x] T1 新建 `src/topbarstatuswidget.h`：`TOPBAR_STATUS_HEIGHT=80`、`AiStatusPollThread`、`TopBarStatusWidget` 接口与 12 个素材成员
- [x] T2 新建 `src/topbarstatuswidget.cpp`：素材加载（qrc → 磁盘兜底）、AI 状态后台轮询、淡入淡出、双卡片/分段流光/停止按钮/时钟绘制、停止按钮点击命中
- [x] T3 `src/mainwidget.h`：前置声明 + `topBarStatus` 成员
- [x] T4 `src/mainwidget.cpp`：布局行高 80、控件创建与信号连接、`onOpenFeederSlt/onCloseFeederSlt` 两态切换、`TimeOutSlt` 每秒兜底同步、全屏页隐藏、`stackedWidget`/`homePageStackedWidget` 高度补偿
- [x] T5 `res.qrc`：登记 12 个 `res/png2/topbar_anim/*.png`
- [x] T6 确认本地 12 个素材文件全部存在，未使用的 `preview_all/fxSweep/fxCardGlow/dotWarn` 不登记 qrc
- [x] T7 本地静态验证：`topbarstatuswidget.cpp`、`mainwidget.cpp` 通过 clang `-fsyntax-only`（Qt 5.11 + Linux 头文件桩）；`moc topbarstatuswidget.h` 及其产物通过语法检查
- [x] T8 资源验证：`rcc res.qrc` 退出码 0，12 个 topbar_anim 素材与 282 个 `res/png2` 资源全部可解析
- [x] T9 本地 git 提交：`f1a477a`（顶部运行态状态栏 + 当前工作区快照）

## 本轮修复（对齐 GIF：一个面板 + 一条跑马灯）

- [x] T14 `topbarstatuswidget`：两张独立卡片改为「一个面板 + 左右两个指标 + 一条贯穿跑马灯」
  - 卡片/按钮素材按 alpha>60 裁剪透明留白后拉伸/九宫格铺满，边框完整罩住全部文字
  - 常量改为 `PANEL(8,6,610,62)`、`METRIC_WIDTH=305`、`STOP(630,15,150,50)`、30 段 × 14px 跑马灯
  - 停止按钮九宫格按素材高度一半切分，左右半圆帽等比缩放
  - 时钟区按 144px 宽重排（图标 38 + 日期 11px + 时间 16px）
  - 清理 `cardRect()` 残留引用，移除 `drawCard()` 内的分段条绘制（避免每条指标各画一条）
- [x] T15 `mainwidget.cpp`：`homePageStackedWidget` 高度由 `-60` 下调到 `-80`，预留底部「分隔线 + 功能键」50px，白框不再压到功能键区
- [x] T16 复验：`topbarstatuswidget.cpp` / `mainwidget.cpp` clang `-fsyntax-only` 通过；`moc` + `rcc` 通过
- [x] T17 顶部改为直角矩形外框（去掉弧形包围框与卡片素材）；中文界面内部只显示中文（隐藏英文副标题，状态显示“正常/异常”）；停止按钮文字改用 `myLan.working_start`（中文为“停止工作”）
- [x] T18 顶部工具按钮（视频采集/关机/参数保存/开始工作/时钟）改为按内容自适应宽度 + 等比例弹簧分隔，起始位置不变，各控件之间以及与时钟的间距完全相同

## 本轮重构（对齐新 GIF：状态栏 + 红方按钮 + 时钟）

- [x] T19 `topbarstatuswidget` 按新设计重构
  - 运行态左侧为状态栏：26px 状态图标（绿=正常/红=异常）+ 18px 状态文字（异常时先显示“异常”，内容由业务层后续提供）+ 底部绿色 30 段跑马灯
  - `setWorkButton(feedSwitchBtn)` 取实际矩形，运行态按钮与空闲态“开始工作”按钮位置完全一致；图标换成红色正方形（`iconWorking.png`），文字仍为 `myLan.working_stop`（中文“开始工作”）
  - 右侧时钟逻辑不变；素材缺失时全部走 QPainter 兜底，不会崩溃
  - 新素材目录 `res/png2/topbar_anim2/`（iconWorking/stateNormal/stateAlarm/statusPanelBg），分段条/底色/时钟沿用 `topbar_anim`
  - 本地 clang `-fsyntax-only`（`topbarstatuswidget.cpp`、`mainwidget.cpp`）与 `moc` 校验通过
- [x] T20 素材到位后把 `res/png2/topbar_anim2/` 四个 PNG 登记进 `res.qrc`（rcc 通过）
- [x] T21 本地真实渲染验证（Qt 5.11 x86 预览程序加载真实素材）：
  - `statusPanelBg.png` 按 alpha>8 裁掉透明留白后再铺满，边框与目标矩形精确对齐
  - 正常/异常两态渲染正确：状态图标 + “正常运行/异常” + 绿色跑马灯 + 红方按钮 + 时钟
  - 修复潜在崩溃：`TopBarStatusWidget` 与“开始工作”按钮是兄弟控件，`QWidget::mapTo` 要求目标必须是祖先，改为 `mapToGlobal + mapFromGlobal`
- [x] T22 按最新要求简化运行态顶部：
  - 取消 `statusPanelBg.png` 面板背景框，状态区不再画边框/底板，直接显示状态
  - 状态图标+文字在高度上居中，与“开始工作”按钮中心对齐；绿色跑马灯保持在底部
  - 状态控件只覆盖“开始工作”按钮左侧区域，按钮与时钟继续复用主界面真实控件，位置与空闲态完全一致
  - 删除自绘按钮/时钟、`stopRequested` 信号与鼠标命中逻辑（停止操作由真实按钮自身处理），`mainwidget.cpp` 不再把状态控件加入网格布局

- [x] T23 开始工作后按钮图标改用 `res/png2/topbar/workStop.png`：
  - 点击开始 → `onOpenFeederSlt` 仍按原逻辑设置 `workStop.png`（原有实现）
  - `TimeOutSlt` 兜底同步运行态时一并切换 `workStop.png` / `workStart.png`，覆盖主屏同步、报警等非手动触发路径

- [x] T24 消除状态区与右侧按钮/时钟区的背景割裂：
  - 状态区背景改为从主界面 `back_deep_clear` 背景画布按自身 `geometry()` 取同一块区域绘制，与右侧背景逐像素一致
  - 无主界面背景纹理时才回退到 `topbarBg.png`/渐变
  - 预览逐像素校验：接缝两侧 RGB 差 ≤1

- [x] T25 AI检测状态页 8 通道显示不全修复（`statusinfopage.cpp`）：
  - 表格标签/数值字体 22px → 16px（`FONT_SIZE_16`）
  - 网格边距 12/8 → 10/6，横向间距 24 → 8，纵向间距 6 → 4
  - 数值列最小宽度 160 → 36 且 `columnStretch=1` 等分剩余宽度；标签列 150
  - 8 个通道（9 列）最小占用约 502px，页面可用约 740px，可完整显示

- [x] T26 顶部状态栏接入逐板卡异常判定（`topbarstatuswidget`）：
  - `AiStatusPollThread` 按板卡序号缓存上次查询的丢包数/延迟超时数，每 2s 轮询后做增量对比
  - 单次增量 > `AI_STATUS_DELTA_ALARM`（默认 10）判定异常；丢包 → “通道x前视/后视出现丢包”，超时 → “通道x前视/后视推理异常”
  - 多条异常用“，”连接并以“。”结尾；计数回退视为板卡重启，只重建基准不告警
  - 信号改为 `statusUpdated(QString 描述)`；正常时为空串显示“正常运行”
  - 异常文案 18px 放不下自动降 15px，仍超长则右侧省略

- [x] T27 顶栏高度回退到 60（`TOPBAR_STATUS_HEIGHT 80 → 60`），运行态状态区高度随之 60，图标/文字/跑马灯布局不变
  - 顶栏 4 个工具按钮图标尺寸 60/80 → 统一 48，适配 60 高的工具行（按钮高 50 不变）
  - 页面高度补偿全部回滚到原值：`stackedWidget -20`、`homePageStackedWidget -80`、HIGH_SET `-10`
  - 回滚 `operatePageHBLayout->setSpacing(0)`、`MySchemeParams` 固定高度/边距/间距、方案参数页背景页 546、背景曲线 360
  - 校验：`mainwidget.cpp`、`topbarstatuswidget.cpp`、`myschemeparams.cpp`、`schemeparams.cpp`、`background.cpp` 语法检查通过；60 高预览两态渲染正常

## 本轮修复：页面布局按原版恢复（用户反馈「控件下移、底部被遮挡」）

- [x] T28 仅改 `src/mainwidget.cpp`，把页面布局逐像素恢复到 71e559f，同时保留顶栏 60
  - [x] T28.1 `mainGridLayout->setContentsMargins(5, 0, 10, 10)`（上边距 10→0，页面起点回到 y=62）；工具栏 5 个控件 addWidget 补 `Qt::AlignVCenter`
  - [x] T28.2 `stackedWidget`：构造 `LCD_HEIGHT - LCD_TITLE_HEIGHT - 20`（698）；switch 初始 `LCD_HEIGHT - LCD_TITLE_HEIGHT - LCD_STATUS_HEIGHT - 10`（618）；SM_VIDEO_PAGE `LCD_HEIGHT - LCD_TITLE_HEIGHT`（718）；default（698）
  - [x] T28.3 `homePageStackedWidget`：默认 `LCD_HEIGHT - 110`（658）；HIGH_SET `LCD_HEIGHT - 60`（708）
  - [x] T28.4 逐页静态核算：工厂设置/颜色相机 708 完整、产量页底部标签完整、视频页 718、AI 模型设置页 768 不变、方案参数页已回滚值复验
  - [x] T28.5 远程 `192.168.172.181` 共享目录编译成功：`/tmp/build.log` 中 `error:` 0 条，`src/mainwidget.cpp` 已重新编译，打包 `rel/duyiSorter_6SXZ-600G_335X_V1.06_35.tar.xz`（70651 KB），`app/DuySorter` 57,276,584 字节
  - [ ] T28.6 真机截图复核工厂设置页与产量页底部不再裁切（待用户上机确认）

## 本轮新增：模型下载 MD5 校验弹窗告警

- [x] T29 训练完成下载模型后，校验 .bin/.dlc 的 MD5 与 JSON 中记录值是否一致（`src/aimodelset.cpp` `downloadFinished`）
  - [x] T29.1 沿用 `downloadJsonFinished` 已解析的 `m_expectedModelMd5`（兼容 md5/file_md5/model_md5/bin_md5/dlc_md5）
  - [x] T29.2 MD5 不一致 → `QMessageBox::warning` 模态弹窗告警（展示模型路径、期望 md5、实际 md5）+ 页面红字 + `resetTrainingState()` 终止
  - [x] T29.3 模型文件无法读取 → 同样弹窗告警并终止，不继续回写 JSON / 不入库
  - [x] T29.4 JSON 中无 md5 字段 → `LOG_WARN_STM` 记录并跳过校验，不阻断下载
  - [x] T29.5 远程 `192.168.172.181` 全量编译成功：`/tmp/build_md5.log` 中 `error:` 0 条；`app/DuySorter` 57,276,584 字节；打包 `rel/duyiSorter_6SXZ-600G_335X_V1.06_35.tar.xz` 71,703,824 字节；二进制中已确认包含「MD5 校验失败」「模型校验失败」文案
  - [ ] T29.6 真机验证：训练完成后人为改坏模型文件，确认弹窗出现且不注册模型

## 本轮新增：数据库老库自动补表（修复模型入库/算法选择页为空）

- [x] T30 `src/sqlitemgr.cpp` `SQLiteMgr::Init()` 打开库后补建缺失表
  - [x] T30.1 板卡实测：`/opt/app/userdata/db/sorter.db` 仅有旧表 paraInfo/modelInfo/modelParaInfo/sfio/encycodeio（5 张全 0 行），缺 model_info/model_params/config
  - [x] T30.2 按现有 SQL 反推 DDL，`CREATE TABLE IF NOT EXISTS` 建 model_info(id modelId UNIQUE modelName isApply isUpload)、model_params(9 列)、config(group_name+param_name UNIQUE)，逐条 exec 失败打 LOG_ERROR_STM，不动旧表旧数据
  - [x] T30.3 远程 192.168.172.181 编译成功：`/tmp/build_db.log` error 0 条；`app/DuySorter` 57,276,608 B；安装包 72,348,920 B；二进制已确认含建表语句
  - [ ] T30.4 板卡安装新版本后启动一次，确认三张表自动创建；重新下载模型确认可入库、算法选择页可见

## 本轮新增：AI 板卡模型上传进度提示（逐板检测 + 跳过/覆盖）

- [x] T31 AI 模型上传改为逐块板异步上传 + 模态进度对话框
  - [x] T31.1 `src/sftp_client.h/.cpp`：新增带进度回调的 `upload()` 重载（每 32KB 回调一次字节进度，回调返回 false 即取消）；原两参版本转调空回调，训练上传行为不变
  - [x] T31.2 `src/rgb/schemeparams/ai.cpp`：新增 `ModelUploadDialog`（总进度条、逐板行状态、成功/跳过/失败统计、单板进度条与字节数、强制重新上传勾选框、取消/关闭/重试失败项）；`reject()/closeEvent` 上传中只转取消，避免线程仍在跑时销毁窗口
  - [x] T31.3 `PageAi::modelUpload`：IP 排序后逐板串行；每板先检测（stat + 远程 `md5sum`），bin 一致且 json 大小一致 → 跳过并计成功，不一致 → 覆盖上传；json 缺失/不同则只补 json；上传放到 `std::thread`，UI 更新经 `QTimer::singleShot` 回主线程
  - [x] T31.4 失败后弹窗内点「重试失败项」仅重传失败 IP；取消后已完成板卡不回滚
  - [x] T31.5 远程 192.168.172.181 编译成功：`/tmp/build_upload2.log` 中 `error:` 0 条；`app/DuySorter` 57,313,472 B；安装包 `rel/duyiSorter_6SXZ-600G_335X_V1.06_35.tar.xz` 72,366,792 B；二进制已确认含「强制重新上传」「已存在，跳过」文案
  - [ ] T31.6 真机验证：多板上传进度刷新、已存在模型跳过、断开某块板验证失败/重试、取消中途上传

## 本轮新增：视频预览不通修复（config 缺省值 + 高度防护）

- [x] T32 视频预览黑屏：config 表空导致高度全 0，接收端 0 高拼帧必失败
  - [x] T32.1 `src/configmgr.cpp`：`InitAiCfgInfo` 缺键时使用 AI 板默认值（64/64/500/256/0）并回写 config 表；`Init` 在 `LoadAllCfg` 失败时继续用默认值初始化
  - [x] T32.2 `src/udpimagereceiver.cpp`：`SetImageHegiht/SetImageWidth` 拒绝 <=0；`start()` 校验尺寸；`resetStreamImage`/`processDatagram` 增加越界防御
  - [x] T32.3 `src/myvideo.cpp`：`startCapture` 高度为 0 时兜底 256 并打 `LOG_ERROR_STM`
  - [x] T32.4 远程 192.168.172.181 全量编译成功：EXIT=0、`error:` 0 条；`app/DuySorter` 57,313,472 B；安装包 `rel/duyiSorter_6SXZ-600G_335X_V1.06_35.tar.xz` 72,361,424 B；二进制已确认含「video view height is 0, fallback to」「AI config missing in db」文案
  - [ ] T32.5 真机验证：启动日志高度 64/64/500/256/0、config 表 5 行、点视频页日志 `set height:256`、AI 板回 `code=1`
  - [ ] T32.6 现场确认 FPGA 是否向 AI 板 eth0 推图（`rx_bytes` 是否增长）；不增长则为采集链路问题

## 阻塞中

- [x] T10 远程编译：编译机改为 `192.168.172.181`（账号密码取 sftp.json），共享目录 `/mnt/hgfs/duyisorter_xinxin3568` 直接编译，无需上传
- [x] T11 编译修复循环：第 1 轮报 `statusinfopage.cpp:630` —— `myLan.ai_front_discard_num` 被误拆成两行（`ai_front_` + `discard_num`），已合并；第 2 轮 `EXIT=0` 编译成功
  - 产物：`app/DuySorter` 55M；安装包 `rel/duyiSorter_6SXZ-600G_335X_V1.06_35.tar.xz` 69M

## 待真机

- [ ] T12 交互验证：开始工作淡入 / 数值刷新 / 报警转红 / 停止淡出还原 / 全屏页隐藏 / 多语言
- [ ] T13 布局核对：底部内容是否因顶栏增高 30px 被裁切，必要时微调 `stackedWidget` 与各页面固定高度
