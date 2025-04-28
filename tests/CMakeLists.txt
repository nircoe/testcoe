cmake_minimum_required(VERSION 3.14)


# Create test executable for example integration tests only
add_executable(testcoe_tests
    examples_integration_tests.cpp
)

copy_mingw_dlls_to_target(testcoe_tests)

# Link ONLY with Google Test - we're testing by running examples as separate processes
if(TARGET GTest::Main)
    target_link_libraries(testcoe_tests PRIVATE GTest::Main GTest::gtest)
else()
    target_link_libraries(testcoe_tests PRIVATE gtest_main gtest)
endif()

# Make sure examples are built before running tests
add_dependencies(testcoe_tests basic_example crash_example filter_example)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(testcoe_tests PRIVATE -Wall -Wextra -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(testcoe_tests PRIVATE /W3 /WX)
endif()

# Target to run tests
add_custom_target(run_tests
    COMMAND testcoe_tests
    DEPENDS testcoe_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running testcoe example integration tests"
)

# Target to run tests including disabled crash tests (for manual testing)
add_custom_target(run_all_tests
    COMMAND testcoe_tests --gtest_also_run_disabled_tests
    DEPENDS testcoe_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running ALL testcoe tests including crash tests"
)