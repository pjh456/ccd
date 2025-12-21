cmake_minimum_required(VERSION 3.5)

project(CCloneDetector LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

add_subdirectory(src)

add_executable(ccd_cli src/main.c)
target_link_libraries(ccd_cli ccd)

enable_testing()
add_subdirectory(tests)

file(GLOB_RECURSE CCD_SELF_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/src/*.c
)

foreach(test_file IN LISTS CCD_SELF_FILES)
    # 生成一个合法的测试名
    string(REPLACE "/" "_" test_name ${test_file})
    string(REPLACE "\\" "_" test_name ${test_name})

    add_test(
        NAME parse_${test_name}
        COMMAND ccd_cli -U ${test_file}
    )
endforeach()