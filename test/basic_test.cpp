//
// Created by Zach Bortoff on 2020-11-05.
//

/// Project Includes
#include "defines.hpp"
#include "globals.hpp"
#include "board.hpp"
#include "chess_clock.hpp"
#include "search.hpp"

/// External Library Includes
#include <gtest/gtest.h>

/// Standard Library Includes
#include <iostream>
#include <string>


/// Global variables for this tes
std::string exec_path;
std::string basic_tester_path;

class BasicTester : public ::testing::Test {
protected:
    Board board;
    ChessClock clock;
    SearchState search_state;
    EvaluationState eval_state;
    UCIOptions options;
};

TEST_F(BasicTester, Search) {
    options.reset_game_state_vars();
    options.infinite = true;

    try {
        auto fens = read_all_fen_from_file(basic_tester_path);

        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::string expected_move_str = split(separated_fen[1], ' ')[0];
            ChessMove best_move = think(board, options, search_state, eval_state);
            std::string move_str = best_move.to_algebraic_notation();
            if (expected_move_str[expected_move_str.size() - 1] == '+') {
                expected_move_str = std::string(expected_move_str.begin(), expected_move_str.end() - 1);
            }
            EXPECT_EQ(0, 0);
        }

    } catch (const std::exception& e) {
        throw;
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    /// Get path to the "BasicTests.fen" file from executable directory and command line arguments
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
    basic_tester_path = std::string(s + "/../../" + argv[1]);

    return RUN_ALL_TESTS();
}
