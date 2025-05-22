cmake_minimum_required(VERSION 3.14)
project(testcoe_examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT TARGET testcoe)
    message(STATUS "Building examples standalone - will find or fetch testcoe")
    find_package(testcoe QUIET)
    if(NOT testcoe_FOUND)
        message(STATUS "testcoe not found, fetching from source")
        include(FetchContent)
        FetchContent_Declare(
            testcoe
            GIT_REPOSITORY https://github.com/nircoe/testcoe.git
            GIT_TAG main # change to v0.1.0 later on
        )
        FetchContent_MakeAvailable(testcoe)
    endif()
endif()

add_subdirectory(basic)
add_subdirectory(crash)
add_subdirectory(filter)

add_custom_target(build_all_examples
    DEPENDS 
        basic_example
        crash_example
        filtering_example
)

# Add a target to run basic examples (those that don't crash)
add_custom_target(run_all_examples
    DEPENDS
        run_basic_example
        run_filtering_example
)