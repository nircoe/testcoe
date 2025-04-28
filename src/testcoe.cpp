#include "testcoe.hpp"
#include <stdexcept>

namespace testcoe
{
    void init()
    {
        ::testing::InitGoogleTest();
        installGridListener();
        installSignalHandlers();
    }

    void init(int *argc, char **argv)
    {
        if(!argc || !argv)
            throw std::invalid_argument("Invalid arguments passed to testcoe::init");

        ::testing::InitGoogleTest(argc, argv);
        installGridListener();
        installSignalHandlers();
    }

    int run(const std::string &test_filter)
    {
        if(test_filter.empty())
            run();

        ::testing::GTEST_FLAG(filter) = test_filter;
        return RUN_ALL_TESTS();
    }

    int run_suite(const std::string &suite_name)
    {
        if(suite_name.empty())
            throw std::invalid_argument("Empty suite name passed to testcoe::run_suite");

        return run(suite_name + ".*");
    }

    int run_test(const std::string &suite_name, const std::string &test_name)
    {
        if(suite_name.empty())
            throw std::invalid_argument("Empty suite name passed to testcoe::run_test");
        if(test_name.empty())
            throw std::invalid_argument("Empty test name passed to testcoe::run_test");

        return run(suite_name + "." + test_name);
    }

} // namespace testcoe
