/// Internal Includes
#include "defines.hpp"
#include "globals.hpp"
#include "chess_clock.hpp"
#include "uci_options.hpp"

/// External Includes
#include <gtest/gtest.h>

/// Standard Library Includes
#include <iostream>
#include <thread>

constexpr double ERROR = 1e-2;

class ChessClockTester : public ::testing::Test {
protected:
    ChessClock cc;
    UCIOptions uci_options;
};

/**
 * Tests that constructor sets the member variables to null.
 */
TEST_F(ChessClockTester, Constructor) {
    EXPECT_EQ(cc.start_time(), cc.end_time());
    EXPECT_EQ(cc.time_is_extensible(), false);
    EXPECT_EQ(cc.allocated_time(), 0);
}

/**
 * Tests the time_is_extensible() function if the UCI Options stipulate a certain amount of time for each player and the
 * time limit is < 10 seconds
 */
TEST_F(ChessClockTester, TimeIsExtensible_TimeLimit_Under10Seconds) {
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 1000;
    uci_options.w_time_milli = 1000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 9999;
    uci_options.w_time_milli = 10;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);
}

/**
 * Tests the time_is_extensible() function if the UCI Options stipulate a certain amount of time for each player and the
 * time limit is > 10 seconds
 */
TEST_F(ChessClockTester, TimeIsExtensible_TimeLimit_Over10Seconds) {
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    EXPECT_EQ(uci_options.search_to_depth_x, -1);
    EXPECT_EQ(uci_options.search_for_time_x, -1);
    uci_options.b_time_milli = 50 * 10 * 1000;
    uci_options.w_time_milli = 50 * 10 * 1000;
    EXPECT_EQ(uci_options.b_time_milli, 50 * 10 * 1000);
    EXPECT_EQ(uci_options.w_time_milli, 50 * 10 * 1000);

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), true);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), true);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 50 * 10 * 1000 - 2000;
    uci_options.w_time_milli = 50 * 10 * 1000 - 1;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);
}

/**
 * Tests the time_is_extensible() function if the UCI Options stipulate to search to a certain depth
 */
TEST_F(ChessClockTester, TimeIsExtensible_SearchToDepthX) {
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    EXPECT_EQ(uci_options.search_to_depth_x, -1);
    EXPECT_EQ(uci_options.search_for_time_x, -1);
    uci_options.search_to_depth_x = 10;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.search_to_depth_x = 15;
    uci_options.w_time_milli = 100000;
    uci_options.b_time_milli = 100000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);
}

/**
 * Tests the time_is_extensible() function if the UCI Options stipulate to search for a certain amount of time
 */
TEST_F(ChessClockTester, TimeIsExtensible_SearchForXSeconds) {
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    EXPECT_EQ(uci_options.search_to_depth_x, -1);
    EXPECT_EQ(uci_options.search_for_time_x, -1);
    uci_options.search_for_time_x = 10;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.search_for_time_x = 15;
    uci_options.w_time_milli = 100000;
    uci_options.b_time_milli = 100000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);
}

/**
 * Tests the alloc_time(...) function if the UCI Options stipuulate a certain amount of time for each player
 */
TEST_F(ChessClockTester, AllocateTime_TimeLimit) {
    EXPECT_EQ(cc.time_is_extensible(), false);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 50 * 10 * 1000;
    uci_options.w_time_milli = 50 * 10 * 1000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 50 * 10 * 1000 / 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 50 * 10 * 1000 / 50);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 9999;
    uci_options.w_time_milli = 10;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 10 / 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 9999 / 50);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.w_time_milli = 50 * 10 * 1000 - 200;
    uci_options.b_time_milli = 50 * 10 * 1000 - 100;


    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), (50 * 10 * 1000 - 200) / 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), (50 * 10 * 1000 - 100) / 50);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.w_time_milli = 1234567;
    uci_options.b_time_milli = 100000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 1234567 / 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 100000 / 50);
}

/**
 * Tests the alloc_time(...) function if the UCI Options stipulate to search to a certain depth
 */
TEST_F(ChessClockTester, AllocateTime_SearchToDepthX) {
    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    EXPECT_EQ(uci_options.search_to_depth_x, -1);
    EXPECT_EQ(uci_options.search_for_time_x, -1);
    uci_options.search_to_depth_x = 10;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), -1);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.search_to_depth_x = 15;
    uci_options.w_time_milli = 100000;
    uci_options.b_time_milli = 100000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), -1);
}

/**
 * Tests the alloc_time(..) function if the UCI Options stipulate to search for a certain amount of time
 */
TEST_F(ChessClockTester, AllocateTime_SearchForXSeconds) {
    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    EXPECT_EQ(uci_options.search_to_depth_x, -1);
    EXPECT_EQ(uci_options.search_for_time_x, -1);
    uci_options.search_for_time_x = 55;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 55 - 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 55 - 50);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.search_for_time_x = 99;
    uci_options.w_time_milli = 100000;
    uci_options.b_time_milli = 100000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 99 - 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 99 - 50);
}

/**
 * Tests that the duration() function returns the expected number of nanoseconds
 */
TEST_F(ChessClockTester, Duration) {
    using namespace std::chrono_literals;
    cc.start();
    std::this_thread::sleep_for(2s);
    cc.stop();
    EXPECT_NEAR(cc.duration() * 1e-9, 2.0, ERROR);
}

/**
 * Tests the extend_time(...) function if the UCI Options stipulate a certain amount of time for each player
 */
TEST_F(ChessClockTester, ExtendTime_TimeLimit) {
    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 1000;
    uci_options.w_time_milli = 1000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 1000 / 50);
    cc.extend_time(0);
    EXPECT_EQ(cc.allocated_time(), 1000 / 50);
    cc.extend_time(10);
    EXPECT_EQ(cc.allocated_time(), 1000 / 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 1000 / 50);
    cc.extend_time(0);
    EXPECT_EQ(cc.allocated_time(), 1000 / 50);
    cc.extend_time(10);
    EXPECT_EQ(cc.allocated_time(), 1000 / 50);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 9999;
    uci_options.w_time_milli = 10;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 10 / 50);
    cc.extend_time(2);
    EXPECT_EQ(cc.allocated_time(), 10 / 50);
    cc.extend_time(3);
    EXPECT_EQ(cc.allocated_time(), 10 / 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 9999 / 50);
    cc.extend_time(0);
    EXPECT_EQ(cc.allocated_time(), 9999 / 50);
    cc.extend_time(9);
    EXPECT_EQ(cc.allocated_time(), 9999 / 50);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 50 * 20 * 1000;
    uci_options.w_time_milli = 50 * 20 * 1000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 50 * 20 * 1000 / 50);
    cc.extend_time(0);
    EXPECT_EQ(cc.allocated_time(), 50 * 20 * 1000 / 50 + 250 * 0);
    cc.extend_time(3);
    EXPECT_EQ(cc.allocated_time(), 50 * 20 * 1000 / 50 + 250 * 0);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 50 * 20 * 1000 / 50);
    cc.extend_time(10);
    EXPECT_EQ(cc.allocated_time(), 50 * 20 * 1000 / 50 + 250 * 10);
    cc.extend_time(9);
    EXPECT_EQ(cc.allocated_time(), 50 * 20 * 1000 / 50 + 250 * 10);
}

/**
 * Tests the extend_time(...) function if the UCI Options stipulate to search to a certain depth
 */
TEST_F(ChessClockTester, ExtendTime_SearchToDepthX) {
/// UCI Options allocated time is < 10 seconds, so time isn't extensible
    EXPECT_EQ(uci_options.search_to_depth_x, -1);
    EXPECT_EQ(uci_options.search_for_time_x, -1);
    uci_options.search_to_depth_x = 10;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.extend_time(9);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.extend_time(0);
    EXPECT_EQ(cc.allocated_time(), -1);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.search_to_depth_x = 15;
    uci_options.w_time_milli = 100000;
    uci_options.b_time_milli = 100000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.extend_time(1);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), -1);
    cc.extend_time(15);
    EXPECT_EQ(cc.allocated_time(), -1);
}

/**
 * Tests the extend_time(...) function the UCI Options stipulate to search for a certain amount of time
 */
TEST_F(ChessClockTester, ExtendTime_SearchForXSeconds) {
/// UCI Options allocated time is < 10 seconds, so time isn't extensible
    EXPECT_EQ(uci_options.search_to_depth_x, -1);
    EXPECT_EQ(uci_options.search_for_time_x, -1);
    uci_options.search_for_time_x = 1000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 1000 - 50);
    cc.extend_time(0);
    EXPECT_EQ(cc.allocated_time(), 1000 - 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 1000 - 50);
    cc.extend_time(12);
    EXPECT_EQ(cc.allocated_time(), 1000 - 50);

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.search_for_time_x = 1050;
    uci_options.w_time_milli = 100000;
    uci_options.b_time_milli = 100000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.allocated_time(), 1050 - 50);
    cc.extend_time(0);
    EXPECT_EQ(cc.allocated_time(), 1050 - 50);
    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.allocated_time(), 1050 - 50);
    cc.extend_time(2);
    EXPECT_EQ(cc.allocated_time(), 1050 - 50);
}

/**
 * Tests the time_has_exceeded(...) function when time has NOT exceeded
 */
TEST_F(ChessClockTester, TimeHasExceeded) {
    using namespace std::chrono_literals;

    /// UCI Options allocated time is < 10 seconds, so time isn't extensible
    uci_options.b_time_milli = 1000;
    uci_options.w_time_milli = 1000;

    /// Test white and black
    cc.alloc_time(uci_options, WHITE);
    EXPECT_EQ(cc.has_exceeded_time(), false);
    cc.start();
    std::this_thread::sleep_for(2s);
    cc.stop();
    EXPECT_EQ(cc.has_exceeded_time(), true);

    cc.alloc_time(uci_options, BLACK);
    EXPECT_EQ(cc.time_is_extensible(), false);
    cc.start();
    std::this_thread::sleep_for(2s);
    cc.stop();
    EXPECT_EQ(cc.has_exceeded_time(), true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}