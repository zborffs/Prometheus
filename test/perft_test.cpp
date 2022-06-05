#include "defines.hpp"
#include "globals.hpp"
#include <gtest/gtest.h>
#include "board.hpp"
#include "move_gen.hpp"
#include "chess_clock.hpp"
#include "search.hpp"
#include <iostream>
#include <string>

std::string exec_path;
std::string mate_in_2_path;

class PerftTester : public ::testing::Test {
protected:
    Board board;
    ChessClock clock;
};

TEST_F(PerftTester, Positions_Perft_1) {
    try {
        auto fens = read_all_fen_from_file(mate_in_2_path);
        uint64_t total_nodes{0};
        double d{0.0};

        for (auto & fen : fens) {
            auto separated_fen = split(fen, ';');
            std::string fen_string = separated_fen[0];
            board.set_board(fen_string);
            std::cout << fen_string << std::endl << board << std::endl;

            for (unsigned i = 1; i < separated_fen.size(); i++) {
                auto depth_string = split(separated_fen[i]);
                assert(depth_string.size() == 2);
                Depth depth = i;
                int expected_leaf_nodes = std::stoi(depth_string[1]);
                std::cout << "[" << (int)depth << "/" << separated_fen.size() - 1 << "]" << std::endl;
                clock.start();
                int leaf_nodes = 0; perft(board, depth, leaf_nodes);
                clock.stop();
                d += clock.duration();
                total_nodes += leaf_nodes;
                EXPECT_EQ(leaf_nodes, expected_leaf_nodes);
            }
        }

        double seconds = d / 1000000000;
        double nps = total_nodes / seconds;
        std::cout << "Nodes " << total_nodes << " in " << seconds << " seconds" << std::endl;
        std::cout << "Nodes Per Second: " << nps << std::endl;
    } catch (const std::exception& e) {
        throw;
    }
}

// Execute: ./PerftTestRunner data/Positions.perft
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    /// Get path to the perft file from executable directory and command line arguments
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
    mate_in_2_path = std::string(s + "/../../" + argv[1]);

    return RUN_ALL_TESTS();
}
