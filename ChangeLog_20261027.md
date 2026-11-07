# DuySorter 代码变更清单 — 2026-10-27

**基于**: git diff 对比上次提交
**变更量**: 19 文件 +382 / -334
**变更主题**: MACHINE_CG 机型支持 + getArithEnable 修复 + 协议追踪日志 + 调试辅助

---

## 一、🔴 核心功能修复（必须提交）

### 1.1 getArithEnable 算法使能读取修复
**文件**: `src/globalflow.cpp:1583-1592`
**类型**: Bug Fix

```diff
-/* 根据物料参数设置模式, 获取某 level/group 下的算法使能 */
+/* 获取某 level/group 下的算法使能
+   返回指定 groupId 对应的算法使能位，支持前后视独立使能 */
 int GlobalFlow::getArithEnable(int level, int groupId, int arith)
 {
-    int mode = struCnfp.nMatAssembleMode;
-    switch (mode)
-    {
-    case PARAMS_ALL_SAME:
-        return struCnfp.nArithmeticEnableGroup[level][0][arith];
-    case PARAMS_FRONT_REAR_SAME:
-        return struCnfp.nArithmeticEnableGroup[level][groupId & ~1][arith];
-    case PARAMS_FIRST_SECOND_SAME:
-        return struCnfp.nArithmeticEnableGroup[level][groupId % 2][arith];
-    case PARAMS_ALL_SEPARATE:
-    default:
-        return struCnfp.nArithmeticEnableGroup[level][groupId][arith];
-    }
+    return struCnfp.nArithmeticEnableGroup[level][groupId][arith];
 }
```

**为什么改**:
- 旧代码 switch 里在 PARAMS_ALL_SAME 等模式下**硬编码 return [0]**，完全忽略传入的 groupId
- 所有调用方（resetMaterial / resetArithmeticEnable / materialParamsCopy）都传了正确的 groupId，但旧逻辑**故意丢弃**
- **改后**: 任何模式下都返回 `struCnfp.nArithmeticEnableGroup[level][groupId][arith]`，让调用方真正拿到指定 group 的使能位
- 这一行改动让前后视算法使能第一次真的可以独立控制

**连锁改动**: material.cpp 里 2 处调用方之前传的是硬编码 `0`，现在改成传 `nGroupId`:
- `resetMaterial()` 内: `getArithEnable(i, 0, j)` → `getArithEnable(i, nGroupId, j)` 
- `materialParamsCopy()` 内: `getArithEnable(i, 0, j)` → `getArithEnable(i, nGroupIdSrc, j)`

---

### 1.2 方案加载后算法使能归一化（临时 Hack）
**文件**: `src/params/paramsprofile.cpp:79-92`
**类型**: 兼容性修复

```cpp
+    /* 算法使能永远全局统一：旧方案里按组独立保存的使能归一化到每层 group 0，
+       否则加载方案/重启后各组算法使能会出现不一致 */
+    for (int lv = 0; lv < MAX_LEVEL; lv++)
+    {
+        for (int g = 1; g < MAX_GROUP_IDTNTIFY; g++)
+        {
+            for (int a = 0; a < ARITHMETIC_TOTAL; a++)
+            {
+                struCnfp.nArithmeticEnableGroup[lv][g][a] 
+                    = struCnfp.nArithmeticEnableGroup[lv][0][a];
+            }
+        }
+    }
```

**为什么加**: getArithEnable 修完后，如果加载的旧方案里 G0/G1 算法使能不一致，会导致启动后各组行为不同。这个补丁把所有 group 的使能统一到 group 0，保证行为一致。**将来如果真需要前后视算法独立开关，这段要移除**。

---

## 二、🟠 MACHINE_CG 机型支持扩展（必须提交）

在多个 switch-case 里给 MACHINE_CF 加了对应的 `case MACHINE_CG:`，让 CG 机型（带接口板的组合/平那台）复用 CF 的逻辑：

| 文件 | 函数 | 改动 |
|------|------|------|
| `src/globalflow.cpp:338` | `getGroupIdentify()` | `case MACHINE_CF: case MACHINE_CG:` |
| `src/globalflow.cpp:643` | `getGroupTick()` | 同上 |
| `src/globalflow.cpp:1807` | `resetBackgroundColor()` | 同上 |
| `src/globalflow.cpp:2667` | `resetFeeder()` | 同上 |
| `src/globalflow.cpp:2742` | `resetLamp()` | 删除一大段注释代码块，case MACHINE_CG 共享 MACHINE_CF 逻辑 |
| `src/globalflow.cpp:3293` | `getColorSelectGroupName()` | 加 MACHINE_CG |
| `src/globalflow.cpp:3331` | `getIdentifyGroupName()` | 加 MACHINE_CG |
| `src/globalflow.cpp:3478` | `getTickGroupName()` | 加 MACHINE_CG |
| `src/globalflow.cpp:3545` | `getLampName()` | 加 MACHINE_CG |
| `src/globalflow.cpp:4308` | `updateLampTotal()` | 加 MACHINE_CG；条件 `>16` 分支也加 MACHINE_CG 判断 |
| `src/globalflow.cpp:4343` | `updateLightSrcLampNum()` | 加 MACHINE_CG |
| `src/rgb/material.cpp:2383` | `materialResetGroupAssemble()` | 加 MACHINE_CG，逻辑同 CF |
| `src/rgb/material.cpp:2870` | `materialCopyAssemble()` | 加 MACHINE_CG，且用 `struLevelInfo[0].nIdentifyGroupTotal` 动态取组数，不再固定支持 0~7 |
| `src/rgb/aimainwidget.cpp:3313` | `setGlobalAIParam()` | 加 MACHINE_CG |
| `src/rgb/degausstime.cpp:114` | `onDegaussTimeTabChanged()` | 加 MACHINE_CG |
| `src/rgb/factoryset.cpp:418` | `factoryUpdateSlt()` | 加 MACHINE_CG |

**删注释块**:
- `globalflow.cpp resetLamp()` 里删除了 15 行被注释掉的 CF1/CI1 灯光 arg4/arg5 计算代码
- `paramscamera.cpp` 里删除了 5 行注释掉的 `nChannelEjectorBegin/End` 初始化代码

---

## 三、🟡 协议追踪日志（可选，调试完可删）

在 `src/rgb/material.cpp` 里**大量**添加了 `LOG_INFO_STM` 追踪日志，格式统一为 `LIC TRACK GROUP ALGO SET：[TAG] ...`。

**新增日志点**:

| TAG | 位置 | 打印内容 |
|-----|------|---------|
| `[RM-ENTER]` | resetMaterial 入口 | L/G/allSens/arith/range |
| `[RM-enable]` | resetMaterial 读算法使能后 | GREY_A/B, DISCOLOR_A/B 使能位 |
| `[RM-dispatch]` | resetMaterial 遍历到 ENABLED 算法时 | L/G/arith 哪个算法被下发 |
| `[RM-GREY_A]` | GREY_A 分支 | sensMin/sensMax/mode/color/row/percent |
| `[RM-GREY_B]` | GREY_B 分支 | 同上 |
| `[RM-DISCOLOR_A/B]` | DISCOLOR 分支 | discolor/mode/sensMin/sensMax |
| `[NewPkt-ENTER]` | materialParamsNewPacketOrganize 入口 | cmd/L/G/idx |
| `[NewPkt-GREY-RAW]` | GREY 参数计算前 | raw.color/mode/sensMin/sensMax + sensBias |
| `[NewPkt-GREY-BIAS]` | sensBias 加完后 | nMin/nMax 偏置前值 |
| `[NewPkt-GREY-FINAL]` | GREY 参数最终值 | sensBias_calced/nMin/nMax |
| `[NewPkt-COM1] GREY_A_1/2` | com1Write 前 | 要发给 FPGA 的完整参数 |
| `[NewPkt-CROSS]` | 差分杂质 | cross.color/sens/row/col/percent |
| `[NewPkt-DISCOLOR-RAW/FINAL]` | DISCOLOR 完整追踪 | discolor/mode/sensMin/sensMax + 偏置 + 最终 nMin/nMax/mainVice |
| `[RGA-ENTER]` | materialResetGroupAssemble | mode (nMatAssembleMode) |
| `[RGA-case0/1]` | 各分支 | 具体执行哪个 resetMaterial |
| `[CopyAssemble]` | materialCopyAssemble | ENTER + total groups |

**重构**: GREY_A/B 和 DISCOLOR_A/B case 块用 `auto &g = struCnfp...struGreyColor[idx]` 提取引用，消除大量重复的嵌套 `struCnfp.struGroupIdentify[...].struGreyColor[idx].nXxx` 调用。逻辑不变，可读性提升。

---

## 四、🟡 com1/com2 串口帧打印格式优化（可选）

**文件**: `src/myqextserialport.cpp`

### 4.1 开启 com2 打印
```cpp
// 原来
#define PRINT_FOR_DEBUG_2 0
// 改成
#define PRINT_FOR_DEBUG_2 1   // 临时开启，对比 test_arm / 主程序 com2 帧差异
```

### 4.2 printf → snprintf + fprintf(stderr)
com1Write 和 com2Write 里的调试打印从：
```cpp
printf("com1(0x%x) write >> ", nCmd);
for (int i = 0; i < 16; i++) printf("%02x ", sPacket[i]);
printf("\n");
fflush(0);
```
改成：
```cpp
char hexbuf[128];
int pos = snprintf(hexbuf, sizeof(hexbuf), "com1(0x%04X) >> ", (unsigned)nCmd);
for (int i = 0; i < 16 && pos < (int)sizeof(hexbuf) - 4; i++)
    pos += snprintf(hexbuf + pos, sizeof(hexbuf) - pos, "%02X ", (uchar)sPacket[i]);
fprintf(stderr, "%s\n", hexbuf);
fflush(stderr);
```

**改进**: 
- snprintf 避免 printf 多次调用的开销和格式串拼接问题
- 全部大写 hex（之前混用 `%02x`）
- stderr 缓冲更可靠

---

## 五、🟡 release 模式开启 DEBUG_OUTPUT（可选）

**文件**: `DuySorter.pro:18`
```qmake
// 原来
-                   DEBUG_OUTPUT=0
// 改成
+                   DEBUG_OUTPUT=1   // 临时让 release 版也开 PRINT_FOR_DEBUG 宏
```

**影响**: 让 release 编译产物里 PRINT_FOR_DEBUG_1/2 的 printf 日志也能编译进二进制。如果不开 DEBUG_OUTPUT=1，PRINT_FOR_DEBUG_1/2 宏虽然值为 1，但被 `#if DEBUG_OUTPUT == 1` 外层挡住了。

---

## 六、⚪ 其他小改动

| 文件 | 改动 | 类型 |
|------|------|------|
| `.gitignore` | 新增 `doc/` 忽略 | 配置 |
| `app/lib/libts.so` / `libts.so.0` | 二进制库更新（git 显示修改，具体内容未知） | 二进制 |
| `src/params/paramscamera.cpp:8` | `/* level camera details */` 缩进修正 | 格式 |

---

## 七、提交建议

### 建议分组提交（方便回滚）

```
commit 1: core/bugfix
  src/globalflow.cpp      ← getArithEnable + material.cpp 里两处硬编码 0 → nGroupId
  src/params/paramsprofile.cpp ← 算法使能归一化 hack（临时）

commit 2: machine_cg_support
  src/globalflow.cpp      ← 各函数加 MACHINE_CG case + resetLamp 删注释
  src/rgb/material.cpp    ← materialResetGroupAssemble / materialCopyAssemble 加 CG 支持
  src/rgb/aimainwidget.cpp
  src/rgb/degausstime.cpp
  src/rgb/factoryset.cpp

commit 3: trace_logs (调试完可 revert)
  src/rgb/material.cpp    ← 所有 LOG_INFO_STM 追踪日志
  src/myqextserialport.cpp ← PRINT_FOR_DEBUG_2 + printf→fprintf
  DuySorter.pro           ← DEBUG_OUTPUT=1

commit 4: misc
  .gitignore              ← 加 doc/
  src/params/paramscamera.cpp ← 缩进 + 删注释
  app/lib/libts.so*       ← 一起提或单独提
```

### 必须回归测试

| 测试项 | 目的 |
|--------|------|
| **单视 CF/CI 机型** | getArithEnable 改后，原 PARAMS_ALL_SAME 场景是否仍然行为一致？（应该一致，因为所有 group 使能都从 ini 加载时就被归一到 group 0 了） |
| **双视 CF/CI 机型** | 前后视独立参数（sensMin 不同）是否真的独立生效？ |
| **CG 机型（带接口板）** | 新增的 MACHINE_CG case 路径是否覆盖完全？ |
| **方案加载/重载** | 加载旧方案、切换方案后算法使能状态是否正确？ |

---

## 八、⚠️ 已知待清理项（调试完后要 revert）

| 项 | 为什么要 revert | 什么时候 revert |
|----|----------------|----------------|
| `PRINT_FOR_DEBUG_2 = 1` | com2 打印在正式版本不需要 | 对比完主程序 / test_arm 的 com2 帧后 |
| `DuySorter.pro DEBUG_OUTPUT=1` | release 版正式应该关调试输出 | 确认 com 帧格式正确后 |
| material.cpp 里所有 `LOG_INFO_STM("LIC TRACK ...")` | 调试追踪日志，不影响功能但会大量刷日志 | GREY_A 前后视独立问题定位完后 |
| `paramsprofile.cpp` 里算法使能归一化 hack | 将来真要支持前后视算法独立开关时会成为障碍 | 永远保留或将来支持独立开关时移除 |
