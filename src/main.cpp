/// Global Variable include -- must be included first
#include "globals.hpp"

/// STL Includes
#include <thread>
#include <iostream>
#include <memory>

/// Third Party Includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

/// Project Includes
#include "eval.hpp"
#include "protocol.hpp"
#include "search.hpp"

#define LOG_FAILURE -1 // flesh out all the quit flags later

bool init_logger(const std::string& path) noexcept;

int main([[maybe_unused]] const int argc, char* argv[]) {
    /// Initialize the logger variables, if it fails to initialize, then quit.
    if (!init_logger(std::string(argv[0]))) {
        return LOG_FAILURE;
    }

    /// Instantiate all state objects
    Board board;
    UCIOptions options;
    SearchState search_state(1024);
    EvaluationState eval_state;

    /// Create loop variables
    bool quit = false;
    std::string input{};

    /// While the user hasn't quit the program, process the user input for "uci" command
    while (!quit) {
        getline(std::cin, input);
        spdlog::get(logger_name)->info("User said \"{}\"", input);

        /// If we get "uci" command, then log it and enter uci protocol function
        if (input == "uci") {
            spdlog::get(logger_name)->info("Starting UCI Protocol...");
            start_uci_protocol(board, options, search_state, eval_state);
            quit = true;

            /// If we get "quit" command, just quit
        } else if (input == "quit") {
            spdlog::get(logger_name)->info("Quitting...");
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
bool init_logger(const std::string& path) noexcept {
    try {
#ifdef WINDOWS
        std::string base(path.substr(0, arg.find_last_of("\\")));
#else
        std::string base(path.substr(0, path.find_last_of("/")));
#endif // WINDOWS
        std::string path_to_log(base + "/../logs/Prometheus.log");

        auto logger = spdlog::daily_logger_st(logger_name, path_to_log, 4, 59);
        logger->set_level(spdlog::level::debug);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}
