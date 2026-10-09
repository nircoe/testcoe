/**
 * @namespace testcoe
 * @brief Grid view of test progress and crash stack traces for Google Test
 */

#pragma once

#include <testcoe/grid_listener.hpp>
#include <testcoe/signal_handler.hpp>
#include <testcoe/terminal_utils.hpp>

namespace testcoe
{
    void init();
    void init(int *argc, char **argv);

    bool isInitialized();

    int run();
    int run(const std::string &test_filter);
    int run_suite(const std::string &suite_name);
    int run_test(const std::string &suite_name, const std::string &test_name);
} // namespace testcoe