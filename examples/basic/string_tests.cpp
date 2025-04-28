#include <gtest/gtest.h>
#include <string>
#include <chrono>
#include <thread>

// Helper function to make tests take varying amounts of time
void delayStringTests(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// String test suite
TEST(StringTest, Length)
{
    delayStringTests(200);
    std::string test = "Hello";
    EXPECT_EQ(test.length(), 5);
}

TEST(StringTest, Concatenation)
{
    delayStringTests(250);
    std::string a = "Hello";
    std::string b = " World";
    EXPECT_EQ(a + b, "Hello World");
}

TEST(StringTest, Comparison)
{
    delayStringTests(150);
    std::string a = "abc";
    std::string b = "def";
    EXPECT_TRUE(a < b);
}

TEST(StringTest, Substring)
{
    delayStringTests(180);
    std::string test = "Hello World";
    EXPECT_EQ(test.substr(0, 5), "Hello");
}