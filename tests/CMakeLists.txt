# 测试CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# 收集测试源文件
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# 为每个测试文件创建单独的可执行文件
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    
    # 链接库，使用灵活的目标名称
    if(TARGET GTest::gtest_main)
        target_link_libraries(${test_name} 
            RichGameLib 
            GTest::gtest_main 
            GTest::gtest
            GTest::gmock
            GTest::gmock_main
        )
    else()
        # 回退到直接名称（如果使用FetchContent）
        target_link_libraries(${test_name} 
            RichGameLib 
            gtest_main 
            gtest
            gmock
            gmock_main
        )
    endif()
    
    # 添加到测试套件
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# 添加代码覆盖率支持（仅在Debug模式下）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_COMPILER_IS_GNUCXX)
        target_compile_options(RichGameLib PRIVATE --coverage)
        target_link_libraries(RichGameLib PRIVATE --coverage)
    endif()
endif()