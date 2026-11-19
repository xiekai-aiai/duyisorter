# Spec：视频预览不通修复（AI 配置缺省值 + 接收端高度防护）

## 1. 目标

恢复视频页「开始」后的实时预览：

1. 板卡 `config` 表为空时，`ConfigMgr` 不再把所有高度读成 0，而是采用 AI 板 `param_config.json` 的默认值（64 / 64 / 500 / 256 / 0），并回写 `config` 表。
2. `UdpImageReceiver` 不再接受非法高度（0），避免 0 高 `QImage` 上 `scanLine(0)` 越界写、`cur_row == height_` 永不出帧。
3. `MyVideo::startCapture` 在配置为 0 时有明确兜底与错误日志，保证预览链路可启动。

## 2. 已确认证据（RK 板 + AI 板）

| 来源 | 证据 |
| --- | --- |
| RK 日志 | `udpimagereceiver.cpp:33 SetImageHegiht() is runnning:0, set height:0`；`start view, nunit:0, level:0` |
| RK 启动日志 | `initUdpImagPara() collect image h:0, infer image h:0, capture pic h:0, video h:0, dev num:2` |
| RK 数据库 | `select * from config` 返回空，`sqlite3` 确认 0 行 |
| AI 板日志 | `OnImgParamCmd() height is zero, collect_height:0, infer_height:0, img_view_height:0, video_view_height:0` |
| AI 板响应 | 下发全 0 → `code=0`（失败）；下发 64/64/500/256/0 → `code=1`（成功） |
| AI 板配置 | `conf/param_config.json`：collectH=64、imgViewH=500、inferH=64、videoViewH=256、slidingStep=0 |
| AI 板网络 | 已收到 `OnViewCmdNew body:0x01` → `SetViewMode() is view:1, host:192.168.4.2, port:19196` |
| AI 板 eth0 | `rx_bytes` 两秒内 5863875186 → 5863875186 不变（FPGA 当前未推相机图像） |

## 3. 根因

- **R1（主因，代码层）**：`ConfigMgr::InitAiCfgInfo` 对缺失键不做赋值，`AiCfgInfo` 默认成员全 0 → 启动时 `initUdpImagPara()` 向 AI 板下发 0 高度（AI 板拒绝）；点击视频页时 `SetImageHegiht(0)`。
- **R2（代码层）**：`UdpImageReceiver` 允许 `height_=0`。`resetStreamImage` 创建 0 高图像，`processDatagram` 中 `scanLine(cur_row)` 返回空指针仍 `memcpy` `bytesPerLine_` 字节；且 `cur_row == height_` 永远不成立，不会 `emit imageReady` → 黑屏。
- **R3（现场链路）**：AI 板 eth0 收包计数不增长，说明 FPGA 当前未推图像到 AI 板。即使 R1/R2 修复，若 FPGA 仍未推图，AI 板无帧可转发（需现场确认相机/采集板状态）。
- 次要：`ConfigMgr::Init()` 在 `LoadAllCfg` 失败时直接 return，缺省值更无机会生效。

## 4. 修改方案（3 个文件，最小改动）

### 4.1 `src/configmgr.cpp`

1. 匿名命名空间新增默认值常量（来源 AI 板 `conf/param_config.json`）：
   - `kDefaultCollectHeight = 64`、`kDefaultInferHeight = 64`、`kDefaultImgViewHeight = 500`、`kDefaultVideoViewHeight = 256`、`kDefaultSlidingStep = 0`。
2. `ConfigMgr::Init()`：`LoadAllCfg` 失败时只打 `LOG_ERROR_STM`，不再 `return false`，用空 map 继续初始化。
3. `ConfigMgr::InitAiCfgInfo()`：
   - 五个键逐个判断：存在则取库值；缺失则取默认值并置 `need_persist = true`。
   - 末尾先 `ai_cfg_info_ = info;`，若 `need_persist` 再调用 `SetAiCfgInfo(info)` 把缺省值写回 `config` 表（写失败也不影响内存值）。
   - 打一条 `LOG_WARN_STM("AI config missing in db, defaults applied ...")`。

### 4.2 `src/udpimagereceiver.cpp`

1. `SetImageWidth(int)` / `SetImageHegiht(int)`：`<= 0` 时 `LOG_ERROR_STM` 并直接返回，不覆盖原有合法值（构造默认 64）。
2. `resetStreamImage()`：`width_ <= 0 || height_ <= 0` 时 `LOG_ERROR_STM` 并返回，不构造非法 `QImage`。
3. `start()`：启动前校验 `width_ <= 0 || height_ <= 0`，非法则 `LOG_ERROR_STM` + `emit statusChanged(...)` 并返回，不 bind。
4. `processDatagram()`：`scanLine` 前增加 `state.cur_row >= height_` 防御（重置流状态后 return），杜绝越界写。

### 4.3 `src/myvideo.cpp`

`MyVideo::startCapture(bool)` 中：

```cpp
quint16 view_height = ConfigMgr::Instance().GetAiCfgInfo().video_view_height_;
if (view_height == 0) {
    view_height = 256;   // 与 AI 板 param_config.json 默认值一致
    LOG_ERROR_STM("video view height is 0, fallback to " << view_height);
}
udp_img_receiver->SetImageHegiht(view_height);
```

替换现有直接 `SetImageHegiht(ConfigMgr::...video_view_height_)` 一行。

## 5. 不改动项

- AI 板程序、`conf/svr_config.json`、`conf/param_config.json` 不改。
- UDP 协议、端口（9193 / 19196）、拼帧格式、`globalflow::initUdpImagPara` 不改（配置修好后其自然下发正确值）。
- `src/3rdparty/`、`.pro`、`sortertypes.h` 结构体不改。

## 6. 验证

1. 远程编译机（账号取 `.vscode/sftp.json`）执行
   `./qtmakeApp.sh release 6SXZ-600G_335X > /tmp/build_view.log 2>&1; echo EXIT=$?`，要求 `EXIT=0`、`error:` 0 条。
2. 部署后板卡启动日志：`initUdpImagPara()` 输出 `collect 64 / infer 64 / capture pic 500 / video 256 / sliding 0`，不再是 0。
3. 板卡 `sqlite3 sorter.db "select group_name,param_name,param_value from config"` 出现 5 行 AI 配置。
4. 点击视频页「开始」：日志 `set height:256`，无越界/崩溃；AI 板日志 `OnImgParamCmd` 四条高度非 0 且 `code=1`。
5. 若仍黑屏：检查 AI 板 `eth0` 的 `rx_bytes` 是否增长、AI 板进程日志是否持续转发视频包 —— 若仍不增长，判定为 FPGA 相机流未启动（现场相机/采集链路问题，非本代码改动范围）。

## 7. 风险

- 默认值取自 AI 板配置文件，若不同机型高度不同，需在「工厂设置 → AI 设置」页手工修改（本改动只在缺失时使用，不覆盖已存在值）。
- R3 为现场链路问题，本改动不能凭空恢复 FPGA 图像流，但可消除「配置为 0 → AI 拒绝 → 永远不会出图」的必然失败。
