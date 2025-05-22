cmake_minimum_required(VERSION 3.14)
project(testcoe_filter_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
# Find testcoe package or fetch it from GitHub
    find_package(testcoe QUIET)
    if(NOT testcoe_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        testcoe
        GIT_REPOSITORY https://github.com/nircoe/testcoe.git
        GIT_TAG main # use v0.1.0 later on
    )
    FetchContent_MakeAvailable(testcoe)
    endif()
endif()
# Note: You don't need to find or fetch GTest separately
# because testcoe already includes and links against it.

# Add the executable
add_executable(filter_example
    main.cpp
    test_suites.cpp
)

# Link against testcoe
target_link_libraries(filter_example
    PRIVATE
        testcoe
)

# Add custom targets for running the example in different ways
add_custom_target(run_filter_example
    COMMAND filter_example --all
    DEPENDS filter_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_filter_example_help
    COMMAND filter_example --help
    DEPENDS filter_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Run with a specific suite
foreach(SUITE MathSuite StringSuite VectorSuite)
    add_custom_target(run_filter_example_suite_${SUITE}
        COMMAND filter_example --suite=${SUITE}
        DEPENDS filter_example
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()

# Run some specific test examples
add_custom_target(run_filter_example_test_math_addition
    COMMAND filter_example --test=MathSuite.Addition
    DEPENDS filter_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_filter_example_test_string_length
    COMMAND filter_example --test=StringSuite.Length
    DEPENDS filter_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)