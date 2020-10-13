//
// Created by Zach Bortoff on 2020-06-03.
//

#include <gtest/gtest.h>
#include "uci_options.hpp"

class UCIOptionsTest : public ::testing::Test {
protected:
    UCIOptions options;
};

TEST_F(UCIOptionsTest, Constructor) {
    EXPECT_EQ(options.style, Style::BALANCED);
    EXPECT_EQ(options.default_tt_size, 1);
    EXPECT_EQ(options.minimum_tt_size, 1);
    EXPECT_EQ(options.maximum_tt_size, 10);
    EXPECT_EQ(options.tt_size, 0);
    EXPECT_EQ(options.own_book, false);
    EXPECT_EQ(options.ponder, false);
    EXPECT_EQ(options.show_current_line, false);
    EXPECT_EQ(options.limit_strength, false);
    EXPECT_EQ(options.default_elo, 3000);
    EXPECT_EQ(options.minimum_elo, 500);
    EXPECT_EQ(options.maximum_elo, 3000);
    EXPECT_EQ(options.elo, 0);
    EXPECT_EQ(options.default_threads, 1);
    EXPECT_EQ(options.minimum_threads, 1);
    EXPECT_EQ(options.maximum_threads, 1);
    EXPECT_EQ(options.threads, 1);

    EXPECT_EQ(options.moves_to_go, -1);
    EXPECT_EQ(options.w_time_milli, -1);
    EXPECT_EQ(options.b_time_milli, -1);
    EXPECT_EQ(options.w_incr_milli, -1);
    EXPECT_EQ(options.b_incr_milli, -1);
    EXPECT_EQ(options.search_to_depth_x, -1);
    EXPECT_EQ(options.search_for_time_x, -1);
    EXPECT_EQ(options.infinite, false);
    EXPECT_EQ(options.ponder, false);
}

TEST_F(UCIOptionsTest, ResetGameStateVariables) {
    EXPECT_EQ(options.moves_to_go, -1);
    EXPECT_EQ(options.w_time_milli, -1);
    EXPECT_EQ(options.b_time_milli, -1);
    EXPECT_EQ(options.w_incr_milli, -1);
    EXPECT_EQ(options.b_incr_milli, -1);
    EXPECT_EQ(options.search_to_depth_x, -1);
    EXPECT_EQ(options.search_for_time_x, -1);
    EXPECT_EQ(options.infinite, false);
    EXPECT_EQ(options.ponder, false);

    options.moves_to_go = 10;
    options.w_time_milli = 1000;
    options.b_time_milli = 5000;
    options.w_incr_milli = 10;
    options.b_incr_milli = 100;
    options.search_to_depth_x = 5;
    options.search_for_time_x = 100;
    options.infinite = true;
    options.ponder = true;

    EXPECT_NE(options.moves_to_go, -1);
    EXPECT_NE(options.w_time_milli, -1);
    EXPECT_NE(options.b_time_milli, -1);
    EXPECT_NE(options.w_incr_milli, -1);
    EXPECT_NE(options.b_incr_milli, -1);
    EXPECT_NE(options.search_to_depth_x, -1);
    EXPECT_NE(options.search_for_time_x, -1);
    EXPECT_NE(options.infinite, false);
    EXPECT_NE(options.ponder, false);

    options.reset_game_state_vars();

    EXPECT_EQ(options.moves_to_go, -1);
    EXPECT_EQ(options.w_time_milli, -1);
    EXPECT_EQ(options.b_time_milli, -1);
    EXPECT_EQ(options.w_incr_milli, -1);
    EXPECT_EQ(options.b_incr_milli, -1);
    EXPECT_EQ(options.search_to_depth_x, -1);
    EXPECT_EQ(options.search_for_time_x, -1);
    EXPECT_EQ(options.infinite, false);
    EXPECT_EQ(options.ponder, false);
}

TEST_F(UCIOptionsTest, Output) {
    std::cout << options << std::endl;
    EXPECT_EQ(0, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}