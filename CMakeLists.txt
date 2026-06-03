cmake_minimum_required(VERSION 3.12)

project(MISC-I)

set(CMAKE_C_STANDARD 23)

add_library(MISC-I SHARED src/vm.c src/pcie.c)
add_executable(misc src/main.c src/vm.c src/pcie.c)

include(GenerateExportHeader)
generate_export_header(MISC-I BASE_NAME misc_i)
target_include_directories(MISC-I PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported)
if(lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    message(STATUS "LTO enabled")
else()
    message(STATUS "LTO unsupported")
endif()

string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3 -march=native -funroll-loops -fstrict-aliasing")