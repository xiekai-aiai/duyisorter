# Git 合并冲突修复记录（2026-11-24）

## 一、合并概况

| 项目 | 内容 |
|---|---|
| 本地分支 | `main`（最后提交 `9fb8543` 统一UI科技风样式） |
| 合并来源 | `origin/main`（`d45b7bd` 调整支持不同厂家的配置） |
| 分叉情况 | 本地领先 8 个提交，落后 17 个提交 |
| 冲突文件数 | 27 个（源码 21、翻译 4、运行时数据库 1、`.DS_Store` 1） |
| 合并方式 | `git merge origin/main`，逐文件人工/脚本解冲突 |

> 说明：合并前本地未提交改动（`tasks.md` 与 3 个根目录文档）已用 `git stash` 暂存；运行时数据库 `app/userdata/db/sorter.db` 先备份到 `/tmp/sorter.db.bak.*` 后删除，以让上游受控版本落盘。

---

## 二、逐文件冲突处理

### 资源与产物

| 文件 | 冲突情况 | 处理方式 |
|---|---|---|
| `res.qrc` | 上游把 `res/png/*` 全部改为 `res/png2/*` + alias，本地新增大量科技风资源 | **取上游结构 + 追加本地 66 条新增资源**（checkbox、adjust_btn_tech、fine 箭头、dialog、pause_play_btn_tech、back_pure_clear、state_page 等），合并后共 370 条 |
| `res/.DS_Store` | 本地修改 / 上游删除 | 接受上游删除（macOS 垃圾文件） |
| `app/userdata/db/sorter.db` | 本地未跟踪 / 上游新增受控版本 | 采用上游版本；本地运行库已备份 |
| `app/i18n/en.qm`、`app/i18n/zh-cn.qm` | 二进制冲突 | 暂取本地版本；构建时 `CONFIG += lrelease` 会由合并后的 ts 重新生成 |
| `i18n/zh-cn.ts`、`i18n/en.ts` | 16 处仅 `<location>` 行冲突 + 1 处上下文大块 | location 取上游；翻译块保留本地已翻译文本（形选、深度学习、深度学习设置等）；`en.ts` 追加 2 条上游独有条目（未翻译） |

### 状态信息页（整体采用上游）

| 文件 | 处理方式 |
|---|---|
| `src/systeminitialization.cpp` | **整体采用 origin/main 版本**（1070 行） |
| `src/systeminitialization.h` | 整体采用 origin/main 版本（192 行） |

- 保留上游新增：恒流源板状态 `m_lightSrcLabel`、新接口板分组 `newInterfaceBoardListGroup`、`cameraStateLbl/testStateLbl/newInterfaceListLayout`、按钮 9-slice 样式（btn_normal/pressed/disabled）
- 放弃本地自绘矩阵改造：`StateMatrixWidget`、`createSummaryCard`、`matrixText`、`stateResPixmap`、`m_matrix/m_ctrlMatrix/m_sumValue`、`paintEvent` 矩阵绘制
- 已全局确认其它文件无引用上述本地符号，不会产生链接错误
- 遗留：`res/png2/state_page/*` 资源仍在 qrc 中但已无代码引用（冗余、不报错）

### AI 相关

| 文件 | 处理方式 |
|---|---|
| `src/aimodelset.cpp` | 12 处冲突全部保留本地实现：`PAGE_CONTENT_OFFSET=60` 与 400/620 布局偏移、进度条/心跳灯、深色 `kDialogLineEditQss`（18px、disabled/QScrollArea/QPushButton）；上游该段为旧基线 |
| `src/aimodelset.h` | 保留本地 `m_trainStageBar/m_heartbeatLamp/m_heartbeatTimer/m_heartbeatOn` |
| `src/aismainwidget.h` | 保留本地 `sftp_client.h`（弃 QFtp）、`SftpClient*/remoteFiles/downloadDirRecursive` |
| `src/aismainwidget.cpp` | 保留本地 SFTP 上传实现（端口 22）、`nullptr` 防护、左侧列表 16px 字体、`setModeToggleStyle/setTechAdjustStyle`；背景按用户此前要求改回 `back_pure_clear.png` |
| `src/rgb/aimainwidget.cpp` | 保留本地“多帧择优”采集（常量、`onImageRead`、抓拍前重置计数）；删除 HEAD 中重复的 `saveForegroundMask` 定义（上游冲突块之后已提供），避免重复定义 |

### 启动与全局流程

| 文件 | 处理方式 |
|---|---|
| `src/globalflow.cpp` | 9 处冲突合并：保留上游 `bootStage(CONFIG, ACTIVE)`、AI 未启用跳过、`chgVFDFreq()` 变频器下发；保留本地延迟取文案、`initInterfaceTransMode(0)`、`BOOT_STAGE_UI/HW` 与加密授权文案；删除上游早期重复的 `configErrors/statusError` 声明 |
| `src/globalflow.h` | 签名冲突取上游（3 处） |
| `src/main.cpp` | 保留本地版本：`QPointer splashGuard` 防悬空、字体/主题、关闭原生对话框、版权头；上游加载页逻辑已在其中 |
| `src/loadingwindow.cpp` | 窗口标志/光标隐藏取本地；产品名每帧 `tr()` 取本地（支持语言热切换）；分号拼接取上游 `QString::fromUtf8` |
| `src/myqextserialport.h` | 仅空格差异，取上游 |
| `src/udpimagereceiver.cpp` | 保留本地高度/尺寸防护与 `cur_row >= height_` 越界保护；上游多相机 `stream_states` 分流逻辑完整保留 |

### 主界面与公共控件

| 文件 | 处理方式 |
|---|---|
| `src/mainwidget.cpp` | 保留本地顶部运行态状态栏、顶部按钮等间距、直角外框、中文只显示中文、跑马灯、底键 18px、首页四按钮 `BTN_WIDTH+60`（深度学习模型/深度学习设置）、喷阀页 `setTechAdjustStyle`；顶栏图标保留 48px（按钮高 50px，上游 60/80px 会被裁切）；背景按用户要求恢复 `back_pure_clear.png` |
| `src/mycurve.cpp` | 保留本地 +/− 尺寸 50×40 与 `setTechAdjustStyle`、暂停/继续 `setTechPausePlayStyle`、前视 `setModeToggleStyle` |
| `src/common/mymutex.cpp` | 选中指示器统一 `setTechCheckBoxStyle`（cb_* 贴图），与 `clearBtnsIcon` 同步；保留上游 `makeCheckIcon`（当前仅作遗留接口） |
| `src/common/mywid.cpp` | `MyCheckBox` 取上游去除内边距样式（之后被 cb_* 五态整体覆盖）；`MyRadioGroup` 保留本地 `setTechCheckBoxStyle` |
| `src/merge_dialog.cpp` | 保留本地深色 QSS、深色 `QFileDialog` 样式与 `pickDirectory()`、字体 18px |

### 算法/参数页

| 文件 | 处理方式 |
|---|---|
| `src/params/paramslang.cpp` | 唯一冲突块两侧文本相同（仅缩进），取上游结构；本地译文（形选/深度学习）在冲突区外完整保留 |
| `src/rgb/schemeparams/background.cpp` | 保留本地颜色 +/− 的 `setTechFineAdjustStyle(1)/(2)` |
| `src/rgb/schemeparams/lamp.cpp` | 保留 `setTechFineAdjustStyle(2)` 与左/右箭头切换逻辑 |
| `src/rgb/schemeparams/arithenable.cpp` | 2 处均取本地 17px 卡片字号（上游为 15px 普通字号，无等价主题实现） |
| `src/aidevicewidget.cpp` | 保留本地深色下载进度框所需头文件；背景取上游 `back_deep_clear.png`（统一深色主题） |

---

## 三、验证结果

### 单文件编译（ARM 交叉编译，`Makefile.Release`）

全部冲突涉及的编译单元逐一编译通过（RC=0）：

```
systeminitialization  aimodelset      aismainwidget   globalflow
loadingwindow         main            mainwidget      merge_dialog
mycurve               udpimagereceiver aidevicewidget rgb/aimainwidget
rgb/schemeparams/arithenable  rgb/schemeparams/background
rgb/schemeparams/lamp  params/paramslang  common/mymutex  common/mywid
```

### 全量构建

- 已在构建机 `192.168.172.182:/home/root123/DuySorter_src` 启动 `./qtmakeApp.sh release 6SXZ-600G_335X`
- 日志：`/tmp/merge_build.log`（全量 distclean + 重编，预计约 1 小时）
- 结果：**构建成功**（0 error）
  - 产物：`rel/duyiSorter_6SXZ-600G_335X_V1.06_35.tar.xz`（69,958 KB）
  - 版本：`V1.06_35`
  - 构建机产物目录：`/home/root123/DuySorter_src/rel/`

### 合并后构建问题修复

| 问题 | 原因 | 修复 |
|---|---|---|
| `Project ERROR: Unknown module(s) in QT: ftp` | 上游 `.pro` 为 QFtp 引入 `QT += ... ftp`，而合并后 AI 上传已改用 libssh2 SFTP（无 QFtp 代码），交叉编译工具链也没有 QtFtp 模块 | `DuySorter.pro` 改为 `QT += network widgets sql` |
| `lrelease error: Unexpected tag <message> at i18n/en.ts:8093` | 合并脚本把上游独有条目追加到 `</context>` 之后、`</TS>` 之前，形成根级 `<message>` | 用 `<context><name>communication</name>` 包裹后重新 lrelease |

---

## 四、遗留风险与后续建议

1. **状态信息页**：放弃本地自绘矩阵后，RK3566 上接口板/相机状态格子为上游 QLabel 方案，数量多时重绘负载可能高于本地自绘版本，需实机观察帧率。
2. **翻译完整度**：`zh-cn.ts` 仍有 471 条、`en.ts` 229 条 `type="unfinished"`（主要来自上游新增功能）；本次合并追加 2 条英文未翻译条目。
3. **上传协议**：本地 AI 模型上传固定 SFTP:22；若现场仍为 FTP:21 会连接失败，需按现场确认。
4. **布局偏移**：`aimodelset` 的 `PAGE_CONTENT_OFFSET=60` 依赖顶部提示行实体控件，当前代码中未找到对应控件，可能存在留白。
5. **顶部图标尺寸**：保留本地 48px，与上游 60/80px 不一致；若后续上游顶栏高度调整，需要同步复核。
6. **背景图**：`mainwidget`、`aismainwidget` 按用户此前要求恢复 `back_pure_clear.png`；`aidevicewidget` 等页面沿用上游 `back_deep_clear.png`。
7. **冗余资源**：`res/png2/state_page/*` 已无代码引用，可在确认不再恢复自绘矩阵后清理。

---

## 五、回滚方法

如需回滚本次合并（合并提交尚未生成时）：

```bash
git merge --abort     # 合并进行中
```

合并提交生成后可回滚到合并前：

```bash
git reset --hard 9fb8543     # 注意：会丢弃合并结果，仅限未推送时使用
```
