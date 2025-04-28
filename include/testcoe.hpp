/**
 * @namespace testcoe
 * @brief Test Console Output Enhancement library
 *
 * The testcoe namespace contains all functionality related to enhancing
 * the test output experience when using Google Test. It provides visual
 * grid-based test progress display, and signal handling for crashes
 * with detailed stack traces using backward-cpp.
 */

#pragma once

#include <testcoe/grid_listener.hpp>
#include <testcoe/signal_handler.hpp>
#include <testcoe/terminal_utils.hpp>

namespace testcoe
{
    void init();
    void init(int *argc, char **argv);

    int run_suite(const std::string &suite_name);
    int run_test(const std::string &suite_name, const std::string &test_name);

    inline int run()
    { 
        return RUN_ALL_TESTS(); 
    }
} // namespace testcoe