//
// Created by Zach Bortoff on 2020-11-06.
//


/// Project Includes
#include "defines.hpp"
#include "globals.hpp"
#include "board.hpp"
#include "chess_clock.hpp"
#include "search.hpp"

/// Standard Library Includes
#include <iostream>
#include <string>

/// Global variables


int main(int argc, char **argv) {
    /// Create local variables at the top
    std::string exec_path;
    std::string basic_tester_path;
    std::string output_file_path;
    Board board;
    ChessClock chess_clock;
    SearchState search_state;
    EvaluationState eval_state;
    UCIOptions options;

    /// Get path to the input file, "BasicTests.fen", and output file, "tools/data/ordering_output.txt", from executable directory and command line arguments
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
    output_file_path = std::string(s + "/../../" + argv[2]);

    /// redirect std::cout to the output file
    std::ofstream output(output_file_path);
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(output.rdbuf()); //redirect std::cout to out.txt!

    /// initialize the uci game options before starting
    options.reset_game_state_vars();
    options.infinite = true; // at some point change this to max depth = ~6

    chess_clock.start();
    try {
        /// read all the FENs from the input file
        auto fens = read_all_fen_from_file(basic_tester_path);

        /// Think for each position (inside of "think" it will cout some useful information)
        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            think(board, options, search_state, eval_state);
        }
    } catch (const std::exception& e) {
        throw;
    }
    chess_clock.stop();

    std::cout.rdbuf(coutbuf); //reset to standard output again

    std::cout << "Data Acquisition Successful! Program took " << (chess_clock.duration() / 1000000.) << " [ms]" << std::endl;

    return 0;
}
