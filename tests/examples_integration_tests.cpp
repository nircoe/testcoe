#include <gtest/gtest.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// Platform-specific includes
#ifdef _WIN32
#include <io.h>
#include <process.h>
#include <windows.h>
#include <limits.h>
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#else
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#endif

class ExampleTests : public ::testing::Test
{
protected:
    // Helper function to run a command and capture its output (cross-platform)
    std::string runCommand(const std::string &command)
    {
        std::string result;
        std::string fullCommand = command;

        // Redirect stderr to stdout for both platforms
        fullCommand += " 2>&1";

#ifdef _WIN32
        FILE *pipe = _popen(fullCommand.c_str(), "r");
#else
        FILE *pipe = popen(fullCommand.c_str(), "r");
#endif

        if (!pipe)
        {
            return "ERROR: Failed to run command: " + command;
        }

        char buffer[512]; // Larger buffer for better performance
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }

        int exitCode;
#ifdef _WIN32
        exitCode = _pclose(pipe);
#else
        exitCode = pclose(pipe);
        // On Unix, pclose returns the exit status in a format that needs WEXITSTATUS
        if (WIFEXITED(exitCode))
        {
            exitCode = WEXITSTATUS(exitCode);
        }
#endif

        // Store exit code for debugging if needed
        (void)exitCode;

        return result;
    }

    // Helper to check if executable exists (cross-platform)
    bool executableExists(const std::string &path)
    {
#ifdef _WIN32
        // On Windows, use _access to check file existence and readability
        return (_access(path.c_str(), 0) == 0);
#else
        // On Unix-like systems, check if file exists and is executable
        return (access(path.c_str(), F_OK) == 0 && access(path.c_str(), X_OK) == 0);
#endif
    }

    // Helper to get the directory where the test executable is located
    std::string getExecutableDirectory()
    {
#ifdef _WIN32
        char path[MAX_PATH];
        DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
        if (length == 0)
        {
            return "."; // Fallback to current directory
        }

        std::string exePath(path);
        size_t lastSlash = exePath.find_last_of("\\/");
        if (lastSlash != std::string::npos)
        {
            return exePath.substr(0, lastSlash);
        }
        return ".";
#else
        char path[PATH_MAX];
        ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (length == -1)
        {
            return "."; // Fallback to current directory
        }

        path[length] = '\0';
        std::string exePath(path);
        size_t lastSlash = exePath.find_last_of('/');
        if (lastSlash != std::string::npos)
        {
            return exePath.substr(0, lastSlash);
        }
        return ".";
#endif
    }

    // Helper to get the correct executable path based on build system
    std::string getExecutablePath(const std::string &exampleName)
    {
        // Get the directory where this test executable is located
        std::string testDir = getExecutableDirectory();

        // Try different relative paths from the test executable location
        std::vector<std::string> possiblePaths;

#ifdef _WIN32
        // Windows paths - calculate from test executable location
        possiblePaths = {
            // From tests/Debug to examples (typical MSVC structure)
            testDir + "/../../examples/" + exampleName + "/Debug/" + exampleName + "_example.exe",
            testDir + "/../../examples/" + exampleName + "/Release/" + exampleName + "_example.exe",
            testDir + "/../../examples/" + exampleName + "/" + exampleName + "_example.exe",
            // From build/tests/Debug to examples
            testDir + "/../examples/" + exampleName + "/Debug/" + exampleName + "_example.exe",
            testDir + "/../examples/" + exampleName + "/Release/" + exampleName + "_example.exe",
            testDir + "/../examples/" + exampleName + "/" + exampleName + "_example.exe",
            // Same directory as test
            testDir + "/" + exampleName + "_example.exe"};
#else
        // Unix-like systems
        possiblePaths = {
            testDir + "/../examples/" + exampleName + "/" + exampleName + "_example",
            testDir + "/../../examples/" + exampleName + "/" + exampleName + "_example",
            testDir + "/" + exampleName + "_example"};
#endif

        // Return the first path that exists
        for (const auto &path : possiblePaths)
        {
            if (executableExists(path))
            {
                std::cout << "Found \"" << exampleName << "\" executable, path - " << path << std::endl;
                return path;
            }
        }

        // If none found, return the most likely path for error reporting
#ifdef _WIN32
        return testDir + "/../../examples/" + exampleName + "/Debug/" + exampleName + "_example.exe";
#else
        return testDir + "/../examples/" + exampleName + "/" + exampleName + "_example";
#endif
    }
};

// Test that basic example runs and shows expected output
TEST_F(ExampleTests, BasicExampleRuns)
{
    std::string executable = getExecutablePath("basic");
    std::cout << "getExecutablePath of \"basic\" returned: " << executable << std::endl;

    // Check if executable exists
    ASSERT_TRUE(executableExists(executable)) << "Basic example executable not found: " << executable;

    // Run the basic example
    std::string output = runCommand(executable);

    // Debug output for CI
    std::cout << "Basic example output length: " << output.length() << std::endl;
    if (output.find("ERROR:") != std::string::npos)
    {
        std::cout << "Error in output: " << output << std::endl;
    }

    // Verify it contains expected elements
    EXPECT_TRUE(output.find("testcoe Basic Example") != std::string::npos)
        << "Missing header in basic example output";

    EXPECT_TRUE(output.find("Running") != std::string::npos)
        << "Missing 'Running' text in output";

    EXPECT_TRUE(output.find("MathTest") != std::string::npos)
        << "Missing MathTest suite in output";

    EXPECT_TRUE(output.find("StringTest") != std::string::npos)
        << "Missing StringTest suite in output";

    EXPECT_TRUE(output.find("VectorTest") != std::string::npos)
        << "Missing VectorTest suite in output";

    // Should show grid visualization elements
    EXPECT_TRUE(output.find("P - Passed") != std::string::npos ||
                output.find("Passed") != std::string::npos)
        << "Missing grid legend in output";

    // Should show summary
    EXPECT_TRUE(output.find("Test  Summary") != std::string::npos ||
                output.find("Summary") != std::string::npos)
        << "Missing test summary in output";
}

// Test that filter example runs with different options
TEST_F(ExampleTests, FilterExampleRuns)
{
    std::string executable = getExecutablePath("filter");

    ASSERT_TRUE(executableExists(executable)) << "Filter example executable not found: " << executable;

    // Test help option
    std::string helpOutput = runCommand(executable + " --help");
    EXPECT_TRUE(helpOutput.find("Usage:") != std::string::npos)
        << "Help option not working. Output: " << helpOutput;

    // Test running all tests
    std::string allOutput = runCommand(executable + " --all");
    EXPECT_TRUE(allOutput.find("Running all tests") != std::string::npos)
        << "All tests option not working";
    EXPECT_TRUE(allOutput.find("MathSuite") != std::string::npos)
        << "Missing MathSuite in filter example";

    // Test running specific suite
    std::string suiteOutput = runCommand(executable + " --suite=MathSuite");
    EXPECT_TRUE(suiteOutput.find("Running suite: MathSuite") != std::string::npos)
        << "Suite filtering not working";
    EXPECT_TRUE(suiteOutput.find("MathSuite") != std::string::npos)
        << "MathSuite not run when filtered";

    // Test running specific test
    std::string testOutput = runCommand(executable + " --test=MathSuite.Addition");
    EXPECT_TRUE(testOutput.find("Running test: MathSuite.Addition") != std::string::npos)
        << "Test filtering not working";
}

// Test that crash example runs (without actual crashes)
TEST_F(ExampleTests, CrashExampleRuns)
{
    std::string executable = getExecutablePath("crash");

    ASSERT_TRUE(executableExists(executable)) << "Crash example executable not found: " << executable;

    // Run without crash flags (should run basic tests only)
    std::string output = runCommand(executable);

    EXPECT_TRUE(output.find("testcoe Crash Handling Example") != std::string::npos)
        << "Missing crash example header";

    EXPECT_TRUE(output.find("BasicTests") != std::string::npos)
        << "Missing BasicTests in crash example";

    // Should mention crash test skipping (look for various skip-related words)
    bool hasSkipMessage = (output.find("Skipping") != std::string::npos) ||
                          (output.find("SKIP") != std::string::npos) ||
                          (output.find("skipped") != std::string::npos) ||
                          (output.find("disabled") != std::string::npos);

    EXPECT_TRUE(hasSkipMessage)
        << "Crash tests should be skipped by default. Output: " << output;
}

// Test that crash example actually handles crashes (this test is risky!)
TEST_F(ExampleTests, DISABLED_CrashExampleHandlesCrashes)
{
    // This test is disabled by default because it intentionally crashes
    // Enable it by running: --gtest_also_run_disabled_tests

    std::string executable = getExecutablePath("crash");

    ASSERT_TRUE(executableExists(executable)) << "Crash example executable not found: " << executable;

    // Test segfault handling
    std::string crashOutput = runCommand(executable + " --run-segfault");

    // Should contain crash report
    EXPECT_TRUE(crashOutput.find("TEST TERMINATED BY") != std::string::npos)
        << "Missing crash termination message";

    EXPECT_TRUE(crashOutput.find("Stack trace") != std::string::npos ||
                crashOutput.find("CRASH REPORT") != std::string::npos)
        << "Missing stack trace in crash output";
}

// Test that examples are built and available
TEST_F(ExampleTests, AllExamplesExist)
{
    // Check that all expected executables exist
    std::vector<std::string> examples = {
        "basic",
        "crash",
        "filter"};

    for (const auto &example : examples)
    {
        std::string exe = getExecutablePath(example);
        EXPECT_TRUE(executableExists(exe)) << "Missing example: " << exe;
    }
}

// Test that examples produce reasonable exit codes
TEST_F(ExampleTests, ExampleExitCodes)
{
    // Basic example should not crash (exit code < 128)
    std::string basicCmd = getExecutablePath("basic");

    // Cross-platform output redirection
#ifdef _WIN32
    basicCmd += " >NUL 2>&1";
#else
    basicCmd += " >/dev/null 2>&1";
#endif

    int basicResult = system(basicCmd.c_str());

    // On Unix systems, system() returns exit code * 256, so we need to extract the real exit code
#ifndef _WIN32
    if (WIFEXITED(basicResult))
    {
        basicResult = WEXITSTATUS(basicResult);
    }
#endif

    EXPECT_LT(basicResult, 128) << "Basic example crashed with exit code: " << basicResult;

    // Filter example with help should exit cleanly
    std::string filterCmd = getExecutablePath("filter");
    filterCmd += " --help";

#ifdef _WIN32
    filterCmd += " >NUL 2>&1";
#else
    filterCmd += " >/dev/null 2>&1";
#endif

    int filterResult = system(filterCmd.c_str());

#ifndef _WIN32
    if (WIFEXITED(filterResult))
    {
        filterResult = WEXITSTATUS(filterResult);
    }
#endif

    EXPECT_EQ(filterResult, 0) << "Filter example help failed with exit code: " << filterResult;
}