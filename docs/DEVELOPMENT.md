# 大富翁游戏开发文档

## 架构设计

### 总体架构

项目采用分层架构设计：

```
Presentation Layer (UI)    -> main.cpp
Business Logic Layer       -> GameConfig, PlayerManager
Data Model Layer           -> Player, Game entities
```

### 核心类设计

#### 1. Player类
- **职责**：表示游戏中的单个玩家
- **属性**：角色、资金、点数、位置
- **方法**：资金管理、状态查询

#### 2. PlayerManager类  
- **职责**：管理所有玩家，处理玩家轮次
- **功能**：添加玩家、轮次切换、破产处理

#### 3. GameConfig类
- **职责**：处理游戏初始化配置
- **功能**：初始资金设置、玩家选择验证

## 设计模式

### 1. 管理器模式 (Manager Pattern)
- **应用**：PlayerManager类
- **优势**：集中管理玩家状态，便于维护

### 2. 枚举模式 (Enum Pattern)
- **应用**：Player::Character枚举
- **优势**：类型安全，避免魔法数字

### 3. RAII模式
- **应用**：资源管理
- **优势**：自动内存管理，异常安全

## 编码规范

### 命名约定

```cpp
// 类名：PascalCase
class PlayerManager {};

// 方法名：camelCase
void addPlayer();

// 变量名：camelCase + 下划线后缀（成员变量）
int currentIndex_;
int localVariable;

// 常量：全大写 + 下划线
const int MAX_PLAYERS = 4;

// 枚举：PascalCase
enum class Character {
    QIAN_FUREN = 1
};
```

### 文件组织

```
include/
├── Player.h           # 玩家相关类声明
├── GameConfig.h       # 配置相关类声明
└── Common.h          # 公共定义（未来）

src/
├── Player.cpp        # 玩家类实现
├── GameConfig.cpp    # 配置类实现
└── main.cpp         # 主程序
```

### 注释规范

```cpp
/**
 * @brief 类的简短描述
 * 
 * 详细描述类的功能和用途
 */
class ExampleClass {
public:
    /**
     * @brief 方法简短描述
     * @param param1 参数1描述
     * @param param2 参数2描述
     * @return 返回值描述
     */
    bool exampleMethod(int param1, const std::string& param2);

private:
    int memberVariable_;  ///< 成员变量描述
};
```

## 测试策略

### 测试分层

1. **单元测试**：测试单个类和方法
2. **集成测试**：测试类之间的交互
3. **系统测试**：测试完整的用户场景

### 测试命名规范

```cpp
TEST(ClassNameTest, MethodName_Condition_ExpectedResult) {
    // 测试实现
}

// 示例
TEST(PlayerTest, SetMoney_ValidAmount_UpdatesMoney) {
    // Arrange
    Player player(Player::Character::QIAN_FUREN, 10000);
    
    // Act
    player.setMoney(5000);
    
    // Assert
    EXPECT_EQ(player.getMoney(), 5000);
}
```

### Mock对象使用

```cpp
// 使用GoogleMock进行依赖注入测试
class MockInputStream : public std::istream {
    // Mock实现
};

TEST(GameConfigTest, PromptForMoney_MockInput_ExpectedBehavior) {
    MockInputStream mockInput;
    // 测试实现
}
```

## 持续集成流程

### GitHub Actions工作流

1. **触发条件**：
   - 推送到main/master/develop分支
   - 创建Pull Request

2. **构建矩阵**：
   - Ubuntu (GCC)
   - Windows (MSVC)
   - macOS (Clang)

3. **检查流程**：
   ```yaml
   steps:
   - Checkout代码
   - 安装依赖
   - CMake配置
   - 编译构建
   - 运行测试
   - 代码质量检查
   - 生成报告
   ```

### 代码质量门禁

- **编译**：必须无警告编译通过
- **测试**：所有单元测试必须通过
- **覆盖率**：目标90%+代码覆盖率
- **静态分析**：cppcheck检查通过
- **格式化**：clang-format检查通过

## 构建系统

### CMake配置说明

#### 主要配置项

```cmake
# C++标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /WX)  # 高警告级别
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
```

#### 目标定义

```cmake
# 游戏库
add_library(RichGameLib ${SOURCES})

# 主程序
add_executable(RichGame src/main.cpp)
target_link_libraries(RichGame RichGameLib)

# 测试程序
add_executable(test_player tests/test_player.cpp)
target_link_libraries(test_player RichGameLib gtest_main)
```

### 构建命令

```bash
# Debug构建
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Release构建
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 并行构建
cmake --build build --parallel

# 安装
cmake --install build --prefix ./install
```

## 错误处理策略

### 异常安全保证

1. **无异常保证**：不抛出异常的操作
2. **基本异常安全**：异常时对象保持有效状态
3. **强异常安全**：异常时恢复到调用前状态

### 错误码vs异常

```cpp
// 使用返回值表示操作结果
bool addPlayer(Character character, int money) {
    if (players_.size() >= MAX_PLAYERS) {
        return false;  // 返回错误码
    }
    // 执行操作
    return true;
}

// 严重错误使用异常
Player& getCurrentPlayer() {
    if (players_.empty()) {
        throw std::runtime_error("No players available");
    }
    return players_[currentIndex_];
}
```

### 输入验证

```cpp
// 参数验证
bool setInitialMoney(int money) {
    if (money < MIN_MONEY || money > MAX_MONEY) {
        return false;  // 参数不合法
    }
    initialMoney_ = money;
    return true;
}

// 状态验证
void nextPlayer() {
    if (players_.empty()) {
        return;  // 静默处理无效状态
    }
    currentPlayerIndex_ = (currentPlayerIndex_ + 1) % players_.size();
}
```

## 性能考虑

### 内存管理

- 使用RAII管理资源
- 优先使用栈分配
- 合理使用STL容器

### 算法复杂度

- 玩家查找：O(1)
- 玩家添加：O(1)
- 玩家移除：O(n)

### 优化建议

1. **预分配内存**：为已知大小的容器预分配空间
2. **避免不必要的拷贝**：使用const引用传参
3. **缓存计算结果**：避免重复计算

## 未来扩展

### 模块化设计

```
Core Module     -> 核心游戏逻辑
Map Module      -> 地图和移动系统
Property Module -> 房产管理系统
Item Module     -> 道具系统
Event Module    -> 特殊事件系统
UI Module       -> 用户界面
Network Module  -> 网络对战（未来）
```

### 插件架构

为未来扩展预留插件接口：

```cpp
class IGamePlugin {
public:
    virtual ~IGamePlugin() = default;
    virtual void initialize() = 0;
    virtual void processEvent(const GameEvent& event) = 0;
    virtual void cleanup() = 0;
};
```

### 配置文件支持

```cpp
class ConfigManager {
public:
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
    
    template<typename T>
    T getValue(const std::string& key) const;
    
    template<typename T>
    void setValue(const std::string& key, const T& value);
};
```

## 总结

本文档提供了大富翁游戏项目的完整开发指南，包括架构设计、编码规范、测试策略和构建流程。随着项目的发展，此文档将持续更新以反映最新的设计决策和最佳实践。