/*
 * Copyright(c) 2024 Advanced Micro Devices, Inc.All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in
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

    std::string getArgument(int index) { return this->m_args[index]; }

    int getArgumentCount() { return this->m_args.size(); }

    bool isArgumentPresent(const std::string& arg)
    {
        return std::find(this->m_args.begin(), this->m_args.end(), arg)
               != this->m_args.end();
    }

    void printArguments()
    {
        for (const auto& a : this->m_args) {
            std::cout << a << std::endl;
        }
    }

    void printHelp()
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "test_logger [options]" << std::endl;
        std::cout << "Options: " << std::endl;
        std::cout << "-h, --help : Print this help message" << std::endl;
        std::cout << "-v : Verbose output" << std::endl;
    }

    bool isHelp()
    {
        return this->isArgumentPresent("-h")
               || this->isArgumentPresent("--help");
    }

    void parse()
    {
        if (this->isHelp()) {
            this->printHelp();
        }
    }

    ~ArgumentParser() {}
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

    // Check if timestamp_construced is inside the ts.getTimestamp()
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
        Priority    p(static_cast<Priority::PriorityLevel>(level));
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
    // , gmock01_write_165{}
    // , gmock00_flush_166{}
    {
        setLoggerName("MockLogger");
    }
};

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

#if 0
int
main()
{
    Timestamp ts;
    std::cout << "Timestamp: " << ts.getTimestamp() << std::endl;
    std::cout << "Hour: " << ts.getHour() << std::endl;
    std::cout << "Minute: " << ts.getMinute() << std::endl;
    std::cout << "Second: " << ts.getSecond() << std::endl;
    std::cout << "Millisecond: " << ts.getMillisecond() << std::endl;
    std::cout << "Microsecond: " << ts.getMicrosecond() << std::endl;
    std::cout << "Nanosecond: " << ts.getNanosecond() << std::endl;

    Message msg("This is a message");
    std::cout << "Message: " << msg.getMsg() << std::endl;

    return 0;
}
#endif
