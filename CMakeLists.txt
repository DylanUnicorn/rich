cmake_minimum_required(VERSION 3.16)
project(RichGame VERSION 1.0.0 LANGUAGES C)

# 设置C标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /WX /utf-8)  # 高警告级别，警告当作错误，使用UTF-8编码
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)  # GCC/Clang编译选项
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含头文件目录
include_directories(include)

# 收集源文件
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

# 创建主要的游戏库
add_library(RichGameLib ${SOURCES} ${HEADERS})
target_include_directories(RichGameLib PUBLIC include)

# 创建主执行文件
add_executable(RichGame src/main.c)
target_link_libraries(RichGame RichGameLib)

# 启用测试
enable_testing()

# 简单的C测试支持
# 添加测试目录
# add_subdirectory(tests)

# 在Linux/Unix上，将 Bash 测试脚本接入到 CTest
if(UNIX)
    add_test(
        NAME run_bash_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND bash tests/run_tests.sh
    )
endif()

# 安装配置
install(TARGETS RichGame DESTINATION bin)
install(TARGETS RichGameLib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# CPack配置（用于打包）
include(CPack)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})