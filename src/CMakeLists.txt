file(GLOB_RECURSE CCD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

list(FILTER CCD_SOURCES EXCLUDE REGEX "main\\.c$")

add_library(ccd STATIC ${CCD_SOURCES})

target_include_directories(ccd
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)