#ifndef CLOAK_MOVE_GEN_HPP
#define CLOAK_MOVE_GEN_HPP

/// STL Includes
#include <algorithm>
#include <memory>
#include <type_traits>
#include <vector>

/// Third-Party Includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

/// Project Includes
#include "attack.hpp"
#include "bitmask.hpp"
#include "board.hpp"
#include "chess_move.hpp"
#include "defines.hpp"
#include "extern.hpp"
#ifdef USE_SSE
#include "magic.hpp"
#endif // USE_SSE

namespace internal {
    void gen_pawn_cap(Board& board, std::vector<ChessMove>& movelist);
    void gen_pawn_prom(Board& board, std::vector<ChessMove>& movelist);
    void gen_pawn_quiet(Board& board, std::vector<ChessMove>& movelist);
    void gen_ep_cap(Board& board, std::vector<ChessMove>& movelist);

    void gen_rook_cap(Board& board, std::vector<ChessMove>& movelist);
    void gen_rook_quiet(Board& board, std::vector<ChessMove>& movelist);

    void gen_knight_cap(Board& board, std::vector<ChessMove>& movelist);
    void gen_knight_quiet(Board& board, std::vector<ChessMove>& movelist);

    void gen_bishop_cap(Board& board, std::vector<ChessMove>& movelist);
    void gen_bishop_quiet(Board& board, std::vector<ChessMove>& movelist);

    void gen_queen_cap(Board& board, std::vector<ChessMove>& movelist);
    void gen_queen_quiet(Board& board, std::vector<ChessMove>& movelist);

    void gen_king_cap(Board& board, std::vector<ChessMove>& movelist);
    void gen_king_quiet(Board& board, std::vector<ChessMove>& movelist);
    void gen_castle(Board& board, std::vector<ChessMove>& movelist);
}

void gen_all_moves(Board& board, std::vector<ChessMove>& movelist);
void gen_all_caps(Board& board, std::vector<ChessMove>& movelist);

#endif // CLOAK_MOVE_GEN_HPP
