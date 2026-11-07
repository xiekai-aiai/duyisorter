# DuySorter 主程序完整启动流程（函数级）

---

## 一、main() 入口 → initAll()

**文件**: `src/main.cpp:197`

```
main()
 ├── QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs)
 ├── myApplication a(argc, argv)                    // QApplication 构造
 ├── LOG_INIT(CNF_LOG_CFG)                           // 初始化 unilog 日志库
 ├── SQLiteMgr::Instance().Init()                    // 初始化 SQLite 数据库
 ├── ConfigMgr::Instance().Init()                    // 初始化 AI 配置
 ├── QFontDatabase::addApplicationFont(...) × 10    // 注册 HarmonyOS Sans 字体
 ├── struGsh.nAuthenticationLevel = OPERATOR         // 默认操作员权限
 ├── 显示 Loading.gif 启动动画
 ├── myFlow.initAll()                                // ⭐ 核心初始化（见第二节）
 ├── a.setFont(getFont())                            // 设置全局字体
 ├── MainWidget w                                    // 创建主窗口（UI 在此构建）
 ├── createSysTable() / createCodeTable() / ...     // 数据库建表
 ├── QTimer::singleShot(2000) → w.showFullScreen()   // 2秒后显示主窗口
 └── a.exec()                                        // 进入 Qt 事件循环
```

**注**: main() 本身不调 initCommunication / initSendAllParams。这两个在 MainWidget 构造函数或后续 UI 事件里调用。

---

## 二、initAll() — 配置加载 + 硬件初始化

**文件**: `src/globalflow.cpp:4731`

```
initAll()
 ├── getSetting()                 // 读取配置文件 (.ini) → struCnfg (机器配置)
 ├── getParamsFileStaus()         // 读取参数文件状态 → struGsh
 ├── getEngineer()                // 读取工程参数 → struCnfe (算法总数等)
 ├── getGlobal()                  // 读取全局保存参数 → struGsh
 ├── setLang()                    // 设置语言
 ├── getCamera()                  // 读取相机参数 → struCnfc (相机通道划分)
 ├── getProfile()                 // ⭐ 读取方案参数 → struCnfp (核心！物料参数/算法使能)
 │   └── paramsprofile.cpp:21  struCnfp.nMatAssembleMode = setting.value(...)
 │   └── paramsprofile.cpp:3166 nMatAssembleMode 默认=PARAMS_ALL_SAME(2)
 ├── getPrefixedName()            // 获取预制方案父子节点名
 ├── initGsh()                    // 初始化全局共享变量 → struGsh (bStatFeed=0 等)
 ├── initSerial()                 // ⭐ 打开串口设备
 │   └── com1=/dev/ttyS1, com2=/dev/ttyS2, com3=/dev/ttyS3
 ├── initMachineType()            // 发送 CMD_INT_VALVE_COMBIN_TYPE → 喷阀组合类型
 ├── initUdpImagPara()            // AI板: 发送图像高度参数 (enableAi=false 跳过)
 └── initPixelImagPara()          // AI板: 发送像素划分参数 (enableAi=false 跳过)
```

**关键数据结构**:
| 变量 | 含义 | 加载来源 |
|------|------|---------|
| `struCnfg` | 机器配置（层数/相机数/接口板数） | .ini 配置文件 |
| `struCnfp` | 方案参数（物料/算法使能/GREY sensMin） | .ini 方案文件 |
| `struCnfe` | 工程参数（算法总数 nArithmeticTotal） | .ini |
| `struCnfc` | 相机参数（通道划分 nChannelBegin/End） | .ini |
| `struGsh` | 全局共享状态（bStatFeed/版本号等） | 运行时 |

---

## 三、参数下发链（initSendAllParams → 硬件帧）

### 3.1 initSendAllParams() — 首次/重载方案时全量下发

**文件**: `src/globalflow.cpp:5262`

```
initSendAllParams()
 └── sendAllParamsRgb()
     ├── initInterfaceTransMode(0)     // 切换接口板到串口模式
     ├── initCamera()                  // 重置相机参数
     │   └── com1Write(CMD_UNIT_CAMERA_RESET, ...) × N
     ├── resetSortParams()             // ⭐ 核心: 物料/算法参数全量下发（见3.2）
     ├── resetControl()                // 重置控制板参数
     │   └── com2Write(CMD_CTRL_..., ...) × N
     └── resetAiModel()                // 重置AI参数 (enableAi=false 跳过)
```

### 3.2 resetSortParams() — 全组遍历，直接读 struCnfp 下发

**文件**: `src/globalflow.cpp:1915`

```
resetSortParams()
 ├── 遍历 i=0..nLevelTotal, j=0..nIdentifyGroupTotal:
 │   └── resetMaterial(i, j, bIsAllSens=1, nArithSeq=0, bIsReserved=1)
 │       // ⚠️ 直接遍历所有组！不管 nMatAssembleMode！
 ├── resetIntelSeq()                   // 智能物料组号方式
 ├── resetBackgroundColor()            // 背景颜色
 ├── resetArithEdgeCut()               // 算法边缘切除
 ├── resetEjectTime()                  // 喷阀时间
 ├── resetTick()                       // 剔除设置
 ├── resetArithmeticEnable()           // ⭐ 算法使能位图 CMD_UNIT_ARITHMETIC_ENABLE
 │   └── com1Write(CMD_UNIT_ARITHMETIC_ENABLE, ..., arg1=sParam1, arg2=sParam2, arg3=sParam3)
 └── 遍历所有 unit:
     └── com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL_ALL, ...)
```

### 3.3 resetMaterial() — 单组参数打包下发

**文件**: `src/rgb/material.cpp:2089`

```
resetMaterial(nLevelId, nGroupId, bIsAllSens, nArithSeq, bIsReserved)
 ├── getArithEnable(i, nGroupId, j) × 全部 level/arith
 │   // ⚠️ 硬编码读 struCnfp.nArithmeticEnableGroup[level][0][arith]
 │   //    忽略传入的 groupId！永远读 group 0！
 ├── 遍历 nArithBegin..nArithEnd (0..nArithmeticTotal):
 │   ├── if GREY_A enable:
 │   │   ├── materialParamsSet(CMD_UNIT_GREY_A, levelId, groupId, idx=0)
 │   │   │   └── materialParamsNewPacketOrganize(CMD_UNIT_GREY_A, ...)
 │   │   │       ├── 读 struCnfp.struGroupIdentify[level][group].struGreyColor[idx]
 │   │   │       ├── nMax = (sensMin * 0.255 + 0.5) / 1   // mode=0 选暗
 │   │   │       ├── sensBias = nSensBias[nUnitAddr][ARITH_GREY_A] * 0.255
 │   │   │       ├── com1Write(CMD_UNIT_NEW_GREY_A_1, UNIT, levelId, unitAddr,
 │   │   │       │              color=0, arg1=0, arg2=0, arg3=nMin, arg4=nMax, ...)
 │   │   │       └── com1Write(CMD_UNIT_NEW_GREY_A_2, UNIT, levelId, unitAddr,
 │   │   │                      row-1, col-1, (percent-1)/256, (percent-1)%256, ...)
 │   │   ├── materialParamsSet(CMD_UNIT_RESERVED_1, ...)  // if bIsReserved
 │   │   └── materialParamsSet(CMD_UNIT_RESERVED_2, ...)
 │   ├── if GREY_B enable: (同理 idx=1)
 │   ├── if DISCOLOR_A enable: (idx=2)
 │   ├── if DISCOLOR_B enable: (idx=3)
 │   └── ... 其他算法
 └── 结束
```

**unitAddr 映射**: `struCnfg.struLevelInfo[level].nUnitId[group]`
- G=0 (first_front) → unitAddr=0
- G=1 (first_rear)  → unitAddr=1

---

## 四、nMatAssembleMode 真正起作用的位置

### 4.1 nMatAssembleMode 的三个作用点

| 位置 | 作用 | 调用者 |
|------|------|--------|
| **① materialCopyAssemble** | 决定是否把 G0 参数复制给 G1/G2/...（影响 struCnfp 内存） | UI 改参数时 |
| **② materialResetGroupAssemble** | 决定 resetMaterial 是只调 G0 还是同步调其他组（下发时分组） | UI 改参数时 |
| **③ resetSortParams → resetMaterial** | ❌ **完全不看 nMatAssembleMode**，直接遍历所有组 | 开料时 (onOff) |

### 4.2 两个"分组"函数的逻辑

**materialCopyAssemble** (material.cpp:2870) — 内存复制：
```
case PARAMS_ALL_SEPARATE:   break; // 不复制，各组独立
case PARAMS_FRONT_REAR_SAME: G0 参数复制给 G1
case PARAMS_ALL_SAME:        G0 参数复制给 G1..G7
```

**materialResetGroupAssemble** (material.cpp:2383) — 下发分组：
```
case PARAMS_ALL_SEPARATE:   resetMaterial(G0) → break
                            // 不碰其他组！每组各自独立下发
case PARAMS_FRONT_REAR_SAME: resetMaterial(G0) + resetMaterial(G1)
case PARAMS_ALL_SAME:        resetMaterial(G0) + resetMaterial(G1..G7)
```

### 4.3 UI 改参数 vs 开料

| 场景 | 调用链 | nMatAssembleMode 是否生效 |
|------|--------|--------------------------|
| **UI 改 sensMin** | arith.cpp / arithenable.cpp | ✅ 生效 |
| | → materialCopyAssemble() | 内存复制 |
| | → materialResetGroupAssemble() | 按模式分组下发 |
| **开料 (onOff)** | onOff() → resetSortParams() | ❌ **完全不看** |
| | → 直接遍历 resetMaterial(i, j) | 每组读自己的 struGreyColor |

**关键结论**: onOff 开料时，resetSortParams **直接遍历全组读各自的 struGreyColor 下发**。所以：
- 如果 UI 上改 G0 sensMin=1000, G1 sensMin=0，设 PARAMS_ALL_SEPARATE
- materialCopyAssemble 不复制（保持独立）
- materialResetGroupAssemble 分别下发 G0 和 G1
- 然后 onOff → resetSortParams → 再次遍历全组，读各自内存下发
- **只要内存里 G1.struGreyColor[0].nSensMin=0，就一定下发 0**
- **如果 G1 被覆盖了，问题就在 materialCopyAssemble 之前**

---

## 五、开料完整流程 (onOff)

**文件**: `src/globalflow.cpp:3743`

```
onOff()
 ├── if bStatFeed == 1 (关料):
 │   ├── struGsh.bStatFeed = 0
 │   ├── resetFeeder(1)                    // com2Write 停供料
 │   ├── sleep(5)                          // 等物料走空
 │   ├── com1Write(CMD_INT_ONOFF, arg8=0)  × 接口板数
 │   ├── com1Write(CMD_UNIT_ONOFF, arg8=0) × 相机数
 │   └── startAiWorker(false)
 │
 └── else (开料):
     ├── struGsh.bStatFeed = 1
     ├── initMachineType()                 // CMD_INT_VALVE_COMBIN_TYPE
     ├── resetSortParams()                 // ⭐ 全组遍历 resetMaterial → 下发 GREY_A 等参数
     │   ├── resetMaterial(0,0,1,0,1)      // G0 unitAddr=0
     │   ├── resetMaterial(0,1,0,1,1)      // G1 unitAddr=1
     │   ├── resetArithmeticEnable()        // CMD_UNIT_ARITHMETIC_ENABLE (bitmap)
     │   └── ...
     ├── com1Write(CMD_INT_ONOFF, arg8=1) × 接口板数
     ├── com1Write(CMD_UNIT_ONOFF, arg8=1) × 相机数
     ├── startAiWorker(true)
     ├── sleep(1)
     ├── resetFeeder(0)                    // com2Write 供料电压/使能
     ├── resetFeeder(1)
     └── struGsh.bFlagAutowipe = 1         // 使能自动清灰
```

---

## 六、test_arm vs 主程序 — 差异点

### 6.1 test_arm 简化了什么

| 主程序 main() | test_arm main() | 影响 |
|--------------|----------------|------|
| LOG_INIT / SQLite / ConfigMgr | ❌ 跳过 | 无（enableAi=false） |
| initAll() | ✅ 调 | ✅ 一致 |
| **initCommunication()** | ✅ 调 | ✅ 版本查询 |
| **initAiCommunication()** | ✅ 调 | ✅ |
| MainWidget 构造（UI 事件链） | ❌ 无 | ⚠️ 可能漏某些参数初始化 |
| initSendAllParams() | ✅ 手动调 | ✅ |
| onOff() | ✅ 调 | ✅ |

### 6.2 test_arm 可以验证的关键点

```
test_grey_a_front_rear() 里手动设:
  struCnfp.struGroupIdentify[0][0].struGreyColor[0].nSensMin = 1000  // G0
  struCnfp.struGroupIdentify[0][1].struGreyColor[0].nSensMin = 0     // G1
  
然后:
  initSendAllParams()
    → resetSortParams()
      → resetMaterial(0,0,1,0,1) → 读 G0.struGreyColor → unitAddr=0 → GREY_A_1 arg5=FF (nMax=255)
      → resetMaterial(0,1,1,0,1) → 读 G1.struGreyColor → unitAddr=1 → GREY_A_1 arg5=00 (nMax=0)
  
如果 com1 帧里 unitAddr=1 的 arg5 不是 00 → 某步内存被覆盖了
```

---

## 七、关键函数速查表

| 函数 | 文件 | 行号 | 作用 |
|------|------|------|------|
| main() | main.cpp | 197 | 入口 |
| initAll() | globalflow.cpp | 4731 | 配置加载 + 串口打开 |
| initCommunication() | globalflow.cpp | 3085 | 查询 FPGA/接口板版本 |
| initSendAllParams() | globalflow.cpp | 5262 | 全量参数下发 |
| sendAllParamsRgb() | globalflow.cpp | 5267 | initSendAllParams 实现体 |
| resetSortParams() | globalflow.cpp | 1915 | 遍历全组 → resetMaterial |
| resetMaterial() | material.cpp | 2089 | 单组 GREY_A/GREY_B 打包下发 |
| materialParamsNewPacketOrganize() | material.cpp | ~210 | sensMin → nMax 计算 + com1Write |
| materialResetGroupAssemble() | material.cpp | 2383 | UI 改参数时按 nMatAssembleMode 分组下发 |
| materialCopyAssemble() | material.cpp | 2870 | UI 改参数时按 nMatAssembleMode 复制内存 |
| onOff() | globalflow.cpp | 3743 | 开料/关料（开料分支内部调 resetSortParams） |
| initMachineType() | globalflow.cpp | 4283 | 喷阀组合类型 |
| initSerial() | globalflow.cpp | 3729 | 打开 com1/com2/com3 |
| getArithEnable() | globalflow.cpp | — | 硬编码读 group 0 的算法使能 |
