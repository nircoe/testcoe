#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>

//==============================================================================
// MathSuite - Basic mathematical operations
//==============================================================================

TEST(MathSuite, Addition)
{
    std::cout << "Running MathSuite.Addition" << std::endl;
    EXPECT_EQ(1 + 1, 2);
}

TEST(MathSuite, Subtraction)
{
    std::cout << "Running MathSuite.Subtraction" << std::endl;
    EXPECT_EQ(5 - 3, 2);
}

TEST(MathSuite, Multiplication)
{
    std::cout << "Running MathSuite.Multiplication" << std::endl;
    EXPECT_EQ(2 * 3, 6);
}

TEST(MathSuite, Division)
{
    std::cout << "Running MathSuite.Division" << std::endl;
    EXPECT_EQ(6 / 2, 3);
}

//==============================================================================
// StringSuite - String operations
//==============================================================================

TEST(StringSuite, Length)
{
    std::cout << "Running StringSuite.Length" << std::endl;
    EXPECT_EQ(std::string("hello").length(), 5);
}

TEST(StringSuite, Concatenation)
{
    std::cout << "Running StringSuite.Concatenation" << std::endl;
    EXPECT_EQ(std::string("hello") + " world", "hello world");
}

TEST(StringSuite, Comparison)
{
    std::cout << "Running StringSuite.Comparison" << std::endl;
    EXPECT_LT(std::string("abc"), "def");
}

//==============================================================================
// VectorSuite - Vector operations
//==============================================================================

TEST(VectorSuite, Size)
{
    std::cout << "Running VectorSuite.Size" << std::endl;
    std::vector<int> v = {1, 2, 3};
    EXPECT_EQ(v.size(), 3);
}

TEST(VectorSuite, PushBack)
{
    std::cout << "Running VectorSuite.PushBack" << std::endl;
    std::vector<int> v;
    v.push_back(42);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], 42);
}

TEST(VectorSuite, Clear)
{
    std::cout << "Running VectorSuite.Clear" << std::endl;
    std::vector<int> v = {1, 2, 3};
    v.clear();
    EXPECT_TRUE(v.empty());
}