#pragma once

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace testcoe
{
    /**
     * @namespace testcoe::color
     * @brief ANSI color code definitions for terminal output
     *
     * This namespace contains string constants representing ANSI color and
     * formatting codes for enhancing terminal output. These codes are used
     * by the GridTestListener to provide colored test status output.
     */
    namespace color
    {
        extern const std::string reset;
        extern const std::string red;
        extern const std::string green;
        extern const std::string yellow;
        extern const std::string bold;
    } // namespace color

    enum class TestStatus : char
    {
        None = '\0',
        NotRun = '.',
        Running = 'R',
        Passed = 'P',
        Failed = 'F'
    };

    class GridTestListener : public testing::TestEventListener
    {
        testing::TestEventListener* m_originalListener;

        std::map<std::string, std::vector<TestStatus>> m_suiteTestStatus;
        std::map<std::string, std::vector<std::string>> m_suiteTestNames;
        std::map<std::string, std::vector<std::string>> m_failedTestOutput;
        std::map<std::string, int> m_currentTestIndex;
        std::string m_currentTestSuite;
        std::string m_currentTest;
        std::stringstream m_currentOutput;
        int m_totalTests;
        int m_completedTests;
        int m_passedTests;
        int m_failedTests;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

        std::streambuf* m_originalCoutBuf;
        std::streambuf* m_originalCerrBuf;
        std::stringstream m_nullStream;

        void printGrid();

    public:
        GridTestListener(testing::TestEventListener* listener);
        virtual ~GridTestListener() override;

        virtual void OnTestProgramStart(const testing::UnitTest &unitTest) override;
        virtual void OnTestIterationStart(const testing::UnitTest &unitTest, int iteration) override;
        virtual void OnEnvironmentsSetUpStart(const testing::UnitTest &unitTest) override;
        virtual void OnEnvironmentsSetUpEnd(const testing::UnitTest &unitTest) override;
        virtual void OnTestSuiteStart(const testing::TestSuite &testSuite) override;
        virtual void OnTestStart(const testing::TestInfo &testInfo) override;
        virtual void OnTestPartResult(const testing::TestPartResult &testPartResult) override;
        virtual void OnTestEnd(const testing::TestInfo &testInfo) override;
        virtual void OnTestSuiteEnd(const testing::TestSuite &testSuite) override;
        virtual void OnEnvironmentsTearDownStart(const testing::UnitTest &unitTest) override;
        virtual void OnEnvironmentsTearDownEnd(const testing::UnitTest &unitTest) override;
        virtual void OnTestIterationEnd(const testing::UnitTest &unitTest, int iteration) override;
        virtual void OnTestProgramEnd(const testing::UnitTest &unitTest) override;
    };

    void installGridListener();
} // namespace testcoe