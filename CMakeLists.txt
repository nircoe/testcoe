cmake_minimum_required(VERSION 3.14)
project(testcoe VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: Remove this when upgrading to a newer version of backward-cpp (newer than v1.6)
set(CMAKE_POLICY_VERSION_MINIMUM 3.14 CACHE STRING "" FORCE)

if(MSVC)
    # Ensure debug symbols are generated and linked
    add_compile_options(/Zi)
    add_link_options(/DEBUG)
    # Keep debug info in release builds too for stack traces
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Enhanced debug info for GCC/Clang
    add_compile_options(-g3 -fno-omit-frame-pointer)
endif()

include(cmake/utils.cmake)

# googletest
find_package(GTest QUIET)
if(GTest_FOUND)
    message(STATUS "[testcoe] Using existing GoogleTest installation")
    ignore_external_warnings(GTest::Main)
else()
    message(STATUS "[testcoe] GoogleTest not found, fetching from source")
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
endif()

# backward-cpp
set(BACKWARD_HAS_BFD 0 CACHE BOOL "Disable BFD support" FORCE)
set(BACKWARD_HAS_DW 0 CACHE BOOL "Disable libdw support" FORCE)
set(BACKWARD_HAS_DWARF 0 CACHE BOOL "Disable libdwarf support" FORCE)
set(BACKWARD_HAS_UNWIND 0 CACHE BOOL "Disable libunwind support" FORCE)
set(STACK_WALKING_UNWIND TRUE CACHE BOOL "Use unwind for stack walking" FORCE)
set(STACK_DETAILS_AUTO_DETECT FALSE CACHE BOOL "Auto detect backward details" FORCE)
set(STACK_DETAILS_BACKTRACE_SYMBOL TRUE CACHE BOOL "Use backtrace symbols" FORCE)

# Windows-specific enhancements for better stack traces
if(WIN32)
    set(BACKWARD_HAS_DBGHELP 1 CACHE BOOL "Enable Windows DbgHelp for better stack traces" FORCE)
endif()

find_package(Backward QUIET)
if(Backward_FOUND)
    message(STATUS "[testcoe] Using existing Backward-cpp installation")
    ignore_external_warnings(Backward::Backward)
else()
    message(STATUS "[testcoe] Backward-cpp not found, fetching from source")
    include(FetchContent)
    FetchContent_Declare(
        backward
        GIT_REPOSITORY https://github.com/bombela/backward-cpp
        GIT_TAG v1.6
    )
    FetchContent_MakeAvailable(backward)
    ignore_external_warnings(backward)
endif()

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

