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

## 阻塞中

- [ ] T10 远程编译：`192.168.172.134` 与 `192.168.172.182` 均 ping 不通、22 端口无响应，`ssh_connect` 连续失败；需用户启动编译机或提供可用服务器后执行 `ssh_connect → upload_files → build_duyi`
- [ ] T11 编译失败修复循环：取 STDERR 首个 error → `read_remote_file` → 最小修复 → `remote_edit_file` → 重编译（≤5 轮）

## 待真机

- [ ] T12 交互验证：开始工作淡入 / 数值刷新 / 报警转红 / 停止淡出还原 / 全屏页隐藏 / 多语言
- [ ] T13 布局核对：底部内容是否因顶栏增高 30px 被裁切，必要时微调 `stackedWidget` 与各页面固定高度
