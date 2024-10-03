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

#include "Capi/au/logger.h"

int
main(int argc, char const* argv[])
{
    Logger* logger = au_logger_create();
    au_logger_add_sink(logger, "ConsoleSink", "Main Console");
    au_logger_add_sink(logger, "FileSink", "AU_Demo.log");
    au_logger_log(logger, "Hello From Logger", AUD_LOG_LEVEL_INFO);
    au_logger_log(logger, "This is default message", AUD_LOG_LEVEL_INFO);
    au_logger_log(logger, "This is trace message", AUD_LOG_LEVEL_TRACE);
    au_logger_log(logger, "This is debug message", AUD_LOG_LEVEL_DEBUG);
    au_logger_log(logger, "This is info message", AUD_LOG_LEVEL_INFO);
    au_logger_log(logger, "This is warn message", AUD_LOG_LEVEL_WARN);
    au_logger_log(logger, "This is error message", AUD_LOG_LEVEL_ERROR);
    au_logger_log(logger, "This is fatal message", AUD_LOG_LEVEL_FATAL);
    au_logger_flush(logger);
    au_logger_destroy(logger);
    return 0;
}
