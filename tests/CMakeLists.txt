# 1. 找到 tests/ 下所有 .c 文件
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# 2. 为每个测试文件单独创建一个可执行文件
foreach(test_src ${TEST_SOURCES})
    # 提取文件名作为测试名
    get_filename_component(test_name ${test_src} NAME_WE)

    # 3. 创建可执行文件（输出目录设置到 build/tests）
    add_executable(${test_name} ${test_src})
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )

    # 4. 链接你的主库
    target_link_libraries(${test_name} ccd)

    # 5. 加入 CTest
    add_test(NAME ${test_name} COMMAND ${CMAKE_BINARY_DIR}/tests/${test_name})
endforeach()