#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

#if defined(__GNUC__) || defined(__clang__)
#define TESTCOE_CRASH_NOINLINE [[gnu::noinline]]
#elif defined(_MSC_VER)
#define TESTCOE_CRASH_NOINLINE __declspec(noinline)
#else
#define TESTCOE_CRASH_NOINLINE
#endif

class CrashTests : public ::testing::Test
{
};

//==============================================================================
// Segmentation Fault Test
//==============================================================================

// Named, non-inlined, externally-linked so the crash backend can resolve a symbol for it
TESTCOE_CRASH_NOINLINE void testcoeCrashExampleNullWrite()
{
    // Both the pointer and the write are volatile, or GCC -O2 sees a known null write and drops the call
    volatile int *volatile nullPtr = nullptr;
    *nullPtr = 42; // testcoe-crash-site
}

TEST(CrashTests, SegmentationFault)
{
    std::cout << "This test will cause a segmentation fault by dereferencing a null pointer." << std::endl;

    testcoeCrashExampleNullWrite();

    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Divide By Zero Test
//==============================================================================

TEST(CrashTests, DivideByZero)
{
    std::cout << "This test will cause a floating point exception by dividing by zero." << std::endl;

    volatile int zero = 0;
    volatile int result = 10 / zero;

    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Abort Test
//==============================================================================

TEST(CrashTests, Abort)
{
    std::cout << "This test will cause a program abort." << std::endl;

    std::abort();

    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Death Test (regression test for testcoe/gtest death-test signal handler collision)
//==============================================================================

// Must pass cleanly, unlike the other tests here.
// gtest's EXPECT_DEATH handles the abort, so testcoe must stay out of its way. Never skipped.
TEST(CrashTests, AbortDeathTest)
{
    EXPECT_DEATH(std::abort(), "");
}

//==============================================================================
// Stack Overflow Test
//==============================================================================

void infiniteRecursion(int depth)
{
    char buffer[1024] = {0};

    if (depth % 100 == 0)
    {
        std::cout << "Recursion depth: " << depth << std::endl;
    }

    infiniteRecursion(depth + 1);
}

TEST(CrashTests, StackOverflow)
{
    std::cout << "This test will cause a stack overflow through infinite recursion." << std::endl;

    GTEST_SKIP() << "Skipping intentional crash test";

    infiniteRecursion(1);

    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Out of Bounds Access Test
//==============================================================================

TEST(CrashTests, OutOfBounds)
{
    std::cout << "This test will cause an out-of-bounds access." << std::endl;

    GTEST_SKIP() << "Skipping intentional crash test";

    std::vector<int> v(5);
    std::cout << "Vector size: " << v.size() << std::endl;

    v.at(10) = 42;

    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Stream Redirection Test
//==============================================================================

TEST(CrashTests, StreamRedirection)
{
    std::cout << "This test will demonstrate stream redirection during a crash." << std::endl;

    GTEST_SKIP() << "Skipping intentional crash test";

    std::stringstream buffer;
    std::streambuf *oldBuf = std::cout.rdbuf(buffer.rdbuf());

    std::cout << "This should be captured in the buffer" << std::endl;

    int *nullPtr = nullptr;
    *nullPtr = 42;

    std::cout.rdbuf(oldBuf);
    std::cout << "Buffer contained: " << buffer.str() << std::endl;
    FAIL() << "Test did not crash as expected";
}

//==============================================================================
// Non-crashing Tests
//==============================================================================

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
