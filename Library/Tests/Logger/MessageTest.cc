/*
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
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
