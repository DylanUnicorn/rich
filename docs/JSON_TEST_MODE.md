# JSON 测试模式（规范与约定）

本项目提供一个“JSON 测试模式”，用于在 CI 和本地自动化地回归验证核心玩法逻辑。该模式读取测试用例目录中的 preset.json、input.txt，并将最终状态写入 dump.json，由 Python runner 对比 expected_result.json 完成断言。

核心理念：测试模式尽量复用游戏现有逻辑，不在测试通道内“另起炉灶”写一套规则，从而更容易测出业务代码里的真实 bug。

## 用例目录结构

- preset.json：初始状态（玩家、房产、已放置道具、当前回合等）
- input.txt：命令序列（step、block、bomb、robot、dump、quit）
- expected_result.json：期望 dump.json（断言目标）

说明：自 2025-10 起，测试用例中不再包含 constraints.json 或 expected.json，统一仅保留 expected_result.json。

## 关键约定

1. ended/winner 一律由 PlayerManager 的真实逻辑推导。忽略 preset.json 中的 ended/winner 字段，保证跨实现的一致性。
2. 道具使用会消耗背包库存（并更新 total）：
   - block n：放置路障并消耗 roadblock（若有）
   - bomb n：放置炸弹并消耗 bomb（若有）
   - robot：清扫前方 1..10 格的炸弹/路障并消耗 robot（若有）
3. step n 的移动规则在测试模式中委托给已有的通用逻辑：
   - 处于医院或监狱的回合：本次不移动，仅调用 HospitalAndPrisonReduceOne 递减天数（归零则释放）
   - 逐格推进，遇炸弹立刻送医并清除炸弹；遇路障停在路障格；落在监狱格触发入狱
4. dump.json 字段：
   - players：资金、点数、位置、存活、道具数量、buff 计数（god/prison/hospital）
   - houses：仅输出 owner/level 合法的房产
   - placed_prop：map 上的炸弹/路障位置（用地块 id 编码）
   - game：now_player、next_player、ended、winner（winner 为玩家索引，不存在为 -1）

## 复用核心逻辑的原则

- 牢靠复用：
  - 医院/监狱回合扣减：HospitalAndPrisonReduceOne
  - 触发入狱：InPrison
  - 触发入院：InHospital
  - 房产/收费：GetTollFee、buy_land、upgrade_land（交互在交互模式下进行，测试模式只覆盖必要的非交互路径）
- 测试模式只做胶水：负责解析输入/输出与最小的流程控制，不重写规则。

## 运行

- 构建：项目根目录执行 tests/run_json_tests.sh，会自动 cmake/build 并批量运行所有 JSON 用例。
- 单例运行：直接执行 RichGame <case_dir>，其中 case_dir 包含上述三件套文件。

## 断言策略

- runner.py 会解析 dump.json 与 expected_result.json，采用“子集比较”：expected_result.json 必须是 dump.json 的子集，允许 dump.json 含有更多字段（例如新增的 god 顶层块）。
- 该策略可使旧用例在新增字段时保持稳定；若某字段值不稳定（例如与随机或冷却相关），建议不要在 expected_result.json 中包含该字段。

## 常见问题

- 为什么我的 expected 里机器人没消耗？从 2025-09 起，测试模式与实际规则对齐：使用 block/bomb/robot 都会消耗库存，请更新期望。
- preset 里 ended/winner 没生效？为了跨实现可移植，dump 的这两个字段一律从 PlayerManager 推导。
 - 为什么没有 constraints.json 了？用例收敛到固定期望文件 expected_result.json；若存在不稳定字段，请直接从期望中省略该字段以利用子集比较特性。
