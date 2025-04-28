#include <testcoe.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    std::cout << "==================================================" << std::endl;
    std::cout << "  testcoe Basic Example" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This example shows how to use testcoe with multiple test files." << std::endl;
    std::cout << "testcoe provides a visual grid display of test progress and" << std::endl;
    std::cout << "enhanced crash reporting." << std::endl;
    std::cout << std::endl;

    // Initialize testcoe (replaces Google Test initialization)
    testcoe::init(&argc, argv);

    // Run all tests
    return testcoe::run();
}