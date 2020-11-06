//
// Created by Zach Bortoff on 2020-06-03.
//

#ifndef PROMETHEUS_PROTOCOL_FUNCS_HPP
#define PROMETHEUS_PROTOCOL_FUNCS_HPP

/// Standard Library Includes
#include <iostream>
#include <memory>
#include <optional>

/// External Library Includes
#include <spdlog/spdlog.h> // logger

/// Project Includes
#include "board.hpp"
#include "chess_move.hpp"
#include "defines.hpp"
#include "eval.hpp"
#include "extern.hpp"
#include "search.hpp"
#include "string_manip.hpp"
#include "uci_options.hpp"

/**
 * Internal namespace contains all functions that won't be called from external files
 */
namespace internal {
    void parse_pos(Board& board, const std::vector<std::string>& position_command);
    void parse_go(UCIOptions& options, const std::vector<std::string>& go_command);
    std::optional<ChessMove> parse_move(Board& board, const std::string& move);
    void set_options(UCIOptions& options, const std::vector<std::string>& commands);
}

void start_uci_protocol(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state);

#endif //PROMETHEUS_PROTOCOL_FUNCS_HPP
