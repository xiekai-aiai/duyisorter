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

## 阻塞中

- [ ] T10 远程编译：`192.168.172.134` 与 `192.168.172.182` 均 ping 不通、22 端口无响应，`ssh_connect` 连续失败；需用户启动编译机或提供可用服务器后执行 `ssh_connect → upload_files → build_duyi`
- [ ] T11 编译失败修复循环：取 STDERR 首个 error → `read_remote_file` → 最小修复 → `remote_edit_file` → 重编译（≤5 轮）

## 待真机

- [ ] T12 交互验证：开始工作淡入 / 数值刷新 / 报警转红 / 停止淡出还原 / 全屏页隐藏 / 多语言
- [ ] T13 布局核对：底部内容是否因顶栏增高 30px 被裁切，必要时微调 `stackedWidget` 与各页面固定高度
