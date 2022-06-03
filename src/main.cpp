/// Global Variable include -- must be included first
#include "globals.hpp"

/// STL Includes
#include <thread>
#include <iostream>
#include <memory>

/// Third Party Includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cereal/archives/binary.hpp>
#include <spdlog/sinks/basic_file_sink.h>

/// Project Includes
#include "eval.hpp"
#include "protocol.hpp"
#include "search.hpp"
#include "book.hpp"

#define LOG_FAILURE (-1) // flesh out all the quit flags later

bool init_logger(const std::string& logpath) noexcept;
void create_book(const std::string& filename, Book& book);

int main([[maybe_unused]] const int argc, char* argv[]) {
    std::string path(argv[0]);
#ifdef WINDOWS
    std::string base(path.substr(0, arg.find_last_of("\\")));
#else
    std::string base(path.substr(0, path.find_last_of('/')));
#endif // WINDOWS

    // Initialize the logger variables, if it fails to initialize, then quit.
    std::string logpath(base + "/../logs/Prometheus.log");
    std::string bookpath(base + "/../data/PrometheusOpening.book");
    if (!init_logger(logpath)) {
        return LOG_FAILURE;
    }

    // Instantiate all state objects
    Board board;
    UCIOptions options;
    SearchState search_state(32 * 1000000 / sizeof(ChessHash));
    EvaluationState eval_state;
    Book book;
    create_book(bookpath, book);

    // Create loop variables
    bool quit = false;
    std::string input{};

    // While the user hasn't quit the program, process the user input for "uci" command
    while (!quit) {
        getline(std::cin, input);

        if (input == "uci") {
            // If we get "uci" command, then log it and enter uci protocol function
            start_uci_protocol(board, options, search_state, eval_state, book);
            quit = true;
        } else if (input == "quit") {
            // If we get "quit" command, just quit
            quit = true;
        }
    }

    return 0;
}

/**
 * initializes the logger; assumes that the folder containing the executable is in the same directory as the folder
 * storing the log text files.
 * @param path the path to the executable of this program
 * @return     boolean representing the success of the function
 */
bool init_logger(const std::string& logpath) noexcept {
    try {
        // setup the file sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logpath, true);
        file_sink->set_level(spdlog::level::trace);

        // setup the logger using both sinks
        spdlog::logger logger(logger_name, {file_sink});
        logger.set_level(spdlog::level::debug);
        spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger_name, spdlog::sinks_init_list({file_sink})));
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

/**
 *
 * @param book
 */
void create_book(const std::string& filename, Book& book) {
    std::ifstream f(filename, std::ios::binary);
    cereal::BinaryInputArchive iarchive(f); // Create an input archive
    iarchive(book); // Read the data from the archive
}