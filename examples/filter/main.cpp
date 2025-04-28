#include <testcoe.hpp>
#include <iostream>
#include <string>

void printHelp()
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
    // Print intro message
    std::cout << "==================================================" << std::endl;
    std::cout << "  testcoe Test Filtering Example" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This example demonstrates testcoe's test filtering capabilities." << std::endl;
    std::cout << "You can run all tests, a specific test suite, or a single test." << std::endl;
    std::cout << std::endl;

    // Initialize testcoe
    testcoe::init(&argc, argv);

    // Parse command line arguments
    bool showHelp = false;
    bool runAll = true;
    std::string suiteName;
    std::string testName;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help")
            showHelp = true;
        else if (arg == "--all")
            runAll = true;
        else if (arg.substr(0, 8) == "--suite=")
        {
            runAll = false;
            suiteName = arg.substr(8);
        }
        else if (arg.substr(0, 7) == "--test=")
        {
            runAll = false;
            std::string fullTest = arg.substr(7);

            // Parse "Suite.Test" format
            size_t dotPos = fullTest.find('.');
            if (dotPos != std::string::npos)
            {
                suiteName = fullTest.substr(0, dotPos);
                testName = fullTest.substr(dotPos + 1);
            }
        }
    }

    // Display help if requested
    if (showHelp)
    {
        printHelp();
        return 0;
    }

    // Run tests based on filtering options
    if (runAll)
    {
        std::cout << "Running all tests..." << std::endl;
        return testcoe::run();
    }
    else if (!testName.empty())
    {
        std::cout << "Running test: " << suiteName << "." << testName << std::endl;
        return testcoe::run_test(suiteName, testName);
    }
    else if (!suiteName.empty())
    {
        std::cout << "Running suite: " << suiteName << std::endl;
        return testcoe::run_suite(suiteName);
    }

    // Default: run all tests
    return testcoe::run();
}