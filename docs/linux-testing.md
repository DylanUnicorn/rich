# Linux 测试指南

本文介绍如何在 Linux 上构建并运行自动化测试。

## 先决条件
- CMake 3.16+
- GCC 或 Clang
- Bash, diff

## 构建
```bash
# 在仓库根目录
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

## 运行测试
```bash
# 在仓库根目录
bash tests/run_tests.sh
```

## 测试说明
- 使用 `tests/cmdlist_helloworld.txt` 作为标准输入
- 预期输出保存在 `tests/expected_helloworld.txt`
- 结果与实际输出对比保存在 `tests/checkresult.txt`

## 故障排查
- 若提示找不到可执行文件，请先执行构建步骤
- 若遇到换行或编码差异，确认 `expected_*.txt` 文件使用 `\n` 结尾
- 若 `sscanf_s` 编译失败，已通过条件编译在非 MSVC 下使用 `sscanf` 替代
