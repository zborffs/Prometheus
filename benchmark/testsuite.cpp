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
#include <spdlog/sinks/basic_file_sink.h>
#include <cereal/archives/binary.hpp>

#define LOG_FAILURE (-1) // flesh out all the quit flags later

bool init_logger(const std::string& path) noexcept;

int main(int argc, char **argv) {
    std::string path(argv[0]);
#ifdef WINDOWS
    std::string base(path.substr(0, path.find_last_of("\\")));
#else
    std::string base(path.substr(0, path.find_last_of('/')));
#endif // WINDOWS

    /// Initialize the logger variables, if it fails to initialize, then quit.
    std::string logfile_path(base + "/../../logs/TestSuiteBenchmark.log");
    std::string bookpath(base + "/../../data/CloakOpening.book");
    if (!init_logger(logfile_path)) {
        return LOG_FAILURE;
    }

    /// Create local variables at the top
    std::string bratko_fen_path;
    std::string output_file_path;
    Board board;
    ChessClock chess_clock;
    SearchState search_state(65536); // arbitrary
    EvaluationState eval_state;
    UCIOptions options;
    Book book;

    /// initialize book
    {
        std::ifstream f(bookpath, std::ios::binary);
        cereal::BinaryInputArchive iarchive(f); // Create an input archive
        iarchive(book); // Read the data from the archive
    }

    /// Get path to the input file, "Bratko-Kopec.fen", and output file, "tools/data/bratko.txt", from executable directory and command line arguments
    path = std::string(argv[0]);
    bool first_is_slash = path[0] == '/';
    auto splitvec = split(path, '/');

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
    bratko_fen_path = std::string(s + "/../../" + argv[1]);
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
        auto fens = read_all_fen_from_file(bratko_fen_path);

        /// Think for each position (inside of "think" it will cout some useful information)
        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::string expected_move_str = board.best_move();
            ChessMove best_move = think(board, options, search_state, eval_state, book);
            std::string move_str = best_move.to_algebraic_notation();

            if (move_str == expected_move_str) {
                std::cout << "passed" << std::endl;
                spdlog::get(logger_name)->info("PASSED! Move {}, == Best Move {}", move_str, expected_move_str);
            } else {
                std::cout << "failed" << std::endl;
                spdlog::get(logger_name)->info("FAILED! Cloak Move {} != Best Move {}", move_str, expected_move_str);
            }
            search_state.tt.clear();
        }
    } catch (const std::exception& e) {
        throw;
    }
    chess_clock.stop();

    std::cout.rdbuf(coutbuf); //reset to standard output again
    spdlog::get(logger_name)->info("Data Acquisition Successful! Program took {} seconds", (chess_clock.duration() / 1e9));
    return 0;
}

/**
 * initializes the logger
 * @param logfile_path path to the logfile
 * @return             whether the function successfully setup the logger
 */
bool init_logger(const std::string& logfile_path) noexcept {
    try {
        /// Setup the console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%D %H:%M:%S] [%^%l%$] [Thread %t] [File:Line %@] [Function: %!] %v");
        console_sink->set_color_mode(spdlog::color_mode::always);

        /// setup the file sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile_path, true);
        file_sink->set_level(spdlog::level::trace);

        /// setup the logger using both sinks
        spdlog::logger logger(logger_name, {console_sink, file_sink});
        logger.set_level(spdlog::level::debug);
        spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger_name, spdlog::sinks_init_list({console_sink, file_sink})));
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Logger initialization failed: " << ex.what() << std::endl;
        return false; // if we fail to initialize the logger, return false
    }

    return true;
}