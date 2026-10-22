#include <testcoe.hpp>
#include <cstddef>
#include <iostream>
#include <string>

void print_help()
{
    std::cout << "Usage: ./filter_example [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help           Display this help message" << std::endl;
    std::cout << "  --all            Run all tests (default)" << std::endl;
    std::cout << "  --suite=NAME     Run only the specified test suite" << std::endl;
    std::cout << "  --test=SUITE.TEST Run only the specified test" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    std::cout << "==================================================" << std::endl;
    std::cout << "  testcoe Test Filtering Example" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This example demonstrates testcoe's test filtering capabilities." << std::endl;
    std::cout << "You can run all tests, a specific test suite, or a single test." << std::endl;
    std::cout << std::endl;

    testcoe::init(&argc, argv);

    bool show_help = false;
    bool run_all = true;
    std::string suite_name;
    std::string test_name;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help")
            show_help = true;
        else if (arg == "--all")
            run_all = true;
        else if (arg.substr(0, 8) == "--suite=")
        {
            run_all = false;
            suite_name = arg.substr(8);
        }
        else if (arg.substr(0, 7) == "--test=")
        {
            run_all = false;
            std::string full_test = arg.substr(7);

            std::size_t dot_pos = full_test.find('.');
            if (dot_pos != std::string::npos)
            {
                suite_name = full_test.substr(0, dot_pos);
                test_name = full_test.substr(dot_pos + 1);
            }
        }
    }

    if (show_help)
    {
        print_help();
        return 0;
    }

    if (run_all)
    {
        std::cout << "Running all tests..." << std::endl;
        return testcoe::run();
    }
    else if (!test_name.empty())
    {
        std::cout << "Running test: " << suite_name << "." << test_name << std::endl;
        return testcoe::run_test(suite_name, test_name);
    }
    else if (!suite_name.empty())
    {
        std::cout << "Running suite: " << suite_name << std::endl;
        return testcoe::run_suite(suite_name);
    }

    return testcoe::run();
}
