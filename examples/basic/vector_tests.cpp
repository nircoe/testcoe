#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

void delay_vector_tests(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

TEST(VectorTest, PushBack)
{
    delay_vector_tests(150);
    std::vector<int> v;
    v.push_back(42);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], 42);
}

TEST(VectorTest, Size)
{
    delay_vector_tests(100);
    std::vector<int> v = {1, 2, 3, 4, 5};
    EXPECT_EQ(v.size(), 5);
}

TEST(VectorTest, Clear)
{
    delay_vector_tests(120);
    std::vector<int> v = {1, 2, 3};
    v.clear();
    EXPECT_TRUE(v.empty());
}

TEST(VectorTest, Sort)
{
    delay_vector_tests(220);
    std::vector<int> v = {5, 3, 1, 4, 2};
    std::sort(v.begin(), v.end());

    for (size_t i = 1; i < v.size(); ++i)
    {
        EXPECT_LE(v[i - 1], v[i]) << "Vector not sorted at index " << i;
    }
}
