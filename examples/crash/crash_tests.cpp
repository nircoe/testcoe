#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

class CrashTests : public ::testing::Test
{
};

//==============================================================================
// Segmentation Fault Test
//==============================================================================

// This test demonstrates a segmentation fault (accessing invalid memory)
TEST(CrashTests, SegmentationFault)
{
    std::cout << "This test will cause a segmentation fault by dereferencing a null pointer." << std::endl;

    // Skip by default - uncomment the next line and comment the GTEST_SKIP to run
    // GTEST_SKIP() << "Skipping intentional crash test";

    // This will cause a segmentation fault
    int *nullPtr = nullptr;
    *nullPtr = 42;

    // We should never reach this point
    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Divide By Zero Test
//==============================================================================

// This test demonstrates a floating point exception (division by zero)
TEST(CrashTests, DivideByZero)
{
    std::cout << "This test will cause a floating point exception by dividing by zero." << std::endl;

    // Skip by default - uncomment the next line and comment the GTEST_SKIP to run
    // GTEST_SKIP() << "Skipping intentional crash test";

    // This will cause a floating point exception
    volatile int zero = 0;
    volatile int result = 10 / zero;

    // We should never reach this point
    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Abort Test
//==============================================================================

// This test demonstrates handling of abort() calls
TEST(CrashTests, Abort)
{
    std::cout << "This test will cause a program abort." << std::endl;

    // Skip by default - uncomment the next line and comment the GTEST_SKIP to run
    // GTEST_SKIP() << "Skipping intentional crash test";

    // This will abort the program
    std::abort();

    // We should never reach this point
    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Stack Overflow Test
//==============================================================================

// Recursive function with no termination condition
void infiniteRecursion(int depth)
{
    // Create a local variable to consume stack space
    char buffer[1024] = {0};

    // Print depth occasionally
    if (depth % 100 == 0)
    {
        std::cout << "Recursion depth: " << depth << std::endl;
    }

    // Recurse deeper with no end condition
    infiniteRecursion(depth + 1);
}

// This test demonstrates a stack overflow
TEST(CrashTests, StackOverflow)
{
    std::cout << "This test will cause a stack overflow through infinite recursion." << std::endl;

    // Skip by default - uncomment the next line and comment the GTEST_SKIP to run
    GTEST_SKIP() << "Skipping intentional crash test";

    // This will cause a stack overflow
    infiniteRecursion(1);

    // We should never reach this point
    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Out of Bounds Access Test
//==============================================================================

// This test demonstrates an out-of-bounds memory access
TEST(CrashTests, OutOfBounds)
{
    std::cout << "This test will cause an out-of-bounds access." << std::endl;

    // Skip by default - uncomment the next line and comment the GTEST_SKIP to run
    GTEST_SKIP() << "Skipping intentional crash test";

    std::vector<int> v(5);
    std::cout << "Vector size: " << v.size() << std::endl;

    // This will cause an out-of-bounds access
    v.at(10) = 42;

    // We should never reach this point
    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Stream Redirection Test
//==============================================================================

// This test demonstrates how TestCOE handles crashes with redirected streams
TEST(CrashTests, StreamRedirection)
{
    std::cout << "This test will demonstrate stream redirection during a crash." << std::endl;

    // Skip by default - uncomment the next line and comment the GTEST_SKIP to run
    GTEST_SKIP() << "Skipping intentional crash test";

    // Redirect stdout to a stringstream
    std::stringstream buffer;
    std::streambuf *oldBuf = std::cout.rdbuf(buffer.rdbuf());

    std::cout << "This should be captured in the buffer" << std::endl;

    // Cause a crash
    int *nullPtr = nullptr;
    *nullPtr = 42;

    // This should never be reached
    std::cout.rdbuf(oldBuf);
    std::cout << "Buffer contained: " << buffer.str() << std::endl;
    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Non-crashing Tests
//==============================================================================

// These tests ensure the example runs successfully when no crash tests are enabled

class BasicTests : public ::testing::Test
{
};

TEST(BasicTests, Addition)
{
    EXPECT_EQ(2 + 2, 4);
}

TEST(BasicTests, Subtraction)
{
    EXPECT_EQ(5 - 3, 2);
}