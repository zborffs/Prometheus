//
// Created by Zach Bortoff on 2019-10-31.
//

#include <gtest/gtest.h>
#include "chess_hash.hpp"

class ChessHashTester : public ::testing::Test {
protected:

protected:

};

TEST_F(ChessHashTester, ToString) {
    EXPECT_EQ(0, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}