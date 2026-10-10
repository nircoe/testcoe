cmake_minimum_required(VERSION 3.22)
project(testcoe_filter_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_executable(filter_example
    main.cpp
    test_suites.cpp
)

testcoe_enable_static_mingw_runtime(filter_example)

target_link_libraries(filter_example
    PRIVATE
        testcoe
)

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

foreach(suite MathSuite StringSuite VectorSuite)
    add_custom_target(run_filter_example_suite_${suite}
        COMMAND filter_example --suite=${suite}
        DEPENDS filter_example
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()

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
