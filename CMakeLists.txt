cmake_minimum_required(VERSION 3.5)

project(CCloneDetector LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

file(GLOB_RECURSE CCD_HEADERS
    ${PROJECT_SOURCE_DIR}/include/*.h
)
file(GLOB_RECURSE CCD_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.c
)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(CCloneDetector STATIC ${CCD_SOURCES} ${CCD_HEADERS})

add_executable(ccd_cli src/main.c)
target_link_libraries(ccd_cli CCloneDetector)