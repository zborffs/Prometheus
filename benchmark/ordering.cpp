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
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

#define LOG_FAILURE -1 // flesh out all the quit flags later

bool init_logger(const std::string& path) noexcept;

int main(int argc, char **argv) {
    /// Initialize the logger variables, if it fails to initialize, then quit.
    if (!init_logger(std::string(argv[0]))) {
        return LOG_FAILURE;
    }

    /// Create local variables at the top
    std::string exec_path;
    std::string basic_tester_path;
    std::string output_file_path;
    Board board;
    ChessClock chess_clock;
    SearchState search_state(16384);
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
            search_state.tt.clear();
        }
    } catch (const std::exception& e) {
        throw;
    }
    chess_clock.stop();

    std::cout.rdbuf(coutbuf); //reset to standard output again

    std::cout << "Data Acquisition Successful! Program took " << (chess_clock.duration() / 1000000.) << " [ms]" << std::endl;

    return 0;
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
        std::string path_to_log(base + "/../../logs/Prometheus-OrderingBenchmark.log");

        auto logger = spdlog::daily_logger_st(logger_name, path_to_log, 4, 59);
        logger->set_level(spdlog::level::debug);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}