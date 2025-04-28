cmake_minimum_required(VERSION 3.14)
project(testcoe_crash_handling_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable debug symbols and stack trace info
set(CMAKE_BUILD_TYPE Debug)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fno-omit-frame-pointer)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
# Find testcoe package or fetch it from GitHub
    find_package(testcoe QUIET)
    if(NOT testcoe_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            testcoe
            GIT_REPOSITORY https://github.com/nircoe/testcoe.git
            GIT_TAG v0.1.0
        )

        set(TESTCOE_BUILD_EXAMPLES ON CACHE BOOL "Disable testcoe examples" FORCE)
        set(TESTCOE_BUILD_TESTS ON CACHE BOOL "Disable testcoe tests" FORCE)

        FetchContent_MakeAvailable(testcoe)
    endif()
endif()

# Note: You don't need to find or fetch GTest separately
# because testcoe already includes and links against it.

# Add the executable
add_executable(crash_example
    main.cpp
    crash_tests.cpp
)

copy_mingw_dlls_to_target(crash_example)

# Link against testcoe
target_link_libraries(crash_example
    PRIVATE
        testcoe
)

# Add custom targets for running specific crash tests
add_custom_target(run_crash_example
    COMMAND crash_example
    DEPENDS crash_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Helper function to create targets for each crash test
function(add_crash_test test_name)
    add_custom_target(run_crash_example_${test_name}
        COMMAND crash_example --gtest_filter=CrashTest.${test_name}
        DEPENDS crash_example
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# Add targets for each crash test
add_crash_test(SegmentationFault)
add_crash_test(DivideByZero)
add_crash_test(Abort)
add_crash_test(StackOverflow)
add_crash_test(OutOfBounds)
add_crash_test(StreamRedirection)