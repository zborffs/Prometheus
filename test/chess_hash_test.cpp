#include <gtest/gtest.h>
#include "chess_hash.hpp"

class ChessHashTester : public ::testing::Test {

};

TEST_F(ChessHashTester, Equality) {
    ChessHash hash1(0x01, 123, A1, B2, MoveFlag::BISHOP_PROMO, 1, HashFlag::NO_INFO, 24);
    std::cout << hash1 << std::endl;
    ChessHash hash2(0x01, 999, A1, B2, MoveFlag::BISHOP_PROMO, 4, HashFlag::EXACT, 29);
    std::cout << hash2 << std::endl;
    ChessHash hash3(0x021, 999, A1, B2, MoveFlag::BISHOP_PROMO, 4, HashFlag::EXACT, 29);
    EXPECT_EQ(hash1, hash2);
    EXPECT_NE(hash2, hash3);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}