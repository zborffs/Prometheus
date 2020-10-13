//
// Created by Zach Bortoff on 2019-11-03.
//

#include "uci_options.hpp"

void UCIOptions::reset_game_state_vars() noexcept {
    moves_to_go = -1;
    w_time_milli = -1;
    b_time_milli = -1;
    w_incr_milli = -1;
    b_incr_milli = -1;
    search_to_depth_x = -1;
    search_for_time_x = -1;
    search_x_nodes = -1;
    infinite = false;
    ponder = false;
    sudden_death = false;
}

void UCIOptions::send_options() noexcept {
    std::cout << std::boolalpha;

    /// Transposition Table Options
    std::cout << "option name Hash type spin default " << default_tt_size << " min " << minimum_tt_size << " max " << maximum_tt_size << std::endl;

    /// OwnBook Options
    std::cout << "option name OwnBook type check default " << own_book << std::endl;

    /// UCI_ShowCurrLine options
    std::cout << "option name UCI_ShowCurrLine type check default " << show_current_line << std::endl;

    /// UCI_LimitStrength options
    std::cout << "option name UCI_LimitStrength type check default " << limit_strength << std::endl;

    /// UCI_Elo options
    std::cout << "option name UCI_Elo type spin default " << default_elo << " min " << minimum_elo << " max " << maximum_elo << std::endl;

    /// Style options
    std::cout << "options name Style type combo default Balanced var Aggressive var Balanced var Defensive" << std::endl;

    /// Threads options
    std::cout << "options name Threads type spin default " << default_threads << " min " << minimum_threads << " max " << maximum_threads << std::endl;

    std::cout << std::noboolalpha;
}
