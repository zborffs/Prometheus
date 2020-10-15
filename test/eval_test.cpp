//
// Created by Zach Bortoff on 2020-07-05.
//

/// Project Includes
#include "globals.hpp"
#include "eval.hpp"
#include "board.hpp"

/// Third-Party Includes
#include <gtest/gtest.h> // testing

std::string exec_path;
std::string fen_path;

class EvaluationTester : public ::testing::Test {
protected:
    Board board;
    EvaluationState eval_state;
};

TEST_F(EvaluationTester, EastAttackFileFill) {
    Bitboard b1 = (C64(1) << A1);
    EXPECT_EQ(FILE_MASK[FILEB], internal::east_attack_file_fill(b1));
}

TEST_F(EvaluationTester, Duo) {
    Bitboard pawns = (C64(1) << 24) | (C64(1) << 25) | (C64(1) << 26) | (C64(1) << 13) | (C64(1) << 14);
    Bitboard duoAnswer = (C64(1) << 13) | (C64(1) << 14);
    EXPECT_EQ(internal::duo(pawns), duoAnswer);
}

TEST_F(EvaluationTester, MaterialSymmetry) {
    try {
        auto fens = read_all_fen_from_file(fen_path);
        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::cout << fen_string << std::endl << board << std::endl;
            internal::def_stage(board, eval_state);
            internal::def_pos_type(board, eval_state);
            Centipawns_t eval1 = internal::material(board, eval_state, Color::BOTH, true);

            /// Flip Sides
            auto split_fen = split(fen_string);
            split_fen[0] = swap_lower_upper_case(split_fen[0]);
            split_fen[1][0] = split_fen[1][0] == 'w' ? 'b' : 'w';
            std::reverse(split_fen[0].begin(), split_fen[0].end());

            /// Set board again and evaluate
            board.set_board(split_fen.begin(), split_fen.end());
            for (int i = 0; i < split_fen.size(); i++) {
                std::cout << split_fen[i] << " ";
            }
            std::cout <<  std::endl << board << std::endl;
            internal::def_stage(board, eval_state);
            internal::def_pos_type(board, eval_state);
            Centipawns_t eval2 = internal::material(board, eval_state, Color::BOTH, true);
            EXPECT_EQ(eval1, -eval2);
        }
    } catch (const std::exception& e) {
        throw;
    }
}

TEST_F(EvaluationTester, TempoSymmetry) {
    try {
        auto fens = read_all_fen_from_file(fen_path);
        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::cout << fen_string << std::endl << board << std::endl;
            internal::def_stage(board, eval_state);
            internal::def_pos_type(board, eval_state);
            Centipawns_t eval1 = internal::eval_tempo(board, eval_state, true);

            /// Flip Sides
            auto split_fen = split(fen_string);
            split_fen[0] = swap_lower_upper_case(split_fen[0]);
            split_fen[1][0] = split_fen[1][0] == 'w' ? 'b' : 'w';
            std::reverse(split_fen[0].begin(), split_fen[0].end());

            /// Set board again and evaluate
            board.set_board(split_fen.begin(), split_fen.end());
            for (int i = 0; i < split_fen.size(); i++) {
                std::cout << split_fen[i] << " ";
            }
            std::cout <<  std::endl << board << std::endl;
            internal::def_stage(board, eval_state);
            internal::def_pos_type(board, eval_state);
            Centipawns_t eval2 = internal::eval_tempo(board, eval_state, true);
            EXPECT_EQ(eval1, -eval2);
        }
    } catch (const std::exception& e) {
        throw;
    }
}

TEST_F(EvaluationTester, PawnSymmetry) {
    try {
        auto fens = read_all_fen_from_file(fen_path);
        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::cout << fen_string << std::endl << board << std::endl;
            internal::def_stage(board, eval_state);
            internal::def_pos_type(board, eval_state);
            Centipawns_t eval1 = internal::eval_pawns(board, eval_state, true);

            /// Flip Sides
            auto split_fen = split(fen_string);
            split_fen[0] = swap_lower_upper_case(split_fen[0]);
            split_fen[1][0] = split_fen[1][0] == 'w' ? 'b' : 'w';
            std::reverse(split_fen[0].begin(), split_fen[0].end());

            /// Set board again and evaluate
            board.set_board(split_fen.begin(), split_fen.end());
            for (int i = 0; i < split_fen.size(); i++) {
                std::cout << split_fen[i] << " ";
            }
            std::cout <<  std::endl << board << std::endl;
            internal::def_stage(board, eval_state);
            internal::def_pos_type(board, eval_state);
            Centipawns_t eval2 = internal::eval_pawns(board, eval_state, true);
            EXPECT_EQ(eval1, -eval2);
        }
    } catch (const std::exception& e) {
        throw;
    }
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

    for (int i = 1; i < splitvec.size() - 1; i++) {
        s += std::string("/" + splitvec[i]);
    }
    fen_path = std::string(s + "/../../" + argv[1]);
    std::cout << fen_path << std::endl;

    return RUN_ALL_TESTS();
}
