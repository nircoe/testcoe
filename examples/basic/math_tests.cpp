#include <gtest/gtest.h>
#include <chrono>
#include <thread>

// Helper function to make tests take varying amounts of time
void delayExecution(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Basic math test suite
TEST(MathTest, Addition)
{
    delayExecution(100);
    EXPECT_EQ(2 + 2, 4);
}

TEST(MathTest, Subtraction)
{
    delayExecution(150);
    EXPECT_EQ(5 - 3, 2);
}

TEST(MathTest, Multiplication)
{
    delayExecution(120);
    EXPECT_EQ(3 * 4, 12);
}

TEST(MathTest, Division)
{
    delayExecution(180);
    EXPECT_EQ(10 / 2, 5);
}

// Add an intentional failure to demonstrate failure reporting
TEST(MathTest, IntentionalFail)
{
    delayExecution(130);
    EXPECT_EQ(2 + 2, 5) << "This test is intentionally failing";
}