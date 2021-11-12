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

    /// Threads options
    std::cout << "options name Threads type spin default " << default_threads << " min " << minimum_threads << " max " << maximum_threads << std::endl;

    std::cout << std::noboolalpha;
}
