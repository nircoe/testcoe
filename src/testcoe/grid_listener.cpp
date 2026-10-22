#include <testcoe/grid_listener.hpp>
#include <testcoe/signal_handler.hpp>
#include <testcoe/terminal_utils.hpp>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace testcoe
{
    namespace color
    {
        static const bool ansi_supported = terminal::is_ansi_enabled();

        const std::string reset = ansi_supported ? "\033[0m" : "";
        const std::string red = ansi_supported ? "\033[31m" : "";
        const std::string green = ansi_supported ? "\033[32m" : "";
        const std::string yellow = ansi_supported ? "\033[33m" : "";
        const std::string bold = ansi_supported ? "\033[1m" : "";
    } // namespace color

    void grid_test_listener::print_grid()
    {
        testcoe::terminal::clear();
        std::cout << color::bold << "Running " << m_total_tests << " tests... "
                  << "Completed: " << m_completed_tests << "/" << m_total_tests << " (P: " << m_passed_tests
                  << ", F: " << m_failed_tests << ")" << color::reset << "\n";

        std::cout << color::green << "P" << color::reset << " - Passed, " << color::red << "F" << color::reset
                  << " - Failed, " << color::yellow << "R" << color::reset << " - Running, "
                  << ". - Not run yet\n\n";

        std::size_t max_name_length = 0;
        if (!m_suite_test_status.empty())
        {
            max_name_length =
                std::max_element(m_suite_test_status.begin(), m_suite_test_status.end(),
                                 [](const auto &a, const auto &b) { return a.first.length() < b.first.length(); })
                    ->first.length();
        }

        for (const auto &suite : m_suite_test_status)
        {
            std::string suite_name = suite.first;
            const auto &statuses = suite.second;

            std::cout << std::setw(max_name_length) << std::left << suite_name << " ";

            int completed_from_suite = 0;
            for (const auto &status : statuses)
            {
                if (status == test_status::passed || status == test_status::failed) ++completed_from_suite;

                std::string color;
                switch (status)
                {
                    case test_status::not_run:
                        color = color::reset;
                        break;
                    case test_status::running:
                        color = color::yellow;
                        break;
                    case test_status::passed:
                        color = color::green;
                        break;
                    case test_status::failed:
                        color = color::red;
                        break;
                    default:
                        break;
                }

                std::cout << color << static_cast<char>(status) << color::reset;
            }

            std::cout << " (" << completed_from_suite << "/" << statuses.size() << ")" << std::endl;
        }
    }

    grid_test_listener::grid_test_listener(testing::TestEventListener *listener)
        : m_original_listener(listener), m_interactive(terminal::is_interactive()), m_total_tests(0),
          m_completed_tests(0), m_passed_tests(0), m_failed_tests(0), m_original_cout_buf(std::cout.rdbuf()),
          m_original_cerr_buf(std::cerr.rdbuf())
    {
        if (!listener) throw std::invalid_argument("Null listener passed to grid_test_listener constructor");

        m_start_time = std::chrono::high_resolution_clock::now();
        g_original_cout_buf = m_original_cout_buf;
        g_original_cerr_buf = m_original_cerr_buf;
    }

    grid_test_listener::~grid_test_listener()
    {
        std::cout.rdbuf(m_original_cout_buf);
        std::cerr.rdbuf(m_original_cerr_buf);
        delete m_original_listener;
    }

    void grid_test_listener::OnTestProgramStart(const testing::UnitTest &unit_test)
    {
        std::cout.rdbuf(m_original_cout_buf);

        m_total_tests = unit_test.total_test_count();
        for (int i = 0; i < unit_test.total_test_suite_count(); ++i)
        {
            const testing::TestSuite *test_suite = unit_test.GetTestSuite(i);
            std::string suite_name = test_suite->name();

            m_suite_test_status[suite_name] =
                std::vector<test_status>(test_suite->total_test_count(), test_status::not_run);
            m_suite_test_names[suite_name] = std::vector<std::string>(test_suite->total_test_count());
            m_current_test_index[suite_name] = 0;

            for (int j = 0; j < test_suite->total_test_count(); ++j)
            {
                const testing::TestInfo *test_info = test_suite->GetTestInfo(j);
                m_suite_test_names[suite_name][j] = test_info->name();
            }
        }

        std::cout << color::bold << "Running " << m_total_tests << " tests..." << std::endl << color::reset;

        if (m_interactive) print_grid();

        std::cout.rdbuf(m_null_stream.rdbuf());
        std::cerr.rdbuf(m_null_stream.rdbuf());
    }

    void grid_test_listener::OnTestIterationStart(const testing::UnitTest &, int) { }

    void grid_test_listener::OnEnvironmentsSetUpStart(const testing::UnitTest &) { }

    void grid_test_listener::OnEnvironmentsSetUpEnd(const testing::UnitTest &) { }

    void grid_test_listener::OnTestSuiteStart(const testing::TestSuite &test_suite)
    {
        m_current_test_suite = test_suite.name();

        if (m_suite_test_status.find(m_current_test_suite) == m_suite_test_status.end())
        {
            m_suite_test_status[m_current_test_suite] = std::vector<test_status>();
            m_suite_test_names[m_current_test_suite] = std::vector<std::string>();
            m_current_test_index[m_current_test_suite] = 0;
        }
    }

    void grid_test_listener::OnTestStart(const testing::TestInfo &test_info)
    {
        if (m_current_test_suite.empty() || m_suite_test_status.find(m_current_test_suite) == m_suite_test_status.end())
            return;

        m_current_test = test_info.name();
        m_current_output.str("");
        m_current_output.clear();

        int test_index = m_current_test_index[m_current_test_suite];
        if (0 <= test_index && test_index < static_cast<int>(m_suite_test_status[m_current_test_suite].size()))
            m_suite_test_status[m_current_test_suite][test_index] = test_status::running;

        if (m_interactive)
        {
            std::cout.rdbuf(m_original_cout_buf);
            print_grid();
            std::cout.rdbuf(m_null_stream.rdbuf());
        }
    }

    void grid_test_listener::OnTestPartResult(const testing::TestPartResult &test_part_result)
    {
        if (test_part_result.failed())
            m_current_output << test_part_result.file_name() << ": " << test_part_result.line_number() << ": Failure"
                             << std::endl
                             << test_part_result.summary() << std::endl;
    }

    void grid_test_listener::OnTestEnd(const testing::TestInfo &test_info)
    {
        int test_index = m_current_test_index[m_current_test_suite]++;

        m_suite_test_status[m_current_test_suite][test_index] =
            (test_info.result()->Passed()) ? test_status::passed : test_status::failed;
        if (test_info.result()->Passed())
            ++m_passed_tests;
        else
        {
            ++m_failed_tests;
            m_failed_test_output[m_current_test_suite].push_back(m_current_test + ":\n" + m_current_output.str());
        }

        ++m_completed_tests;

        if (m_interactive)
        {
            std::cout.rdbuf(m_original_cout_buf);
            print_grid();
            std::cout.rdbuf(m_null_stream.rdbuf());
        }
    }

    void grid_test_listener::OnTestSuiteEnd(const testing::TestSuite &) { }

    void grid_test_listener::OnEnvironmentsTearDownStart(const testing::UnitTest &) { }

    void grid_test_listener::OnEnvironmentsTearDownEnd(const testing::UnitTest &) { }

    void grid_test_listener::OnTestIterationEnd(const testing::UnitTest &, int) { }

    void grid_test_listener::OnTestProgramEnd(const testing::UnitTest &)
    {
        std::cout.rdbuf(m_original_cout_buf);
        std::cerr.rdbuf(m_original_cerr_buf);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - m_start_time);

        std::stringstream time_elapsed;
        if (duration.count() >= 3600)
            time_elapsed << (duration.count() / 3600) << " hours, " << ((duration.count() % 3600) / 60)
                         << " minutes and " << (duration.count() % 60) << " seconds";
        else if (duration.count() >= 60)
            time_elapsed << (duration.count() / 60) << " minutes and " << (duration.count() % 60) << " seconds";
        else
            time_elapsed << duration.count() << " seconds";

        print_grid();

        std::cout << std::endl
                  << color::bold << "=========================================" << std::endl
                  << "              Test  Summary              " << std::endl
                  << "=========================================" << color::reset << std::endl
                  << "Total Tests: " << m_total_tests << std::endl
                  << "Passed Tests: " << color::green << m_passed_tests << color::reset << std::endl
                  << "Failed Tests: " << (m_failed_tests > 0 ? color::red : color::green) << m_failed_tests
                  << color::reset << std::endl
                  << "Time Elapsed: " << time_elapsed.str() << std::endl;

        if (m_failed_tests > 0)
        {
            std::cout << std::endl
                      << color::bold << color::red << "=========================================" << std::endl
                      << "              Failed  Tests              " << std::endl
                      << "=========================================" << color::reset << std::endl;

            for (const auto &suite : m_failed_test_output)
            {
                std::cout << color::bold << "Test Suite: " << suite.first << color::reset << std::endl;
                for (const auto &test : suite.second)
                    std::cout << "  " << test << std::endl;
            }

            std::cout << std::endl
                      << color::bold << color::red << m_failed_tests << " TESTS FAILED. See details above."
                      << color::reset << std::endl;
        }
        else
        {
            std::cout << std::endl << color::bold << color::green << "ALL TESTS PASSED!" << color::reset << std::endl;
        }
    }

    void install_grid_listener()
    {
        auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
        auto default_listener = listeners.Release(listeners.default_result_printer());

        listeners.Append(new grid_test_listener(default_listener));
    }
} // namespace testcoe
