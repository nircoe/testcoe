cmake_minimum_required(VERSION 3.22)
project(testcoe VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(cmake/testcoe_config.cmake)

option(TESTCOE_BUILD_EXAMPLES "Build the examples" OFF)
option(TESTCOE_BUILD_TESTS "Build the testcoe test suite" OFF)

if(MSVC)
    add_compile_options(/Z7)
    add_link_options(/DEBUG)
    # Release builds keep debug info too, stack traces need it
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Z7")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
else()
    add_compile_options(-g3 -fno-omit-frame-pointer)
endif()

testcoe_fetch_googletest()
testcoe_ignore_external_warnings(gtest_main)

testcoe_detect_stacktrace()
testcoe_generate_config_header()

add_subdirectory(include)
add_subdirectory(src)

if(TESTCOE_BUILD_EXAMPLES)
    add_subdirectory(examples)

    if(TESTCOE_BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()
