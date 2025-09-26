# 已测试通过的功能一览（基于测试模式）

本文档汇总当前在“测试模式”下已覆盖并通过的功能点与用例，便于对齐《测试功能补充要求.md》，并作为回归清单。

更新时间：2025-09-26

---

## 1. 测试模式能力

- 启动方式：`./RichGame <test_case_dir>`
- 输入/输出：
  - 读取 `<test_case_dir>/preset.json`（可选）完成初始化
  - 从 `<test_case_dir>/input.txt` 读取指令序列
  - 执行 `dump` 时输出 `<test_case_dir>/dump.json` 并立即结束
- 已支持的测试指令：
  - `dump`：导出当前完整游戏状态并结束进程
  - `step <n>`：前进 n 步（n 可为任意大的正整数）
  - `quit`：退出（不导出 dump）
  - 扩展（测试模式辅助指令，便于构造局部状态）：
    - `block <offset>`：在当前位置的相对 offset（-10..10 且非 0）放置路障
    - `bomb <offset>`：在当前位置的相对 offset（-10..10 且非 0）放置炸弹
    - `robot`：清除当前位置后方 1..10 范围内的路障/炸弹（不消耗道具计数，测试模式定义）

## 2. preset.json 解析（已覆盖）

- players（数组）
  - `index`、`name(Q/A/S/J)`、`fund`、`credit`、`location`
  - `prop`：`{ bomb, barrier, robot, total }`
  - `buff`：`{ god, prison, hospital }`（均为剩余回合数），并同步 in_* 标志
- houses（对象，键为地块 id 的字符串）：`{ owner, level }`
- placed_prop：`bomb`（'@'）与 `barrier`（'#'）落盘
- game：`now_player` 生效（兼容 ended/winner 解析；可切换为忽略）

## 3. dump.json 输出（已覆盖）

- players：含 `prop`、`buff(god/prison/hospital)` 的真实数值
- houses：仅输出有所有者的地块
- placed_prop：当前炸弹与路障位置
- game：`now_player`、`next_player`、`ended`、`winner`

## 4. 核心逻辑（测试模式下的确定性实现）

- step 行为：
  - 有 hospital/prison 天数：各自倒计时减 1，本回合不移动
  - 否则逐步前进：
    - 炸弹（'@'）：立刻送医院(14)，并清除炸弹
    - 路障（'#'）：停到该格并结束本回合
    - 落地监狱（'P'）：入狱 3 天
- robot：清除 [1..10] 范围内炸弹/路障；不消耗玩家道具计数（测试模式）
- 倒计时归零时清除 in_hospital/in_prison 标志（缺陷已修复）

## 5. 用例清单与验证点（22/22 通过）

### _root
- simple_init
  - 无 preset；输入资金与选角后 dump
  - 验证：
    - 默认三位玩家创建顺序
    - 资金/点数/位置默认值
    - 道具为 0
    - buff 为 0
    - game 索引
- step_move
  - 无 preset；step 移动 5 格
  - 验证：
    - 当前位置按 70 取模移动
    - 无障碍交互
- preset_basic
  - 有 preset：两名玩家、房产与道具、放置障碍和炸弹，now_player=1
  - 输入：step 4；验证：
    - 位置变化
    - placed_prop 保持
    - houses 映射与 owner/level 正确

### buffs
- god_counter
  - preset 指定 `buff.god=5`
  - 验证：
    - dump buff.god 为 5，其他为 0
    - 不影响移动
- hospital_countdown
  - hospital=2，位置在医院 14
  - 验证：
    - 不移动
    - dump 时 hospital=2
- hospital_step_skip
  - hospital=1；输入 step 5
  - 验证：
    - 本回合不移动
    - hospital 倒数至 0
- hospital_release_then_move
  - hospital=1；输入 step 3、step 4
  - 验证：
    - 第 1 次不移动且 hospital→0
    - 第 2 次按规则移动到 18
- prison_countdown
  - prison=2
  - 验证：
    - dump 时 prison=2
    - 不移动
- prison_step_skip
  - prison=1；输入 step 6
  - 验证：
    - 本回合不移动
    - prison→0
- prison_release_then_move
  - prison=1；输入 step 1、step 2
  - 验证：
    - 第 1 次释放
    - 第 2 次移动 2 格

### movement
- barrier_stop
  - 放置 `block 3`；输入 step 5
  - 验证：
    - 遇路障停在 3
    - 路障保留
- bomb_hospital
  - 放置 `bomb 2`；输入 step 5
  - 验证：
    - 踩到炸弹→位置 14
    - 炸弹被清除
- land_on_prison
  - 从 47 step 2
  - 验证：落到 49（P）入狱 3 天
- large_step_wrap
  - 大步数跨越取模
  - 验证：位置按 70 环绕
- block_negative_wrap
  - `block -2` 于位置 1
  - 验证：放置在 69（跨 0 边界）
- bomb_negative_wrap
  - `bomb -2`，随后 step 70
  - 验证：绕行一圈时会踩到负向放置的炸弹并进医院
- multi_obstacle_path
  - 预置序列：前方 1 炸弹、2/6 路障；step 3
  - 验证：先命中炸弹进医院；障碍保留

### props
- place_and_clear
  - 输入：`block 3`、`bomb 5`、`robot`、`dump`
  - 验证：
    - 机器人清除 [1..10] 范围内的障碍/炸弹（均清空）
    - 玩家道具计数不变
- robot_clear
  - 预置多枚炸弹/路障在 [1..10] 范围内；输入 `robot`
  - 验证：全部被清除
- robot_range_boundary
  - 预置 10 与 11 的炸弹/路障；输入 `robot`
  - 验证：10 被清，11 保留

### houses
- owner_and_level
  - 预置多个地块 owner/level
  - 验证：dump 的 houses 映射正确；owner 按代号绑定到玩家

### victory
- winner_in_preset
  - 预置 game.ended=1、winner=1
  - 验证：dump 显示 ended/winner 字段（如需严格忽略解析可切换）

## 6. 运行方式

```bash
bash tests/run_json_tests.sh
```

- 构建并运行所有 JSON 用例；打印模块通过率与总通过率

## 7. 兼容性与说明

- 未提供的字段使用合理默认值（资金 10000、点数 0、位置 0）
- `game` 按规范仅 `now_player` 必须生效；当前兼容 ended/winner 的读取，可按需关闭
- 测试模式非交互、无随机数，便于稳定比对

---

如需将某些扩展指令（block/bomb/robot）从测试模式移除或改为严格的“道具消耗”逻辑，请提出要求，我会同步调整实现与用例。