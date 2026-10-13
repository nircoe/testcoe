cmake_minimum_required(VERSION 3.22)
project(testcoe_examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT TARGET testcoe)
    message(STATUS "[testcoe] Building examples standalone - fetching testcoe")
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

add_subdirectory(basic)
add_subdirectory(crash)
add_subdirectory(filter)

add_custom_target(build_all_examples
    DEPENDS
        basic_example
        crash_example
        filter_example
)

# Only the examples that don't crash
add_custom_target(run_all_examples
    DEPENDS
        run_filter_example
)
