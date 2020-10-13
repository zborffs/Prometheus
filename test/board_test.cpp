//
// Created by Zach Bortoff on 2019-11-16.
//
#include "defines.hpp"
#include "globals.hpp"
#include <gtest/gtest.h>
#include "board.hpp"
#include <iostream>

class BoardTester : public ::testing::Test {
protected:
    Board board;
};

TEST_F(BoardTester, PrintBoard) {
    std::cout << board << std::endl;
    EXPECT_EQ(0, 0);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

///position startpos moves e2e4 e7e5 g1f3 g8f6 f3g1 f6g8 g1f3 g8f6 f3g1 f6g8