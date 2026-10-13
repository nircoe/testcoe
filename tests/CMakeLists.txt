cmake_minimum_required(VERSION 3.22)


add_executable(testcoe_tests
    examples_integration_tests.cpp
)

testcoe_enable_static_mingw_runtime(testcoe_tests)

# Link ONLY with Google Test, the tests run the examples as separate processes
target_link_libraries(testcoe_tests
    PRIVATE
        gtest_main
        gtest
)

add_dependencies(testcoe_tests basic_example crash_example filter_example)

target_compile_definitions(testcoe_tests
    PRIVATE
        TESTCOE_TEST_EXPECT_SOURCE_LOCATIONS=${TESTCOE_STACKTRACE_BACKEND_STD}
        TESTCOE_TEST_EXPECT_FUNCTION_NAMES=$<NOT:${TESTCOE_STACKTRACE_BACKEND_NONE}>
)

testcoe_enable_strict_warnings(testcoe_tests)
