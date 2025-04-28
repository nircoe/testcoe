#include <testcoe.hpp>
#include <stdexcept>

namespace testcoe
{
    static bool s_initialized = false;

    void init()
    {
        if(s_initialized) return;

        ::testing::InitGoogleTest();
        installGridListener();
        installSignalHandlers();

        s_initialized = true;
    }

    void init(int *argc, char **argv)
    {
        if(s_initialized) return;

        if(!argc || !argv)
            throw std::invalid_argument("Invalid arguments passed to testcoe::init");

        ::testing::InitGoogleTest(argc, argv);
        installGridListener();
        installSignalHandlers();

        s_initialized = true;
    }

    bool isInitialized()
    {
        return s_initialized;
    }

    int run()
    {
        if(!s_initialized)
            throw std::runtime_error("testcoe is not initialized");

        return RUN_ALL_TESTS();
    }

    int run(const std::string &test_filter)
    {
        if(!s_initialized)
            throw std::runtime_error("testcoe is not initialized");

        if(test_filter.empty())
            throw std::invalid_argument("Empty test filter passed to testcoe::run");
        
        ::testing::GTEST_FLAG(filter) = test_filter;
        return RUN_ALL_TESTS();
    }

    int run_suite(const std::string &suite_name)
    {
        if (!s_initialized)
            throw std::runtime_error("testcoe is not initialized");

        if(suite_name.empty())
            throw std::invalid_argument("Empty suite name passed to testcoe::run_suite");

        return run(suite_name + ".*");
    }

    int run_test(const std::string &suite_name, const std::string &test_name)
    {
        if (!s_initialized)
            throw std::runtime_error("testcoe is not initialized");

        if(suite_name.empty())
            throw std::invalid_argument("Empty suite name passed to testcoe::run_test");
        if(test_name.empty())
            throw std::invalid_argument("Empty test name passed to testcoe::run_test");

        return run(suite_name + "." + test_name);
    }

} // namespace testcoe
