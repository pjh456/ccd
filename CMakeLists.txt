cmake_minimum_required(VERSION 3.5)

project(CCloneDetector LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

# file(GLOB_RECURSE CCD_HEADERS
# ${PROJECT_SOURCE_DIR}/include/*.h
# )

# include_directories(${PROJECT_SOURCE_DIR}/include)
# add_library(ccd STATIC ${CCD_HEADERS})
add_subdirectory(src)

add_executable(ccd_cli src/main.c)
target_link_libraries(ccd_cli ccd)

enable_testing()
add_subdirectory(tests)