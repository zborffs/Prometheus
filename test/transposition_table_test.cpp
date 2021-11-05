/// Project Includes
#include "defines.hpp"
#include "globals.hpp"
#include "transposition_table.hpp"

/// STL Includes
#include <iostream>

/// Third Party Includes
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

class TranspositionTableTester : public ::testing::Test {

};

#define LOG_FAILURE -1 // flesh out all the quit flags later

bool init_logger(const std::string& path) noexcept;


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    /// Initialize the logger variables, if it fails to initialize, then quit.
    if (!init_logger(std::string(argv[0]))) {
        return LOG_FAILURE;
    }

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
        std::string path_to_log(base + "/../../logs/Prometheus-TranspositionTableTest.log");

        auto logger = spdlog::daily_logger_st(logger_name, path_to_log, 4, 59);
        logger->set_level(spdlog::level::debug);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}