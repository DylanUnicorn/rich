cmake_minimum_required(VERSION 3.16)
project(RichGame VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# 创建主要的游戏库
add_library(RichGameLib ${SOURCES} ${HEADERS})
target_include_directories(RichGameLib PUBLIC include)

# 创建主执行文件
add_executable(RichGame src/main.cpp)
target_link_libraries(RichGame RichGameLib)

# 启用测试
enable_testing()

# Google Test集成
include(FetchContent)

# 尝试使用系统安装的GoogleTest，如果没有则下载
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "GoogleTest not found, downloading from GitHub...")
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/release-1.12.1.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # 创建别名以保持一致性
    if(NOT TARGET GTest::gtest_main)
        add_library(GTest::gtest_main ALIAS gtest_main)
        add_library(GTest::gtest ALIAS gtest)
        add_library(GTest::gmock ALIAS gmock)
        add_library(GTest::gmock_main ALIAS gmock_main)
    endif()
else()
    message(STATUS "Using system GoogleTest")
endif()

# 添加测试目录
add_subdirectory(tests)

# 安装配置
install(TARGETS RichGame DESTINATION bin)
install(TARGETS RichGameLib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# CPack配置（用于打包）
include(CPack)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})