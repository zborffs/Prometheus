/// Internal Includes
#include "defines.hpp"
#include "globals.hpp"
#include "board.hpp"

/// STL Includes
#include <iostream>

/// External Includes
#include <gtest/gtest.h>

std::string exec_path;
std::string fen_path;

class BoardTester : public ::testing::Test {
protected:
    Board board;
};

TEST_F(BoardTester, KingLocation) {
    try {
        auto fens = read_all_fen_from_file(fen_path);
        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::cout << fen_string << std::endl << board << std::endl;

            /// Get King Locations from .fen file
            auto king_locs = split(separated_fen[1]);
            std::string w_king_loc_str = king_locs[0];
            std::string b_king_loc_str = king_locs[1];

            File_t file = w_king_loc_str[0] - 'a';
            Rank_t rank = w_king_loc_str[1] - '1';
            Square_t w_king_loc_sq = file + 8 * rank;

            file = b_king_loc_str[0] - 'a';
            rank = b_king_loc_str[1] - '1';
            Square_t b_king_loc_sq = file + 8 * rank;

            EXPECT_EQ(board.king_location(WHITE), w_king_loc_sq);
            EXPECT_EQ(board.king_location(BLACK), b_king_loc_sq);
        }
    } catch (const std::exception& e) {
        throw;
    }
}

TEST_F(BoardTester, IsSquareAttacked) {
    board.set_board("1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - - bm Qd1+");
    std::cout << board << std::endl;

    EXPECT_EQ(board.is_sq_attacked(A1, BLACK), false);
    EXPECT_EQ(board.is_sq_attacked(A1, WHITE), false);
    EXPECT_EQ(board.is_sq_attacked(A2, WHITE), true);
    EXPECT_EQ(board.is_sq_attacked(A2, BLACK), false);
    EXPECT_EQ(board.is_sq_attacked(F2, WHITE), true);
    EXPECT_EQ(board.is_sq_attacked(F2, BLACK), false);
    EXPECT_EQ(board.is_sq_attacked(D7, WHITE), true);
    EXPECT_EQ(board.is_sq_attacked(D7, BLACK), true);
    EXPECT_EQ(board.is_sq_attacked(B8, WHITE), false);
    EXPECT_EQ(board.is_sq_attacked(B8, BLACK), true);
    EXPECT_EQ(board.is_sq_attacked(H2, WHITE), false);
    EXPECT_EQ(board.is_sq_attacked(H2, BLACK), false);
}

TEST_F(BoardTester, IsKingChecked) {
    board.set_board("1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - - bm Qd1+");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - - bm d5; f2 f8");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("8/3Pb1p1/8/3P2P1/5P2/7k/4K3/8 w - - bm Kd3");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("6k1/1p2p1bp/p5p1/4pb2/1q6/4Q3/1P2BPPP/2R3K1 w - - bm Qa3");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("8/p4p2/1p2k2p/6p1/P4b1P/1P6/3B1PP1/6K1 w - - bm Bxf4");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("7k/1q6/8/8/8/8/1K6/8 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), true);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("7k/8/5Q2/8/8/8/1K6/8 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), true);

    board.set_board("7k/p3b1P1/7P/7r/2Bp3p/6p1/1K6/8 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), true);

    board.set_board("7k/p3bP2/7P/7r/2B4p/2p3p1/1K6/8 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), true);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("7k/p3bP2/6NP/7r/2B4p/1p4p1/8/K7 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), true);

    board.set_board("8/p3bN2/7P/4k2r/2B4p/1p4p1/8/K7 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), true);

    board.set_board("8/p3b3/2N4P/4k2r/2B4p/1p4p1/8/K7 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), false);
    EXPECT_EQ(board.is_king_checked(BLACK), true);

    board.set_board("8/p3b3/7P/4k2r/2B4p/1p4p1/2n5/K7 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), true);
    EXPECT_EQ(board.is_king_checked(BLACK), false);

    board.set_board("8/p3b3/7P/4k2r/2B4p/Kp4p1/2n5/8 w - - 0 1");
    std::cout << board << std::endl;
    EXPECT_EQ(board.is_king_checked(WHITE), true);
    EXPECT_EQ(board.is_king_checked(BLACK), false);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    /// Get path to the eval file from executable directory and command line arguments
    exec_path = std::string(argv[0]);
    bool first_is_slash = exec_path[0] == '/';
    auto splitvec = split(exec_path, '/');
    std::string s{""};
    assert(!splitvec.empty());

    if (first_is_slash) {
        s += "/" + splitvec[0];
    } else {
        s += splitvec[0];
    }

    for (unsigned i = 1; i < splitvec.size() - 1; i++) {
        s += std::string("/" + splitvec[i]);
    }
    fen_path = std::string(s + "/../../" + argv[1]);

    return RUN_ALL_TESTS();
}

///position startpos moves e2e4 e7e5 g1f3 g8f6 f3g1 f6g8 g1f3 g8f6 f3g1 f6g8