#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <io.h>
    #include <limits.h>
    #include <process.h>
    #include <windows.h>
    #ifndef MAX_PATH
        #define MAX_PATH 260
    #endif
#else
    #include <limits.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #ifndef PATH_MAX
        #define PATH_MAX 4096
    #endif
#endif

class ExampleTests : public ::testing::Test
{
protected:
    std::string run_command(const std::string &command, int *exit_code_out = nullptr)
    {
        std::string result;
        std::string full_command = command;

        full_command += " 2>&1";

#ifdef _WIN32
        FILE *pipe = _popen(full_command.c_str(), "r");
#else
        FILE *pipe = popen(full_command.c_str(), "r");
#endif

        if (!pipe)
        {
            return "ERROR: Failed to run command: " + command;
        }

        char buffer[512];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }

        int exit_code;
#ifdef _WIN32
        exit_code = _pclose(pipe);
#else
        exit_code = pclose(pipe);
        // On Unix pclose returns the raw wait status
        if (WIFEXITED(exit_code))
        {
            exit_code = WEXITSTATUS(exit_code);
        }
#endif

        if (exit_code_out) *exit_code_out = exit_code;

        return result;
    }

    bool executable_exists(const std::string &path)
    {
#ifdef _WIN32
        return (_access(path.c_str(), 0) == 0);
#else
        return (access(path.c_str(), F_OK) == 0 && access(path.c_str(), X_OK) == 0);
#endif
    }

    std::string get_executable_directory()
    {
#ifdef _WIN32
        char path[MAX_PATH];
        DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
        if (length == 0)
        {
            return ".";
        }

        std::string exe_path(path);
        size_t last_slash = exe_path.find_last_of("\\/");
        if (last_slash != std::string::npos)
        {
            return exe_path.substr(0, last_slash);
        }
        return ".";
#else
        char path[PATH_MAX];
        ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (length == -1)
        {
            return ".";
        }

        path[length] = '\0';
        std::string exe_path(path);
        size_t last_slash = exe_path.find_last_of('/');
        if (last_slash != std::string::npos)
        {
            return exe_path.substr(0, last_slash);
        }
        return ".";
#endif
    }

    std::string get_executable_path(const std::string &example_name)
    {
        std::string test_dir = get_executable_directory();

        std::vector<std::string> possible_paths;

#ifdef _WIN32
        possible_paths = {// From tests/Debug to examples (typical MSVC structure)
                          test_dir + "/../../examples/" + example_name + "/Debug/" + example_name + "_example.exe",
                          test_dir + "/../../examples/" + example_name + "/Release/" + example_name + "_example.exe",
                          test_dir + "/../../examples/" + example_name + "/" + example_name + "_example.exe",
                          // From build/tests/Debug to examples
                          test_dir + "/../examples/" + example_name + "/Debug/" + example_name + "_example.exe",
                          test_dir + "/../examples/" + example_name + "/Release/" + example_name + "_example.exe",
                          test_dir + "/../examples/" + example_name + "/" + example_name + "_example.exe",
                          test_dir + "/" + example_name + "_example.exe"};
#else
        possible_paths = {test_dir + "/../examples/" + example_name + "/" + example_name + "_example",
                          test_dir + "/../../examples/" + example_name + "/" + example_name + "_example",
                          test_dir + "/" + example_name + "_example"};
#endif

        for (const auto &path : possible_paths)
        {
            if (executable_exists(path))
            {
                std::cout << "Found \"" << example_name << "\" executable, path - " << path << std::endl;
                return path;
            }
        }

#ifdef _WIN32
        return test_dir + "/../../examples/" + example_name + "/Debug/" + example_name + "_example.exe";
#else
        return test_dir + "/../examples/" + example_name + "/" + example_name + "_example";
#endif
    }
};

TEST_F(ExampleTests, BasicExampleRuns)
{
    std::string executable = get_executable_path("basic");
    std::cout << "get_executable_path of \"basic\" returned: " << executable << std::endl;

    ASSERT_TRUE(executable_exists(executable)) << "Basic example executable not found: " << executable;

    std::string output = run_command(executable);

    std::cout << "Basic example output length: " << output.length() << std::endl;
    if (output.find("ERROR:") != std::string::npos)
    {
        std::cout << "Error in output: " << output << std::endl;
    }

    EXPECT_TRUE(output.find("testcoe Basic Example") != std::string::npos) << "Missing header in basic example output";

    EXPECT_TRUE(output.find("Running") != std::string::npos) << "Missing 'Running' text in output";

    EXPECT_TRUE(output.find("MathTest") != std::string::npos) << "Missing MathTest suite in output";

    EXPECT_TRUE(output.find("StringTest") != std::string::npos) << "Missing StringTest suite in output";

    EXPECT_TRUE(output.find("VectorTest") != std::string::npos) << "Missing VectorTest suite in output";

    EXPECT_TRUE(output.find("P - Passed") != std::string::npos || output.find("Passed") != std::string::npos)
        << "Missing grid legend in output";

    EXPECT_TRUE(output.find("Test  Summary") != std::string::npos || output.find("Summary") != std::string::npos)
        << "Missing test summary in output";
}

// run_command() pipes output through popen/_popen, which is never a TTY, so this covers
// the non-interactive path (see terminal::is_interactive()).
TEST_F(ExampleTests, BasicExampleNonInteractiveOutputHasNoDuplicateGridFrames)
{
    std::string executable = get_executable_path("basic");
    ASSERT_TRUE(executable_exists(executable)) << "Basic example executable not found: " << executable;

    std::string output = run_command(executable);

    // Sits between print_grid()'s two dynamic counts, so it's never split by ANSI color codes.
    const std::string grid_frame_marker = " tests... Completed: ";

    std::size_t frame_count = 0;
    std::size_t pos = 0;
    while ((pos = output.find(grid_frame_marker, pos)) != std::string::npos)
    {
        ++frame_count;
        pos += grid_frame_marker.length();
    }

    EXPECT_EQ(frame_count, 1u) << "Non-interactive output should contain exactly one grid frame (the final summary), "
                               << "not one per test start/end event. Output:\n"
                               << output;

    // Loose bound. A duplicate-frame regression would blow well past this.
    std::size_t line_count = 0;
    for (char c : output)
        if (c == '\n') ++line_count;

    EXPECT_LT(line_count, 60u) << "Non-interactive output is unexpectedly long (" << line_count << " lines) - possible "
                               << "duplicate-frame regression. Output:\n"
                               << output;
}

TEST_F(ExampleTests, FilterExampleRuns)
{
    std::string executable = get_executable_path("filter");

    ASSERT_TRUE(executable_exists(executable)) << "Filter example executable not found: " << executable;

    std::string help_output = run_command(executable + " --help");
    EXPECT_TRUE(help_output.find("Usage:") != std::string::npos) << "Help option not working. Output: " << help_output;

    std::string all_output = run_command(executable + " --all");
    EXPECT_TRUE(all_output.find("Running all tests") != std::string::npos) << "All tests option not working";
    EXPECT_TRUE(all_output.find("MathSuite") != std::string::npos) << "Missing MathSuite in filter example";

    std::string suite_output = run_command(executable + " --suite=MathSuite");
    EXPECT_TRUE(suite_output.find("Running suite: MathSuite") != std::string::npos) << "Suite filtering not working";
    EXPECT_TRUE(suite_output.find("MathSuite") != std::string::npos) << "MathSuite not run when filtered";

    std::string test_output = run_command(executable + " --test=MathSuite.Addition");
    EXPECT_TRUE(test_output.find("Running test: MathSuite.Addition") != std::string::npos)
        << "Test filtering not working";
}

TEST_F(ExampleTests, CrashExampleRuns)
{
    std::string executable = get_executable_path("crash");

    ASSERT_TRUE(executable_exists(executable)) << "Crash example executable not found: " << executable;

    // Without a --run-* flag only the basic tests run
    std::string output = run_command(executable);

    EXPECT_TRUE(output.find("testcoe Crash Handling Example") != std::string::npos) << "Missing crash example header";

    EXPECT_TRUE(output.find("BasicTests") != std::string::npos) << "Missing BasicTests in crash example";

    bool has_skip_message =
        (output.find("Skipping") != std::string::npos) || (output.find("SKIP") != std::string::npos) ||
        (output.find("skipped") != std::string::npos) || (output.find("disabled") != std::string::npos);

    EXPECT_TRUE(has_skip_message) << "Crash tests should be skipped by default. Output: " << output;
}

// EXPECT_DEATH must pass cleanly under testcoe's handlers (see the internal_run_death_test guard in src/testcoe.cpp)
TEST_F(ExampleTests, CrashExampleDeathTestPassesCleanly)
{
    std::string executable = get_executable_path("crash");
    ASSERT_TRUE(executable_exists(executable)) << "Crash example executable not found: " << executable;

    int exit_code = -1;
    std::string output = run_command(executable + " --run-death-test", &exit_code);

    EXPECT_EQ(exit_code, 0) << "EXPECT_DEATH regression test should exit 0 (gtest reports it as passed). Output: "
                            << output;

    EXPECT_TRUE(output.find("TEST TERMINATED BY") == std::string::npos)
        << "testcoe's own crash handler must not fire for a passing EXPECT_DEATH test. Output: " << output;

    EXPECT_TRUE(output.find("ALL TESTS PASSED") != std::string::npos)
        << "Death test run should report success via testcoe's own summary. Output: " << output;
}

TEST_F(ExampleTests, CrashExampleHandlesCrashes)
{
    std::string executable = get_executable_path("crash");

    ASSERT_TRUE(executable_exists(executable)) << "Crash example executable not found: " << executable;

    int exit_code = -1;
    std::string crash_output = run_command(executable + " --run-segfault", &exit_code);

    EXPECT_NE(exit_code, 0) << "Crash example should exit non-zero after a segfault. Output: " << crash_output;

    EXPECT_TRUE(crash_output.find("TEST TERMINATED BY") != std::string::npos)
        << "Missing crash termination message. Output: " << crash_output;

    EXPECT_TRUE(crash_output.find("END OF CRASH REPORT") != std::string::npos)
        << "Missing end-of-crash-report marker. Output: " << crash_output;

#if TESTCOE_TEST_EXPECT_FUNCTION_NAMES
    EXPECT_TRUE(crash_output.find("testcoe_crash_example_null_write") != std::string::npos)
        << "Missing crashing function name in stack trace. Output: " << crash_output;
#endif

#if TESTCOE_TEST_EXPECT_SOURCE_LOCATIONS
    EXPECT_TRUE(crash_output.find("crash_tests.cpp:") != std::string::npos)
        << "Missing source file:line in stack trace. Output: " << crash_output;

    EXPECT_TRUE(crash_output.find("testcoe-crash-site") != std::string::npos)
        << "Missing source snippet in stack trace. Output: " << crash_output;
#endif
}

TEST_F(ExampleTests, AllExamplesExist)
{
    std::vector<std::string> examples = {"basic", "crash", "filter"};

    for (const auto &example : examples)
    {
        std::string exe = get_executable_path(example);
        EXPECT_TRUE(executable_exists(exe)) << "Missing example: " << exe;
    }
}

TEST_F(ExampleTests, ExampleExitCodes)
{
    // Basic example should not crash (exit code < 128)
    std::string basic_cmd = get_executable_path("basic");

#ifdef _WIN32
    basic_cmd += " >NUL 2>&1";
#else
    basic_cmd += " >/dev/null 2>&1";
#endif

    int basic_result = system(basic_cmd.c_str());

    // On Unix system() returns the raw wait status (exit code * 256)
#ifndef _WIN32
    if (WIFEXITED(basic_result))
    {
        basic_result = WEXITSTATUS(basic_result);
    }
#endif

    EXPECT_LT(basic_result, 128) << "Basic example crashed with exit code: " << basic_result;

    std::string filter_cmd = get_executable_path("filter");
    filter_cmd += " --help";

#ifdef _WIN32
    filter_cmd += " >NUL 2>&1";
#else
    filter_cmd += " >/dev/null 2>&1";
#endif

    int filter_result = system(filter_cmd.c_str());

#ifndef _WIN32
    if (WIFEXITED(filter_result))
    {
        filter_result = WEXITSTATUS(filter_result);
    }
#endif

    EXPECT_EQ(filter_result, 0) << "Filter example help failed with exit code: " << filter_result;
}
