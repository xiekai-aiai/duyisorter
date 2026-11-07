# 多机型非深度学习算法下发异常 — 排查结论与修复规格

- 日期：2026-10-24
- 状态：**待评审，未修改任何代码**（按用户要求“先不改代码”）
- 当前基线：`/Users/thinklic/Desktop/duyisorter`
- 参考基线：`/Users/thinklic/Desktop/DuySorter_rk3566`（用户给出的 `/home/a64/Documents/arm/sigmastar/DuySorter_rk3566` 在本机不存在，编译机 192.168.172.134 / 192.168.172.174 均不可达，暂用本地副本；**需确认是否为同一份代码**）

## 1. 现象

算法参数多次、按不同配置下发后，非深度学习算法工作不正常。

## 2. 排查结论（按优先级）

### P0（直接对应“多机型非深度学习算法下发协议调整”）：协议路由分支缺失

对比函数：`materialParamsSet`

- 参考：`src/rgb/material.cpp#L2382-L2423`
- 当前：`src/rgb/material.cpp#L2057-L2078`

参考实现的路由：

| 条件 | 使用协议 |
| --- | --- |
| `MACHINE_CI` 且偶数组 | 新协议（T3） |
| `SENSOR_T_2564 / D2 / P5` + `MACHINE_TD / T1 / T2` | **老协议（T2）** |
| 同上 + `ANCOO_RDC / RC / RSC` | 默认老协议，仅选芽 `CMD_UNIT_EARTHPEA_PARAMS1_MISC / 2_MISC` 走新协议 |
| 其它机型 | 新协议（T3） |
| `SENSOR_T_2566` / 其它传感器 | 老协议（T2） |

当前实现：`SENSOR_T_2564 / D2 / P5` 下**所有机型一律走新协议**，`TD / T1 / T2` 与彩色米机特例全部丢失。

后果：对 TD/T1/T2（多次/茶叶）和 ANCOO 米机，非深度学习算法仍以 `CMD_UNIT_NEW_*` 下发，而 FPGA 只把 `CMD_UNIT_*` 解释为对应算法参数块。参数没有进入正确的寄存器区，重复下发只会不断覆盖/残留错误状态，表现为“多次不同配置下发后工作不正常”。

对 CF/CG 的影响：参考代码中 CF/CG 同样走 `default → 新协议`，所以 **CF/CG 不受 P0 影响**。

### P1：分层算法使能来源错误（TD/T1/T2）

- 参考 `resetMaterial`：`src/rgb/material.cpp#L2434-L2674`，使能取值在 `#L2458-L2464`
  - 红外：`struCnfp.nArithmeticEnableInfra[j]`
  - TD/T1/T2：`struCnfp.nArithmeticEnableLevel[i][j]`（**按层**）
  - 其它：`struCnfp.nArithmeticEnable[j]`
- 参考 `materialParamsCopy` 同样按上述规则取使能（`#L3026-L3029`）
- 当前 `resetMaterial` / `materialParamsCopy` 统一调用 `myFlow.getArithEnable(i, nGroupId, j)`，而该函数固定读 `nArithmeticEnableLevel[level][0]`

后果：多层机型所有层继承第 0 层的使能状态，某一层的非深度学习算法可能根本不下发，或被错误下发；多次下发后各层状态互相覆盖。

对 CF/CG 的影响：当前设计要求“算法使能全局统一、只由 `setArithmeticEnable()` 写”，P1 属预期行为，不影响 CF/CG。

### P2：识别组范围不一致（0~7 vs 实际组数）

- 当前 `materialResetGroupAssemble`：`src/rgb/material.cpp#L2367-L2481`
  - `switch (nGroupId)` 只枚举 `case 0..7`，`default: break`
  - 因此 `nGroupId >= 8` 时**一条参数都不下发**
- 当前 `materialCopyAssemble`：`src/rgb/material.cpp#L2854-L2914`（已改为按 `nIdentifyGroupTotal` 循环，最多 16 组）
- 参考：CF 分支同样只处理 0..7，但参考为多组机型单独写了分支；当前仓库没有这些分支

后果：组数 > 8 时，拷贝发生了但 reset/下发被 `default` 吞掉，FPGA 永远收不到这些组的新参数；多次改参数后表现为“有时生效、有时不生效”。

现状约束：`setmachinemode.cpp` 只会生成 2/4/6/8 组，所以该问题在 CF/CG 正常配置下是**潜伏**的；手工改配置或导入其它机型方案才会触发。

### P3：`sensorPixelsQuo` 的 2048 条件不同

- 当前：`src/rgb/material.cpp#L37-L52`
  - `T_2564 + 1024` → 2
  - `T_2564 + 2048` → **无条件也置 2**
- 参考：`src/rgb/material.cpp#L37-L46`
  - `T_2564 + 1024` → 2
  - `T_2564 + 2048` 仅 `ANCOO_GENERAL / ANCOO_GARLIC` → 2，其余为 1

后果：2048 分辨率下，形状/长度类非深度学习算法的长度、面积上限被整体放大 2 倍，选长/选短、面积限制行为异常。该问题与机型无关，**CF/CG 使用 2048 传感器时同样会触发**，是“非深度学习算法不正常”的强候选。

### P4：单元地址映射简化

- 当前 `com1Write`：`src/myqextserialport.cpp#L276-L300`
  - `< MAX_UNIT_PER_INT` 原样；否则接口板地址 +1 并减 `MAX_UNIT_PER_INT`
- 参考：`src/myqextserialport.cpp#L257` 起
  - 额外包含 `ASSIST_BASE_ADDR(32)` 之后的辅配相机分支
  - 还包含 `OPT_PRODUCT_ANCOO_RS` 下特定查询命令 `nCount=1` 的处理

后果：辅配相机/多视机型的单元地址会算错，参数写到错误单元。

### P5：CI/红外分支缺失

参考存在 `CMD_INF_PARAMS1/2`、`struGreyColorInfra`、`ARITH_INFRA` 相关分支；当前仓库完全没有。若现场包含 CI/红外机型，红外算法参数不会下发。

## 3. 结论指向

- 若故障机是 TD/T1/T2/CI/LD/LDS 系列：**P0 + P1（+P4/P5）是根因**，必须补齐参考代码的机型路由与分层下发。
- 若故障机是 CF/CG（2/4/6/8 组、T_2564 1024）：P0/P1 不适用，重点查 **P3（2048 传感器）** 与 **P2（组数是否真的 > 8）**；建议现场打印 `nMachine / nSensorType / nSensorPixelLevel / nIdentifyGroupTotal / nMatAssembleMode` 五个运行时值确认。

## 4. 修复规格（待批准后执行）

### F1 机型枚举补齐

文件：`src/globalparams.h`（当前仅 `MACHINE_CF=1`、`MACHINE_CG=15`）

按参考仓库数值补齐，保证与 `cnf.global`/工程文件中的 `nMachine` 一致：

| 名称 | 数值 |
| --- | --- |
| CF | 1 |
| LD2 | 2 |
| LDS | 3 |
| TD | 4 |
| SD | 5 |
| T1 | 6 |
| T2 | 7 |
| CI | 11 |
| LD4 | 12 |
| CI2 | 13 |
| LDS2 | 14 |
| CG | 15 |

风险：枚举值必须与现场配置一致，否则会把机型识别错；执行前先读取现场 `cnf.global` 确认。

### F2 `materialParamsSet` 路由

文件：`src/rgb/material.cpp` `materialParamsSet`（当前 `#L2057-L2078`）

按参考 `#L2382-L2423` 落地：
- `MACHINE_CI && nGroupId % 2 == 0` → 新协议并 return
- `SENSOR_T_2564 / D2 / P5`：
  - `MACHINE_TD / T1 / T2` → `materialParamsPacketOrganize`（老协议）
  - `ANCOO_RDC / RC / RSC` → 默认老协议，仅两条选芽 `*_MISC` 走新协议
  - 其它 → `materialParamsNewPacketOrganize`（新协议）
- `SENSOR_T_2566` / 其它 → 老协议

若暂不支持 CI/ANCOO 彩色米机，可先只补 TD/T1/T2 分支，但需在代码注释中标注剩余分支未实现。

### F3 `resetMaterial` 使能与分层分支

文件：`src/rgb/material.cpp` `resetMaterial`（当前 `#L2089` 起）

- 使能来源改为参考规则（TD/T1/T2 用 `nArithmeticEnableLevel[i][j]`，其它用统一数组）
- 补 TD/T1 的按层 reset：
  - level 0 编辑、`PARAMS_ALL_SAME` 时同时 reset level 1/2 的组 0/1
  - level 1 / level 2 编辑时只 reset 本层
- 保留现有 `[RM-*]` 日志

### F4 `materialResetGroupAssemble` 组范围

文件：`src/rgb/material.cpp` `materialResetGroupAssemble`（当前 `#L2367-L2481`）

- `default` 改为：`nGroupId < nIdentifyGroupTotal` 时执行 `resetMaterial(0, nGroupId, ...)`，不再直接 break
- 或按实际 `nIdentifyGroupTotal` 生成 0..N-1 的显式分支
- 补 TD/T1/层分支（参考 `#L2953-L2982`）

### F5 `sensorPixelsQuo`

文件：`src/rgb/material.cpp` `#L37-L52`

- 2048 仅在 `ANCOO_GENERAL / ANCOO_GARLIC` 时置 2，其余保持 1
- 若当前仓库无 `nProduct` 枚举，需要同步补 `OPT_PRODUCT_ANCOO_GENERAL / GARLIC`（或按现场产品号等价判断）

### F6 `com1Write` 单元地址

文件：`src/myqextserialport.cpp` `#L276-L300`

按参考 `#L257` 起补齐 `ASSIST_BASE_ADDR` 辅配相机分支；`ANCOO_RS` 查询命令特例按需补。

## 5. 验证方案

1. 远程编译：`build_duyi`，要求 `EXIT_CODE=0`。
2. 板卡验证（每组配置至少连续下发 3 次不同参数）：
   - `全部独立 / 全部相同 / 前后视相同 / 一二相同` 四种模式
   - 编辑 G0、G1、G7（若支持 >8 组再测 G8/G15）
   - 日志核对：`[CopyAssemble]`、`[RGA-ENTER]`、`[NewPkt-*]`
   - 串口帧核对：非深度学习算法在 TD/T1/T2 上必须是 `CMD_UNIT_*` 老协议；CF/CG 必须是 `CMD_UNIT_NEW_*`
3. 2048 传感器：确认形状/长度参数不再整体翻倍。
4. TD/T1/T2：三层分别改参数，确认每层都收到 reset，且层间使能互不覆盖。

## 6. 执行前待确认

1. `/Users/thinklic/Desktop/DuySorter_rk3566` 是否与用户指定的 `/home/a64/...` 为同一份代码。
2. 现场故障机型与 `nMachine` 实际值（TD/T1/T2 还是 CF/CG）。
3. 是否使用 2048 分辨率传感器。
4. 是否存在 >8 个识别组的配置。
