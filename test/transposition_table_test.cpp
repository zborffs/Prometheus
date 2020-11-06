//
// Created by Zach Bortoff on 2019-11-25.
//

#include "defines.hpp"
#include "globals.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include "transposition_table.hpp"

class TranspositionTableTester : public ::testing::Test {
protected:

};

TEST_F(TranspositionTableTester, Constructor) {
    int size = 1024;
    TranspositionTable tt(size);
    EXPECT_EQ(tt.size(), size);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}