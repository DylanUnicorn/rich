#!/bin/bash

# 大富翁游戏构建脚本
# 使用方法: ./build.sh [clean|test|debug|release|format|check]

set -e  # 遇到错误立即退出

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 清理构建目录
clean() {
    log_info "清理构建目录..."
    rm -rf "$BUILD_DIR"
    log_success "构建目录已清理"
}

# 配置CMake
configure() {
    local build_type=${1:-Release}
    log_info "配置CMake (构建类型: $build_type)..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    cmake .. -DCMAKE_BUILD_TYPE="$build_type" \
             -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    
    log_success "CMake配置完成"
}

# 构建项目
build() {
    local parallel_jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    log_info "构建项目 (并行任务数: $parallel_jobs)..."
    
    cd "$BUILD_DIR"
    cmake --build . --parallel "$parallel_jobs"
    
    log_success "项目构建完成"
}

# 运行测试
run_tests() {
    log_info "运行测试..."
    
    cd "$BUILD_DIR"
    ctest --output-on-failure --verbose
    
    local exit_code=$?
    if [ $exit_code -eq 0 ]; then
        log_success "所有测试通过"
    else
        log_error "测试失败"
        exit $exit_code
    fi
}

# 格式化代码
format_code() {
    log_info "格式化代码..."
    
    if ! command -v clang-format &> /dev/null; then
        log_warning "clang-format未安装，跳过代码格式化"
        return
    fi
    
    find "$PROJECT_DIR/src" "$PROJECT_DIR/include" "$PROJECT_DIR/tests" \
         -name "*.cpp" -o -name "*.h" | \
         xargs clang-format -i
    
    log_success "代码格式化完成"
}

# 检查代码格式
check_format() {
    log_info "检查代码格式..."
    
    if ! command -v clang-format &> /dev/null; then
        log_warning "clang-format未安装，跳过格式检查"
        return
    fi
    
    local format_issues=$(find "$PROJECT_DIR/src" "$PROJECT_DIR/include" "$PROJECT_DIR/tests" \
                          -name "*.cpp" -o -name "*.h" | \
                          xargs clang-format --dry-run --Werror 2>&1 || true)
    
    if [ -n "$format_issues" ]; then
        log_error "代码格式不符合规范:"
        echo "$format_issues"
        log_info "运行 './build.sh format' 来自动格式化代码"
        exit 1
    else
        log_success "代码格式检查通过"
    fi
}

# 静态代码分析
static_analysis() {
    log_info "运行静态代码分析..."
    
    if ! command -v cppcheck &> /dev/null; then
        log_warning "cppcheck未安装，跳过静态分析"
        return
    fi
    
    cppcheck --enable=all \
             --std=c++17 \
             --suppress=missingIncludeSystem \
             --suppress=unusedFunction \
             --error-exitcode=1 \
             --quiet \
             "$PROJECT_DIR/src" "$PROJECT_DIR/include"
    
    log_success "静态代码分析通过"
}

# 运行主程序
run_game() {
    log_info "运行游戏..."
    
    if [ ! -f "$BUILD_DIR/bin/RichGame" ]; then
        log_error "游戏可执行文件不存在，请先构建项目"
        exit 1
    fi
    
    cd "$BUILD_DIR"
    ./bin/RichGame
}

# 安装项目
install_project() {
    local prefix=${1:-"$PROJECT_DIR/install"}
    log_info "安装项目到: $prefix"
    
    cd "$BUILD_DIR"
    cmake --install . --prefix "$prefix"
    
    log_success "项目安装完成"
}

# 打包项目
package_project() {
    log_info "打包项目..."
    
    cd "$BUILD_DIR"
    cpack
    
    log_success "项目打包完成"
}

# 完整的CI流程
ci_pipeline() {
    log_info "开始CI流程..."
    
    clean
    configure "Release"
    build
    run_tests
    check_format
    static_analysis
    
    log_success "CI流程完成"
}

# 显示使用说明
show_help() {
    echo "大富翁游戏构建脚本"
    echo
    echo "使用方法: $0 [命令] [选项]"
    echo
    echo "命令:"
    echo "  clean          清理构建目录"
    echo "  debug          Debug模式构建"
    echo "  release        Release模式构建 (默认)"
    echo "  test           运行测试"
    echo "  format         格式化代码"
    echo "  check          检查代码格式和质量"
    echo "  run            运行游戏"
    echo "  install [路径] 安装项目"
    echo "  package        打包项目"
    echo "  ci             运行完整CI流程"
    echo "  help           显示此帮助信息"
    echo
    echo "示例:"
    echo "  $0 debug      # Debug模式构建并测试"
    echo "  $0 release    # Release模式构建"
    echo "  $0 ci         # 运行完整CI流程"
}

# 主函数
main() {
    case "${1:-release}" in
        "clean")
            clean
            ;;
        "debug")
            clean
            configure "Debug"
            build
            run_tests
            ;;
        "release")
            clean
            configure "Release"
            build
            ;;
        "test")
            if [ ! -d "$BUILD_DIR" ]; then
                configure "Debug"
                build
            fi
            run_tests
            ;;
        "format")
            format_code
            ;;
        "check")
            check_format
            static_analysis
            ;;
        "run")
            if [ ! -d "$BUILD_DIR" ]; then
                configure "Release"
                build
            fi
            run_game
            ;;
        "install")
            if [ ! -d "$BUILD_DIR" ]; then
                configure "Release"
                build
            fi
            install_project "$2"
            ;;
        "package")
            if [ ! -d "$BUILD_DIR" ]; then
                configure "Release"
                build
            fi
            package_project
            ;;
        "ci")
            ci_pipeline
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            log_error "未知命令: $1"
            show_help
            exit 1
            ;;
    esac
}

# 检查是否在项目根目录
if [ ! -f "$PROJECT_DIR/CMakeLists.txt" ]; then
    log_error "请在项目根目录运行此脚本"
    exit 1
fi

# 执行主函数
main "$@"