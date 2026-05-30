cmake_minimum_required(VERSION 3.12)

project(MISC-I)

set(CMAKE_C_STANDARD 23)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)
add_library(MISC-I SHARED ${SOURCES})

include(GenerateExportHeader)
generate_export_header(MISC-I BASE_NAME misc_i)
target_include_directories(MISC-I PUBLIC ${CMAKE_CURRENT_BINARY_DIR})