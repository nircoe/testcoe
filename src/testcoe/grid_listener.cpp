#include <testcoe/grid_listener.hpp>
#include <testcoe/signal_handler.hpp>
#include <testcoe/terminal_utils.hpp>
#include <algorithm>

namespace testcoe 
{
    namespace color
    {
        static const bool ansiSupported = terminal::isAnsiEnabled();

        const std::string reset = ansiSupported ? "\033[0m" : "";
        const std::string red = ansiSupported ? "\033[31m" : "";
        const std::string green = ansiSupported ? "\033[32m" : "";
        const std::string yellow = ansiSupported ? "\033[33m" : "";
        const std::string bold = ansiSupported ? "\033[1m" : "";
    } // namespace color

    void GridTestListener::printGrid()
    {
        testcoe::terminal::clear();
        std::cout << color::bold << "Running " << m_totalTests << " tests... "
                  << "Completed: " << m_completedTests << "/" << m_totalTests
                  << " (P: " << m_passedTests << ", F: " << m_failedTests << ")"
                  << color::reset << "\n";

        std::cout << color::green << "P" << color::reset << " - Passed, "
                  << color::red << "F" << color::reset << " - Failed, "
                  << color::yellow << "R" << color::reset << " - Running, "
                  << ". - Not run yet\n\n";

        std::size_t maxNameLength = 0;
        if(!m_suiteTestStatus.empty())
        {
            maxNameLength = std::max_element(m_suiteTestStatus.begin(), m_suiteTestStatus.end(),
                                            [](const auto &a, const auto &b)
                                            {
                                                return a.first.length() < b.first.length();
                                            })->first.length();
        }

        for(const auto &suite : m_suiteTestStatus)
        {
            std::string suiteName = suite.first;
            const auto &statuses = suite.second;
            
            std::cout << std::setw(maxNameLength) << std::left << suiteName << " ";

            int completedFromSuite = 0;
            for(const auto& status : statuses)
            {
                if(status == TestStatus::Passed || status == TestStatus::Failed)
                    ++completedFromSuite;

                std::string color;
                switch(status)
                {
                    case TestStatus::NotRun:
                        color = color::reset;
                        break;
                    case TestStatus::Running:
                        color = color::yellow;
                        break;
                    case TestStatus::Passed:
                        color = color::green;
                        break;
                    case TestStatus::Failed:
                        color = color::red;
                        break;
                    default:
                        break;
                }

                std::cout << color << static_cast<char>(status) << color::reset;
            }

            std::cout << " (" << completedFromSuite << "/" << statuses.size() << ")" << std::endl;
        }
    }

    GridTestListener::GridTestListener(testing::TestEventListener *listener) :
        m_originalListener(listener),
        m_totalTests(0), m_completedTests(0), m_passedTests(0), m_failedTests(0),
        m_originalCoutBuf(std::cout.rdbuf()), m_originalCerrBuf(std::cerr.rdbuf())
    {
        if (!listener)
            throw std::invalid_argument("Null listener passed to GridTestListener constructor");

        m_startTime = std::chrono::high_resolution_clock::now();
        g_originalCoutBuf = m_originalCoutBuf;
        g_originalCerrBuf = m_originalCerrBuf;
    }

    GridTestListener::~GridTestListener()
    {
        std::cout.rdbuf(m_originalCoutBuf);
        std::cerr.rdbuf(m_originalCerrBuf);
        delete m_originalListener;
    }

    void GridTestListener::OnTestProgramStart(const testing::UnitTest &unitTest)
    {
        std::cout.rdbuf(m_originalCoutBuf);

        m_totalTests = unitTest.total_test_count();
        for (int i = 0; i < unitTest.total_test_suite_count(); ++i)
        {
            const testing::TestSuite *testSuite = unitTest.GetTestSuite(i);
            std::string suiteName = testSuite->name();

            m_suiteTestStatus[suiteName] = std::vector<TestStatus>(testSuite->total_test_count(), TestStatus::NotRun);
            m_suiteTestNames[suiteName] = std::vector<std::string>(testSuite->total_test_count());
            m_currentTestIndex[suiteName] = 0;

            for (int j = 0; j < testSuite->total_test_count(); ++j)
            {
                const testing::TestInfo *testInfo = testSuite->GetTestInfo(j);
                m_suiteTestNames[suiteName][j] = testInfo->name();
            }
        }

        std::cout << color::bold << "Running " << m_totalTests << " tests..." << std::endl
                  << color::reset;

        printGrid();

        std::cout.rdbuf(m_nullStream.rdbuf());
        std::cerr.rdbuf(m_nullStream.rdbuf());
    }

    void GridTestListener::OnTestIterationStart(const testing::UnitTest &, int) { }

    void GridTestListener::OnEnvironmentsSetUpStart(const testing::UnitTest &) { }

    void GridTestListener::OnEnvironmentsSetUpEnd(const testing::UnitTest &) { }

    void GridTestListener::OnTestSuiteStart(const testing::TestSuite &testSuite)
    {
        m_currentTestSuite = testSuite.name();

        if(m_suiteTestStatus.find(m_currentTestSuite) == m_suiteTestStatus.end())
        {
            // should not happen, just in case.
            m_suiteTestStatus[m_currentTestSuite] = std::vector<TestStatus>();
            m_suiteTestNames[m_currentTestSuite] = std::vector<std::string>();
            m_currentTestIndex[m_currentTestSuite] = 0;
        }
    }

    void GridTestListener::OnTestStart(const testing::TestInfo &testInfo)
    {
        if (m_currentTestSuite.empty() || m_suiteTestStatus.find(m_currentTestSuite) == m_suiteTestStatus.end())
            return; // should not happen, just in case.

        m_currentTest = testInfo.name();
        m_currentOutput.str("");
        m_currentOutput.clear();

        int testIndex = m_currentTestIndex[m_currentTestSuite];
        if(0 <= testIndex && testIndex < static_cast<int>(m_suiteTestStatus[m_currentTestSuite].size()))
            m_suiteTestStatus[m_currentTestSuite][testIndex] = TestStatus::Running;

        std::cout.rdbuf(m_originalCoutBuf);
        printGrid();
        std::cout.rdbuf(m_nullStream.rdbuf());
    }

    void GridTestListener::OnTestPartResult(const testing::TestPartResult &testPartResult)
    {
        if(testPartResult.failed())
            m_currentOutput << testPartResult.file_name() << ": "
                            << testPartResult.line_number() << ": Failure" << std::endl
                            << testPartResult.summary() << std::endl;
    }

    void GridTestListener::OnTestEnd(const testing::TestInfo &testInfo)
    {
        int testIndex = m_currentTestIndex[m_currentTestSuite]++;

        m_suiteTestStatus[m_currentTestSuite][testIndex] = (testInfo.result()->Passed()) ? 
                                                            TestStatus::Passed : TestStatus::Failed;
        if(testInfo.result()->Passed())
            ++m_passedTests;
        else
        {
            ++m_failedTests;
            m_failedTestOutput[m_currentTestSuite].push_back(m_currentTest + ":\n" + m_currentOutput.str());
        }

        ++m_completedTests;

        std::cout.rdbuf(m_originalCoutBuf);
        printGrid();
        std::cout.rdbuf(m_nullStream.rdbuf());
    }

    void GridTestListener::OnTestSuiteEnd(const testing::TestSuite &) { }

    void GridTestListener::OnEnvironmentsTearDownStart(const testing::UnitTest &) { }

    void GridTestListener::OnEnvironmentsTearDownEnd(const testing::UnitTest &) { }

    void GridTestListener::OnTestIterationEnd(const testing::UnitTest &, int) { }

    void GridTestListener::OnTestProgramEnd(const testing::UnitTest &)
    {
        std::cout.rdbuf(m_originalCoutBuf);
        std::cerr.rdbuf(m_originalCerrBuf);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - m_startTime);

        std::stringstream time_elapsed;
        if(duration.count() >= 3600) // hours
            time_elapsed << (duration.count() / 3600) << " hours, " << ((duration.count() % 3600) / 60) 
                         << " minutes and " << (duration.count() % 60) << " seconds";
        else if(duration.count() >= 60) // minutes
            time_elapsed << (duration.count() / 60) << " minutes and " << (duration.count() % 60) << " seconds";
        else // seconds
            time_elapsed << duration.count() << " seconds";

        printGrid();

        std::cout << std::endl
                  << color::bold
                  << "=========================================" << std::endl
                  << "              Test  Summary              " << std::endl
                  << "=========================================" << color::reset << std::endl
                  << "Total Tests: " << m_totalTests << std::endl
                  << "Passed Tests: " << color::green << m_passedTests << color::reset << std::endl
                  << "Failed Tests: " << (m_failedTests > 0 ? color::red : color::green)
                  << m_failedTests << color::reset << std::endl
                  << "Time Elapsed: " << time_elapsed.str() << std::endl;

        if(m_failedTests > 0)
        {
            std::cout << std::endl
                      << color::bold << color::red
                      << "=========================================" << std::endl
                      << "              Failed  Tests              " << std::endl
                      << "=========================================" << color::reset << std::endl;
            
            for(const auto &suite : m_failedTestOutput)
            {
                std::cout << color::bold << "Test Suite: " << suite.first << color::reset << std::endl;
                for(const auto &test : suite.second)
                    std::cout << "  " << test << std::endl;
            }

            std::cout << std::endl
                      << color::bold << color::red << m_failedTests << " TESTS FAILED. See details above." 
                      << color::reset << std::endl;
        }
        else // m_failedTests == 0
        {
            std::cout << std::endl
                      << color::bold << color::green << "ALL TESTS PASSED!" << color::reset << std::endl;
        }
    }

    void installGridListener()
    {
        auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
        auto defaultListener = listeners.Release(listeners.default_result_printer());

        listeners.Append(new GridTestListener(defaultListener));
    }
} // namespace testcoe
