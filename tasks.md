# 任务清单 — 多机型非深度学习算法下发协议修复

> 状态：全部待用户批准后执行；当前未修改任何业务代码。

## T0 前置确认（必须最先完成）

- [ ] 确认 `/Users/thinklic/Desktop/DuySorter_rk3566` 与 `/home/a64/Documents/arm/sigmastar/DuySorter_rk3566` 为同一份代码（比对 `src/rgb/material.cpp` 行数与 `materialParamsSet` 内容）
- [ ] 现场读取并记录：`nMachine`、`nProduct`、`nSensorType`、`nSensorPixelLevel`、`nIdentifyGroupTotal`、`nMatAssembleMode`
- [ ] 对照现场 `cnf.global` / 工程文件确认机型枚举数值（CF=1、TD=4、T1=6、T2=7、CG=15）

## T1 机型枚举补齐

- [ ] `src/globalparams.h`：按参考数值补 `MACHINE_LD2=2 / LDS=3 / TD=4 / SD=5 / T1=6 / T2=7 / CI=11 / LD4=12 / CI2=13 / LDS2=14`
- [ ] 若只修 TD/T1/T2，可仅补 `TD=4 / T1=6 / T2=7`，其余保持现状并注释说明

## T2 协议路由

- [ ] `src/rgb/material.cpp` `materialParamsSet`（当前 L2057-2078）：补 TD/T1/T2 → 老协议分支
- [ ] 补 `ANCOO_RDC / RC / RSC` 特例（默认老协议，仅 `CMD_UNIT_EARTHPEA_PARAMS1_MISC / 2_MISC` 走新协议）
- [ ] 补 `MACHINE_CI && nGroupId%2==0` → 新协议并 return（如支持 CI）
- [ ] 确认 `SENSOR_T_2566` / default 仍走老协议

## T3 分层使能与 reset

- [ ] `resetMaterial`：TD/T1/T2 改用 `nArithmeticEnableLevel[i][j]`；其它保持统一来源
- [ ] `resetMaterial`：补 TD/T1 的按层 reset（level 0 ALL_SAME 时同步 level 1/2 的组 0/1）
- [ ] `materialParamsCopy`：使能取值与 resetMaterial 保持一致
- [ ] 保留 `[RM-*]` 日志并补充 layer 字段

## T4 识别组范围

- [ ] `materialResetGroupAssemble`（当前 L2367-2481）：`default` 分支改为下发 `resetMaterial(0, nGroupId, ...)`（受 `nIdentifyGroupTotal` 约束）
- [ ] 与 `materialCopyAssemble` 的组范围保持一致
- [ ] 若支持 TD/T1：补参考 L2953-2982 的层分支

## T5 2048 形状系数

- [ ] `materialParamsNewPacketOrganize`：2048 仅在 `ANCOO_GENERAL / ANCOO_GARLIC` 时 `sensorPixelsQuo=2`，其余为 1
- [ ] 如缺 `nProduct` 枚举，同步补 `OPT_PRODUCT_ANCOO_GENERAL / GARLIC`

## T6 单元地址

- [ ] `src/myqextserialport.cpp` `com1Write`（当前 L276-300）：补 `ASSIST_BASE_ADDR` 辅配相机地址分支
- [ ] 按需补 `OPT_PRODUCT_ANCOO_RS` 查询命令 `nCount=1` 特例

## T7 编译验证

- [ ] `ssh_connect` + `upload_files`
- [ ] `build_duyi`，确认 `EXIT_CODE=0`
- [ ] 若失败，仅修根因，最多 5 轮

## T8 板卡验证

- [ ] `board_connect`
- [ ] 四种参数模式各连续下发 3 次不同参数
- [ ] 校验串口命令字：TD/T1/T2 非深度学习算法为 `CMD_UNIT_*`；CF/CG 为 `CMD_UNIT_NEW_*`
- [ ] 校验组范围：编辑 G0/G1/G7（及 G8/G15，若适用）均有对应 reset 帧
- [ ] 校验 2048 下形状/长度参数不翻倍
- [ ] 校验 TD/T1/T2 每层均收到 reset，层间使能不互相覆盖
