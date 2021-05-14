/// Project Includes
#include "defines.hpp"
#include "globals.hpp"
#include "board.hpp"
#include "chess_clock.hpp"
#include "search.hpp"
#include "book.hpp"

/// Standard Library Includes
#include <iostream>
#include <string>

/// Third-Party Includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <cereal/archives/binary.hpp>
#include <spdlog/sinks/basic_file_sink.h>

#define LOG_FAILURE -1 // flesh out all the quit flags later

bool init_logger(const std::string& path) noexcept;
void create_book(const std::string& filename, Book& book);

int main([[maybe_unused]] int argc, char **argv) {
    std::string path(argv[0]);
#ifdef WINDOWS
    std::string base(path.substr(0, path.find_last_of("\\")));
#else
    std::string base(path.substr(0, path.find_last_of('/')));
#endif // WINDOWS

    /// Initialize the logger variables, if it fails to initialize, then quit.
    std::string logfile_path(base + "/../../logs/TestSuiteBenchmark.log");
    if (!init_logger(logfile_path)) {
        return LOG_FAILURE;
    }

    /// Create local variables at the top
    std::string basic_tester_path;
    std::string output_file_path;
    std::string book_path;
    Board board;
    ChessClock chess_clock;
    SearchState search_state(16384);
    EvaluationState eval_state;
    UCIOptions options;
    Book book;

    /// Get path to the input file, "BasicTests.fen", and output file, "tools/data/ordering_output.txt", from executable directory and command line arguments
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
    basic_tester_path = std::string(s + "/../../" + argv[1]);
    output_file_path = std::string(s + "/../../" + argv[2]);
    book_path = std::string(s + "/../../data/CloakOpening.book");

    /// redirect std::cout to the output file
    std::ofstream output(output_file_path);
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(output.rdbuf()); //redirect std::cout to out.txt!

    /// initialize the uci game options before starting
    options.reset_game_state_vars();
    options.infinite = true; // at some point change this to max depth = ~6
    create_book(book_path, book);

    chess_clock.start();
    try {
        /// read all the FENs from the input file
        auto fens = read_all_fen_from_file(basic_tester_path);

        /// Think for each position (inside of "think" it will cout some useful information)
        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            think(board, options, search_state, eval_state, book);
            search_state.tt.clear();
        }
    } catch (const std::exception& e) {
        throw;
    }
    chess_clock.stop();

    std::cout.rdbuf(coutbuf); // reset to standard output again

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
        /// Setup the console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%D %H:%M:%S] [%^%l%$] [Thread %t] [File:Line %@] [Function: %!] %v");
        console_sink->set_color_mode(spdlog::color_mode::always);

        /// setup the file sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, true);
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

/**
 *
 * @param filename
 * @param book
 */
void create_book(const std::string& filename, Book& book) {
    std::ifstream f(filename, std::ios::binary);
    cereal::BinaryInputArchive iarchive(f);
    iarchive(book);
}