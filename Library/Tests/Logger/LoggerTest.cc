/*
 * Copyright(c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <fstream>
#include <iostream>
#include <thread>

#include "Au/Logger/LogManager.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace Au::Logger;

bool verbose = false;

// Argument Parser
class ArgumentParser
{
  private:
    std::vector<std::string> m_args;

  public:
    ArgumentParser(int argc, char** argv)
        : m_args()
    {
        for (int i = 1; i < argc; ++i) {
            this->m_args.push_back(argv[i]);
        }
    }

    std::string getArgument(size_t index) const
    {
        if (index < this->m_args.size()) {
            return this->m_args[index];
        }
        throw std::out_of_range("Index out of range");
    }

    int getArgumentCount() { return this->m_args.size(); }

    bool isArgumentPresent(const std::string& arg) const
    {
        return std::find(this->m_args.begin(), this->m_args.end(), arg)
               != this->m_args.end();
    }

    void printArguments() const
    {
        for (const auto& a : this->m_args) {
            std::cout << a << std::endl;
        }
    }

    void printHelp() const
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "test_logger [options]" << std::endl;
        std::cout << "Options: " << std::endl;
        std::cout << "-h, --help : Print this help message" << std::endl;
        std::cout << "-v : Verbose output" << std::endl;
    }

    bool isHelp() const
    {
        return this->isArgumentPresent("-h")
               || this->isArgumentPresent("--help");
    }

    void parse() const
    {
        if (this->isHelp()) {
            this->printHelp();
        }
    }

    ~ArgumentParser() = default;
};

TEST(LoggerTest, TimestampTest)
{
    Timestamp ts;

    if (verbose) {
        std::cout << "Timestamp: " << ts.getTimestamp() << std::endl;
        std::cout << "Hour: " << ts.getHour() << std::endl;
        std::cout << "Minute: " << ts.getMinute() << std::endl;
        std::cout << "Second: " << ts.getSecond() << std::endl;
        std::cout << "Millisecond: " << ts.getMillisecond() << std::endl;
        std::cout << "Microsecond: " << ts.getMicrosecond() << std::endl;
        std::cout << "Nanosecond: " << ts.getNanosecond() << std::endl;
    }

    std::string timestamp_constructed =
        std::to_string(ts.getHour()) + ":" + (ts.getMinute() < 10 ? "0" : "")
        + std::to_string(ts.getMinute()) + ":"
        + (ts.getSecond() < 10 ? "0" : "") + std::to_string(ts.getSecond());

    // Check if timestamp_constructed is inside the ts.getTimestamp()
    EXPECT_TRUE(ts.getTimestamp().find(timestamp_constructed)
                != std::string::npos)
        << "Timestamp: " << ts.getTimestamp()
        << " Constructed: " << timestamp_constructed;
}

TEST(LoggerTest, MessageTest)
{
    // Default log Message check
    Message msg("This is a message");

    if (verbose) {
        std::cout << "Message: " << msg.getMsg() << std::endl;
    }

    // Check if the message is same as the one passed
    EXPECT_TRUE(msg.getMsg().find("This is a message") != std::string::npos);
    EXPECT_TRUE(msg.getMsg().find("Info") != std::string::npos);

    std::vector<Priority::PriorityLevel> priorityLevels = {
        Priority::PriorityLevel::eFatal,  Priority::PriorityLevel::ePanic,
        Priority::PriorityLevel::eError,  Priority::PriorityLevel::eWarning,
        Priority::PriorityLevel::eNotice, Priority::PriorityLevel::eInfo,
        Priority::PriorityLevel::eDebug,  Priority::PriorityLevel::eTrace,
    };
    // Loop through priority levels
    for (auto level : priorityLevels) {
        Priority    p(level);
        std::string priorityName = p.toStr();

        // Log Message check
        std::string messageText = "This is a " + priorityName + " message";
        msg                     = Message(messageText, p);

        if (verbose) {
            std::cout << "Message: " << msg.getMsg() << std::endl;
        }

        // Check if the message is same as the one passed
        EXPECT_TRUE(msg.getMsg().find(messageText) != std::string::npos);
        EXPECT_TRUE(msg.getMsg().find(priorityName) != std::string::npos);

        // Check if the priority is same as the one passed
        EXPECT_TRUE(msg.getPriority() == p);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class MockLogger : public GenericLogger
{
  public:
    MOCK_METHOD(void, write, (const Message& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    std::string getLoggerType() const override { return "MockLogger"; }
    // Destructor
    virtual ~MockLogger() {}
    MockLogger()
        : GenericLogger()
    {
        setLoggerName("MockLogger");
    }
};
#pragma GCC diagnostic pop

TEST(LoggerTest, LoggerFactoryTest)
{
    // Lets use gmock to mock logger classes

    // Create a mockLogger
    std::unique_ptr<MockLogger> mockLogger = std::make_unique<MockLogger>();

#if 1
    // Set the expectations
    for (int i = 0; i < 10; i++) {
        // Mock method to write
        EXPECT_CALL(*mockLogger.get(),
                    write(testing::Truly([i](const Message& msg) {
                        return msg.getMsg().find("This is a message "
                                                 + std::to_string(i))
                               != std::string::npos;
                    })))
            .Times(1);
    }
#else
    EXPECT_CALL(*mockLogger.get(), write(testing::Truly([](const Message& msg) {
        return msg.getMsg().find("This is a message ") != std::string::npos;
    }))).Times(10);
#endif

    // Mock method to flush
    EXPECT_CALL(*mockLogger.get(), flush()).Times(1);

    // Set the mock logger to the singleton LogWriter
    LogWriter::setLogger(std::move(mockLogger));

    // Get the singleton instance of LogWriter
    auto logWriter = LogWriter::getLogWriter();

    std::vector<Message> messageVect = {};
    for (int i = 0; i < 10; i++) {
        messageVect.emplace_back("This is a message " + std::to_string(i));
        // Print Message
        if (verbose) {
            std::cout << "Message: " << messageVect.back().getMsg()
                      << std::endl;
        }
    }
    // Write messages to logWriter
    logWriter->log(messageVect);

    logWriter->stop();
}

TEST(LoggerTest, LoggerClass)
{
    // Create a MockLogger
    std::unique_ptr<MockLogger> mockLogger = std::make_unique<MockLogger>();

    // Create expectations
    EXPECT_CALL(*mockLogger.get(), write(testing::Truly([](const Message& msg) {
        return msg.getMsg().find("This is a message") != std::string::npos;
    }))).Times(1);

    // Create expectations
    EXPECT_CALL(*mockLogger.get(), write(testing::Truly([](const Message& msg) {
        return msg.getMsg().find("This is another message")
               != std::string::npos;
    }))).Times(1);

    // Mock method to flush
    EXPECT_CALL(*mockLogger.get(), flush()).Times(1);

    // Set the mock logger to the singleton LogWriter
    LogWriter::setLogger(std::move(mockLogger));

    // Get the singleton instance of LogWriter
    auto logWriter = LogWriter::getLogWriter();

    // Create a Logger
    LogManager logger(logWriter);

    // Create a Message
    Message msg("This is a message");

    // Log the message
    logger.log(msg);

    // Create another Message
    Message msg2("This is another message");

    // Log the message
    logger.log(msg2);

    // Flush the logger
    logger.flush();

    // Stop the logWriter
    logWriter->stop();
}

TEST(LoggerTest, FileLoggerTest)
{
    const std::string testFilename = "test_file_logger.log";
    {
        auto fileLogger =
            LoggerFactory::createLogger("FileLogger", testFilename);

        EXPECT_EQ(fileLogger->getLoggerType(), "FileLogger");

        LogWriter::setLogger(std::move(fileLogger));
        LogManager logManager(LogWriter::getLogWriter());

        logManager << "This is a FileLogger test message";
        logManager.flush();

        LogWriter::getLogWriter()->stop();
    }

    std::ifstream infile(testFilename);
    ASSERT_TRUE(infile.good()) << "Log file not created";
    std::string content((std::istreambuf_iterator<char>(infile)),
                        std::istreambuf_iterator<char>());
    infile.close();
    EXPECT_NE(content.find("This is a FileLogger test message"),
              std::string::npos);

    // Clean up
    std::remove(testFilename.c_str());
}

TEST(LoggerTest, GenericLoggerTest)
{
    // We expect validateLoggerType to throw
    EXPECT_THROW(LoggerFactory::validateLoggerType("GenericLogger"),
                 std::invalid_argument);

    // We expect createLogger to return nullptr
    auto logger = LoggerFactory::createLogger("GenericLogger", "GenericLogger");

    EXPECT_EQ(logger, nullptr);

    GenericLogger logger1;
    EXPECT_EQ(logger1.getLoggerType(), "GenericLogger");

    // These tests can only be run in debug mode
#ifndef NDEBUG
    // Test that GenericLogger::flush() asserts
    EXPECT_DEATH(logger1.flush(), ".*");

    // Test that GenericLogger::write() asserts
    Message msg("Test message");
    EXPECT_DEATH(logger1.write(msg), ".*");
#endif

    // Test logger name get/set
    logger1.setLoggerName("TestLogger");
    EXPECT_EQ(logger1.getLoggerName(), "TestLogger");
}

TEST(LoggerTest, ConsoleLoggerTest)
{
    auto consoleLogger =
        LoggerFactory::createLogger("ConsoleLogger", "ConsoleLogger");

    EXPECT_EQ(consoleLogger->getLoggerType(), "ConsoleLogger");

    LogWriter::setLogger(std::move(consoleLogger));
    LogManager logger(LogWriter::getLogWriter());
    Message    msg("This is a ConsoleLogger test message");

    testing::internal::CaptureStdout();
    logger << msg;
    logger.flush();
    LogWriter::getLogWriter()->stop();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("This is a ConsoleLogger test message"),
              std::string::npos);
}

TEST(LoggerTest, DummyLoggerTest)
{
    testing::internal::CaptureStdout();
    auto dummyLogger =
        LoggerFactory::createLogger("DummyLogger", "TestDummyLogger");
    EXPECT_EQ(dummyLogger->getLoggerType(), "DummyLogger");

    LogWriter::setLogger(std::move(dummyLogger));
    LogManager logger(LogWriter::getLogWriter());
    Message    msg("This is a DummyLogger test message");

    logger.log(msg);
    logger.flush();
    LogWriter::getLogWriter()->stop();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.empty());
}

// Gtest main with an argument parser
int
main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    ArgumentParser parser(argc, argv);
    parser.parse();
    verbose = parser.isArgumentPresent("-v");
    return RUN_ALL_TESTS();
}
