#include <gtest/gtest.h>
#include <testcoe.hpp>
#include <thread>
#include <future>
#include <chrono>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

// Define a test process executor that runs tests in a separate process
class TestProcessExecutor
{
public:
    static std::string runTestProcess(const std::string &executable, const std::string &arguments)
    {
        std::string command = executable + " " + arguments;
#ifdef _WIN32
        command += " 2>&1"; // Redirect stderr to stdout on Windows
        FILE *pipe = _popen(command.c_str(), "r");
#else
        command += " 2>&1"; // Redirect stderr to stdout on Unix
        FILE *pipe = popen(command.c_str(), "r");
#endif
        if (!pipe)
        {
            return "Error: Failed to open process";
        }

        char buffer[128];
        std::string result;

        while (!feof(pipe))
        {
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                result += buffer;
            }
        }

#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif

        return result;
    }

    // Helper to create a temporary test executable
    static std::string createTestExecutable(const std::string &testName, const std::string &testCode)
    {
        // Generate unique filename
        std::string filename = testName + "_test";
        std::string cppFile = filename + ".cpp";

        // Write test code to file
        std::ofstream outFile(cppFile);
        outFile << testCode;
        outFile.close();

        // Compile the test (platform-specific)
#ifdef _WIN32
        std::string compileCmd = "g++ -std=c++17 -o " + filename + ".exe " + cppFile +
                                 " -I../include -L../lib -ltestcoe -lgtest -lgtest_main -pthread";
#else
        std::string compileCmd = "g++ -std=c++17 -o " + filename + " " + cppFile +
                                 " -I../include -L../lib -ltestcoe -lgtest -lgtest_main -pthread";
#endif

        system(compileCmd.c_str());

#ifdef _WIN32
        return filename + ".exe";
#else
        return "./" + filename;
#endif
    }
};

// Sample test suite with predefined outcomes
class SampleTestSuite : public ::testing::Test
{
};

TEST_F(SampleTestSuite, PassingTest)
{
    EXPECT_TRUE(true);
}

TEST_F(SampleTestSuite, FailingTest)
{
    EXPECT_TRUE(false);
}

// Integration tests class
class IntegrationTests : public ::testing::Test
{
};

// Test init functionality
TEST_F(IntegrationTests, InitFunctionality)
{
    // Test that testcoe::init() doesn't crash
    EXPECT_NO_THROW({
        testcoe::init();
    });

    // Test init with arguments
    int argc = 1;
    char *argv[] = {(char *)"test_program"};
    EXPECT_NO_THROW({
        testcoe::init(&argc, argv);
    });

    // Test exceptions are thrown for invalid arguments
    EXPECT_THROW({ testcoe::init(nullptr, argv); }, std::invalid_argument);
}

// Test run functionality
TEST_F(IntegrationTests, RunFunctionality)
{
    // Test run() doesn't crash
    EXPECT_NO_THROW({
        // We're not actually running the tests, just making sure the function doesn't crash
        testcoe::run();
    });

    // Test invalid suite name throws
    EXPECT_THROW({ testcoe::run_suite(""); }, std::invalid_argument);

    // Test invalid test name throws
    EXPECT_THROW({ testcoe::run_test("ValidSuiteName", ""); }, std::invalid_argument);

    EXPECT_THROW({ testcoe::run_test("", "ValidTestName"); }, std::invalid_argument);
}

// Test grid visualization for mixed test results
TEST_F(IntegrationTests, GridVisualization)
{
    // This test would run better in a separate process, but we'll do a basic check
    std::string testCode = R"(
        #include <gtest/gtest.h>
        #include <testcoe.hpp>
        
        TEST(GridTest, PassingTest1) {
            EXPECT_TRUE(true);
        }
        
        TEST(GridTest, PassingTest2) {
            EXPECT_TRUE(true);
        }
        
        TEST(GridTest, FailingTest) {
            EXPECT_TRUE(false);
        }
        
        int main(int argc, char** argv) {
            testcoe::init(&argc, argv);
            return testcoe::run();
        }
    )";

    // Skip actual test execution in normal test runs as it requires compilation
    // This would typically be run in a CI environment
    if (std::getenv("RUN_INTEGRATION_TESTS") != nullptr)
    {
        std::string executable = TestProcessExecutor::createTestExecutable("grid_viz", testCode);
        std::string output = TestProcessExecutor::runTestProcess(executable, "");

        // Verify grid visualization elements are present
        EXPECT_TRUE(output.find("GridTest") != std::string::npos);
        EXPECT_TRUE(output.find("P") != std::string::npos); // Passed tests
        EXPECT_TRUE(output.find("F") != std::string::npos); // Failed test
    }
    else
    {
        SUCCEED() << "Skipping compilation-dependent test";
    }
}

// Test test filtering
TEST_F(IntegrationTests, TestFiltering)
{
    std::string testCode = R"(
        #include <gtest/gtest.h>
        #include <testcoe.hpp>
        
        TEST(FilterTest, Test1) {
            EXPECT_TRUE(true);
        }
        
        TEST(FilterTest, Test2) {
            EXPECT_TRUE(true);
        }
        
        TEST(OtherSuite, Test1) {
            EXPECT_TRUE(true);
        }
        
        int main(int argc, char** argv) {
            testcoe::init(&argc, argv);
            if (std::string(argv[1]) == "suite") {
                return testcoe::run_suite(argv[2]);
            } else if (std::string(argv[1]) == "test") {
                return testcoe::run_test(argv[2], argv[3]);
            } else {
                return testcoe::run();
            }
        }
    )";

    // Skip actual test execution in normal test runs
    if (std::getenv("RUN_INTEGRATION_TESTS") != nullptr)
    {
        std::string executable = TestProcessExecutor::createTestExecutable("filter", testCode);

        // Run with filter for specific test suite
        std::string output1 = TestProcessExecutor::runTestProcess(executable, "suite FilterTest");
        EXPECT_TRUE(output1.find("FilterTest") != std::string::npos);
        EXPECT_TRUE(output1.find("OtherSuite") == std::string::npos);

        // Run with filter for specific test
        std::string output2 = TestProcessExecutor::runTestProcess(executable, "test FilterTest Test1");
        EXPECT_TRUE(output2.find("Test1") != std::string::npos);
        EXPECT_TRUE(output2.find("Test2") == std::string::npos);
    }
    else
    {
        SUCCEED() << "Skipping compilation-dependent test";
    }
}

// Test run_suite functionality
TEST_F(IntegrationTests, RunSuiteFunctionality)
{
    std::string testCode = R"(
        #include <gtest/gtest.h>
        #include <testcoe.hpp>
        #include <iostream>
        
        TEST(Suite1, Test1) {
            std::cout << "Running Suite1.Test1" << std::endl;
            EXPECT_TRUE(true);
        }
        
        TEST(Suite1, Test2) {
            std::cout << "Running Suite1.Test2" << std::endl;
            EXPECT_TRUE(true);
        }
        
        TEST(Suite2, Test1) {
            std::cout << "Running Suite2.Test1" << std::endl;
            EXPECT_TRUE(true);
        }
        
        int main(int argc, char** argv) {
            testcoe::init(&argc, argv);
            return testcoe::run_suite(argv[1]);
        }
    )";

    // Skip actual test execution in normal test runs
    if (std::getenv("RUN_INTEGRATION_TESTS") != nullptr)
    {
        std::string executable = TestProcessExecutor::createTestExecutable("run_suite", testCode);

        // Run Suite1
        std::string output1 = TestProcessExecutor::runTestProcess(executable, "Suite1");
        EXPECT_TRUE(output1.find("Running Suite1.Test1") != std::string::npos);
        EXPECT_TRUE(output1.find("Running Suite1.Test2") != std::string::npos);
        EXPECT_TRUE(output1.find("Running Suite2.Test1") == std::string::npos);

        // Run Suite2
        std::string output2 = TestProcessExecutor::runTestProcess(executable, "Suite2");
        EXPECT_TRUE(output2.find("Running Suite1.Test1") == std::string::npos);
        EXPECT_TRUE(output2.find("Running Suite2.Test1") != std::string::npos);
    }
    else
    {
        SUCCEED() << "Skipping compilation-dependent test";
    }
}

// Test run_test functionality
TEST_F(IntegrationTests, RunTestFunctionality)
{
    std::string testCode = R"(
        #include <gtest/gtest.h>
        #include <testcoe.hpp>
        #include <iostream>
        
        TEST(SpecificSuite, Test1) {
            std::cout << "Running SpecificSuite.Test1" << std::endl;
            EXPECT_TRUE(true);
        }
        
        TEST(SpecificSuite, Test2) {
            std::cout << "Running SpecificSuite.Test2" << std::endl;
            EXPECT_TRUE(true);
        }
        
        int main(int argc, char** argv) {
            testcoe::init(&argc, argv);
            return testcoe::run_test(argv[1], argv[2]);
        }
    )";

    // Skip actual test execution in normal test runs
    if (std::getenv("RUN_INTEGRATION_TESTS") != nullptr)
    {
        std::string executable = TestProcessExecutor::createTestExecutable("run_test", testCode);

        // Run specific test
        std::string output = TestProcessExecutor::runTestProcess(executable, "SpecificSuite Test1");
        EXPECT_TRUE(output.find("Running SpecificSuite.Test1") != std::string::npos);
        EXPECT_TRUE(output.find("Running SpecificSuite.Test2") == std::string::npos);
    }
    else
    {
        SUCCEED() << "Skipping compilation-dependent test";
    }
}