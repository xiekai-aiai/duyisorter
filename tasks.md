# Tasks：顶部运行态实时状态栏

## 已完成（代码已落盘）

- [x] T1 新建 `src/topbarstatuswidget.h`：`TOPBAR_STATUS_HEIGHT=80`、`AiStatusPollThread`、`TopBarStatusWidget` 接口与 12 个素材成员
- [x] T2 新建 `src/topbarstatuswidget.cpp`：素材加载（qrc → 磁盘兜底）、AI 状态后台轮询、淡入淡出、双卡片/分段流光/停止按钮/时钟绘制、停止按钮点击命中
- [x] T3 `src/mainwidget.h`：前置声明 + `topBarStatus` 成员
- [x] T4 `src/mainwidget.cpp`：布局行高 80、控件创建与信号连接、`onOpenFeederSlt/onCloseFeederSlt` 两态切换、`TimeOutSlt` 每秒兜底同步、全屏页隐藏、`stackedWidget`/`homePageStackedWidget` 高度补偿
- [x] T5 `res.qrc`：登记 12 个 `res/png2/topbar_anim/*.png`
- [x] T6 确认本地 12 个素材文件全部存在，未使用的 `preview_all/fxSweep/fxCardGlow/dotWarn` 不登记 qrc

## 待执行

- [ ] T7 恢复编译机连通性：`192.168.172.134` 当前 `Host is down`；确认网络/VM 启动，或由用户指定可用服务器与凭据
- [ ] T8 `ssh_connect` → `upload_files` → `build_duyi(release, 6SXZ-600G_335X)`，确认 EXIT_CODE=0
- [ ] T9 若编译失败：取 STDERR 首个 error，`read_remote_file` → 最小修复 → `remote_edit_file` → 重编译（≤5 轮）
- [ ] T10 真机验证：开始工作淡入 / 数值刷新 / 报警转红 / 停止淡出还原 / 全屏页隐藏 / 多语言
- [ ] T11 布局核对：底部内容是否因顶栏增高 30px 被裁切，必要时微调 `stackedWidget` 与各页面固定高度

## 需要用户决定

1. 编译服务器地址：`192.168.172.134`（任务约定，当前不可达）还是 `.vscode/sftp.json` 中的 `192.168.172.182`
2. 是否接受「素材缺失时仅 QPainter 兜底」的降级：若担心 rcc 失败，可临时注释 qrc 中 12 条（当前素材已齐，建议保留）
