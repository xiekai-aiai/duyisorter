# Spec：点击「开始工作」后的顶部运行态实时状态栏

## 1. 目标

DuySorter（RK3566 / Qt5 Widgets / 1024×768）主界面在点击「开始工作」后，顶部 80px 区域从空闲态（原工具按钮 + 时钟）切换为运行态：

- 左：相机丢包指标卡（296×58）
- 右：推理超时指标卡（296×58）
- 中：停止工作按钮（186×56，红胶囊）
- 右端：时间图标 + 日期 + 时钟
- 指标卡含：中文标题、英文副标题、实时数值、单位、NORMAL/ABNORMAL 状态、18 段流光条
- 停止工作后 0.4s 内淡出，还原原工具按钮
- 实时数据来自各 AI 单元 UDP 状态查询，后台线程轮询，不阻塞 UI

## 2. 素材约定（已确认本地全部存在）

目录：`res/png2/topbar_anim/`，并已登记到 `res.qrc`（alias 前缀一致）：

| 文件 | 用途 |
| --- | --- |
| cardMetricNormal.png | 正常态卡片九宫格底图 |
| cardMetricAlarm.png | 报警态卡片九宫格底图 |
| dotNormal.png | 正常态状态点 |
| dotAlarm.png | 报警态状态点 |
| segOn.png | 正常态点亮分段 |
| segOff.png | 熄灭分段 |
| segOnAlarm.png | 报警态点亮分段 |
| btnStop.png | 停止按钮九宫格底图 |
| btnStopPressed.png | 停止按钮按下态 |
| iconStop.png | 停止图标 |
| topbarBg.png | 顶栏背景（1024×80） |
| iconClock.png | 时钟图标 |

说明：`preview_all.png`、`fxSweep.png`、`fxCardGlow.png`、`dotWarn.png` 为设计预览/未使用素材，**不登记 qrc**，避免强制文件依赖。代码在素材缺失时全部走 QPainter 兜底绘制。

## 3. 已实现改动（代码已落盘）

### 3.1 新增 [src/topbarstatuswidget.h](file:///Users/thinklic/Desktop/duyisorter_xinxin3568/src/topbarstatuswidget.h)

- `#define TOPBAR_STATUS_HEIGHT 80`
- `class AiStatusPollThread : public QThread`：`startPolling()/stopPolling()`、`signal statusUpdated(quint32 discard_num, quint32 timeout_num)`、`volatile bool m_stopped`
- `class TopBarStatusWidget : public QWidget`：`isRunning()`、`slot setRunning(bool)`、`slot onStatusUpdated(quint32,quint32)`、`signal stopRequested()`、`paintEvent/mousePressEvent/mouseReleaseEvent`、`drawCard/drawSegments/drawStopButton/drawClock/drawNineSlice/loadAsset`、12 个 QPixmap 成员

### 3.2 新增 [src/topbarstatuswidget.cpp](file:///Users/thinklic/Desktop/duyisorter_xinxin3568/src/topbarstatuswidget.cpp)

- **素材加载** `loadAsset()`：优先 `:/res/png2/topbar_anim/<name>`，其次 `applicationDirPath()/res/png2/topbar_anim/<name>`，均无返回空 QPixmap
- **后台轮询** `AiStatusPollThread::run()`：
  - 单元数 `struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal`
  - 请求 `cmdworker::AiStatusRequest()`，逐单元 `ai_helper::GetAiIpByIndex(idx)`
  - `CmdUdpManager::instance().onSendCommand(QHostAddress(ip), AI_UPD_CMD_PORT, request, response, 500)`
  - `cmdworker::ParseCmdPkg()` + `cmdworker::AiStatusResponse()` 累加 `discard_num_` / `timeout_num_`
  - 2s 间隔（100ms×20 分段休眠，停止可及时退出）
- **状态切换** `setRunning()`：进入清计数、淡入（`m_fade` 0→255，80ms/帧 +48）、启动 `m_anim_timer`/`m_clock_timer`/轮询线程、`show()+raise()`；退出停轮询、`m_fade_target=0` 淡出后 `hide()`
- **布局**：卡片 `cardRect(0/1)` = x 8 / 312，y 8，296×58，间距 8；`stopRect()` = x 616，y 12，186×56；`clockRect()` = x width-196，184×80
- **绘制**：卡片九宫格（slice 48 / margin 16）、状态点 14×14、标题 15px Bold `#EAF6FF`、副标题 8px `#4E8FA6`、数值 22/26/30px 随位数、单位 11px、NORMAL/ABNORMAL 10px、18 段 10×5 流光（5 段亮）；停止按钮九宫格（slice 88 / margin 26）+ 图标 + `myLan.working_stop`；时钟 `yyyy-MM-dd` / `HH:mm:ss`
- **交互**：停止按钮按下/释放判定，区域内释放 `emit stopRequested()`
- **析构**：`stopPolling()` + `wait()`
- **i18n**：复用 `myLan.ai_front_discard_num`、`ai_front_delay_num`、`times`、`working_stop`，不新增 .ts key

### 3.3 修改 [src/mainwidget.h](file:///Users/thinklic/Desktop/duyisorter_xinxin3568/src/mainwidget.h)

- 前置声明 `class TopBarStatusWidget;`
- 新成员 `TopBarStatusWidget* topBarStatus;`

### 3.4 修改 [src/mainwidget.cpp](file:///Users/thinklic/Desktop/duyisorter_xinxin3568/src/mainwidget.cpp)

- 构造首段 `topBarStatus = 0;`（防页面切换信号提前访问）
- `stackedWidget->setFixedHeight(LCD_HEIGHT - TOPBAR_STATUS_HEIGHT - 20)`
- `mainGridLayout->setRowMinimumHeight(0, TOPBAR_STATUS_HEIGHT)`（构造 + `onStackedWidgetIndexChangedSlt`）
- 顶部按钮 addWidget 补 `Qt::AlignVCenter`
- 尾部创建并加入 `(0,0,1,7)`，`stopRequested → onFeedSwitchBtnClickedSlt()`
- `onCloseFeederSlt()` 末：`setRunning(false)`；`onOpenFeederSlt()` 末：`setRunning(true)`
- `TimeOutSlt()`：按 `struGsh.bStatFeed == MODE_OPEN` 每秒兜底同步
- `onStackedWidgetIndexChangedSlt()`：高度按 `TOPBAR_STATUS_HEIGHT` 换算；全屏页（`SM_AI_PAGE`/`SM_VIDEO_PAGE`/`SM_AI_MODEL_SET_PAGE`/`SM_AUTO_ANALYSIS_PAGE`）运行时隐藏状态栏
- `homePageStackedWidget` 高度两处按新顶栏高度下调

### 3.5 修改 [res.qrc](file:///Users/thinklic/Desktop/duyisorter_xinxin3568/res.qrc)

- 新增 12 条 `res/png2/topbar_anim/*.png`（第 213–224 行）

### 3.6 不改动项

- `DuySorter.pro`：`src/*.h`、`src/*.cpp` 通配符已自动包含新文件
- `src/3rdparty/`、`.pro` 配置、`LCD_TITLE_HEIGHT` 宏（避免影响其他页面）

## 4. 风险与已知限制

1. `res.qrc` 显式列出 12 个素材：任一文件缺失会导致 `rcc` 编译失败（本地已确认 12 个文件均存在）
2. `CmdUdpManager` 在后台线程使用，需确认其内部有互斥保护（`QMutex` 成员已见）；无则需改为串行复用
3. 顶栏由 50px 增至 80px，内容区缩小 30px，已同步下调 `stackedWidget`/`homePageStackedWidget`，但底部是否裁切需真机验证
4. 远程编译机 `192.168.172.134` 当前不可达（`ping: Host is down`，`ssh_connect` 两次失败），编译验证被阻塞；`.vscode/sftp.json` 现有配置为 `192.168.172.182/root123`，与任务约定服务器不一致

## 5. 验收标准

- `./qtmakeApp.sh release 6SXZ-600G_335X` 退出码 0，`app/DuySorter` 生成
- 编译日志中 `qrc_res.cpp` 包含 12 个 topbar_anim 资源且无缺失告警
- 运行：开始工作 → 顶栏 80px 淡入两张卡 + 停止按钮 + 时钟；数值随 AI 单元回传刷新；`discard_num>0` 或缺包时卡片转红并显示 ABNORMAL
- 停止工作 → 0.4s 内淡出并还原原工具按钮；进入视频/AI 等全屏页状态栏自动隐藏
- 多语言切换后标题与按钮文字随 `myLan` 变化，无 raw key 显示

## 6. 验证步骤

1. `ssh_connect(192.168.172.134, mcgs, 7012917, /home/mcgs/Desktop/DuySorter_rk3566_simple)`
2. `upload_files(session_id)`
3. `build_duyi(session_id, mode=release, product=6SXZ-600G_335X)`
4. 失败则取 STDERR 首个 error → `read_remote_file` → 最小改动修复 → `remote_edit_file` → 重编译（最多 5 轮）
5. 成功后按第 5 节做真机交互核对
