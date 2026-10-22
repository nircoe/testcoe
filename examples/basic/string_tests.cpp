#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include <thread>

void delay_string_tests(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

TEST(StringTest, Length)
{
    delay_string_tests(200);
    std::string test = "Hello";
    EXPECT_EQ(test.length(), 5);
}

TEST(StringTest, Concatenation)
{
    delay_string_tests(250);
    std::string a = "Hello";
    std::string b = " World";
    EXPECT_EQ(a + b, "Hello World");
}

TEST(StringTest, Comparison)
{
    delay_string_tests(150);
    std::string a = "abc";
    std::string b = "def";
    EXPECT_TRUE(a < b);
}

TEST(StringTest, Substring)
{
    delay_string_tests(180);
    std::string test = "Hello World";
    EXPECT_EQ(test.substr(0, 5), "Hello");
}
