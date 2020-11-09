//
// Created by Zach Bortoff on 2020-11-04.
//

/// Project Includes
#include "defines.hpp"
#include "globals.hpp"
#include "board.hpp"
#include "chess_clock.hpp"
#include "search.hpp"

/// External Library Includes
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

/// Standard Library Includes
#include <iostream>
#include <string>


/// Global variables for this tes
std::string exec_path;
std::string mate_in_2_path;

class SearchTester : public ::testing::Test {
protected:
    Board board;
    ChessClock clock;
    SearchState* search_state;
    EvaluationState eval_state;
    UCIOptions options;

protected:
    void SetUp() override {
        search_state = new SearchState(16384);
    }

    void TearDown() override {
        delete search_state;
        search_state = nullptr;
    }
};

TEST_F(SearchTester, Mate_In_2) {
    options.reset_game_state_vars();
    options.infinite = true;

    try {
        auto fens = read_all_fen_from_file(mate_in_2_path);
        double d{0.0};

        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::cout << fen_string << std::endl << board << std::endl;
            std::string expected_move_str = split(separated_fen[1], ' ')[0];
            clock.start();
            ChessMove best_move = think(board, options, *search_state, eval_state);
            clock.stop();
            d += clock.duration();
            std::string move_str = best_move.to_algebraic_notation();
            if (expected_move_str[expected_move_str.size() - 1] == '+') {
                expected_move_str = std::string(expected_move_str.begin(), expected_move_str.end() - 1);
            }
            EXPECT_EQ(expected_move_str, move_str);
        }

        double seconds = d / 1000000000;
        std::cout << " Success! It took " << seconds << " seconds" << std::endl;
    } catch (const std::exception& e) {
        throw;
    }
}

#define LOG_FAILURE -1 // flesh out all the quit flags later

bool init_logger(const std::string& path) noexcept;

int main(int argc, char **argv) {
    if (!init_logger(argv[0])) {
        return LOG_FAILURE;
    }

    testing::InitGoogleTest(&argc, argv);

    /// Get path to the "mate-in-2.puz" file from executable directory and command line arguments
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
    mate_in_2_path = std::string(s + "/../../" + argv[1]);

    return RUN_ALL_TESTS();
}

/**
 * initializes the logger; assumes that the folder containing the executable is in the same directory as the folder
 * storing the log text files.
 * @param path the path to the executable of this program
 * @return     boolean representing the success of the function
 */
bool init_logger(const std::string& path) noexcept {
    try {
#ifdef WINDOWS
        std::string base(path.substr(0, arg.find_last_of("\\")));
#else
        std::string base(path.substr(0, path.find_last_of("/")));
#endif // WINDOWS
        std::string path_to_log(base + "/../../logs/Prometheus-SearchTest.log");

        auto logger = spdlog::daily_logger_st(logger_name, path_to_log, 4, 59);
        logger->set_level(spdlog::level::debug);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}