#include <gtest/gtest.h>
#include <chrono>
#include <thread>

void delay_execution(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

TEST(MathTest, Addition)
{
    delay_execution(100);
    EXPECT_EQ(2 + 2, 4);
}

TEST(MathTest, Subtraction)
{
    delay_execution(150);
    EXPECT_EQ(5 - 3, 2);
}

TEST(MathTest, Multiplication)
{
    delay_execution(120);
    EXPECT_EQ(3 * 4, 12);
}

TEST(MathTest, Division)
{
    delay_execution(180);
    EXPECT_EQ(10 / 2, 5);
}

TEST(MathTest, IntentionalFail)
{
    delay_execution(130);
    EXPECT_EQ(2 + 2, 5) << "This test is intentionally failing";
}
