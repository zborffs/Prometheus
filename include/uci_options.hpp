//
// Created by Zach Bortoff on 2019-11-03.
//

#ifndef PROMETHEUS_UCI_OPTIONS_HPP
#define PROMETHEUS_UCI_OPTIONS_HPP

/// STL Includes
#include <string>
#include <iostream>
#include <ios>

/// Project Includes
#include "defines.hpp"

struct UCIOptions {
    /// Engine State Variables (Options)
    Style style{Style::BALANCED};
    int default_tt_size{1}; // in MB
    int minimum_tt_size{1}; // in MB
    int maximum_tt_size{10}; // in MB
    int tt_size{0}; // in MB
    bool own_book{false}; // use Prometheus's own book
    bool show_current_line{false}; // output the current line
    bool limit_strength{false}; // should the engine limit its strength
    int default_elo{3000};
    int minimum_elo{500};
    int maximum_elo{3000};
    int elo{0};
    int default_threads{1};
    int minimum_threads{1};
    int maximum_threads{1};
    int threads{1};

    /// Game State Variables
    int moves_to_go{-1}; // the # of moves in the game to search to
    int w_time_milli{-1}; // total white milliseconds for the match
    int b_time_milli{-1};
    int w_incr_milli{-1};
    int b_incr_milli{-1};
    int search_to_depth_x{-1};
    int search_for_time_x{-1};
    int search_x_nodes{-1};
    int search_for_mate_in_x{-1};
    int move_time{-1};
    bool infinite{false};
    bool ponder{false};
    bool sudden_death{false};

    /**
     * sets all game-state variables (moves_to_go, w_time_milli, etc.) to their default values
     */
    void reset_game_state_vars() noexcept;

    /**
     * prints all engine-state options to stdout
     */
    void send_options() noexcept;

    /**
     * prints the a UCIOptions object instance
     * @param os  reference to std::ostream
     * @param opt UCIOptions instance
     * @return    updated version of std::ostream reference
     */
    friend std::ostream& operator<<(std::ostream& os, const UCIOptions& opt) {
        os << "UCIOptions:" << std::endl;
        os << "- Style: ";

        switch (opt.style) {
            case Style::AGGRESSIVE: os << "Aggressive"; break;
            case Style::BALANCED: os << "Balanced"; break;
            case Style::DEFENSIVE: os << "Defensive"; break;
        }

        os << std::endl;

        os << std::boolalpha;

        os << "  Default Transposition Table Size: " << opt.default_tt_size << " [MB]" << std::endl;
        os << "  Minimum Transposition Table Size: " << opt.minimum_tt_size << " [MB]" << std::endl;
        os << "  Maximum Transposition Table Size: " << opt.maximum_tt_size << " [MB]" << std::endl;
        os << "  Actual Transposition Table Size: " << opt.tt_size << " [MB]" << std::endl;
        os << "  Use Own Book: " << opt.own_book << std::endl;
        os << "  Show Current Line: " << opt.show_current_line << std::endl;
        os << "  Limit Strength: " << opt.limit_strength << std::endl;
        os << "  Default ELO: " << opt.default_elo << std::endl;
        os << "  Minimum ELO: " << opt.minimum_elo << std::endl;
        os << "  Maximum ELO: " << opt.maximum_elo << std::endl;
        os << "  Actual ELO: " << opt.elo << std::endl;
        os << "  Default Number of Threads: " << opt.default_threads << std::endl;
        os << "  Minimum Number of Threads: " << opt.minimum_threads << std::endl;
        os << "  Maximum Number of Threads: " << opt.maximum_threads << std::endl;
        os << "  Number of Threads: " << opt.threads << std::endl;
        os << "  Moves to Go: " << opt.moves_to_go << std::endl;
        os << "  White Time Left: " << opt.w_time_milli << " [ms]" << std::endl;
        os << "  White Incremental Time: " << opt.w_incr_milli << " [ms]" << std::endl;
        os << "  Black Time Left: " << opt.b_time_milli << " [ms]" << std::endl;
        os << "  Black Incremental Time: " << opt.b_incr_milli << " [ms]" << std::endl;
        os << "  Search to Depth: " << opt.search_to_depth_x << std::endl;
        os << "  Search for Time: " << opt.search_for_time_x << " [ms]" << std::endl;
        os << "  Search for Nodes: " << opt.search_x_nodes << " [nodes]" << std::endl;
        os << "  Search for Mate in: " << opt.search_for_mate_in_x << " [moves]" << std::endl;
        os << "  Search for Exactly: " << opt.move_time << " [ms]" << std::endl;
        os << "  Infinite: " << opt.infinite << std::endl;
        os << "  Ponder: " << opt.ponder << std::endl;
        os << "  Sudden Death: " << opt.sudden_death << std::endl;

        os << std::noboolalpha;
        return os;
    }
};

#endif //PROMETHEUS_UCI_OPTIONS_HPP
