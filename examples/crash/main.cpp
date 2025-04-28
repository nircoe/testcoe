#include <testcoe.hpp>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    std::cout << "==================================================" << std::endl;
    std::cout << "  testcoe Crash Handling Example" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This example demonstrates testcoe's crash handling capabilities." << std::endl;
    std::cout << "When a test crashes, testcoe will:" << std::endl;
    std::cout << "  - Display the type of signal that occurred" << std::endl;
    std::cout << "  - Show a detailed stack trace" << std::endl;
    std::cout << "  - Provide helpful debugging information" << std::endl;
    std::cout << std::endl;
    std::cout << "IMPORTANT: By default, all crash tests are skipped." << std::endl;
    std::cout << "To run a specific crash test, use: --gtest_filter=CrashTest.Name" << std::endl;
    std::cout << std::endl;

    // Initialize testcoe
    testcoe::init(&argc, argv);

    // Run tests
    return testcoe::run();

    // TODO: add specific test run, and why do they say "by default all crash tests are skipped"?? 
    // you don't really do anything with the --gtest_filter flag, and we should call it "--run-test"
}