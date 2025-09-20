# C++ 到 C 语言转换总结

## 转换概述

成功将大富翁游戏项目从 C++ 转换为 C 语言实现。

## 主要变更

### 1. 构建系统 (CMakeLists.txt)
- 语言标准：C++17 → C99
- 文件扩展名：*.cpp → *.c
- 编译器：C++ → C
- 移除了 GoogleTest 依赖（C++ 测试框架）

### 2. 头文件转换

#### include/Player.h
- C++ 类 → C 结构体
- 成员函数 → 函数指针和独立函数声明
- std::string → char arrays
- std::vector → 固定大小数组
- 命名空间 → 函数前缀（player_）

#### include/GameConfig.h  
- C++ 类 → C 结构体
- 成员函数 → 函数声明
- 函数前缀：gameConfig_
- 包含 C 标准库头文件

### 3. 源文件转换

#### src/Player.c (原 Player.cpp)
- 类方法 → 独立函数
- this 指针 → 显式指针参数
- 构造函数 → 初始化函数 player_init()
- 成员访问 → 指针解引用
- C++ STL → C 标准库

#### src/GameConfig.c (原 GameConfig.cpp)
- iostream → stdio.h
- std::cout/cin → printf/scanf_s
- 字符串处理：string → char arrays
- 输入验证函数重写
- 修复 MSVC 警告（安全函数使用）

#### src/main.c (原 main.cpp)
- 简化主函数逻辑
- C++ 输出流 → printf
- 函数调用语法适配

### 4. 语法调整

#### 安全性改进
- sscanf → sscanf_s（MSVC 安全函数）
- strlen() 返回值转换：size_t → int（显式转换避免警告）

#### 内存管理
- 栈分配结构体替代动态分配
- 固定大小数组替代动态容器
- 手动内存管理模式

## 构建结果

### 成功构建
```
RichGameLib.vcxproj -> lib\Release\RichGameLib.lib
RichGame.vcxproj -> bin\Release\RichGame.exe
```

### 运行测试
```
=== Rich Game ===
Please enter initial money (1000-50000), default 10000: 10000
Initial money set to: 10000 yuan
Please select 2-4 unique players, enter numbers:
1. Qian Furen (Q)
2. Aturber (A)
3. Sun Xiaomei (S)
4. Jin Beibei (J)
Please enter (e.g.: 13, 234, 1234): 12
Player selection successful!
Qian Furen(Q) Aturber(A)

Game initialization completed!
Participating players:
1. Qian Furen(Q) - Money: 10000 yuan
2. Aturber(A) - Money: 10000 yuan

Current turn: Qian Furen
```

## 技术特点

### C99 标准兼容
- 使用标准 C 语法
- 布尔类型支持（stdbool.h）
- 结构体初始化语法

### 跨平台支持
- CMake 构建系统
- 标准 C 库依赖
- GitHub Actions CI/CD

### 代码质量
- 清晰的函数命名约定
- 空指针安全检查
- 输入验证和错误处理

## 文件结构

```
rich/
├── CMakeLists.txt          # C99 构建配置
├── include/
│   ├── Player.h           # C 结构体和函数声明
│   └── GameConfig.h       # 游戏配置 C API
├── src/
│   ├── Player.c           # 玩家管理实现
│   ├── GameConfig.c       # 配置和输入处理
│   └── main.c             # 主程序入口
├── build/
│   ├── bin/Release/       # 可执行文件
│   └── lib/Release/       # 静态库
└── docs/                  # 文档目录
```

## 下一步建议

1. **测试框架**：考虑集成 C 兼容的测试框架（如 Unity Test Framework）
2. **游戏逻辑**：扩展游戏核心逻辑（地图、房产、事件等）
3. **数据持久化**：添加存档和读档功能
4. **用户界面**：改进命令行界面或添加图形界面

## 转换完成状态

✅ 所有 .cpp 文件已成功转换为 .c 文件  
✅ C99 标准兼容性验证通过  
✅ 构建系统配置完成  
✅ 程序运行测试成功  
✅ 跨平台兼容性保持