//
// Created by Zach Bortoff on 2019-11-16.
//

#include "defines.hpp"
#include "globals.hpp"
#include <gtest/gtest.h>
#include "bitmask.hpp"
#include <iostream>

class BitmaskTester : public ::testing::Test {
protected:
    Bitboard AFile = 0x101010101010101;
    Bitboard BFile = 0x202020202020202;
    Bitboard Rank1 = 0xFF;
    Bitboard Rank2 = 0xFF00;
    Bitboard Rank3 = 0xFF0000;
    Bitboard Rank4 = 0xFF000000;
    Bitboard Rank5 = 0xFF00000000;
    Bitboard Rank6 = 0xFF0000000000;
    Bitboard Rank7 = 0xFF000000000000;
    Bitboard Rank8 = 0xFF00000000000000;
    Bitboard checkerboard = 0b1010101001010101101010100101010110101010010101011010101001010101;

    Bitboard occ1 = (C64(1 << 8) | C64(1 << 1));
    Bitboard occ2 = (C64(1 << 16) | C64(1 << 2));
    Bitboard occ3 = (C64(1 << 24) | C64(1 << 3));
    Bitboard occ4 = (C64(1) << A8) | (C64(1) << H1);
};

TEST_F(BitmaskTester, BitboardToString) {
    std::string afile_str = draw_bitboard(AFile);
    std::string afile_expect = "8 1 0 0 0 0 0 0 0 \n"
                               "7 1 0 0 0 0 0 0 0 \n"
                               "6 1 0 0 0 0 0 0 0 \n"
                               "5 1 0 0 0 0 0 0 0 \n"
                               "4 1 0 0 0 0 0 0 0 \n"
                               "3 1 0 0 0 0 0 0 0 \n"
                               "2 1 0 0 0 0 0 0 0 \n"
                               "1 1 0 0 0 0 0 0 0 \n"
                               "  A B C D E F G H\n";
    auto bfile_str = draw_bitboard(BFile);
    std::string bfile_expect = "8 0 1 0 0 0 0 0 0 \n"
                               "7 0 1 0 0 0 0 0 0 \n"
                               "6 0 1 0 0 0 0 0 0 \n"
                               "5 0 1 0 0 0 0 0 0 \n"
                               "4 0 1 0 0 0 0 0 0 \n"
                               "3 0 1 0 0 0 0 0 0 \n"
                               "2 0 1 0 0 0 0 0 0 \n"
                               "1 0 1 0 0 0 0 0 0 \n"
                               "  A B C D E F G H\n";
    auto rank8_str = draw_bitboard(Rank8);
    std::string rank8_expect = "8 1 1 1 1 1 1 1 1 \n"
                               "7 0 0 0 0 0 0 0 0 \n"
                               "6 0 0 0 0 0 0 0 0 \n"
                               "5 0 0 0 0 0 0 0 0 \n"
                               "4 0 0 0 0 0 0 0 0 \n"
                               "3 0 0 0 0 0 0 0 0 \n"
                               "2 0 0 0 0 0 0 0 0 \n"
                               "1 0 0 0 0 0 0 0 0 \n"
                               "  A B C D E F G H\n";
    auto checkerboard_str = draw_bitboard(checkerboard);
    std::string checkerboard_expect = "8 0 1 0 1 0 1 0 1 \n"
                                      "7 1 0 1 0 1 0 1 0 \n"
                                      "6 0 1 0 1 0 1 0 1 \n"
                                      "5 1 0 1 0 1 0 1 0 \n"
                                      "4 0 1 0 1 0 1 0 1 \n"
                                      "3 1 0 1 0 1 0 1 0 \n"
                                      "2 0 1 0 1 0 1 0 1 \n"
                                      "1 1 0 1 0 1 0 1 0 \n"
                                      "  A B C D E F G H\n";

    EXPECT_STREQ(afile_str.c_str(), afile_expect.c_str());
    EXPECT_STREQ(bfile_str.c_str(), bfile_expect.c_str());
    EXPECT_STREQ(rank8_str.c_str(), rank8_expect.c_str());
    EXPECT_STREQ(checkerboard_str.c_str(), checkerboard_expect.c_str());
}

TEST_F(BitmaskTester, BitScanForward) {
    EXPECT_EQ(bitscan_forward(AFile), 0);
    EXPECT_EQ(bitscan_forward(BFile), 1);
    EXPECT_EQ(bitscan_forward(Rank8), 56);
    EXPECT_EQ(bitscan_forward(checkerboard), 0);

    for (unsigned long long int i = 0; i < 64; i++) {
        Bitboard stranded_pawn = ((unsigned long long int)1 << i);
        EXPECT_EQ(bitscan_forward(stranded_pawn), i);
    }
}

TEST_F(BitmaskTester, BitScanReverse) {
    EXPECT_EQ(bitscan_reverse(AFile), 63 - 56);
    EXPECT_EQ(bitscan_reverse(BFile), 63 - 57);
    EXPECT_EQ(bitscan_reverse(Rank8), 0);
    EXPECT_EQ(bitscan_reverse(checkerboard), 0);

    for (unsigned long long int i = 0; i < 64; i++) {
        Bitboard stranded_pawn = ((unsigned long long int)1 << i);
        EXPECT_EQ(bitscan_forward(stranded_pawn), i);
    }
}

TEST_F(BitmaskTester, PopCount) {
    EXPECT_EQ(pop_count(AFile), 8);
    EXPECT_EQ(pop_count(BFile), 8);
    EXPECT_EQ(pop_count(Rank8), 8);
    EXPECT_EQ(pop_count(checkerboard), 32);
}

TEST_F(BitmaskTester, NorthFill) {
    EXPECT_EQ(north_fill(AFile), AFile);
    EXPECT_EQ(north_fill(BFile), BFile);
    EXPECT_EQ(north_fill(Rank8), Rank8);
    EXPECT_EQ(north_fill(checkerboard), (C64(0b01010101) | Rank2 | Rank3 | Rank4 | Rank5 | Rank6 | Rank7 | Rank8));
    Bitboard n_fill1 = north_fill(C64(1) << G4);
    Bitboard n_fill1_ex = ((C64(1) << G4) | C64(1) << G5) | (C64(1) << G6) | (C64(1) << G7) | (C64(1) << G8);
    EXPECT_EQ(n_fill1, n_fill1_ex);
}

TEST_F(BitmaskTester, SouthFill) {
    EXPECT_EQ(south_fill(AFile), AFile);
    EXPECT_EQ(south_fill(BFile), BFile);
    EXPECT_EQ(south_fill(Rank8), C64(-1));
    EXPECT_EQ(south_fill(checkerboard), ((C64(0b10101010) << 56) | Rank1 | Rank2 | Rank3 | Rank4 | Rank5 | Rank6 | Rank7));
}

TEST_F(BitmaskTester, FileFill) {
    EXPECT_EQ(file_fill(AFile), AFile);
    EXPECT_EQ(file_fill(BFile), BFile);
    EXPECT_EQ(file_fill(Rank8), C64(-1));
    EXPECT_EQ(file_fill(checkerboard), C64(-1));
}

TEST_F(BitmaskTester, FlipVertically) {
    EXPECT_EQ(flip_vertically(AFile), AFile);
    EXPECT_EQ(flip_vertically(BFile), BFile);
    EXPECT_EQ(flip_vertically(Rank8), Rank1);
}

TEST_F(BitmaskTester, Reverse) {
    EXPECT_EQ(reverse(AFile), C64(AFile << 7));
    EXPECT_EQ(reverse(BFile), C64(BFile << 5));
    EXPECT_EQ(reverse(Rank8), Rank1);
    EXPECT_EQ(reverse(checkerboard), checkerboard);
}

TEST_F(BitmaskTester, HorzVertMask) {
    EXPECT_EQ(horzvert_mask(A1, occ1), occ1);
    EXPECT_EQ(horzvert_mask(A1, occ2), occ1 | occ2);
    EXPECT_EQ(horzvert_mask(A1, occ3), occ1 | occ2 | occ3);
    EXPECT_EQ(horzvert_mask(A1, occ4), (AFile | Rank1) & ~(C64(1) << A1));
}

TEST_F(BitmaskTester, DiagMask) {
    /// Nothing yet.
}

TEST_F(BitmaskTester, FlipFlop) {
    EXPECT_EQ(flip_flop(AFile), C64(AFile << 7));
    EXPECT_EQ(flip_flop(BFile), C64(BFile << 5));
    EXPECT_EQ(flip_flop(Rank8), Rank1);
    EXPECT_EQ(flip_flop(checkerboard), checkerboard);
}

TEST_F(BitmaskTester, Mirror) {
    EXPECT_EQ(mirror(FILE_MASK[FILEA]), FILE_MASK[FILEH]);
    EXPECT_EQ(mirror(RANK_MASK[RANK1]), RANK_MASK[RANK1]);
}

TEST_F(BitmaskTester, DotProduct) {
    /// Nothing yet
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}