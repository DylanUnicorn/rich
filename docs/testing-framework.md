# 大富翁游戏自动化测试框架设计

## 概述

本测试框架基于命令行输入输出模式，支持从文件读取测试命令序列，并验证程序输出结果。

## 测试命令格式

### 1. 基础命令

#### `testhelloworld`
- 功能：输出"Hello World!"
- 输出：`Hello World!`
- 用途：验证测试框架基本功能

#### `dump`
- 功能：输出当前游戏状态并结束程序
- 输出：`system dump complete`
- 用途：结束测试序列并输出状态信息

### 2. 命令列表文件格式 (.txt)

```
命令1
命令2
...
dump
```

示例 `cmdlist_helloworld.txt`:
```
testhelloworld
dump
```

### 3. 预期输出文件格式

对应的预期输出文件应包含每个命令的预期输出：

示例 `expected_helloworld.txt`:
```
Hello World!
system dump complete
```

## 测试执行模式

### 命令行参数模式
```bash
RichGame.exe testhelloworld
```
- 直接执行单个命令
- 适用于简单测试

### 标准输入重定向模式
```bash
RichGame.exe < cmdlist_case1.txt
```
- 从文件读取命令序列
- 适用于复杂测试场景
- 自动检测非交互模式

## 自动化测试脚本

### 测试脚本：`run_tests.bat`

```batch
@echo off
set GAME_EXE=..\build\bin\Release\RichGame.exe
set RESULT_FILE=checkresult.txt

# 运行测试
%GAME_EXE% < cmdlist_case1.txt > actual_output.txt

# 比较结果
fc actual_output.txt expected_case1.txt
if %errorlevel% equ 0 (
    echo case1 result PASS >> %RESULT_FILE%
) else (
    echo case1 result FAIL >> %RESULT_FILE%
)
```

### 结果文件格式：`checkresult.txt`

```
case1 helloworld result PASS
case2 gameflow result FAIL
case3 playeraction result PASS
```

## 扩展设计思路

### 未来支持的命令类型

1. **游戏初始化命令**
   ```
   preset user [AQSJ]
   preset map [n] [A|Q|S|J] [level]
   preset fund [A|Q|S|J] [number]
   preset credit [A|Q|S|J] [number]
   ```

2. **游戏运行命令**
   ```
   step [number]
   buy [yes|no]
   gift [bomb|barrier|robot|god]
   ```

3. **状态查询命令**
   ```
   dump
   status
   map
   ```

### 预期的dump输出格式

```
user AQS
map[0] A 1
map[1] Q 2
fund A 1000
fund Q 3000
credit A 30
credit Q 50
gift A bomb 1
userloc A 25
nextuser A
```

## 使用示例

### 1. 简单hello world测试

文件：`cmdlist_helloworld.txt`
```
testhelloworld
dump
```

预期输出：`expected_helloworld.txt`
```
Hello World!
system dump complete
```

运行：
```bash
RichGame.exe < cmdlist_helloworld.txt
```

### 2. 批量测试运行

```bash
cmd /c run_tests.bat
```

输出：
```
Starting automated tests...
Running hello world test with command list...
PASS: Hello world test with cmdlist
Test results saved to checkresult.txt
case1 helloworld result PASS
```

## 技术实现要点

1. **命令解析**：程序从stdin读取命令，逐行处理
2. **模式检测**：使用`isatty()`检测是否为交互模式
3. **输出格式**：保持一致的输出格式便于自动化验证
4. **错误处理**：未知命令给出明确错误信息
5. **跨平台**：使用标准C库保证Windows/Linux兼容性

## 下一步扩展

1. 添加更多游戏逻辑命令
2. 实现完整的dump状态输出
3. 支持多用户游戏测试场景
4. 添加性能测试和压力测试
5. 集成到CI/CD流水线中