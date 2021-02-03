#ifndef CLOAK_SEARCH_HPP
#define CLOAK_SEARCH_HPP

#include "board.hpp"
#include "chess_clock.hpp"
#include "chess_move.hpp"
#include "defines.hpp"
#include "eval.hpp"
#include "extern.hpp"
#include "move_gen.hpp"
#include "uci_options.hpp"
#include "transposition_table.hpp"
#include "book.hpp"

#include <array>
#include <utility>
#include <random>

struct SearchState {
    ChessClock clock;
    Result result_flag;
    TranspositionTable tt;
    bool time_exit;
    Depth height; // ply
    std::array<std::pair<ChessMove, ChessMove>, 64> killer_move; // * 4 is for good measure
    std::array<std::array<Centipawns_t, 65>, 14> history_heuristic; // 13 being the max value a PieceType can be and 64 being the number of squares

#ifndef NDEBUG
    int leaf_nodes;
    int raw_nodes;
    int fail_high_count;
    int fail_high_first_count;
    int window_widen_count;
    int window_success_count;
#endif // NDEBUG

    explicit SearchState(int tt_size);

#ifndef NDEBUG
    [[nodiscard]] inline double ordering() const {
        return (double)fail_high_first_count / fail_high_count;
    }

    [[nodiscard]] inline double window_ratio() const {
        return (double)window_success_count / window_widen_count;
    }
#endif // NDEBUG
    void reset();

    friend std::ostream& operator<<(std::ostream& os, SearchState& ss) {
        std::string result_flag_str{};

        switch (ss.result_flag) {
            case NO_RESULT: result_flag_str = "No Result"; break;
            case WHITE_IS_MATED: result_flag_str = "Black Wins!"; break;
            case BLACK_IS_MATED: result_flag_str = "White Wins!"; break;
            case DRAW: result_flag_str = "Draw"; break;
            case STALEMATE: result_flag_str = "Stalemate"; break;
        }

        os << "SearchState:" << std::endl;
        os << "- result_flag: " << result_flag_str << std::endl;
        os << "  time_exit: " << ss.time_exit << std::endl;
        os << "  height: " << ss.height << std::endl;
#ifndef NDEBUG
        os << "  leaf_nodes: " << ss.leaf_nodes << std::endl;
        os << "  raw_nodes: " << ss.raw_nodes << std::endl;
        os << "  fail_high_count: " << ss.fail_high_count << std::endl;
        os << "  fail_high_first_count: " << ss.fail_high_first_count << std::endl;
        os << "  window_widen_count: " << ss.window_widen_count << std::endl;
        os << "  window_success_count: " << ss.window_success_count << std::endl;
#endif // NDEBUG
        os << ss.tt << std::endl;

        return os;
    }
};

namespace internal {
    std::tuple<ChessMove, Centipawns_t> search_root(Board &board, UCIOptions &options, SearchState &search_state, EvaluationState &eval_state, unsigned depth, Centipawns_t alpha, Centipawns_t beta);
    Centipawns_t search(Board &board, UCIOptions &options, SearchState &search_state, EvaluationState& eval_state, unsigned depth, Centipawns_t alpha, Centipawns_t beta, bool do_null);
    Centipawns_t q_search(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state, unsigned depth, Centipawns_t alpha, Centipawns_t beta);
    bool check_stop_search(unsigned depth, UCIOptions& options, SearchState& search_state);
    void order_moves(std::vector<ChessMove>& movelist, SearchState& search_state, ChessMove* hash_move = nullptr);
    std::size_t choose_book_move(const std::vector<BookEdge> book_moves);
};

bool is_move_legal(Board& board, ChessMove& move);
ChessMove think(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state, Book& book);
void perft(Board& board, Depth depth, int& leaf_nodes);

#endif // CLOAK_SEARCH_HPP
