#include <gtest/gtest.h>
#include <testcoe.hpp>
#include <sstream>
#include <memory>

class GridListenerTests : public ::testing::Test
{
protected:
    std::streambuf *m_originalCoutBuf;
    std::streambuf *m_originalCerrBuf;
    std::stringstream m_capturedOutput;

    void SetUp() override
    {
        m_originalCoutBuf = std::cout.rdbuf();
        m_originalCerrBuf = std::cerr.rdbuf();
        std::cout.rdbuf(m_capturedOutput.rdbuf());
        std::cerr.rdbuf(m_capturedOutput.rdbuf());
    }

    void TearDown() override
    {
        std::cout.rdbuf(m_originalCoutBuf);
        std::cerr.rdbuf(m_originalCerrBuf);
    }
};

TEST_F(GridListenerTests, TestStatusEnumValues)
{
    EXPECT_EQ(static_cast<char>(testcoe::TestStatus::NotRun), '.');
    EXPECT_EQ(static_cast<char>(testcoe::TestStatus::Running), 'R');
    EXPECT_EQ(static_cast<char>(testcoe::TestStatus::Passed), 'P');
    EXPECT_EQ(static_cast<char>(testcoe::TestStatus::Failed), 'F');
    EXPECT_EQ(static_cast<char>(testcoe::TestStatus::None), '\0');
}

TEST_F(GridListenerTests, ColorDefinitions)
{
    bool ansiEnabled = testcoe::terminal::isAnsiEnabled();

    bool resetEmpty = testcoe::color::reset.empty();
    EXPECT_TRUE(ansiEnabled ? !resetEmpty : resetEmpty);
    bool redEmpty = testcoe::color::red.empty();
    EXPECT_TRUE(ansiEnabled ? !redEmpty : redEmpty);
    bool greenEmpty = testcoe::color::green.empty();
    EXPECT_TRUE(ansiEnabled ? !greenEmpty : greenEmpty);
    bool yellowEmpty = testcoe::color::yellow.empty();
    EXPECT_TRUE(ansiEnabled ? !yellowEmpty : yellowEmpty);
    bool boldEmpty = testcoe::color::bold.empty();
    EXPECT_TRUE(ansiEnabled ? !boldEmpty : boldEmpty);
}

TEST_F(GridListenerTests, ConstructorDestructor)
{
    EXPECT_NO_THROW(
    {
        auto emptyListener = new ::testing::EmptyTestEventListener();
        testcoe::GridTestListener gridListener(emptyListener);
    });
}

TEST_F(GridListenerTests, ConstructorExceptionHandling)
{
    EXPECT_THROW({ testcoe::GridTestListener gridListener(nullptr); }, std::invalid_argument);
}

TEST_F(GridListenerTests, InstallGridListener)
{
    EXPECT_NO_THROW(
    {
        auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
        auto defaultListener = listeners.Release(listeners.default_result_printer());

        testcoe::installGridListener();

        while (listeners.Release(listeners.default_result_printer())) { }
        listeners.Append(defaultListener);
    });
}

TEST_F(GridListenerTests, OnTestProgramStart)
{
    auto emptyListener = new ::testing::EmptyTestEventListener();
    testcoe::GridTestListener gridListener(emptyListener);

    m_capturedOutput.str("");

    const ::testing::UnitTest &unitTest = *::testing::UnitTest::GetInstance();
    gridListener.OnTestProgramStart(unitTest);

    std::string output = m_capturedOutput.str();
    EXPECT_FALSE(output.empty());
    EXPECT_TRUE(output.find("Running") != std::string::npos);
}

TEST_F(GridListenerTests, TerminalUtilities)
{
    EXPECT_NO_THROW({ bool ansiEnabled = testcoe::terminal::isAnsiEnabled(); });

    EXPECT_NO_THROW({ testcoe::terminal::clear(); });
}