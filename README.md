# 大富翁游戏 / Rich (Monopoly Game)

[![CI/CD Pipeline](https://github.com/DylanUnicorn/rich/actions/workflows/ci.yml/badge.svg)](https://github.com/DylanUnicorn/rich/actions/workflows/ci.yml)
[![Python 3.8+](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/downloads/)
[![License: Apache-2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

一个基于终端命令行的大富翁游戏，采用测试驱动开发(TDD)和敏捷开发方法论。

A terminal command-line based Monopoly game developed using Test-Driven Development (TDD) and Agile methodology.

## ✨ 特性 / Features

- 🎲 **骰子系统**: 单骰子和双骰子投掷功能 / Dice system with single and double dice rolling
- 🧪 **测试驱动开发**: 95%+ 测试覆盖率 / Test-driven development with 95%+ test coverage
- 🔄 **持续集成**: 自动化测试和质量检查 / Continuous Integration with automated testing and quality checks
- 📖 **敏捷开发**: 完整的敏捷开发文档和流程 / Agile development with comprehensive documentation and processes
- 🐍 **Python 3.8+**: 现代Python支持 / Modern Python support

## 🚀 快速开始 / Quick Start

### 安装依赖 / Install Dependencies

```bash
# 克隆项目 / Clone the repository
git clone https://github.com/DylanUnicorn/rich.git
cd rich

# 安装依赖 / Install dependencies
pip install -r requirements.txt
```

### 运行测试 / Run Tests

```bash
# 运行所有测试 / Run all tests
python -m pytest

# 运行带覆盖率的测试 / Run tests with coverage
python -m pytest --cov=src --cov-report=term-missing

# 运行特定测试 / Run specific tests
python -m pytest tests/test_dice.py -v
```

### 使用骰子功能 / Using Dice Functionality

```python
from src.game.dice import Dice

# 创建骰子实例 / Create dice instance
dice = Dice()

# 投掷单个骰子 (1-6) / Roll single dice (1-6)
result = dice.roll()
print(f"单骰子结果: {result}")

# 投掷双骰子 (2-12) / Roll double dice (2-12)
double_result = dice.roll_double()
print(f"双骰子结果: {double_result}")

# 获取最后一次投掷结果 / Get last roll result
last_roll = dice.get_last_roll()
print(f"最后一次投掷: {last_roll}")
```

## 📁 项目结构 / Project Structure

```
rich/
├── src/                        # 源代码 / Source code
│   ├── __init__.py
│   └── game/                   # 游戏逻辑 / Game logic
│       ├── __init__.py
│       └── dice.py            # 骰子功能 / Dice functionality
├── tests/                      # 测试代码 / Test code
│   ├── __init__.py
│   └── test_dice.py           # 骰子测试 / Dice tests
├── docs/                       # 文档 / Documentation
│   └── AGILE_DEVELOPMENT.md   # 敏捷开发文档 / Agile development docs
├── .github/workflows/          # CI/CD 流水线 / CI/CD pipelines
│   └── ci.yml                 # GitHub Actions 配置
├── requirements.txt            # Python 依赖 / Python dependencies
├── pyproject.toml             # 项目配置 / Project configuration
└── README.md                  # 项目说明 / Project documentation
```

## 🧪 测试策略 / Testing Strategy

本项目采用测试金字塔策略：
This project follows the test pyramid strategy:

- **单元测试 (Unit Tests)**: 70% - 快速、隔离的功能测试
- **集成测试 (Integration Tests)**: 20% - 模块间交互测试  
- **端到端测试 (E2E Tests)**: 10% - 完整用户流程测试

### 测试覆盖率目标 / Test Coverage Goals

- 目标覆盖率: 95%+ / Target coverage: 95%+
- 当前覆盖率: 95% / Current coverage: 95%

## 🔄 开发流程 / Development Process

### 测试驱动开发 (TDD) / Test-Driven Development

1. **🔴 红色阶段**: 编写失败的测试 / Write failing tests
2. **🟢 绿色阶段**: 编写最少代码使测试通过 / Write minimal code to pass tests
3. **🔄 重构阶段**: 优化代码质量 / Refactor for code quality

### 分支策略 / Branching Strategy

- `main/master`: 生产就绪代码 / Production-ready code
- `develop`: 开发分支 / Development branch
- `feature/*`: 功能分支 / Feature branches
- `hotfix/*`: 热修复分支 / Hotfix branches

## 📈 持续集成 / Continuous Integration

我们的CI/CD流水线包括：
Our CI/CD pipeline includes:

- ✅ 自动化测试 (多Python版本) / Automated testing (multiple Python versions)
- 🔍 代码质量检查 (flake8, black, isort, mypy) / Code quality checks
- 🔒 安全扫描 (bandit, safety) / Security scanning
- 📊 测试覆盖率报告 / Test coverage reporting

## 🛠️ 开发工具 / Development Tools

### 必需工具 / Required Tools
- Python 3.8+
- pytest (测试框架 / Testing framework)
- pytest-cov (覆盖率 / Coverage)

### 推荐工具 / Recommended Tools
- black (代码格式化 / Code formatting)
- isort (导入排序 / Import sorting)
- flake8 (代码风格 / Code style)
- mypy (类型检查 / Type checking)

## 📋 迭代计划 / Sprint Planning

### Sprint 1: 基础架构 ✅ (已完成 / Completed)
- [x] 项目结构搭建
- [x] 测试框架配置
- [x] CI/CD 流水线设置
- [x] 骰子功能开发和测试

### Sprint 2: 游戏板块 🚧 (计划中 / Planned)
- [ ] 游戏板块类设计
- [ ] 位置和属性管理
- [ ] 板块测试套件

### 未来计划 / Future Plans
- Sprint 3: 玩家系统 / Player System
- Sprint 4: 游戏逻辑 / Game Logic  
- Sprint 5: 用户界面 / User Interface

## 📚 文档 / Documentation

- [敏捷开发文档 / Agile Development Documentation](docs/AGILE_DEVELOPMENT.md)
- [API 文档 / API Documentation](docs/API.md) (计划中 / Planned)
- [用户指南 / User Guide](docs/USER_GUIDE.md) (计划中 / Planned)

## 🤝 贡献指南 / Contributing

1. Fork 项目 / Fork the project
2. 创建功能分支 / Create feature branch (`git checkout -b feature/AmazingFeature`)
3. 提交更改 / Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. 推送分支 / Push to branch (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request / Open a Pull Request

### 代码质量要求 / Code Quality Requirements
- 测试覆盖率 > 90% / Test coverage > 90%
- 所有测试必须通过 / All tests must pass
- 代码风格检查通过 / Code style checks must pass
- 包含适当的文档 / Include appropriate documentation

## 📝 许可证 / License

本项目采用 Apache License 2.0 许可证。详情请参阅 [LICENSE](LICENSE) 文件。

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

## 👥 团队 / Team

- **开发团队 / Development Team**: 负责功能开发和测试
- **质量保证 / Quality Assurance**: 确保代码质量和测试覆盖率
- **DevOps**: 维护CI/CD流水线和部署流程

---

**最后更新 / Last Updated**: 2024年12月  
**版本 / Version**: v1.0.0  
**状态 / Status**: 开发中 / In Development 🚧
