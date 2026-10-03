cmake_minimum_required(VERSION 3.14)
project(testcoe VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    # Ensure debug symbols are generated and linked
    add_compile_options(/Z7)
    add_link_options(/DEBUG)
    # Keep debug info in release builds too for stack traces
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Z7")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Enhanced debug info for GCC/Clang
    add_compile_options(-g3 -fno-omit-frame-pointer)
endif()

include(cmake/utils.cmake)
include(cmake/stacktrace.cmake)
include(cmake/config_header.cmake)

# googletest
message(STATUS "[testcoe] Fetching GoogleTest v1.16.0")
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
ignore_external_warnings(gtest_main)

testcoe_detect_stacktrace()
generate_testcoe_config_header()

# testcoe
add_subdirectory(include)
add_subdirectory(src)

option(TESTCOE_BUILD_EXAMPLES "Build the examples" OFF)
option(TESTCOE_BUILD_TESTS "Build the tests" OFF)

if(TESTCOE_BUILD_EXAMPLES)
    add_subdirectory(examples)

    if(TESTCOE_BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

