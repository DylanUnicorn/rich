# 大富翁游戏 (Rich Game)

一个基于C++17开发的大富翁游戏，支持2-4人游戏，包含完整的测试框架和持续集成流程。

## 项目特性

- 🎮 **经典大富翁玩法**：支持购买地产、升级房屋、收取过路费等核心功能
- 🎯 **多角色选择**：钱夫人(Q)、阿土伯(A)、孙小美(S)、金贝贝(J)四个角色
- 🏗️ **现代C++**：使用C++17标准，采用面向对象设计
- 🧪 **完整测试**：基于GoogleTest的单元测试框架
- 🔄 **持续集成**：GitHub Actions自动化构建、测试和代码质量检查
- 📦 **跨平台**：支持Windows、Linux、macOS

## 快速开始

### 环境要求

- CMake 3.16+
- C++17兼容的编译器
  - GCC 7+
  - Clang 5+
  - MSVC 2019+

### 构建项目

```bash
# 克隆项目
git clone <repository-url>
cd rich

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel

# 运行游戏
./bin/RichGame
```

### 运行测试

```bash
# 在build目录中运行所有测试
ctest --output-on-failure --verbose

# 或者运行特定测试
./bin/test_player
./bin/test_game_config
./bin/test_player_manager
```

## 项目结构

```
rich/
├── CMakeLists.txt          # 主CMake配置文件
├── README.md               # 项目说明文档
├── .gitignore             # Git忽略规则
├── .clang-format          # 代码格式化配置
├── .github/workflows/     # GitHub Actions CI/CD配置
│   └── ci.yml
├── include/               # 头文件目录
│   ├── Player.h           # 玩家和玩家管理器类
│   └── GameConfig.h       # 游戏配置类
├── src/                   # 源代码目录
│   ├── main.cpp           # 主程序入口
│   ├── Player.cpp         # 玩家类实现
│   └── GameConfig.cpp     # 游戏配置类实现
├── tests/                 # 测试代码目录
│   ├── CMakeLists.txt     # 测试CMake配置
│   ├── test_player.cpp    # 玩家类测试
│   ├── test_player_manager.cpp  # 玩家管理器测试
│   └── test_game_config.cpp     # 游戏配置测试
├── build/                 # 构建输出目录（生成）
└── docs/                  # 文档目录
```

## 核心功能

### 1. 游戏初始化

#### 1.1 设置初始资金
- 支持1000-50000元范围内的初始资金设置
- 默认值为10000元
- 输入验证和错误处理

#### 1.2 角色选择
- 四个可选角色：钱夫人(Q)、阿土伯(A)、孙小美(S)、金贝贝(J)
- 支持2-4人游戏
- 防止重复选择同一角色

### 2. 玩家管理
- 玩家轮流机制
- 资金和点数管理
- 破产玩家移除
- 游戏状态维护

## 开发指南

### 代码规范

项目使用基于Google Style的代码规范，配置文件：`.clang-format`

```bash
# 格式化代码
find src include -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

### 添加新功能

1. 在`include/`目录添加头文件
2. 在`src/`目录添加实现文件
3. 在`tests/`目录添加对应测试
4. 更新CMakeLists.txt（如果需要）

### 测试指南

- 所有新功能必须包含单元测试
- 测试覆盖率目标：90%+
- 使用GoogleTest框架
- 测试文件命名规范：`test_<module_name>.cpp`

### 持续集成

每次提交都会触发：
- 多平台构建测试（Windows、Linux、macOS）
- 单元测试执行
- 代码格式检查
- 静态代码分析（cppcheck）
- 代码覆盖率报告

## 版本历史

### v1.0.0 (当前)
- ✅ 游戏初始化和角色选择
- ✅ 玩家管理系统
- ✅ 完整的测试框架
- ✅ CI/CD流程

### 规划中的功能
- 🔲 地图系统和移动机制
- 🔲 房产购买和升级
- 🔲 道具系统
- 🔲 特殊建筑事件
- 🔲 游戏胜利条件

## 贡献指南

1. Fork项目
2. 创建功能分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建Pull Request

## 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 联系方式

如有问题或建议，请提交Issue或联系开发团队。

---

**快速命令参考：**

```bash
# 构建
cmake -B build && cmake --build build --parallel

# 测试
cd build && ctest --output-on-failure

# 格式化
clang-format -i src/*.cpp include/*.h

# 静态检查
cppcheck --enable=all src/ include/
```
