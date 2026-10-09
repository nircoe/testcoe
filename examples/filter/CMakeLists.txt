cmake_minimum_required(VERSION 3.14)
project(testcoe_filter_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_executable(filter_example
    main.cpp
    test_suites.cpp
)

copy_mingw_dlls_to_target(filter_example)

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

foreach(SUITE MathSuite StringSuite VectorSuite)
    add_custom_target(run_filter_example_suite_${SUITE}
        COMMAND filter_example --suite=${SUITE}
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