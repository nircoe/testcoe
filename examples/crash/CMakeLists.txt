cmake_minimum_required(VERSION 3.14)
project(testcoe_crash_handling_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fno-omit-frame-pointer)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
# Find testcoe or fetch it from GitHub
    find_package(testcoe QUIET)
    if(NOT testcoe_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            testcoe
            GIT_REPOSITORY https://github.com/nircoe/testcoe.git
            GIT_TAG v0.2.0
        )

        set(TESTCOE_BUILD_EXAMPLES ON CACHE BOOL "Disable testcoe examples" FORCE)
        set(TESTCOE_BUILD_TESTS ON CACHE BOOL "Disable testcoe tests" FORCE)

        FetchContent_MakeAvailable(testcoe)
    endif()
endif()

# testcoe already brings GTest, no need to find or fetch it.

add_executable(crash_example
    main.cpp
    crash_tests.cpp
)

copy_mingw_dlls_to_target(crash_example)

target_link_libraries(crash_example
    PRIVATE
        testcoe
)

add_custom_target(run_crash_example
    COMMAND crash_example
    DEPENDS crash_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

function(add_crash_test test_name flag)
    add_custom_target(run_crash_example_${test_name}
        COMMAND crash_example ${flag}
        DEPENDS crash_example
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

add_crash_test(SegmentationFault --run-segfault)
add_crash_test(DivideByZero --run-divbyzero)
add_crash_test(Abort --run-abort)
add_crash_test(AbortDeathTest --run-death-test)