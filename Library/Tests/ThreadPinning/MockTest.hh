/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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
#include "Au/ThreadPinning.hh"
#include "Au/ThreadPinning/ThreadPinning.hh"
#include "Au/Types.hh"
#include "gtest/gtest.h"

namespace {
using namespace Au;

class ThreadPinningTest
    : public ::testing::TestWithParam<
          std::tuple<String,
                     int,
                     int,
                     std::vector<std::vector<CoreMask>>,
                     std::vector<std::vector<CoreMask>>,
                     std::vector<CoreMask>,
                     std::vector<int>,
                     std::vector<int>,
                     std::vector<int>>>
{};
// clang-format off
auto testParametersCpuTopology = {
    // Test case 1: No hyperThreading Singlecache number of threads less than
    // number of processors
    std::make_tuple(
        String("Test1: NoHT_SingleCache_PLTC"),
        8,
        2,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1, 0) },
            { std::make_pair(0b10, 0) },
            { std::make_pair(0b100, 0) },
            { std::make_pair(0b1000, 0) },
            { std::make_pair(0b10000, 0) },
            { std::make_pair(0b100000, 0) },
            { std::make_pair(0b1000000, 0) },
            { std::make_pair(0b10000000, 0) },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b11111111, 8) },
        std::vector<int>{ 0, 1 },
        std::vector<int>{ 0, 1 },
        std::vector<int>{ 0, 4 }),

    // Test case 2: No hyperThreading Singlecache number of threads greater than
    // number of processors

    std::make_tuple(
        String("Test2: NoHT_SingleCache_PGTP"),
        8,
        16,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            { std::make_pair(0b10000000, 0) },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b11111111, 8) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 }),

    // Test case 3: No hyperThreading Singlecache number of threads equal to
    // number of processors

    std::make_tuple(
        String("Test3: NoHT_SingleCache_PETP"),
        8,
        8,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111, 0) } },
        std::vector<CoreMask>{ std::make_pair(0b11111111, 8) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7 }),
    // Test case 4: No hyperThreading Multicache number of threads less than
    // number of processors
    std::make_tuple(
        String("Test4: NoHT_MultiCache_PLTC"),
        32,
        15,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1111111111111111, 0),
            },
            {
                std::make_pair(0b11111111111111110000000000000000, 0),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 }),
    // Test case 5: No hyperThreading Multicache number of threads greater than
    // number of processors

    std::make_tuple(
        String("Test5: NoHT_MultiCache_PGTP"),
        16,
        35,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11111111, 0),
            },
            {
                std::make_pair(0b1111111100000000, 0),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(0b111111111111111, 16) },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11,
                          12, 13, 14, 15, 0,  1,  2,  3,  4, 5, 6,  7,
                          8,  9,  10, 11, 12, 13, 14, 15, 0, 1, 2 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11,
                          12, 13, 14, 15, 0,  1,  2,  3,  4, 5, 6,  7,
                          8,  9,  10, 11, 12, 13, 14, 15, 0, 1, 2 },
        std::vector<int>{ 0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4, 4,
                          5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10,
                          10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15 }),
    // Test case 6: No HyperThreading Multicache number of threads equal to
    // number of processors
    std::make_tuple(String("Test6: NoHT_MultiCache_PETP"),
                    16,
                    16,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b1, 0),
                        },
                        {
                            std::make_pair(0b10, 0),
                        },
                        {
                            std::make_pair(0b100, 0),
                        },
                        {
                            std::make_pair(0b1000, 0),
                        },
                        {
                            std::make_pair(0b10000, 0),
                        },
                        {
                            std::make_pair(0b100000, 0),
                        },
                        {
                            std::make_pair(0b1000000, 0),
                        },
                        {
                            std::make_pair(0b10000000, 0),
                        },
                        {
                            std::make_pair(0b100000000, 0),
                        },
                        {
                            std::make_pair(0b1000000000, 0),
                        },
                        {
                            std::make_pair(0b10000000000, 0),
                        },
                        {
                            std::make_pair(0b100000000000, 0),
                        },
                        {
                            std::make_pair(0b1000000000000, 0),
                        },
                        {
                            std::make_pair(0b10000000000000, 0),
                        },
                        {
                            std::make_pair(0b100000000000000, 0),
                        },
                        {
                            std::make_pair(0b1000000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11111111, 0),
                        },
                        {
                            std::make_pair(0b1111111100000000, 0),
                        },
                    },
                    std::vector<CoreMask>{
                        std::make_pair(0b111111111111111, 16) },
                    std::vector<int>{
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
                    std::vector<int>{
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
                    std::vector<int>{
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }),
    // Test case 7: No HyperThreading Multicache multicore group number of
    // threads less than number of processors
    std::make_tuple(
        String("Test7: NoHT_MultiCache_PLTC"),
        96,
        15,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(0b1, 1),
            },
            {
                std::make_pair(0b10, 1),
            },
            {
                std::make_pair(0b100, 1),
            },
            {
                std::make_pair(0b1000, 1),
            },
            {
                std::make_pair(0b10000, 1),
            },
            {
                std::make_pair(0b100000, 1),
            },
            {
                std::make_pair(0b1000000, 1),
            },
            {
                std::make_pair(0b10000000, 1),
            },
            {
                std::make_pair(0b100000000, 1),
            },
            {
                std::make_pair(0b1000000000, 1),
            },
            {
                std::make_pair(0b10000000000, 1),
            },
            {
                std::make_pair(0b100000000000, 1),
            },
            {
                std::make_pair(0b1000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(
                    0b1111111111111111111111111111111111111111111111111111111111111111,
                    0),
            },
            {
                std::make_pair(0b11111111111111111111111111111111, 1),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(
                0b1111111111111111111111111111111111111111111111111111111111111111,
                64),
            std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{
            0, 8, 16, 24, 32, 40, 48, 56, 64, 68, 72, 76, 80, 84, 88 }),
    // Test case 8: No HyperThreading Multicache multicore group number of
    // threads greater than number of processors

    std::make_tuple(
        String("Test8: NoHT_MultiCache_PGTC"),
        96,
        100,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b10000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b100000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b1000000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b10000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b100000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1000000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(0b1, 1),
            },
            {
                std::make_pair(0b10, 1),
            },
            {
                std::make_pair(0b100, 1),
            },
            {
                std::make_pair(0b1000, 1),
            },
            {
                std::make_pair(0b10000, 1),
            },
            {
                std::make_pair(0b100000, 1),
            },
            {
                std::make_pair(0b1000000, 1),
            },
            {
                std::make_pair(0b10000000, 1),
            },
            {
                std::make_pair(0b100000000, 1),
            },
            {
                std::make_pair(0b1000000000, 1),
            },
            {
                std::make_pair(0b10000000000, 1),
            },
            {
                std::make_pair(0b100000000000, 1),
            },
            {
                std::make_pair(0b1000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(
                    0b1111111111111111111111111111111111111111111111111111111111111111,
                    0),
            },
            {
                std::make_pair(0b11111111111111111111111111111111, 1),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(
                0b1111111111111111111111111111111111111111111111111111111111111111,
                64),
            std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 0,  1,  2,  3 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 0,  1,  2,  3 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  8,  9,  10, 12, 13, 14, 16, 17, 18, 20,
            21, 22, 24, 25, 26, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 40, 41,
            42, 44, 45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64,
            64, 65, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74,
            75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85,
            86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95 }),
    // Test case 9:  No HyperThreading Multicache multicore group number of
    // threads equal to number of processors
    // Test case 10: No HyperThreading Singlecache multicore group number of
    // threads less than number of processors
    // The current facilities in windows indicates that an L3 cache is shared by
    // maximum of 64 cores, see GroupMask.Mask from _CACHE_RELATIONSHIP
    // structure
    // for now we will test core group containing 16 cores each. for all of them
    // sharing same cache
    std::make_tuple(
        String("Test 10: NoHT_SingleCache_PLTC"),
        32,
        15,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000, 0),
            },
            {
                std::make_pair(0b1, 1),
            },
            {
                std::make_pair(0b10, 1),
            },
            {
                std::make_pair(0b100, 1),
            },
            {
                std::make_pair(0b1000, 1),
            },
            {
                std::make_pair(0b10000, 1),
            },
            {
                std::make_pair(0b100000, 1),
            },
            {
                std::make_pair(0b1000000, 1),
            },
            {
                std::make_pair(0b10000000, 1),
            },
            {
                std::make_pair(0b100000000, 1),
            },
            {
                std::make_pair(0b1000000000, 1),
            },
            {
                std::make_pair(0b10000000000, 1),
            },
            {
                std::make_pair(0b100000000000, 1),
            },
            {
                std::make_pair(0b1000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111111111111111111111111111, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111, 16),
            std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 }),
    // Test case 11: No HyperThreading Singlecache multicore group number of
    // threads greater than number of processors
    std::make_tuple(
        String("Test11: NoHT_SingleCache_PGTP"),
        32,
        35,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
            {
                std::make_pair(0b1000000000000000, 0),
            },
            {
                std::make_pair(0b1, 1),
            },
            {
                std::make_pair(0b10, 1),
            },
            {
                std::make_pair(0b100, 1),
            },
            {
                std::make_pair(0b1000, 1),
            },
            {
                std::make_pair(0b10000, 1),
            },
            {
                std::make_pair(0b100000, 1),
            },
            {
                std::make_pair(0b1000000, 1),
            },
            {
                std::make_pair(0b10000000, 1),
            },
            {
                std::make_pair(0b100000000, 1),
            },
            {
                std::make_pair(0b1000000000, 1),
            },
            {
                std::make_pair(0b10000000000, 1),
            },
            {
                std::make_pair(0b100000000000, 1),
            },
            {
                std::make_pair(0b1000000000000, 1),
            },
            {
                std::make_pair(0b10000000000000, 1),
            },
            {
                std::make_pair(0b100000000000000, 1),
            },
            {
                std::make_pair(0b1000000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111111111111111111111111111, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111, 16),
            std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31, 0,  1,  2 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31, 0,  1,  2 },
        std::vector<int>{ 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,
                          10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20,
                          21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 }),
    // Test case 12: No HyperThreading Singlecache multicore group number of
    // threads equal to number of processors
    // Test case 13: HyperThreading Singlecache number of threads less than
    // number of processors
    std::make_tuple(String("Test13: HT_SingleCache_PLTC"),
                    16,
                    9,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11, 0),
                        },
                        {
                            std::make_pair(0b1100, 0),
                        },
                        {
                            std::make_pair(0b110000, 0),
                        },
                        {
                            std::make_pair(0b11000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        { std::make_pair(0b1111111111111111, 0) } },
                    std::vector<CoreMask>{
                        std::make_pair(0b1111111111111111, 16) },
                    std::vector<int>{ 0, 2, 4, 6, 8, 10, 12, 14, 1 },
                    std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
                    std::vector<int>{ 0, 1, 2, 4, 6, 8, 10, 12, 14 }),
    // Test case 14: HyperThreading Singlecache number of threads greater than
    // number of processors
    std::make_tuple(
        String("Test14: HT_SingleCache_PGTP"),
        16,
        18,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111111111, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{
            0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15, 0, 2 },
        std::vector<int>{
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1 },
        std::vector<int>{
            0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15 }),
    // Test case 15: HyperThreading Singlecache number of threads equal to
    // number of processors
    std::make_tuple(String("Test15: HT_SingleCache_PETP"),
                    16,
                    16,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11, 0),
                        },
                        {
                            std::make_pair(0b1100, 0),
                        },
                        {
                            std::make_pair(0b110000, 0),
                        },
                        {
                            std::make_pair(0b11000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        { std::make_pair(0b1111111111111111, 0) } },
                    std::vector<CoreMask>{
                        std::make_pair(0b1111111111111111, 16) },
                    std::vector<int>{
                        0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15 },
                    std::vector<int>{
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
                    std::vector<int>{
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }),
    // Test case 16: HyperThreading Multicache Singlecore group number of
    // threads less than number of processors
    std::make_tuple(
        String("Test16: HT_MultiCache_Singlecore_PLTC"),
        32,
        15,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1111111111111111, 0),
            },
            {
                std::make_pair(0b11111111111111110000000000000000, 0),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 }),
    // Test case 17: HyperThreading Multicache Singlecore group number of
    // threads greater than number of processors
    std::make_tuple(
        String("Test17: HT_MultiCache_Singlecore_PGTP"),
        16,
        35,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11111111, 0),
            },
            {
                std::make_pair(0b1111111100000000, 0),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0, 2,  4,  6,  8, 10, 12, 14, 1, 3,  5,  7,
                          9, 11, 13, 15, 0, 2,  4,  6,  8, 10, 12, 14,
                          1, 3,  5,  7,  9, 11, 13, 15, 0, 2,  4 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11,
                          12, 13, 14, 15, 0,  1,  2,  3,  4, 5, 6,  7,
                          8,  9,  10, 11, 12, 13, 14, 15, 0, 1, 2 },
        std::vector<int>{ 0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4, 4,
                          5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10,
                          10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15 }),

    // Test case 18: HyperThreading Multicache Singlecore group number of
    // threads equal to number of processors
    std::make_tuple(String("Test18: HT_MultiCache_Singlecore_PETP"),
                    16,
                    16,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11, 0),
                        },
                        {
                            std::make_pair(0b1100, 0),
                        },
                        {
                            std::make_pair(0b110000, 0),
                        },
                        {
                            std::make_pair(0b11000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11111111, 0),
                        },
                        {
                            std::make_pair(0b1111111100000000, 0),
                        } },
                    std::vector<CoreMask>{
                        std::make_pair(0b1111111111111111, 16) },
                    std::vector<int>{
                        0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15 },
                    std::vector<int>{
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
                    std::vector<int>{
                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }),
    // Test case 19: HyperThreading Multicache multicore group number of threads
    // less than number of processors
    std::make_tuple(
        String("Test19: NoHT_MultiCache_PLTC"),
        96,
        15,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b1100000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(
                    0b110000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b11000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b1100000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b110000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b11000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1100000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b110000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b11000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1100000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(0b11, 1),
            },
            {
                std::make_pair(0b1100, 1),
            },
            {
                std::make_pair(0b110000, 1),
            },
            {
                std::make_pair(0b11000000, 1),
            },
            {
                std::make_pair(0b1100000000, 1),
            },
            {
                std::make_pair(0b110000000000, 1),
            },
            {
                std::make_pair(0b11000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000, 1),
            },
            {
                std::make_pair(0b110000000000000000, 1),
            },
            {
                std::make_pair(0b11000000000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000000000, 1),
            },
            {
                std::make_pair(0b110000000000000000000000, 1),
            },
            {
                std::make_pair(0b11000000000000000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000000000000000, 1),
            },
            {
                std::make_pair(0b110000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b11000000000000000000000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(
                0b1111111111111111111111111111111111111111111111111111111111111111,
                0) },
            { std::make_pair(0b11111111111111111111111111111111, 1) } },
        std::vector<CoreMask>{
            std::make_pair(
                0b1111111111111111111111111111111111111111111111111111111111111111,
                64),
            std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{
            0, 8, 16, 24, 32, 40, 48, 56, 64, 68, 72, 76, 80, 84, 88 }),
    // Test case 20: HyperThreading Multicache multicore group number of threads
    // greater than number of processors
    std::make_tuple(
        String("Test20: NoHT_MultiCache_PGTC"),
        96,
        100,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(0b1100000000000000000000000000000000000000000000,
                               0),
            },
            {
                std::make_pair(
                    0b110000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b11000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b1100000000000000000000000000000000000000000000000000, 0),
            },
            {
                std::make_pair(
                    0b110000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b11000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1100000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b110000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b11000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(
                    0b1100000000000000000000000000000000000000000000000000000000000000,
                    0),
            },
            {
                std::make_pair(0b11, 1),
            },
            {
                std::make_pair(0b1100, 1),
            },
            {
                std::make_pair(0b110000, 1),
            },
            {
                std::make_pair(0b11000000, 1),
            },
            {
                std::make_pair(0b1100000000, 1),
            },
            {
                std::make_pair(0b110000000000, 1),
            },
            {
                std::make_pair(0b11000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000, 1),
            },
            {
                std::make_pair(0b110000000000000000, 1),
            },
            {
                std::make_pair(0b11000000000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000000000, 1),
            },
            {
                std::make_pair(0b110000000000000000000000, 1),
            },
            {
                std::make_pair(0b11000000000000000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000000000000000, 1),
            },
            {
                std::make_pair(0b110000000000000000000000000000, 1),
            },
            {
                std::make_pair(0b11000000000000000000000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(
                0b1111111111111111111111111111111111111111111111111111111111111111,
                0) },
            { std::make_pair(0b11111111111111111111111111111111, 1) } },
        std::vector<CoreMask>{
            std::make_pair(
                0b1111111111111111111111111111111111111111111111111111111111111111,
                64),
            std::make_pair(0b11111111111111111111111111111111, 32) },
        std::vector<int>{
            0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
            34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66,
            68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 1,  3,  5,
            7,  9,  11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
            41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73,
            75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 0,  2,  4,  6 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
            85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 0,  1,  2,  3 },
        std::vector<int>{
            0,  1,  2,  3,  4,  5,  6,  8,  9,  10, 12, 13, 14, 16, 17, 18, 20,
            21, 22, 24, 25, 26, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 40, 41,
            42, 44, 45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64,
            64, 65, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74,
            75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85,
            86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95 }),
    // Test case 21: HyperThreading Multicache multicore group number of threads
    // equal to number of processors
    // Test case 22: HyperThreading Singlecache multicore group number of
    // threads less than number of processors
    std::make_tuple(
        String("Test22: NoHT_SingleCache_PLTP"),
        32,
        30,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
            {
                std::make_pair(0b11, 1),
            },
            {
                std::make_pair(0b1100, 1),
            },
            {
                std::make_pair(0b110000, 1),
            },
            {
                std::make_pair(0b11000000, 1),
            },
            {
                std::make_pair(0b1100000000, 1),
            },
            {
                std::make_pair(0b110000000000, 1),
            },
            {
                std::make_pair(0b11000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111111111111111111111111111, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111, 16),
            std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18,
                          20, 22, 24, 26, 28, 30, 1,  3,  5,  7,
                          9,  11, 13, 15, 17, 19, 21, 23, 25, 27 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                          10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                          20, 21, 22, 23, 24, 25, 26, 27, 28, 29 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                          10, 11, 12, 13, 14, 16, 17, 18, 19, 20,
                          21, 22, 23, 24, 25, 26, 27, 28, 29, 30 }),
    // Test case 23: HyperThreading Singlecache multicore group number of
    // threads greater than number of processors
    std::make_tuple(
        String("Test23: NoHT_SingleCache_PGTP"),
        32,
        35,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
            {
                std::make_pair(0b11, 1),
            },
            {
                std::make_pair(0b1100, 1),
            },
            {
                std::make_pair(0b110000, 1),
            },
            {
                std::make_pair(0b11000000, 1),
            },
            {
                std::make_pair(0b1100000000, 1),
            },
            {
                std::make_pair(0b110000000000, 1),
            },
            {
                std::make_pair(0b11000000000000, 1),
            },
            {
                std::make_pair(0b1100000000000000, 1),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b11111111111111111111111111111111, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111, 16),
            std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22,
                          24, 26, 28, 30, 1,  3,  5,  7,  9,  11, 13, 15,
                          17, 19, 21, 23, 25, 27, 29, 31, 0,  2,  4 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                          12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                          24, 25, 26, 27, 28, 29, 30, 31, 0,  1,  2 },
        std::vector<int>{ 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,
                          10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20,
                          21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 }),
    // Test case 24: HyperThreading Singlecache multicore group number of
    // threads equal to number of processors
    // Test case 25: No HyperThreading multicache uneven distribution single
    // coregroup. Number of threads less than number of processors
    std::make_tuple(String("Test25: NoHT_MultiCache_UnevenDistribution_PLTC"),
                    15,
                    2,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b1, 0),
                        },
                        {
                            std::make_pair(0b10, 0),
                        },
                        {
                            std::make_pair(0b100, 0),
                        },
                        {
                            std::make_pair(0b1000, 0),
                        },
                        {
                            std::make_pair(0b10000, 0),
                        },
                        {
                            std::make_pair(0b100000, 0),
                        },
                        {
                            std::make_pair(0b1000000, 0),
                        },
                        {
                            std::make_pair(0b10000000, 0),
                        },
                        {
                            std::make_pair(0b100000000, 0),
                        },
                        {
                            std::make_pair(0b1000000000, 0),
                        },
                        {
                            std::make_pair(0b10000000000, 0),
                        },
                        {
                            std::make_pair(0b100000000000, 0),
                        },
                        {
                            std::make_pair(0b1000000000000, 0),
                        },
                        {
                            std::make_pair(0b10000000000000, 0),
                        },
                        {
                            std::make_pair(0b100000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b1111111111, 0),
                        },
                        { std::make_pair(0b111110000000000, 0) } },
                    std::vector<CoreMask>{
                        std::make_pair(0b111111111111111, 15) },
                    std::vector<int>{ 0, 1 },
                    std::vector<int>{ 0, 1 },
                    std::vector<int>{ 0, 10 }),
    std::make_tuple(String("Test25b: NoHT_MultiCache_UnevenDistribution_PLTC"),
                    15,
                    7,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b1, 0),
                        },
                        {
                            std::make_pair(0b10, 0),
                        },
                        {
                            std::make_pair(0b100, 0),
                        },
                        {
                            std::make_pair(0b1000, 0),
                        },
                        {
                            std::make_pair(0b10000, 0),
                        },
                        {
                            std::make_pair(0b100000, 0),
                        },
                        {
                            std::make_pair(0b1000000, 0),
                        },
                        {
                            std::make_pair(0b10000000, 0),
                        },
                        {
                            std::make_pair(0b100000000, 0),
                        },
                        {
                            std::make_pair(0b1000000000, 0),
                        },
                        {
                            std::make_pair(0b10000000000, 0),
                        },
                        {
                            std::make_pair(0b100000000000, 0),
                        },
                        {
                            std::make_pair(0b1000000000000, 0),
                        },
                        {
                            std::make_pair(0b10000000000000, 0),
                        },
                        {
                            std::make_pair(0b100000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        { std::make_pair(0b1111111111, 0) },
                        { std::make_pair(0b111110000000000, 0) } },
                    std::vector<CoreMask>{
                        std::make_pair(0b111111111111111, 15) },
                    std::vector<int>{ 0, 1, 2, 3, 4, 5, 6 },
                    std::vector<int>{ 0, 1, 2, 3, 4, 5, 6 },
                    std::vector<int>{ 0, 3, 5, 8, 10, 12, 13 }),
    // Test case 26: No HyperThreading multicache uneven distribution single
    // coregroup. Number of threads greater than number of processors
    std::make_tuple(
        String("Test26: NoHT_MultiCache_UnevenDistribution_PGTP"),
        15,
        18,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b111111111111, 0),
            },
            { std::make_pair(0b111000000000000, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b111111111111111, 15) },
        std::vector<int>{
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 1, 2 },
        std::vector<int>{
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 1, 2 },
        std::vector<int>{
            0, 1, 2, 3, 5, 6, 8, 9, 11, 12, 12, 12, 13, 13, 14, 14, 14, 14 }),
    // Test case 27: No HyperThreading multicache uneven distribution single
    // coregroup. Number of threads equal to number of processors
    std::make_tuple(
        String("Test27: NoHT_MultiCache_UnevenDistribution_PETP"),
        15,
        15,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1, 0),
            },
            {
                std::make_pair(0b10, 0),
            },
            {
                std::make_pair(0b100, 0),
            },
            {
                std::make_pair(0b1000, 0),
            },
            {
                std::make_pair(0b10000, 0),
            },
            {
                std::make_pair(0b100000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
            },
            {
                std::make_pair(0b100000000, 0),
            },
            {
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b100000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111, 0) },
            { std::make_pair(0b111110000000000, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b111111111111111, 15) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 },
        std::vector<int>{ 0, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 12, 13, 13, 14 }),
    // Test case 28: No HyperThreading multicache uneven distribution
    // multicoregroup. Number of threads less than number of processors
    // Test case 29: No HyperThreading multicache uneven distribution
    // multicoregroup. Number of threads greater than number of processors
    // Test case 30: No HyperThreading multicache uneven distribution
    // multicoregroup. Number of threads equal to number of processors
    // Test case 31: HyperThreading multicache uneven distribution single
    // coregroup. Number of threads less than number of processors
    std::make_tuple(String("Test31: HT_MultiCache_UnevenDistribution_PLTC"),
                    30,
                    2,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11, 0),
                        },
                        {
                            std::make_pair(0b1100, 0),
                        },
                        {
                            std::make_pair(0b110000, 0),
                        },
                        {
                            std::make_pair(0b11000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000000000000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b1111111111, 0),
                        },
                        { std::make_pair(0b111111111111111111110000000000, 0) },
                    },
                    std::vector<CoreMask>{
                        std::make_pair(0b1111111111111111111111111111111, 30) },
                    std::vector<int>{ 0, 2 },
                    std::vector<int>{ 0, 1 },
                    std::vector<int>{ 0, 10 }),
    // Test case 32: HyperThreading multicache uneven distribution single
    // coregroup. Number of threads greater than number of processors
    std::make_tuple(
        String("Test32: HT_MultiCache_UnevenDistribution_PGTP"),
        30,
        32,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b11, 0),
            },
            {
                std::make_pair(0b1100, 0),
            },
            {
                std::make_pair(0b110000, 0),
            },
            {
                std::make_pair(0b11000000, 0),
            },
            {
                std::make_pair(0b1100000000, 0),
            },
            {
                std::make_pair(0b110000000000, 0),
            },
            {
                std::make_pair(0b11000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000, 0),
            },
            {
                std::make_pair(0b11000000000000000000000000, 0),
            },
            {
                std::make_pair(0b1100000000000000000000000000, 0),
            },
            {
                std::make_pair(0b110000000000000000000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1111111111, 0),
            },
            { std::make_pair(0b111111111111111111110000000000, 0) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111111111111111111, 30) },
        std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20,
                          22, 24, 26, 28, 1,  3,  5,  7,  9,  11, 13,
                          15, 17, 19, 21, 23, 25, 27, 29, 0,  2 },
        std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                          11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                          22, 23, 24, 25, 26, 27, 28, 29, 0,  1 },
        std::vector<int>{ 0,  1,  2,  2,  3,  3,  4,  4,  5,  6,  7,
                          7,  8,  8,  9,  9,  10, 12, 13, 14, 15, 17,
                          18, 19, 20, 22, 23, 24, 25, 27, 28, 29 }),
    // Test case 33: HyperThreading multicache uneven distribution single
    // coregroup. Number of threads equal to number of processors
    std::make_tuple(String("Test33: HT_MultiCache_UnevenDistribution_PETP"),
                    30,
                    30,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11, 0),
                        },
                        {
                            std::make_pair(0b1100, 0),
                        },
                        {
                            std::make_pair(0b110000, 0),
                        },
                        {
                            std::make_pair(0b11000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b11000000000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000000000000000000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000000000000000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b1111111111, 0),
                        },
                        { std::make_pair(0b111111111111111111110000000000, 0) },
                    },
                    std::vector<CoreMask>{
                        std::make_pair(0b1111111111111111111111111111111, 30) },
                    std::vector<int>{ 0,  2,  4,  6,  8,  10, 12, 14, 16, 18,
                                      20, 22, 24, 26, 28, 1,  3,  5,  7,  9,
                                      11, 13, 15, 17, 19, 21, 23, 25, 27, 29 },
                    std::vector<int>{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                      10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                      20, 21, 22, 23, 24, 25, 26, 27, 28, 29 },
                    std::vector<int>{ 0,  1,  2,  2,  3,  3,  4,  4,  5,  6,
                                      7,  7,  8,  8,  9,  10, 12, 13, 14, 15,
                                      17, 18, 19, 20, 22, 23, 24, 25, 27, 28 }),

    // Test case 34: HyperThreading multicache uneven distribution
    // multicoregroup.  Number of threads less than number of processors
    // Test case 35: HyperThreading multicache uneven distribution
    // multicoregroup.  Number of threads greater than number of processors
    // Test case 36: HyperThreading multicache uneven distribution
    // multicoregroup.  Number of threads equal to number of processors
    // # ***************Tests targetted for linux
    // systems******************************** #/
    // Test case 37: HyperThreading singlecache nonneighbouring siblings and
    // total number of cores less than 32
    std::make_tuple(
        String("Test27: HT_SingleCache_NonNeighbouringSiblings_PLTP"),
        12,
        15,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1000001, 0),
            },
            {
                std::make_pair(0b10000010, 0),
            },
            {
                std::make_pair(0b100000100, 0),
            },
            {
                std::make_pair(0b1000001000, 0),
            },
            {
                std::make_pair(0b10000010000, 0),
            },
            {
                std::make_pair(0b100000100000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b111111111111, 0) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b111111111111, 12) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2 },
        std::vector<int>{ 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11 }),
    std::make_tuple(
        String("Test27: HT_SingleCache_NonNeighbouringSiblings_PLTP"),
        12,
        10,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1000001, 0),
            },
            {
                std::make_pair(0b10000010, 0),
            },
            {
                std::make_pair(0b100000100, 0),
            },
            {
                std::make_pair(0b1000001000, 0),
            },
            {
                std::make_pair(0b10000010000, 0),
            },
            {
                std::make_pair(0b100000100000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b111111111111, 0) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b111111111111, 12) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        std::vector<int>{ 0, 1, 2, 3, 5, 6, 7, 8, 9, 11 }),
    std::make_tuple(
        String("Test27: HT_SingleCache_NonNeighbouringSiblings_PLTP"),
        12,
        12,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b1000001, 0),
            },
            {
                std::make_pair(0b10000010, 0),
            },
            {
                std::make_pair(0b100000100, 0),
            },
            {
                std::make_pair(0b1000001000, 0),
            },
            {
                std::make_pair(0b10000010000, 0),
            },
            {
                std::make_pair(0b100000100000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b111111111111, 0) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b111111111111, 12) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }),
    /*
     * Test case 38 : lib-genoa-09
     * CPU(s):                  384
     * On-line CPU(s) list:   0-191
     * Off-line CPU(s) list:  192-383
     * Vendor ID:               AuthenticAMD
     * Model name:            AMD Eng Sample: 100-000000999
     * CPU family:          25
     * Model:               17
     * Thread(s) per core:  1
     * Core(s) per socket:  96
     * Socket(s):           2
     * Stepping:            1
     * Caches (sum of all):
     * L1d:                   6 MiB (192 instances)
     * L1i:                   6 MiB (192 instances)
     * L2:                    192 MiB (192 instances)
     * L3:                    768 MiB (24 instances)
     */
    std::make_tuple(
        String("Test38: lib-genoa-09"),
        192,
        16,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b00000000000000000000000000000001, 0) },
            { std::make_pair(0b00000000000000000000000000000010, 0) },
            { std::make_pair(0b00000000000000000000000000000100, 0) },
            { std::make_pair(0b00000000000000000000000000001000, 0) },
            { std::make_pair(0b00000000000000000000000000010000, 0) },
            { std::make_pair(0b00000000000000000000000000100000, 0) },
            { std::make_pair(0b00000000000000000000000001000000, 0) },
            { std::make_pair(0b00000000000000000000000010000000, 0) },
            { std::make_pair(0b00000000000000000000000100000000, 0) },
            { std::make_pair(0b00000000000000000000001000000000, 0) },
            { std::make_pair(0b00000000000000000000010000000000, 0) },
            { std::make_pair(0b00000000000000000000100000000000, 0) },
            { std::make_pair(0b00000000000000000001000000000000, 0) },
            { std::make_pair(0b00000000000000000010000000000000, 0) },
            { std::make_pair(0b00000000000000000100000000000000, 0) },
            { std::make_pair(0b00000000000000001000000000000000, 0) },
            { std::make_pair(0b00000000000000010000000000000000, 0) },
            { std::make_pair(0b00000000000000100000000000000000, 0) },
            { std::make_pair(0b00000000000001000000000000000000, 0) },
            { std::make_pair(0b00000000000010000000000000000000, 0) },
            { std::make_pair(0b00000000000100000000000000000000, 0) },
            { std::make_pair(0b00000000001000000000000000000000, 0) },
            { std::make_pair(0b00000000010000000000000000000000, 0) },
            { std::make_pair(0b00000000100000000000000000000000, 0) },
            { std::make_pair(0b00000001000000000000000000000000, 0) },
            { std::make_pair(0b00000010000000000000000000000000, 0) },
            { std::make_pair(0b00000100000000000000000000000000, 0) },
            { std::make_pair(0b00001000000000000000000000000000, 0) },
            { std::make_pair(0b00010000000000000000000000000000, 0) },
            { std::make_pair(0b00100000000000000000000000000000, 0) },
            { std::make_pair(0b01000000000000000000000000000000, 0) },
            { std::make_pair(0b10000000000000000000000000000000, 0) },
            { std::make_pair(0b00000000000000000000000000000001, 1) },
            { std::make_pair(0b00000000000000000000000000000010, 1) },
            { std::make_pair(0b00000000000000000000000000000100, 1) },
            { std::make_pair(0b00000000000000000000000000001000, 1) },
            { std::make_pair(0b00000000000000000000000000010000, 1) },
            { std::make_pair(0b00000000000000000000000000100000, 1) },
            { std::make_pair(0b00000000000000000000000001000000, 1) },
            { std::make_pair(0b00000000000000000000000010000000, 1) },
            { std::make_pair(0b00000000000000000000000100000000, 1) },
            { std::make_pair(0b00000000000000000000001000000000, 1) },
            { std::make_pair(0b00000000000000000000010000000000, 1) },
            { std::make_pair(0b00000000000000000000100000000000, 1) },
            { std::make_pair(0b00000000000000000001000000000000, 1) },
            { std::make_pair(0b00000000000000000010000000000000, 1) },
            { std::make_pair(0b00000000000000000100000000000000, 1) },
            { std::make_pair(0b00000000000000001000000000000000, 1) },
            { std::make_pair(0b00000000000000010000000000000000, 1) },
            { std::make_pair(0b00000000000000100000000000000000, 1) },
            { std::make_pair(0b00000000000001000000000000000000, 1) },
            { std::make_pair(0b00000000000010000000000000000000, 1) },
            { std::make_pair(0b00000000000100000000000000000000, 1) },
            { std::make_pair(0b00000000001000000000000000000000, 1) },
            { std::make_pair(0b00000000010000000000000000000000, 1) },
            { std::make_pair(0b00000000100000000000000000000000, 1) },
            { std::make_pair(0b00000001000000000000000000000000, 1) },
            { std::make_pair(0b00000010000000000000000000000000, 1) },
            { std::make_pair(0b00000100000000000000000000000000, 1) },
            { std::make_pair(0b00001000000000000000000000000000, 1) },
            { std::make_pair(0b00010000000000000000000000000000, 1) },
            { std::make_pair(0b00100000000000000000000000000000, 1) },
            { std::make_pair(0b01000000000000000000000000000000, 1) },
            { std::make_pair(0b10000000000000000000000000000000, 1) },
            { std::make_pair(0b00000000000000000000000000000001, 2) },
            { std::make_pair(0b00000000000000000000000000000010, 2) },
            { std::make_pair(0b00000000000000000000000000000100, 2) },
            { std::make_pair(0b00000000000000000000000000001000, 2) },
            { std::make_pair(0b00000000000000000000000000010000, 2) },
            { std::make_pair(0b00000000000000000000000000100000, 2) },
            { std::make_pair(0b00000000000000000000000001000000, 2) },
            { std::make_pair(0b00000000000000000000000010000000, 2) },
            { std::make_pair(0b00000000000000000000000100000000, 2) },
            { std::make_pair(0b00000000000000000000001000000000, 2) },
            { std::make_pair(0b00000000000000000000010000000000, 2) },
            { std::make_pair(0b00000000000000000000100000000000, 2) },
            { std::make_pair(0b00000000000000000001000000000000, 2) },
            { std::make_pair(0b00000000000000000010000000000000, 2) },
            { std::make_pair(0b00000000000000000100000000000000, 2) },
            { std::make_pair(0b00000000000000001000000000000000, 2) },
            { std::make_pair(0b00000000000000010000000000000000, 2) },
            { std::make_pair(0b00000000000000100000000000000000, 2) },
            { std::make_pair(0b00000000000001000000000000000000, 2) },
            { std::make_pair(0b00000000000010000000000000000000, 2) },
            { std::make_pair(0b00000000000100000000000000000000, 2) },
            { std::make_pair(0b00000000001000000000000000000000, 2) },
            { std::make_pair(0b00000000010000000000000000000000, 2) },
            { std::make_pair(0b00000000100000000000000000000000, 2) },
            { std::make_pair(0b00000001000000000000000000000000, 2) },
            { std::make_pair(0b00000010000000000000000000000000, 2) },
            { std::make_pair(0b00000100000000000000000000000000, 2) },
            { std::make_pair(0b00001000000000000000000000000000, 2) },
            { std::make_pair(0b00010000000000000000000000000000, 2) },
            { std::make_pair(0b00100000000000000000000000000000, 2) },
            { std::make_pair(0b01000000000000000000000000000000, 2) },
            { std::make_pair(0b10000000000000000000000000000000, 2) },
            { std::make_pair(0b00000000000000000000000000000001, 3) },
            { std::make_pair(0b00000000000000000000000000000010, 3) },
            { std::make_pair(0b00000000000000000000000000000100, 3) },
            { std::make_pair(0b00000000000000000000000000001000, 3) },
            { std::make_pair(0b00000000000000000000000000010000, 3) },
            { std::make_pair(0b00000000000000000000000000100000, 3) },
            { std::make_pair(0b00000000000000000000000001000000, 3) },
            { std::make_pair(0b00000000000000000000000010000000, 3) },
            { std::make_pair(0b00000000000000000000000100000000, 3) },
            { std::make_pair(0b00000000000000000000001000000000, 3) },
            { std::make_pair(0b00000000000000000000010000000000, 3) },
            { std::make_pair(0b00000000000000000000100000000000, 3) },
            { std::make_pair(0b00000000000000000001000000000000, 3) },
            { std::make_pair(0b00000000000000000010000000000000, 3) },
            { std::make_pair(0b00000000000000000100000000000000, 3) },
            { std::make_pair(0b00000000000000001000000000000000, 3) },
            { std::make_pair(0b00000000000000010000000000000000, 3) },
            { std::make_pair(0b00000000000000100000000000000000, 3) },
            { std::make_pair(0b00000000000001000000000000000000, 3) },
            { std::make_pair(0b00000000000010000000000000000000, 3) },
            { std::make_pair(0b00000000000100000000000000000000, 3) },
            { std::make_pair(0b00000000001000000000000000000000, 3) },
            { std::make_pair(0b00000000010000000000000000000000, 3) },
            { std::make_pair(0b00000000100000000000000000000000, 3) },
            { std::make_pair(0b00000001000000000000000000000000, 3) },
            { std::make_pair(0b00000010000000000000000000000000, 3) },
            { std::make_pair(0b00000100000000000000000000000000, 3) },
            { std::make_pair(0b00001000000000000000000000000000, 3) },
            { std::make_pair(0b00010000000000000000000000000000, 3) },
            { std::make_pair(0b00100000000000000000000000000000, 3) },
            { std::make_pair(0b01000000000000000000000000000000, 3) },
            { std::make_pair(0b10000000000000000000000000000000, 3) },
            { std::make_pair(0b00000000000000000000000000000001, 4) },
            { std::make_pair(0b00000000000000000000000000000010, 4) },
            { std::make_pair(0b00000000000000000000000000000100, 4) },
            { std::make_pair(0b00000000000000000000000000001000, 4) },
            { std::make_pair(0b00000000000000000000000000010000, 4) },
            { std::make_pair(0b00000000000000000000000000100000, 4) },
            { std::make_pair(0b00000000000000000000000001000000, 4) },
            { std::make_pair(0b00000000000000000000000010000000, 4) },
            { std::make_pair(0b00000000000000000000000100000000, 4) },
            { std::make_pair(0b00000000000000000000001000000000, 4) },
            { std::make_pair(0b00000000000000000000010000000000, 4) },
            { std::make_pair(0b00000000000000000000100000000000, 4) },
            { std::make_pair(0b00000000000000000001000000000000, 4) },
            { std::make_pair(0b00000000000000000010000000000000, 4) },
            { std::make_pair(0b00000000000000000100000000000000, 4) },
            { std::make_pair(0b00000000000000001000000000000000, 4) },
            { std::make_pair(0b00000000000000010000000000000000, 4) },
            { std::make_pair(0b00000000000000100000000000000000, 4) },
            { std::make_pair(0b00000000000001000000000000000000, 4) },
            { std::make_pair(0b00000000000010000000000000000000, 4) },
            { std::make_pair(0b00000000000100000000000000000000, 4) },
            { std::make_pair(0b00000000001000000000000000000000, 4) },
            { std::make_pair(0b00000000010000000000000000000000, 4) },
            { std::make_pair(0b00000000100000000000000000000000, 4) },
            { std::make_pair(0b00000001000000000000000000000000, 4) },
            { std::make_pair(0b00000010000000000000000000000000, 4) },
            { std::make_pair(0b00000100000000000000000000000000, 4) },
            { std::make_pair(0b00001000000000000000000000000000, 4) },
            { std::make_pair(0b00010000000000000000000000000000, 4) },
            { std::make_pair(0b00100000000000000000000000000000, 4) },
            { std::make_pair(0b01000000000000000000000000000000, 4) },
            { std::make_pair(0b10000000000000000000000000000000, 4) },
            { std::make_pair(0b00000000000000000000000000000001, 5) },
            { std::make_pair(0b00000000000000000000000000000010, 5) },
            { std::make_pair(0b00000000000000000000000000000100, 5) },
            { std::make_pair(0b00000000000000000000000000001000, 5) },
            { std::make_pair(0b00000000000000000000000000010000, 5) },
            { std::make_pair(0b00000000000000000000000000100000, 5) },
            { std::make_pair(0b00000000000000000000000001000000, 5) },
            { std::make_pair(0b00000000000000000000000010000000, 5) },
            { std::make_pair(0b00000000000000000000000100000000, 5) },
            { std::make_pair(0b00000000000000000000001000000000, 5) },
            { std::make_pair(0b00000000000000000000010000000000, 5) },
            { std::make_pair(0b00000000000000000000100000000000, 5) },
            { std::make_pair(0b00000000000000000001000000000000, 5) },
            { std::make_pair(0b00000000000000000010000000000000, 5) },
            { std::make_pair(0b00000000000000000100000000000000, 5) },
            { std::make_pair(0b00000000000000001000000000000000, 5) },
            { std::make_pair(0b00000000000000010000000000000000, 5) },
            { std::make_pair(0b00000000000000100000000000000000, 5) },
            { std::make_pair(0b00000000000001000000000000000000, 5) },
            { std::make_pair(0b00000000000010000000000000000000, 5) },
            { std::make_pair(0b00000000000100000000000000000000, 5) },
            { std::make_pair(0b00000000001000000000000000000000, 5) },
            { std::make_pair(0b00000000010000000000000000000000, 5) },
            { std::make_pair(0b00000000100000000000000000000000, 5) },
            { std::make_pair(0b00000001000000000000000000000000, 5) },
            { std::make_pair(0b00000010000000000000000000000000, 5) },
            { std::make_pair(0b00000100000000000000000000000000, 5) },
            { std::make_pair(0b00001000000000000000000000000000, 5) },
            { std::make_pair(0b00010000000000000000000000000000, 5) },
            { std::make_pair(0b00100000000000000000000000000000, 5) },
            { std::make_pair(0b01000000000000000000000000000000, 5) },
            { std::make_pair(0b10000000000000000000000000000000, 5) },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b00000000000000000000000011111111, 0) },
            { std::make_pair(0b00000000000000001111111100000000, 0) },
            { std::make_pair(0b00000000111111110000000000000000, 0) },
            { std::make_pair(0b11111111000000000000000000000000, 0) },
            { std::make_pair(0b00000000000000000000000011111111, 1) },
            { std::make_pair(0b00000000000000001111111100000000, 1) },
            { std::make_pair(0b00000000111111110000000000000000, 1) },
            { std::make_pair(0b11111111000000000000000000000000, 1) },
            { std::make_pair(0b00000000000000000000000011111111, 2) },
            { std::make_pair(0b00000000000000001111111100000000, 2) },
            { std::make_pair(0b00000000111111110000000000000000, 2) },
            { std::make_pair(0b11111111000000000000000000000000, 2) },
            { std::make_pair(0b00000000000000000000000011111111, 3) },
            { std::make_pair(0b00000000000000001111111100000000, 3) },
            { std::make_pair(0b00000000111111110000000000000000, 3) },
            { std::make_pair(0b11111111000000000000000000000000, 3) },
            { std::make_pair(0b00000000000000000000000011111111, 4) },
            { std::make_pair(0b00000000000000001111111100000000, 4) },
            { std::make_pair(0b00000000111111110000000000000000, 4) },
            { std::make_pair(0b11111111000000000000000000000000, 4) },
            { std::make_pair(0b00000000000000000000000011111111, 5) },
            { std::make_pair(0b00000000000000001111111100000000, 5) },
            { std::make_pair(0b00000000111111110000000000000000, 5) },
            { std::make_pair(0b11111111000000000000000000000000, 5) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
        },
        std::vector<int>{
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0,
                          16,
                          24,
                          40,
                          48,
                          64,
                          72,
                          88,
                          96,
                          112,
                          120,
                          136,
                          144,
                          160,
                          168,
                          184 }),
    std::make_tuple(
        String("Test39: lib-genoa-09-tweak-cachegroups"),
        192,
        16,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b00000000000000000000000000000001, 0) },
            { std::make_pair(0b00000000000000000000000000000010, 0) },
            { std::make_pair(0b00000000000000000000000000000100, 0) },
            { std::make_pair(0b00000000000000000000000000001000, 0) },
            { std::make_pair(0b00000000000000000000000000010000, 0) },
            { std::make_pair(0b00000000000000000000000000100000, 0) },
            { std::make_pair(0b00000000000000000000000001000000, 0) },
            { std::make_pair(0b00000000000000000000000010000000, 0) },
            { std::make_pair(0b00000000000000000000000100000000, 0) },
            { std::make_pair(0b00000000000000000000001000000000, 0) },
            { std::make_pair(0b00000000000000000000010000000000, 0) },
            { std::make_pair(0b00000000000000000000100000000000, 0) },
            { std::make_pair(0b00000000000000000001000000000000, 0) },
            { std::make_pair(0b00000000000000000010000000000000, 0) },
            { std::make_pair(0b00000000000000000100000000000000, 0) },
            { std::make_pair(0b00000000000000001000000000000000, 0) },
            { std::make_pair(0b00000000000000010000000000000000, 0) },
            { std::make_pair(0b00000000000000100000000000000000, 0) },
            { std::make_pair(0b00000000000001000000000000000000, 0) },
            { std::make_pair(0b00000000000010000000000000000000, 0) },
            { std::make_pair(0b00000000000100000000000000000000, 0) },
            { std::make_pair(0b00000000001000000000000000000000, 0) },
            { std::make_pair(0b00000000010000000000000000000000, 0) },
            { std::make_pair(0b00000000100000000000000000000000, 0) },
            { std::make_pair(0b00000001000000000000000000000000, 0) },
            { std::make_pair(0b00000010000000000000000000000000, 0) },
            { std::make_pair(0b00000100000000000000000000000000, 0) },
            { std::make_pair(0b00001000000000000000000000000000, 0) },
            { std::make_pair(0b00010000000000000000000000000000, 0) },
            { std::make_pair(0b00100000000000000000000000000000, 0) },
            { std::make_pair(0b01000000000000000000000000000000, 0) },
            { std::make_pair(0b10000000000000000000000000000000, 0) },
            { std::make_pair(0b00000000000000000000000000000001, 1) },
            { std::make_pair(0b00000000000000000000000000000010, 1) },
            { std::make_pair(0b00000000000000000000000000000100, 1) },
            { std::make_pair(0b00000000000000000000000000001000, 1) },
            { std::make_pair(0b00000000000000000000000000010000, 1) },
            { std::make_pair(0b00000000000000000000000000100000, 1) },
            { std::make_pair(0b00000000000000000000000001000000, 1) },
            { std::make_pair(0b00000000000000000000000010000000, 1) },
            { std::make_pair(0b00000000000000000000000100000000, 1) },
            { std::make_pair(0b00000000000000000000001000000000, 1) },
            { std::make_pair(0b00000000000000000000010000000000, 1) },
            { std::make_pair(0b00000000000000000000100000000000, 1) },
            { std::make_pair(0b00000000000000000001000000000000, 1) },
            { std::make_pair(0b00000000000000000010000000000000, 1) },
            { std::make_pair(0b00000000000000000100000000000000, 1) },
            { std::make_pair(0b00000000000000001000000000000000, 1) },
            { std::make_pair(0b00000000000000010000000000000000, 1) },
            { std::make_pair(0b00000000000000100000000000000000, 1) },
            { std::make_pair(0b00000000000001000000000000000000, 1) },
            { std::make_pair(0b00000000000010000000000000000000, 1) },
            { std::make_pair(0b00000000000100000000000000000000, 1) },
            { std::make_pair(0b00000000001000000000000000000000, 1) },
            { std::make_pair(0b00000000010000000000000000000000, 1) },
            { std::make_pair(0b00000000100000000000000000000000, 1) },
            { std::make_pair(0b00000001000000000000000000000000, 1) },
            { std::make_pair(0b00000010000000000000000000000000, 1) },
            { std::make_pair(0b00000100000000000000000000000000, 1) },
            { std::make_pair(0b00001000000000000000000000000000, 1) },
            { std::make_pair(0b00010000000000000000000000000000, 1) },
            { std::make_pair(0b00100000000000000000000000000000, 1) },
            { std::make_pair(0b01000000000000000000000000000000, 1) },
            { std::make_pair(0b10000000000000000000000000000000, 1) },
            { std::make_pair(0b00000000000000000000000000000001, 2) },
            { std::make_pair(0b00000000000000000000000000000010, 2) },
            { std::make_pair(0b00000000000000000000000000000100, 2) },
            { std::make_pair(0b00000000000000000000000000001000, 2) },
            { std::make_pair(0b00000000000000000000000000010000, 2) },
            { std::make_pair(0b00000000000000000000000000100000, 2) },
            { std::make_pair(0b00000000000000000000000001000000, 2) },
            { std::make_pair(0b00000000000000000000000010000000, 2) },
            { std::make_pair(0b00000000000000000000000100000000, 2) },
            { std::make_pair(0b00000000000000000000001000000000, 2) },
            { std::make_pair(0b00000000000000000000010000000000, 2) },
            { std::make_pair(0b00000000000000000000100000000000, 2) },
            { std::make_pair(0b00000000000000000001000000000000, 2) },
            { std::make_pair(0b00000000000000000010000000000000, 2) },
            { std::make_pair(0b00000000000000000100000000000000, 2) },
            { std::make_pair(0b00000000000000001000000000000000, 2) },
            { std::make_pair(0b00000000000000010000000000000000, 2) },
            { std::make_pair(0b00000000000000100000000000000000, 2) },
            { std::make_pair(0b00000000000001000000000000000000, 2) },
            { std::make_pair(0b00000000000010000000000000000000, 2) },
            { std::make_pair(0b00000000000100000000000000000000, 2) },
            { std::make_pair(0b00000000001000000000000000000000, 2) },
            { std::make_pair(0b00000000010000000000000000000000, 2) },
            { std::make_pair(0b00000000100000000000000000000000, 2) },
            { std::make_pair(0b00000001000000000000000000000000, 2) },
            { std::make_pair(0b00000010000000000000000000000000, 2) },
            { std::make_pair(0b00000100000000000000000000000000, 2) },
            { std::make_pair(0b00001000000000000000000000000000, 2) },
            { std::make_pair(0b00010000000000000000000000000000, 2) },
            { std::make_pair(0b00100000000000000000000000000000, 2) },
            { std::make_pair(0b01000000000000000000000000000000, 2) },
            { std::make_pair(0b10000000000000000000000000000000, 2) },
            { std::make_pair(0b00000000000000000000000000000001, 3) },
            { std::make_pair(0b00000000000000000000000000000010, 3) },
            { std::make_pair(0b00000000000000000000000000000100, 3) },
            { std::make_pair(0b00000000000000000000000000001000, 3) },
            { std::make_pair(0b00000000000000000000000000010000, 3) },
            { std::make_pair(0b00000000000000000000000000100000, 3) },
            { std::make_pair(0b00000000000000000000000001000000, 3) },
            { std::make_pair(0b00000000000000000000000010000000, 3) },
            { std::make_pair(0b00000000000000000000000100000000, 3) },
            { std::make_pair(0b00000000000000000000001000000000, 3) },
            { std::make_pair(0b00000000000000000000010000000000, 3) },
            { std::make_pair(0b00000000000000000000100000000000, 3) },
            { std::make_pair(0b00000000000000000001000000000000, 3) },
            { std::make_pair(0b00000000000000000010000000000000, 3) },
            { std::make_pair(0b00000000000000000100000000000000, 3) },
            { std::make_pair(0b00000000000000001000000000000000, 3) },
            { std::make_pair(0b00000000000000010000000000000000, 3) },
            { std::make_pair(0b00000000000000100000000000000000, 3) },
            { std::make_pair(0b00000000000001000000000000000000, 3) },
            { std::make_pair(0b00000000000010000000000000000000, 3) },
            { std::make_pair(0b00000000000100000000000000000000, 3) },
            { std::make_pair(0b00000000001000000000000000000000, 3) },
            { std::make_pair(0b00000000010000000000000000000000, 3) },
            { std::make_pair(0b00000000100000000000000000000000, 3) },
            { std::make_pair(0b00000001000000000000000000000000, 3) },
            { std::make_pair(0b00000010000000000000000000000000, 3) },
            { std::make_pair(0b00000100000000000000000000000000, 3) },
            { std::make_pair(0b00001000000000000000000000000000, 3) },
            { std::make_pair(0b00010000000000000000000000000000, 3) },
            { std::make_pair(0b00100000000000000000000000000000, 3) },
            { std::make_pair(0b01000000000000000000000000000000, 3) },
            { std::make_pair(0b10000000000000000000000000000000, 3) },
            { std::make_pair(0b00000000000000000000000000000001, 4) },
            { std::make_pair(0b00000000000000000000000000000010, 4) },
            { std::make_pair(0b00000000000000000000000000000100, 4) },
            { std::make_pair(0b00000000000000000000000000001000, 4) },
            { std::make_pair(0b00000000000000000000000000010000, 4) },
            { std::make_pair(0b00000000000000000000000000100000, 4) },
            { std::make_pair(0b00000000000000000000000001000000, 4) },
            { std::make_pair(0b00000000000000000000000010000000, 4) },
            { std::make_pair(0b00000000000000000000000100000000, 4) },
            { std::make_pair(0b00000000000000000000001000000000, 4) },
            { std::make_pair(0b00000000000000000000010000000000, 4) },
            { std::make_pair(0b00000000000000000000100000000000, 4) },
            { std::make_pair(0b00000000000000000001000000000000, 4) },
            { std::make_pair(0b00000000000000000010000000000000, 4) },
            { std::make_pair(0b00000000000000000100000000000000, 4) },
            { std::make_pair(0b00000000000000001000000000000000, 4) },
            { std::make_pair(0b00000000000000010000000000000000, 4) },
            { std::make_pair(0b00000000000000100000000000000000, 4) },
            { std::make_pair(0b00000000000001000000000000000000, 4) },
            { std::make_pair(0b00000000000010000000000000000000, 4) },
            { std::make_pair(0b00000000000100000000000000000000, 4) },
            { std::make_pair(0b00000000001000000000000000000000, 4) },
            { std::make_pair(0b00000000010000000000000000000000, 4) },
            { std::make_pair(0b00000000100000000000000000000000, 4) },
            { std::make_pair(0b00000001000000000000000000000000, 4) },
            { std::make_pair(0b00000010000000000000000000000000, 4) },
            { std::make_pair(0b00000100000000000000000000000000, 4) },
            { std::make_pair(0b00001000000000000000000000000000, 4) },
            { std::make_pair(0b00010000000000000000000000000000, 4) },
            { std::make_pair(0b00100000000000000000000000000000, 4) },
            { std::make_pair(0b01000000000000000000000000000000, 4) },
            { std::make_pair(0b10000000000000000000000000000000, 4) },
            { std::make_pair(0b00000000000000000000000000000001, 5) },
            { std::make_pair(0b00000000000000000000000000000010, 5) },
            { std::make_pair(0b00000000000000000000000000000100, 5) },
            { std::make_pair(0b00000000000000000000000000001000, 5) },
            { std::make_pair(0b00000000000000000000000000010000, 5) },
            { std::make_pair(0b00000000000000000000000000100000, 5) },
            { std::make_pair(0b00000000000000000000000001000000, 5) },
            { std::make_pair(0b00000000000000000000000010000000, 5) },
            { std::make_pair(0b00000000000000000000000100000000, 5) },
            { std::make_pair(0b00000000000000000000001000000000, 5) },
            { std::make_pair(0b00000000000000000000010000000000, 5) },
            { std::make_pair(0b00000000000000000000100000000000, 5) },
            { std::make_pair(0b00000000000000000001000000000000, 5) },
            { std::make_pair(0b00000000000000000010000000000000, 5) },
            { std::make_pair(0b00000000000000000100000000000000, 5) },
            { std::make_pair(0b00000000000000001000000000000000, 5) },
            { std::make_pair(0b00000000000000010000000000000000, 5) },
            { std::make_pair(0b00000000000000100000000000000000, 5) },
            { std::make_pair(0b00000000000001000000000000000000, 5) },
            { std::make_pair(0b00000000000010000000000000000000, 5) },
            { std::make_pair(0b00000000000100000000000000000000, 5) },
            { std::make_pair(0b00000000001000000000000000000000, 5) },
            { std::make_pair(0b00000000010000000000000000000000, 5) },
            { std::make_pair(0b00000000100000000000000000000000, 5) },
            { std::make_pair(0b00000001000000000000000000000000, 5) },
            { std::make_pair(0b00000010000000000000000000000000, 5) },
            { std::make_pair(0b00000100000000000000000000000000, 5) },
            { std::make_pair(0b00001000000000000000000000000000, 5) },
            { std::make_pair(0b00010000000000000000000000000000, 5) },
            { std::make_pair(0b00100000000000000000000000000000, 5) },
            { std::make_pair(0b01000000000000000000000000000000, 5) },
            { std::make_pair(0b10000000000000000000000000000000, 5) },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b00000000000000000000000011111111, 0),
              std::make_pair(0b00000000000000000000000011111111, 3) },
            { std::make_pair(0b00000000000000001111111100000000, 0),
              std::make_pair(0b00000000000000001111111100000000, 3) },
            { std::make_pair(0b00000000111111110000000000000000, 0),
              std::make_pair(0b00000000111111110000000000000000, 3) },
            { std::make_pair(0b11111111000000000000000000000000, 0),
              std::make_pair(0b11111111000000000000000000000000, 3) },
            { std::make_pair(0b00000000000000000000000011111111, 1),
              std::make_pair(0b00000000000000000000000011111111, 4) },
            { std::make_pair(0b00000000000000001111111100000000, 1),
              std::make_pair(0b00000000000000001111111100000000, 4) },
            { std::make_pair(0b00000000111111110000000000000000, 1),
              std::make_pair(0b00000000111111110000000000000000, 4) },
            { std::make_pair(0b11111111000000000000000000000000, 1),
              std::make_pair(0b11111111000000000000000000000000, 4) },
            { std::make_pair(0b00000000000000000000000011111111, 2),
              std::make_pair(0b00000000000000000000000011111111, 5) },
            { std::make_pair(0b00000000000000001111111100000000, 2),
              std::make_pair(0b00000000000000001111111100000000, 5) },
            { std::make_pair(0b00000000111111110000000000000000, 2),
              std::make_pair(0b00000000111111110000000000000000, 5) },
            { std::make_pair(0b11111111000000000000000000000000, 2),
              std::make_pair(0b11111111000000000000000000000000, 5) },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
        },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 8, 16, 112, 24, 32, 40, 136, 48, 56, 64, 160, 72, 80, 88, 184 }),
    std::make_tuple(
        String("Test13: HT_SingleCache_PLTC"),
        16,
        9,
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1, 0), std::make_pair(0b100000000, 0) },
            {
                std::make_pair(0b10, 0),
                std::make_pair(0b1000000000, 0),
            },
            {
                std::make_pair(0b100, 0),
                std::make_pair(0b10000000000, 0),
            },
            {
                std::make_pair(0b1000, 0),
                std::make_pair(0b100000000000, 0),
            },
            {
                std::make_pair(0b10000, 0),
                std::make_pair(0b1000000000000, 0),
            },
            {
                std::make_pair(0b100000, 0),
                std::make_pair(0b10000000000000, 0),
            },
            {
                std::make_pair(0b1000000, 0),
                std::make_pair(0b100000000000000, 0),
            },
            {
                std::make_pair(0b10000000, 0),
                std::make_pair(0b1000000000000000, 0),
            },
        },
        std::vector<std::vector<CoreMask>>{
            { std::make_pair(0b1111111111111111, 0) } },
        std::vector<CoreMask>{
            std::make_pair(0b1111111111111111, 16) },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
        std::vector<int>{ 0, 1, 2, 4, 6, 8, 10, 12, 14 }),
    /*
     */
    std::make_tuple(String("Test13: HT_SingleCache_PLTC"),
                    12,
                    16,
                    std::vector<std::vector<CoreMask>>{
                        {
                            std::make_pair(0b11, 0),
                        },
                        {
                            std::make_pair(0b1100, 0),
                        },
                        {
                            std::make_pair(0b110000, 0),
                        },
                        {
                            std::make_pair(0b11000000, 0),
                        },
                        {
                            std::make_pair(0b1100000000, 0),
                        },
                        {
                            std::make_pair(0b110000000000, 0),
                        },
                    },
                    std::vector<std::vector<CoreMask>>{
                        { std::make_pair(0b111111111111, 0) } },
                    std::vector<CoreMask>{
                        std::make_pair(0b11111111111111, 12) },
                    std::vector<int>{ 0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6 },
                    std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3 },
                    std::vector<int>{ 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 11 }),
    std::make_tuple(
        String("Test39: lib-genoa-10"),
        192,
        16,
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b00000000000000000000000000000001, 0),
                std::make_pair(0b00000000000000000000000000000001, 8),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 0),
                std::make_pair(0b00000000000000000000000000000010, 8),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 0),
                std::make_pair(0b00000000000000000000000000000100, 8),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 0),
                std::make_pair(0b00000000000000000000000000001000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 0),
                std::make_pair(0b00000000000000000000000000010000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 0),
                std::make_pair(0b00000000000000000000000000100000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 0),
                std::make_pair(0b00000000000000000000000001000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 0),
                std::make_pair(0b00000000000000000000000010000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 0),
                std::make_pair(0b00000000000000000000000100000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 0),
                std::make_pair(0b00000000000000000000001000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 0),
                std::make_pair(0b00000000000000000000010000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 0),
                std::make_pair(0b00000000000000000000100000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 0),
                std::make_pair(0b00000000000000000001000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 0),
                std::make_pair(0b00000000000000000010000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 0),
                std::make_pair(0b00000000000000000100000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 0),
                std::make_pair(0b00000000000000001000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 0),
                std::make_pair(0b00000000000000010000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 0),
                std::make_pair(0b00000000000000100000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 0),
                std::make_pair(0b00000000000001000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 0),
                std::make_pair(0b00000000000010000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 0),
                std::make_pair(0b00000000000100000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 0),
                std::make_pair(0b00000000001000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 0),
                std::make_pair(0b00000000010000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 0),
                std::make_pair(0b00000000100000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 0),
                std::make_pair(0b00000001000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 0),
                std::make_pair(0b00000010000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 0),
                std::make_pair(0b00000100000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 0),
                std::make_pair(0b00001000000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 0),
                std::make_pair(0b00010000000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 0),
                std::make_pair(0b00100000000000000000000000000000, 8),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 0),
                std::make_pair(0b01000000000000000000000000000000, 8),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 0),
                std::make_pair(0b10000000000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000000000001, 1),
                std::make_pair(0b00000000000000000000000000000001, 9),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 1),
                std::make_pair(0b00000000000000000000000000000010, 9),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 1),
                std::make_pair(0b00000000000000000000000000000100, 9),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 1),
                std::make_pair(0b00000000000000000000000000001000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 1),
                std::make_pair(0b00000000000000000000000000010000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 1),
                std::make_pair(0b00000000000000000000000000100000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 1),
                std::make_pair(0b00000000000000000000000001000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 1),
                std::make_pair(0b00000000000000000000000010000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 1),
                std::make_pair(0b00000000000000000000000100000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 1),
                std::make_pair(0b00000000000000000000001000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 1),
                std::make_pair(0b00000000000000000000010000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 1),
                std::make_pair(0b00000000000000000000100000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 1),
                std::make_pair(0b00000000000000000001000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 1),
                std::make_pair(0b00000000000000000010000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 1),
                std::make_pair(0b00000000000000000100000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 1),
                std::make_pair(0b00000000000000001000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 1),
                std::make_pair(0b00000000000000010000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 1),
                std::make_pair(0b00000000000000100000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 1),
                std::make_pair(0b00000000000001000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 1),
                std::make_pair(0b00000000000010000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 1),
                std::make_pair(0b00000000000100000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 1),
                std::make_pair(0b00000000001000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 1),
                std::make_pair(0b00000000010000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 1),
                std::make_pair(0b00000000100000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 1),
                std::make_pair(0b00000001000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 1),
                std::make_pair(0b00000010000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 1),
                std::make_pair(0b00000100000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 1),
                std::make_pair(0b00001000000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 1),
                std::make_pair(0b00010000000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 1),
                std::make_pair(0b00100000000000000000000000000000, 9),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 1),
                std::make_pair(0b01000000000000000000000000000000, 9),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 1),
                std::make_pair(0b10000000000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000000000001, 2),
                std::make_pair(0b00000000000000000000000000000001, 10),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 2),
                std::make_pair(0b00000000000000000000000000000010, 10),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 2),
                std::make_pair(0b00000000000000000000000000000100, 10),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 2),
                std::make_pair(0b00000000000000000000000000001000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 2),
                std::make_pair(0b00000000000000000000000000010000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 2),
                std::make_pair(0b00000000000000000000000000100000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 2),
                std::make_pair(0b00000000000000000000000001000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 2),
                std::make_pair(0b00000000000000000000000010000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 2),
                std::make_pair(0b00000000000000000000000100000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 2),
                std::make_pair(0b00000000000000000000001000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 2),
                std::make_pair(0b00000000000000000000010000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 2),
                std::make_pair(0b00000000000000000000100000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 2),
                std::make_pair(0b00000000000000000001000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 2),
                std::make_pair(0b00000000000000000010000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 2),
                std::make_pair(0b00000000000000000100000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 2),
                std::make_pair(0b00000000000000001000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 2),
                std::make_pair(0b00000000000000010000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 2),
                std::make_pair(0b00000000000000100000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 2),
                std::make_pair(0b00000000000001000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 2),
                std::make_pair(0b00000000000010000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 2),
                std::make_pair(0b00000000000100000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 2),
                std::make_pair(0b00000000001000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 2),
                std::make_pair(0b00000000010000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 2),
                std::make_pair(0b00000000100000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 2),
                std::make_pair(0b00000001000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 2),
                std::make_pair(0b00000010000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 2),
                std::make_pair(0b00000100000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 2),
                std::make_pair(0b00001000000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 2),
                std::make_pair(0b00010000000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 2),
                std::make_pair(0b00100000000000000000000000000000, 10),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 2),
                std::make_pair(0b01000000000000000000000000000000, 10),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 2),
                std::make_pair(0b10000000000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000000000001, 3),
                std::make_pair(0b00000000000000000000000000000001, 11),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 3),
                std::make_pair(0b00000000000000000000000000000010, 11),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 3),
                std::make_pair(0b00000000000000000000000000000100, 11),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 3),
                std::make_pair(0b00000000000000000000000000001000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 3),
                std::make_pair(0b00000000000000000000000000010000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 3),
                std::make_pair(0b00000000000000000000000000100000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 3),
                std::make_pair(0b00000000000000000000000001000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 3),
                std::make_pair(0b00000000000000000000000010000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 3),
                std::make_pair(0b00000000000000000000000100000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 3),
                std::make_pair(0b00000000000000000000001000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 3),
                std::make_pair(0b00000000000000000000010000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 3),
                std::make_pair(0b00000000000000000000100000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 3),
                std::make_pair(0b00000000000000000001000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 3),
                std::make_pair(0b00000000000000000010000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 3),
                std::make_pair(0b00000000000000000100000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 3),
                std::make_pair(0b00000000000000001000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 3),
                std::make_pair(0b00000000000000010000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 3),
                std::make_pair(0b00000000000000100000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 3),
                std::make_pair(0b00000000000001000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 3),
                std::make_pair(0b00000000000010000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 3),
                std::make_pair(0b00000000000100000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 3),
                std::make_pair(0b00000000001000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 3),
                std::make_pair(0b00000000010000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 3),
                std::make_pair(0b00000000100000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 3),
                std::make_pair(0b00000001000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 3),
                std::make_pair(0b00000010000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 3),
                std::make_pair(0b00000100000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 3),
                std::make_pair(0b00001000000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 3),
                std::make_pair(0b00010000000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 3),
                std::make_pair(0b00100000000000000000000000000000, 11),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 3),
                std::make_pair(0b01000000000000000000000000000000, 11),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 3),
                std::make_pair(0b10000000000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000000000001, 4),
                std::make_pair(0b00000000000000000000000000000001, 12),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 4),
                std::make_pair(0b00000000000000000000000000000010, 12),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 4),
                std::make_pair(0b00000000000000000000000000000100, 12),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 4),
                std::make_pair(0b00000000000000000000000000001000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 4),
                std::make_pair(0b00000000000000000000000000010000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 4),
                std::make_pair(0b00000000000000000000000000100000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 4),
                std::make_pair(0b00000000000000000000000001000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 4),
                std::make_pair(0b00000000000000000000000010000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 4),
                std::make_pair(0b00000000000000000000000100000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 4),
                std::make_pair(0b00000000000000000000001000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 4),
                std::make_pair(0b00000000000000000000010000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 4),
                std::make_pair(0b00000000000000000000100000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 4),
                std::make_pair(0b00000000000000000001000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 4),
                std::make_pair(0b00000000000000000010000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 4),
                std::make_pair(0b00000000000000000100000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 4),
                std::make_pair(0b00000000000000001000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 4),
                std::make_pair(0b00000000000000010000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 4),
                std::make_pair(0b00000000000000100000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 4),
                std::make_pair(0b00000000000001000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 4),
                std::make_pair(0b00000000000010000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 4),
                std::make_pair(0b00000000000100000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 4),
                std::make_pair(0b00000000001000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 4),
                std::make_pair(0b00000000010000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 4),
                std::make_pair(0b00000000100000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 4),
                std::make_pair(0b00000001000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 4),
                std::make_pair(0b00000010000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 4),
                std::make_pair(0b00000100000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 4),
                std::make_pair(0b00001000000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 4),
                std::make_pair(0b00010000000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 4),
                std::make_pair(0b00100000000000000000000000000000, 12),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 4),
                std::make_pair(0b01000000000000000000000000000000, 12),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 4),
                std::make_pair(0b10000000000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000000000001, 5),
                std::make_pair(0b00000000000000000000000000000001, 13),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 5),
                std::make_pair(0b00000000000000000000000000000010, 13),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 5),
                std::make_pair(0b00000000000000000000000000000100, 13),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 5),
                std::make_pair(0b00000000000000000000000000001000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 5),
                std::make_pair(0b00000000000000000000000000010000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 5),
                std::make_pair(0b00000000000000000000000000100000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 5),
                std::make_pair(0b00000000000000000000000001000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 5),
                std::make_pair(0b00000000000000000000000010000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 5),
                std::make_pair(0b00000000000000000000000100000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 5),
                std::make_pair(0b00000000000000000000001000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 5),
                std::make_pair(0b00000000000000000000010000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 5),
                std::make_pair(0b00000000000000000000100000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 5),
                std::make_pair(0b00000000000000000001000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 5),
                std::make_pair(0b00000000000000000010000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 5),
                std::make_pair(0b00000000000000000100000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 5),
                std::make_pair(0b00000000000000001000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 5),
                std::make_pair(0b00000000000000010000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 5),
                std::make_pair(0b00000000000000100000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 5),
                std::make_pair(0b00000000000001000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 5),
                std::make_pair(0b00000000000010000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 5),
                std::make_pair(0b00000000000100000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 5),
                std::make_pair(0b00000000001000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 5),
                std::make_pair(0b00000000010000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 5),
                std::make_pair(0b00000000100000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 5),
                std::make_pair(0b00000001000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 5),
                std::make_pair(0b00000010000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 5),
                std::make_pair(0b00000100000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 5),
                std::make_pair(0b00001000000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 5),
                std::make_pair(0b00010000000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 5),
                std::make_pair(0b00100000000000000000000000000000, 13),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 5),
                std::make_pair(0b01000000000000000000000000000000, 13),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 5),
                std::make_pair(0b10000000000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000000000001, 6),
                std::make_pair(0b00000000000000000000000000000001, 14),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 6),
                std::make_pair(0b00000000000000000000000000000010, 14),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 6),
                std::make_pair(0b00000000000000000000000000000100, 14),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 6),
                std::make_pair(0b00000000000000000000000000001000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 6),
                std::make_pair(0b00000000000000000000000000010000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 6),
                std::make_pair(0b00000000000000000000000000100000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 6),
                std::make_pair(0b00000000000000000000000001000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 6),
                std::make_pair(0b00000000000000000000000010000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 6),
                std::make_pair(0b00000000000000000000000100000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 6),
                std::make_pair(0b00000000000000000000001000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 6),
                std::make_pair(0b00000000000000000000010000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 6),
                std::make_pair(0b00000000000000000000100000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 6),
                std::make_pair(0b00000000000000000001000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 6),
                std::make_pair(0b00000000000000000010000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 6),
                std::make_pair(0b00000000000000000100000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 6),
                std::make_pair(0b00000000000000001000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 6),
                std::make_pair(0b00000000000000010000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 6),
                std::make_pair(0b00000000000000100000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 6),
                std::make_pair(0b00000000000001000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 6),
                std::make_pair(0b00000000000010000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 6),
                std::make_pair(0b00000000000100000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 6),
                std::make_pair(0b00000000001000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 6),
                std::make_pair(0b00000000010000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 6),
                std::make_pair(0b00000000100000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 6),
                std::make_pair(0b00000001000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 6),
                std::make_pair(0b00000010000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 6),
                std::make_pair(0b00000100000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 6),
                std::make_pair(0b00001000000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 6),
                std::make_pair(0b00010000000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 6),
                std::make_pair(0b00100000000000000000000000000000, 14),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 6),
                std::make_pair(0b01000000000000000000000000000000, 14),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 6),
                std::make_pair(0b10000000000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000000000001, 7),
                std::make_pair(0b00000000000000000000000000000001, 15),
            },
            {
                std::make_pair(0b00000000000000000000000000000010, 7),
                std::make_pair(0b00000000000000000000000000000010, 15),
            },
            {
                std::make_pair(0b00000000000000000000000000000100, 7),
                std::make_pair(0b00000000000000000000000000000100, 15),
            },
            {
                std::make_pair(0b00000000000000000000000000001000, 7),
                std::make_pair(0b00000000000000000000000000001000, 15),
            },
            {
                std::make_pair(0b00000000000000000000000000010000, 7),
                std::make_pair(0b00000000000000000000000000010000, 15),
            },
            {
                std::make_pair(0b00000000000000000000000000100000, 7),
                std::make_pair(0b00000000000000000000000000100000, 15),
            },
            {
                std::make_pair(0b00000000000000000000000001000000, 7),
                std::make_pair(0b00000000000000000000000001000000, 15),
            },
            {
                std::make_pair(0b00000000000000000000000010000000, 7),
                std::make_pair(0b00000000000000000000000010000000, 15),
            },
            {
                std::make_pair(0b00000000000000000000000100000000, 7),
                std::make_pair(0b00000000000000000000000100000000, 15),
            },
            {
                std::make_pair(0b00000000000000000000001000000000, 7),
                std::make_pair(0b00000000000000000000001000000000, 15),
            },
            {
                std::make_pair(0b00000000000000000000010000000000, 7),
                std::make_pair(0b00000000000000000000010000000000, 15),
            },
            {
                std::make_pair(0b00000000000000000000100000000000, 7),
                std::make_pair(0b00000000000000000000100000000000, 15),
            },
            {
                std::make_pair(0b00000000000000000001000000000000, 7),
                std::make_pair(0b00000000000000000001000000000000, 15),
            },
            {
                std::make_pair(0b00000000000000000010000000000000, 7),
                std::make_pair(0b00000000000000000010000000000000, 15),
            },
            {
                std::make_pair(0b00000000000000000100000000000000, 7),
                std::make_pair(0b00000000000000000100000000000000, 15),
            },
            {
                std::make_pair(0b00000000000000001000000000000000, 7),
                std::make_pair(0b00000000000000001000000000000000, 15),
            },
            {
                std::make_pair(0b00000000000000010000000000000000, 7),
                std::make_pair(0b00000000000000010000000000000000, 15),
            },
            {
                std::make_pair(0b00000000000000100000000000000000, 7),
                std::make_pair(0b00000000000000100000000000000000, 15),
            },
            {
                std::make_pair(0b00000000000001000000000000000000, 7),
                std::make_pair(0b00000000000001000000000000000000, 15),
            },
            {
                std::make_pair(0b00000000000010000000000000000000, 7),
                std::make_pair(0b00000000000010000000000000000000, 15),
            },
            {
                std::make_pair(0b00000000000100000000000000000000, 7),
                std::make_pair(0b00000000000100000000000000000000, 15),
            },
            {
                std::make_pair(0b00000000001000000000000000000000, 7),
                std::make_pair(0b00000000001000000000000000000000, 15),
            },
            {
                std::make_pair(0b00000000010000000000000000000000, 7),
                std::make_pair(0b00000000010000000000000000000000, 15),
            },
            {
                std::make_pair(0b00000000100000000000000000000000, 7),
                std::make_pair(0b00000000100000000000000000000000, 15),
            },
            {
                std::make_pair(0b00000001000000000000000000000000, 7),
                std::make_pair(0b00000001000000000000000000000000, 15),
            },
            {
                std::make_pair(0b00000010000000000000000000000000, 7),
                std::make_pair(0b00000010000000000000000000000000, 15),
            },
            {
                std::make_pair(0b00000100000000000000000000000000, 7),
                std::make_pair(0b00000100000000000000000000000000, 15),
            },
            {
                std::make_pair(0b00001000000000000000000000000000, 7),
                std::make_pair(0b00001000000000000000000000000000, 15),
            },
            {
                std::make_pair(0b00010000000000000000000000000000, 7),
                std::make_pair(0b00010000000000000000000000000000, 15),
            },
            {
                std::make_pair(0b00100000000000000000000000000000, 7),
                std::make_pair(0b00100000000000000000000000000000, 15),
            },
            {
                std::make_pair(0b01000000000000000000000000000000, 7),
                std::make_pair(0b01000000000000000000000000000000, 15),
            },
            {
                std::make_pair(0b10000000000000000000000000000000, 7),
                std::make_pair(0b10000000000000000000000000000000, 15),
            },
        },
        std::vector<std::vector<CoreMask>>{
            {
                std::make_pair(0b00000000000000000000000011111111, 0),
                std::make_pair(0b00000000000000000000000011111111, 8),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 0),
                std::make_pair(0b00000000000000001111111100000000, 8),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 0),
                std::make_pair(0b00000000111111110000000000000000, 8),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 0),
                std::make_pair(0b11111111000000000000000000000000, 8),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 1),
                std::make_pair(0b00000000000000000000000011111111, 9),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 1),
                std::make_pair(0b00000000000000001111111100000000, 9),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 1),
                std::make_pair(0b00000000111111110000000000000000, 9),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 1),
                std::make_pair(0b11111111000000000000000000000000, 9),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 2),
                std::make_pair(0b00000000000000000000000011111111, 10),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 2),
                std::make_pair(0b00000000000000001111111100000000, 10),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 2),
                std::make_pair(0b00000000111111110000000000000000, 10),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 2),
                std::make_pair(0b11111111000000000000000000000000, 10),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 3),
                std::make_pair(0b00000000000000000000000011111111, 11),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 3),
                std::make_pair(0b00000000000000001111111100000000, 11),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 3),
                std::make_pair(0b00000000111111110000000000000000, 11),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 3),
                std::make_pair(0b11111111000000000000000000000000, 11),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 4),
                std::make_pair(0b00000000000000000000000011111111, 12),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 4),
                std::make_pair(0b00000000000000001111111100000000, 12),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 4),
                std::make_pair(0b00000000111111110000000000000000, 12),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 4),
                std::make_pair(0b11111111000000000000000000000000, 12),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 5),
                std::make_pair(0b00000000000000000000000011111111, 13),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 5),
                std::make_pair(0b00000000000000001111111100000000, 13),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 5),
                std::make_pair(0b00000000111111110000000000000000, 13),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 5),
                std::make_pair(0b11111111000000000000000000000000, 13),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 6),
                std::make_pair(0b00000000000000000000000011111111, 14),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 6),
                std::make_pair(0b00000000000000001111111100000000, 14),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 6),
                std::make_pair(0b00000000111111110000000000000000, 14),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 6),
                std::make_pair(0b11111111000000000000000000000000, 14),
            },
            {
                std::make_pair(0b00000000000000000000000011111111, 7),
                std::make_pair(0b00000000000000000000000011111111, 15),
            },
            {
                std::make_pair(0b00000000000000001111111100000000, 7),
                std::make_pair(0b00000000000000001111111100000000, 15),
            },
            {
                std::make_pair(0b00000000111111110000000000000000, 7),
                std::make_pair(0b00000000111111110000000000000000, 15),
            },
            {
                std::make_pair(0b11111111000000000000000000000000, 7),
                std::make_pair(0b11111111000000000000000000000000, 15),
            },
        },
        std::vector<CoreMask>{
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
            std::make_pair(0b11111111111111111111111111111111, 32),
        },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        std::vector<int>{ 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 }),
};
// clang-format on
} // namespace
