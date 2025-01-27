/*
 * Copyright(c) 2025 Advanced Micro Devices, Inc. All rights reserved.
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

#include "Au/Logger/Message.hh"
#include <gtest/gtest.h>

using namespace Au::Logger;

TEST(MessageTest, TimestampCoverage)
{
    Timestamp ts;
    // Calls all getters to ensure coverage
    auto strTime = ts.getTimestamp();
    auto hour    = ts.getHour();
    auto minute  = ts.getMinute();
    auto second  = ts.getSecond();
    auto ms      = ts.getMillisecond();
    auto us      = ts.getMicrosecond();
    auto ns      = ts.getNanosecond();

    EXPECT_FALSE(strTime.empty());
    EXPECT_GE(hour, 0);
    EXPECT_LT(hour, 24);
    EXPECT_GE(minute, 0);
    EXPECT_LT(minute, 60);
    EXPECT_GE(second, 0);
    EXPECT_LT(second, 60);
    EXPECT_GE(ms, 0);
    EXPECT_GE(us, 0);
    EXPECT_GE(ns, 0);
    EXPECT_GE(us, ms);
    EXPECT_GE(ns, us);
}

TEST(MessageTest, PriorityCoverage)
{
    // Default priority (Info)
    Priority p1;
    EXPECT_EQ(p1.toStr(), "Info");

    // Custom priority (Error)
    Priority p2(Priority::PriorityLevel::eError);
    EXPECT_EQ(p2.toStr(), "Error");

    // Comparison operators - Error has higher priority than Info
    EXPECT_TRUE(p2 < p1); // Error is lower priority (higher value) than Info
    EXPECT_FALSE(p2 > p1);
    EXPECT_TRUE(p2 <= p1);
    EXPECT_FALSE(p2 >= p1);
    EXPECT_FALSE(p1 == p2);
    EXPECT_TRUE(p1 != p2);
}

TEST(MessageTest, PriorityLevelsComparison)
{
    // Create priorities for all levels
    Priority fatal(Priority::PriorityLevel::eFatal);
    Priority panic(Priority::PriorityLevel::ePanic);
    Priority error(Priority::PriorityLevel::eError);
    Priority warning(Priority::PriorityLevel::eWarning);
    Priority info(Priority::PriorityLevel::eInfo);
    Priority notice(Priority::PriorityLevel::eNotice);
    Priority debug(Priority::PriorityLevel::eDebug);
    Priority trace(Priority::PriorityLevel::eTrace);

    // Verify string representations
    EXPECT_EQ(fatal.toStr(), "Fatal");
    EXPECT_EQ(panic.toStr(), "Panic");
    EXPECT_EQ(error.toStr(), "Error");
    EXPECT_EQ(warning.toStr(), "Warning");
    EXPECT_EQ(info.toStr(), "Info");
    EXPECT_EQ(notice.toStr(), "Notice");
    EXPECT_EQ(debug.toStr(), "Debug");
    EXPECT_EQ(trace.toStr(), "Trace");

    // Test priority ordering (Fatal highest, Trace lowest)
    EXPECT_TRUE(panic > fatal);
    EXPECT_TRUE(error > panic);
    EXPECT_TRUE(warning > error);
    EXPECT_TRUE(info > warning);
    EXPECT_TRUE(notice < info);
    EXPECT_TRUE(debug > notice);
    EXPECT_TRUE(trace > debug);

    // Test >= operator
    EXPECT_TRUE(panic >= panic);
    EXPECT_TRUE(error >= panic);
    EXPECT_FALSE(panic >= error);

    // Test <= operator
    EXPECT_TRUE(warning <= warning);
    EXPECT_TRUE(warning <= info);
    EXPECT_FALSE(info <= warning);

    // Test equality
    EXPECT_TRUE(info == info);
    EXPECT_FALSE(info == warning);

    // Test inequality
    EXPECT_TRUE(debug != trace);
    EXPECT_FALSE(notice != notice);
}

// This test covers the Message class constructors and their functionalities.
TEST(MessageTest, MessageCoverage)
{
    // Single-arg constructor
    Message msg1("Test message");
    EXPECT_TRUE(msg1.getMsg().find("Test message") != std::string::npos);
    EXPECT_EQ(msg1.getPriority().toStr(), "Info");
    EXPECT_FALSE(msg1.getTimestamp().getTimestamp().empty());

    // Two-arg constructor
    Priority p(Priority::PriorityLevel::eWarning);
    Message  msg2("Priority message", p);
    EXPECT_TRUE(msg2.getMsg().find("Priority message") != std::string::npos);
    EXPECT_EQ(msg2.getPriority().toStr(), "Warning");
}
