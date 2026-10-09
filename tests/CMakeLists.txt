cmake_minimum_required(VERSION 3.14)


add_executable(testcoe_tests
    examples_integration_tests.cpp
)

copy_mingw_dlls_to_target(testcoe_tests)

# Link ONLY with Google Test, the tests run the examples as separate processes
target_link_libraries(testcoe_tests PRIVATE gtest_main gtest)

add_dependencies(testcoe_tests basic_example crash_example filter_example)

target_compile_definitions(testcoe_tests
    PRIVATE
        TESTCOE_TEST_EXPECT_SOURCE_LOCATIONS=${TESTCOE_STACKTRACE_BACKEND_STD}
        TESTCOE_TEST_EXPECT_FUNCTION_NAMES=$<NOT:${TESTCOE_STACKTRACE_BACKEND_NONE}>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(testcoe_tests PRIVATE -Wall -Wextra -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(testcoe_tests PRIVATE /W3 /WX)
endif()

add_custom_target(run_tests
    COMMAND testcoe_tests
    DEPENDS testcoe_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running testcoe example integration tests"
)

add_custom_target(run_all_tests
    COMMAND testcoe_tests --gtest_also_run_disabled_tests
    DEPENDS testcoe_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running ALL testcoe tests, including any DISABLED_ ones"
)
