#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace testcoe
{
    /**
     * @namespace testcoe::color
     * @brief ANSI color and style codes for terminal output
     */
    namespace color
    {
        extern const std::string reset;
        extern const std::string red;
        extern const std::string green;
        extern const std::string yellow;
        extern const std::string bold;
    } // namespace color

    enum class test_status : char
    {
        none = '\0',
        not_run = '.',
        running = 'R',
        passed = 'P',
        failed = 'F'
    };

    class grid_test_listener : public testing::TestEventListener
    {
        testing::TestEventListener *m_original_listener;
        bool m_interactive; // true only on a real TTY, gates per-event grid redraws

        std::map<std::string, std::vector<test_status>> m_suite_test_status;
        std::map<std::string, std::vector<std::string>> m_suite_test_names;
        std::map<std::string, std::vector<std::string>> m_failed_test_output;
        std::map<std::string, int> m_current_test_index;
        std::string m_current_test_suite;
        std::string m_current_test;
        std::stringstream m_current_output;
        int m_total_tests;
        int m_completed_tests;
        int m_passed_tests;
        int m_failed_tests;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;

        std::streambuf *m_original_cout_buf;
        std::streambuf *m_original_cerr_buf;
        std::stringstream m_null_stream;

        void print_grid();

    public:
        grid_test_listener(testing::TestEventListener *listener);
        virtual ~grid_test_listener() override;

        virtual void OnTestProgramStart(const testing::UnitTest &unit_test) override;
        virtual void OnTestIterationStart(const testing::UnitTest &unit_test, int iteration) override;
        virtual void OnEnvironmentsSetUpStart(const testing::UnitTest &unit_test) override;
        virtual void OnEnvironmentsSetUpEnd(const testing::UnitTest &unit_test) override;
        virtual void OnTestSuiteStart(const testing::TestSuite &test_suite) override;
        virtual void OnTestStart(const testing::TestInfo &test_info) override;
        virtual void OnTestPartResult(const testing::TestPartResult &test_part_result) override;
        virtual void OnTestEnd(const testing::TestInfo &test_info) override;
        virtual void OnTestSuiteEnd(const testing::TestSuite &test_suite) override;
        virtual void OnEnvironmentsTearDownStart(const testing::UnitTest &unit_test) override;
        virtual void OnEnvironmentsTearDownEnd(const testing::UnitTest &unit_test) override;
        virtual void OnTestIterationEnd(const testing::UnitTest &unit_test, int iteration) override;
        virtual void OnTestProgramEnd(const testing::UnitTest &unit_test) override;
    };

    void install_grid_listener();
} // namespace testcoe
