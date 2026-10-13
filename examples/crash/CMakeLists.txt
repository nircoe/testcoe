cmake_minimum_required(VERSION 3.22)
project(testcoe_crash_handling_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
    add_compile_options(-fno-omit-frame-pointer)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(FetchContent)
    FetchContent_Declare(
        testcoe
        GIT_REPOSITORY https://github.com/nircoe/testcoe.git
        GIT_TAG v0.2.0
        GIT_SHALLOW TRUE
    )

    set(TESTCOE_BUILD_EXAMPLES ON CACHE BOOL "Disable testcoe examples" FORCE)
    set(TESTCOE_BUILD_TESTS ON CACHE BOOL "Disable testcoe tests" FORCE)

    FetchContent_MakeAvailable(testcoe)
endif()

# testcoe already brings GTest, no need to find or fetch it.

add_executable(crash_example
    main.cpp
    crash_tests.cpp
)

testcoe_enable_static_mingw_runtime(crash_example)

target_link_libraries(crash_example
    PRIVATE
        testcoe
)

add_custom_target(run_crash_example
    COMMAND crash_example
    DEPENDS crash_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

function(testcoe_add_crash_test test_name flag)
    add_custom_target(run_crash_example_${test_name}
        COMMAND crash_example ${flag}
        DEPENDS crash_example
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

testcoe_add_crash_test(SegmentationFault --run-segfault)
testcoe_add_crash_test(DivideByZero --run-divbyzero)
testcoe_add_crash_test(Abort --run-abort)
testcoe_add_crash_test(AbortDeathTest --run-death-test)
