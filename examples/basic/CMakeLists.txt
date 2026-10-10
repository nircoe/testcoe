cmake_minimum_required(VERSION 3.22)
project(testcoe_basic_example LANGUAGES CXX)

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

add_executable(basic_example
    main.cpp
    math_tests.cpp
    string_tests.cpp
    vector_tests.cpp
)

testcoe_enable_static_mingw_runtime(basic_example)

target_link_libraries(basic_example
    PRIVATE
        testcoe
)

add_custom_target(run_basic_example
    COMMAND basic_example
    DEPENDS basic_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
