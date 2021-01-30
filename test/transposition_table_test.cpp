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

TEST_F(TranspositionTableTester, TwoEquivalent_BothExact) {
    int size = 1024;
    TranspositionTable tt(size);
    EXPECT_EQ(tt.size(), size);
#ifndef NDEBUG
    tt.find(0x01);
    EXPECT_EQ(tt.hit_rate(), 0);
    EXPECT_EQ(tt.num_entries(), 0);
    tt.reset_tracking_variables();
#endif // NDEBUG

    ChessHash hash1(0x01, 123, A1, B2, QUIET_MOVE, 2, EXACT, 2);
    ChessHash hash2(0x01, 234, A1, B2, QUIET_MOVE, 1, EXACT, 4);
    EXPECT_EQ(hash1, hash2);

    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);

    // Because the origin insert is EXACT and hasn't expired and it has a smaller depth, it shouldn't be replaced
    EXPECT_EQ(tt.insert(hash2), false);
    EXPECT_EQ(*tt.find(hash1.key), hash1);

    ChessHash hash3(0x01, 222, A1, B2, QUIET_MOVE, 1, EXACT, hash1.age + AGE_DIFFERENCE_THRESHOLD + 1);
    EXPECT_EQ(tt.insert(hash3), true);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    tt.clear();
    EXPECT_EQ(tt.find(hash3.key), nullptr);

    ChessHash hash4(0x01, 222, A1, B2, QUIET_MOVE, 1, EXACT, hash1.age + AGE_DIFFERENCE_THRESHOLD + 2);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash4), true);
    EXPECT_EQ(*tt.find(hash4.key), hash4);
    tt.clear();
    EXPECT_EQ(tt.find(hash4.key), nullptr);

    ChessHash hash5(0x01, 222, A1, B2, QUIET_MOVE, 1, EXACT, hash1.age - hash1.depth + AGE_DIFFERENCE_THRESHOLD);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    bool expect_false = tt.insert(hash5);
    EXPECT_EQ(expect_false, false);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
}

TEST_F(TranspositionTableTester, TwoEquivalent_NotExact) {
    int size = 1024;
    TranspositionTable tt(size);
    EXPECT_EQ(tt.size(), size);
#ifndef NDEBUG
    tt.find(0x01);
    EXPECT_EQ(tt.hit_rate(), 0);
    EXPECT_EQ(tt.num_entries(), 0);
    tt.reset_tracking_variables();
#endif // NDEBUG

    ChessHash hash1(0x01, 123, A1, B2, QUIET_MOVE, 2, UPPER_BOUND, 2);
    ChessHash hash2(0x01, 234, A1, B2, QUIET_MOVE, 4, HashFlag::LOWER_BOUND, 4);
    EXPECT_EQ(hash1, hash2);

    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash2), true);
    EXPECT_EQ(*tt.find(hash1.key), hash2);
    tt.clear();
    EXPECT_EQ(tt.find(hash1.key), nullptr);

    ChessHash hash3(0x01, 222, A1, B2, QUIET_MOVE, 1, HashFlag::LOWER_BOUND, 4);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash3), false);
    EXPECT_EQ(*tt.find(hash3.key), hash1);
    tt.clear();
    EXPECT_EQ(tt.find(hash3.key), nullptr);

    ChessHash hash4(0x01, 222, A1, B2, QUIET_MOVE, 3, HashFlag::AVOID_NULL, hash1.age + AGE_DIFFERENCE_THRESHOLD + 4);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash4), true);
    EXPECT_EQ(*tt.find(hash4.key), hash4);
    tt.clear();
    EXPECT_EQ(tt.find(hash4.key), nullptr);

    ChessHash hash5(0x01, 222, A1, B2, QUIET_MOVE, 1, HashFlag::NO_INFO, hash1.age - hash1.depth + AGE_DIFFERENCE_THRESHOLD);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash5), false);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
}

TEST_F(TranspositionTableTester, ExactIn_NotExactOut) {
    int size = 1024;
    TranspositionTable tt(size);
    EXPECT_EQ(tt.size(), size);
#ifndef NDEBUG
    tt.find(0x01);
    EXPECT_EQ(tt.hit_rate(), 0);
    EXPECT_EQ(tt.num_entries(), 0);
    tt.reset_tracking_variables();
#endif // NDEBUG

    ChessHash hash1(0x01, 123, A1, B2, QUIET_MOVE, 2, EXACT, 2);
    ChessHash hash2(0x01, 234, A1, B2, QUIET_MOVE, 4, LOWER_BOUND, 4);
    EXPECT_EQ(hash1, hash2);

    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);

    // Because the origin insert is EXACT and hasn't expired, it shouldn't be replaced
    EXPECT_EQ(tt.insert(hash2), false);
    EXPECT_EQ(*tt.find(hash1.key), hash1);

    ChessHash hash3(0x01, 222, A1, B2, QUIET_MOVE, 3, HashFlag::LOWER_BOUND, hash1.age + AGE_DIFFERENCE_THRESHOLD + 3);
    EXPECT_EQ(tt.insert(hash3), true);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    tt.clear();
    EXPECT_EQ(tt.find(hash3.key), nullptr);

    ChessHash hash4(0x01, 222, A1, B2, QUIET_MOVE, 3, HashFlag::LOWER_BOUND, hash1.age + AGE_DIFFERENCE_THRESHOLD + 4);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash4), true);
    EXPECT_EQ(*tt.find(hash4.key), hash4);
    tt.clear();
    EXPECT_EQ(tt.find(hash4.key), nullptr);

    ChessHash hash5(0x01, 222, A1, B2, QUIET_MOVE, 4, HashFlag::LOWER_BOUND, hash1.age + AGE_DIFFERENCE_THRESHOLD - 1);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    bool expect_false = tt.insert(hash5);
    EXPECT_EQ(expect_false, false);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
}

TEST_F(TranspositionTableTester, ExactOut_NotExactIn) {
    int size = 1024;
    TranspositionTable tt(size);
    EXPECT_EQ(tt.size(), size);
#ifndef NDEBUG
    tt.find(0x01);
    EXPECT_EQ(tt.hit_rate(), 0);
    EXPECT_EQ(tt.num_entries(), 0);
    tt.reset_tracking_variables();
#endif // NDEBUG

    ChessHash hash1(0x01, 123, A1, B2, QUIET_MOVE, 2, AVOID_NULL, 2);
    ChessHash hash2(0x01, 234, A1, B2, QUIET_MOVE, 4, EXACT, 4);
    EXPECT_EQ(hash1, hash2);

    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash2), true);
    EXPECT_EQ(*tt.find(hash1.key), hash2);
    tt.clear();
    EXPECT_EQ(tt.find(hash1.key), nullptr);

    ChessHash hash3(0x01, 222, A1, B2, QUIET_MOVE, 1, EXACT, 4);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash3), true);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    tt.clear();
    EXPECT_EQ(tt.find(hash3.key), nullptr);

    ChessHash hash4(0x01, 222, A1, B2, QUIET_MOVE, 3, HashFlag::EXACT, hash1.age + AGE_DIFFERENCE_THRESHOLD + 4);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash4), true);
    EXPECT_EQ(*tt.find(hash4.key), hash4);
    tt.clear();
    EXPECT_EQ(tt.find(hash4.key), nullptr);

    ChessHash hash5(0x01, 222, A1, B2, QUIET_MOVE, 1, HashFlag::EXACT, hash1.age - hash1.depth + AGE_DIFFERENCE_THRESHOLD);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash5), true);
    EXPECT_EQ(*tt.find(hash1.key), hash5);
}

TEST_F(TranspositionTableTester, Saturated_TwoEquivalent_Exact) {
    int size = 16;
    TranspositionTable tt(size);
    EXPECT_EQ(tt.size(), size);
#ifndef NDEBUG
    tt.find(0x01);
    EXPECT_EQ(tt.hit_rate(), 0);
    EXPECT_EQ(tt.num_entries(), 0);
    tt.reset_tracking_variables();
#endif // NDEBUG

    std::vector<PositionKey> keys({16, 32, 48, 64});
    ChessHash hash1(keys[0], 123, A1, B2, QUIET_MOVE, 2, LOWER_BOUND, 2);
    ChessHash hash2(keys[1], 234, A1, B2, QUIET_MOVE, 4, UPPER_BOUND, 4);
    ChessHash hash3(keys[2], 222, A1, G8, CAPTURE_MOVE, 5, AVOID_NULL, 21);
    ChessHash hash4(keys[3], 222, F2, G8, CAPTURE_MOVE, 5, LOWER_BOUND, 20);
    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash1, hash4);
    EXPECT_NE(hash2, hash3);
    EXPECT_NE(hash2, hash4);
    EXPECT_NE(hash3, hash4);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash2), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(tt.insert(hash3), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(tt.insert(hash4), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);

    // Try inserting an EXACT
    ChessHash hash5(80, -23, C2, D4, QUIET_MOVE, 1, EXACT, 4);
    EXPECT_EQ(tt.insert(hash5), true);
    EXPECT_EQ(*tt.find(hash5.key), hash5);
    EXPECT_EQ(tt.find(hash1.key), nullptr);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);

    // Try inserting not EXACT
    ChessHash hash6(96, -23, C2, D4, QUIET_MOVE, 25, EXACT, 29);
    EXPECT_EQ(tt.insert(hash6), true);
    EXPECT_EQ(*tt.find(hash5.key), hash5);
    EXPECT_EQ(*tt.find(hash6.key), hash6);
    EXPECT_EQ(tt.find(hash2.key), nullptr);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);

    // Try inserting EXACT but one of the original hashes has expired
    ChessHash hash7(16, -2441, F8, G8, CAPTURE_MOVE, 1, AVOID_NULL, hash5.age - hash5.depth + AGE_DIFFERENCE_THRESHOLD + 1);
    EXPECT_EQ(tt.insert(hash7), true);
    EXPECT_EQ(*tt.find(hash7.key), hash7);
    EXPECT_EQ(tt.find(hash5.key), nullptr);
    EXPECT_EQ(*tt.find(hash6.key), hash6);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);
}

TEST_F(TranspositionTableTester, Saturated_TwoEquivalent_NotExact) {
    int size = 16;
    TranspositionTable tt(size);
    EXPECT_EQ(tt.size(), size);
#ifndef NDEBUG
    tt.find(0x01);
    EXPECT_EQ(tt.hit_rate(), 0);
    EXPECT_EQ(tt.num_entries(), 0);
    tt.reset_tracking_variables();
#endif // NDEBUG

    std::vector<PositionKey> keys({16, 32, 48, 64});
    ChessHash hash1(keys[0], 123, A1, B2, QUIET_MOVE, 2, EXACT, 2);
    ChessHash hash2(keys[1], 234, A1, B2, QUIET_MOVE, 4, EXACT, 4);
    ChessHash hash3(keys[2], 222, A1, G8, CAPTURE_MOVE, 5, EXACT, 21);
    ChessHash hash4(keys[3], 222, F2, G8, CAPTURE_MOVE, 5, EXACT, 20);
    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash1, hash4);
    EXPECT_NE(hash2, hash3);
    EXPECT_NE(hash2, hash4);
    EXPECT_NE(hash3, hash4);
    EXPECT_EQ(tt.insert(hash1), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(tt.insert(hash2), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(tt.insert(hash3), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(tt.insert(hash4), true);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);

    // Try inserting an EXACT
    ChessHash hash5(80, -23, C2, D4, QUIET_MOVE, 1, EXACT, 4);
    EXPECT_EQ(tt.insert(hash5), false);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);

    // Try inserting not EXACT
    ChessHash hash6(80, -23, C2, D4, QUIET_MOVE, 25, LOWER_BOUND, 29);
    EXPECT_EQ(tt.insert(hash6), false);
    EXPECT_EQ(*tt.find(hash1.key), hash1);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);

    // Try inserting EXACT but one of the original hashes has expired
    ChessHash hash7(80, -2441, F8, G8, CAPTURE_MOVE, 1, EXACT, hash1.age - hash1.depth + AGE_DIFFERENCE_THRESHOLD + 1);
    EXPECT_EQ(tt.insert(hash7), true);
    EXPECT_EQ(*tt.find(hash7.key), hash7);
    EXPECT_EQ(tt.find(hash1.key), nullptr);
    EXPECT_EQ(*tt.find(hash2.key), hash2);
    EXPECT_EQ(*tt.find(hash3.key), hash3);
    EXPECT_EQ(*tt.find(hash4.key), hash4);
}

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