//
// Created by Zach Bortoff on 2020-06-16.
//

#include "search.hpp"

/**
 * SearchState constructor: sets all member variables to default values
 */
SearchState::SearchState() : result_flag(Result::NO_RESULT)
#ifndef NDEBUG
, leaf_nodes(0), raw_nodes(0), fail_high_count(0), fail_high_first_count(0), window_widen_count(0), window_success_count(0)
#endif // NDEBUG
{

}

/**
 * resets all SearchState member variables to default values
 */
void SearchState::reset() {
    result_flag = Result::NO_RESULT;
#ifndef NDEBUG
    leaf_nodes = 0;
    raw_nodes = 0;
    fail_high_count = 0;
    fail_high_first_count = 0;
    window_widen_count = 0;
    window_success_count = 0;
#endif // NDEBUG
}

namespace internal {
    /**
     * searches all moves emanating from the root of the search tree
     * @param board        instance of Board object under search
     * @param options      instance of UCIOptions object informing exit conditions
     * @param search_state instance of SearchState object holding search data
     * @param eval_state   instance of EvaluationState object holding evaluation data
     * @param depth        the depth to which we want to search
     * @param alpha        the lower bound on the score in centipawns
     * @param beta         the upper bound on the score in centipawns
     * @return             the best move from the root and its score
     */
    std::tuple<ChessMove, Centipawns_t> search_root(Board &board, UCIOptions &options, SearchState &search_state, EvaluationState &eval_state, unsigned depth, Centipawns_t alpha, Centipawns_t beta) {
        /// If it's a draw from the root position, then just return a dummy and a score of 0, and change the result flag
        if (board.is_draw()) {
            search_state.result_flag = Result::DRAW;
            return std::tuple<ChessMove, Centipawns_t>(ChessMove(), 0);
        }

        /// initialize all the local variables at the top
        Centipawns_t init_alpha = alpha;
        std::vector<ChessMove> movelist;
        movelist.reserve(128); // reserve 128 for performance (should be less than that)
        ChessMove best_move;
        Centipawns_t root_score = -INF;

        /// generate all the moves
        gen_all_moves(board, movelist);
//        order_moves(movelist);

        for (int i = 0; i < movelist.size(); i++) {
            /// make the move, then determine if it's legal by playing it and seeing whether you put yourself in check
            board.make_move(movelist[i]);
            if (board.is_king_checked((!board.side_2_move()))) {
                movelist.erase(movelist.begin() + i);
                board.unmake_move();
                --i;
                continue;
            }

            /// in debug mode, it's useful to count raw nodes, so do so
#ifndef NDEBUG
            search_state.raw_nodes++;
#endif // NDEBUG

            /// Search the current position using negamax and null-move pruning
            root_score = -search(board, options, search_state, eval_state, depth - 1, -beta, -alpha, true);
            board.unmake_move();

            /// if the root score exceeds the current best move score or there's not best yet, then update those fields
            if (root_score > alpha || best_move == ChessMove()) {
                alpha = root_score;
                best_move = movelist[i];
            }
        }

        /// if there are no legal moves from the current position, then it's either a stalemate or a checkmate
        if (movelist.empty()) {
            Centipawns_t ret_score = 0;

            /// it's a checkmate if the king is checked
            if (board.is_king_checked(board.side_2_move())) {
                if (board.side_2_move() == WHITE) {
                    search_state.result_flag = WHITE_IS_MATED;
                    // ***** MAY NEED TO BE CHANGED TO 'INF' not '-INF'
                    ret_score = -INF;
                } else {
                    search_state.result_flag = BLACK_IS_MATED;
                    // ***** MAY NEED TO BE CHANGED TO 'INF' not '-INF'
                    ret_score = -INF;
                }
            } else {
                search_state.result_flag = STALEMATE;
            }

            return std::tuple<ChessMove, Centipawns_t>(ChessMove(), ret_score);
        }

        return std::tuple<ChessMove, Centipawns_t>(best_move, alpha);
    }

    /**
     * core alpha-beta search function
     * @param board        instance of Board object under search
     * @param options      instance of UCIOptions object informing exit conditions
     * @param search_state instance of SearchState object holding search data
     * @param eval_state   instance of EvaluationState object holding evaluation data
     * @param depth        the depth to which we want to search
     * @param alpha        the lower bound on the score in centipawns
     * @param beta         the upper bound on the score in centipawns
     * @param do_null      whether we ought to do null move pruning
     * @return             the score of the best position in centipawns
     */
    Centipawns_t search(Board &board, UCIOptions &options, SearchState &search_state, EvaluationState& eval_state, unsigned depth, Centipawns_t alpha, Centipawns_t beta, bool do_null) {
        /// if the current position is a draw, return 0 immediately
        if (board.is_draw()) {
            return 0;
        }

        /// if this is the last depth we want to search to, then perform the quiescence search
        if ((int)depth == 0) {
            return q_search(board, options, search_state, eval_state, MAX_DEPTH, alpha, beta);
        }

        /// if we've run out of time, then quit. By returning alpha, we don't give any new information
        if (internal::check_stop_search(depth, options, search_state)) {
            return alpha;
        }

        /// initialize local variables
        Centipawns_t init_alpha = alpha;
        Centipawns_t score = -INF;
        ChessMove best_move;
        std::vector<ChessMove> movelist;
        movelist.reserve(128); // performance

        /// generate all the moves from this position
        gen_all_moves(board, movelist);
//        order_moves(movelist);

        for (int i = 0; i < movelist.size(); i++) {
            /// for each move, determine its legality by playing it and seeing whether you put yourself in check
            board.make_move(movelist[i]);
            if (board.is_king_checked(!board.side_2_move())) {
                movelist.erase(movelist.begin() + i);
                board.unmake_move();
                --i;
                continue;
            }

#ifndef NDEBUG
            search_state.raw_nodes++;
#endif // NDEBUG

            /// recursively call this function, decrementing the depth and flipping the scores
            score = -search(board, options, search_state, eval_state, depth - 1, -beta, -alpha, true);
            board.unmake_move();

            /// if the score exceeds alpha (the best score found so far), then update the alpha value
            if (score > alpha) {
                /// if this score is better than or equal to beta, return immediately (beta-cutoff)
                if (score >= beta) {
#ifndef NDEBUG
                    if (i == 0) {
                        ++search_state.fail_high_first_count;
                    }
                    ++search_state.fail_high_count;
#endif // NDEBUG
                    return beta;
                }
                alpha = score;
                best_move = movelist[i];
            }
        }

        /// if no moves were found, it's either checkmate or stalemate
        if (movelist.empty()) {
            /// checkmate if the king is checked
            if (board.is_king_checked(board.side_2_move())) {
                if (board.side_2_move() == WHITE) {
                    search_state.result_flag = WHITE_IS_MATED;
                    // ***** MAY NEED TO BE CHANGED TO 'INF' not '-INF'
                    alpha = -INF + board.current_ply(); // this makes sure closer mates are prioritized
                } else {
                    search_state.result_flag = BLACK_IS_MATED;
                    // ***** MAY NEED TO BE CHANGED TO '-INF' not 'INF'
                    alpha = -INF + board.current_ply(); // this makes sure closer mates are prioritized
                }
            } else {
                search_state.result_flag = STALEMATE;
                alpha = 0;
            }
        }

        return alpha;
    }

    /**
     * quiescence search function to dampen score oscillation and check for exchances
     * @param board        instance of Board object under search
     * @param options      instance of UCIOptions object informing exit conditions
     * @param search_state instance of SearchState object holding search data
     * @param eval_state   instance of EvaluationState object holding evaluation data
     * @param depth        the depth to which we want to search
     * @param alpha        the lower bound on the score in centipawns
     * @param beta         the upper bound on the score in centipawns
     * @return             the score of the best position in centipawns
     */
    Centipawns_t q_search(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state, unsigned depth, Centipawns_t alpha, Centipawns_t beta) {
        /// initialize the local variables
        Centipawns_t score = -INF;
        Centipawns_t init_alpha = alpha;
        Centipawns_t standing_pat = evaluate(board, eval_state); // evaluate the current position

        /// if this a leaf node (can't go any further) or it's better than the cutoff score, return immediately
        if ((int)depth == 0 || standing_pat >= beta) {
#ifndef NDEBUG
            search_state.leaf_nodes++;
#endif // NDEBUG
            return standing_pat;
        } else if (standing_pat > alpha) {
            alpha = standing_pat; // update the best score with the current score, if the current score is better
        }

        ChessMove best_move;
        std::vector<ChessMove> movelist;
        movelist.reserve(128);

        /// only generate captures and promotions
        gen_all_caps(board, movelist);
//        order_moves(movelist);

        for (int i = 0; i < movelist.size(); i++) {
            /// make the move and test that it's legal
            board.make_move(movelist[i]);
            if (board.is_king_checked(!board.side_2_move())) {
                movelist.erase(movelist.begin() + i);
                board.unmake_move();
                --i;
                continue;
            }

#ifndef NDEBUG
            search_state.raw_nodes++;
#endif // NDEBUG

            /// recursively search and return to the original position
            score = -q_search(board, options, search_state, eval_state, depth - 1, -beta, -alpha);
            board.unmake_move();

            /// update the score if it's better than alpha, and return immediately if it's better than beta
            if (score > alpha) {
                if (score >= beta) {
#ifndef NDEBUG
                    if (i == 0) {
                        ++search_state.fail_high_first_count;
                    }
                    ++search_state.fail_high_count;
#endif // NDEBUG
                    return beta;
                }

                best_move = movelist[i];
                alpha = score;
            }
        }

        /// if the movelist if empty i.e. there are no captures or whatever, just return the standing pat
        if (movelist.empty()) {
            return standing_pat;
        }

        return alpha;
    }

    /**
     * checks all exit conditions for search to determine whether or not to stop searching
     * @param depth        the depth that we have reached
     * @param options      instance of UCIOptions object informing exit conditions
     * @param search_state instance of SearchState object holding search data
     * @return             true means stop searching, false means don't stop searching
     */
    bool check_stop_search(unsigned depth, UCIOptions& options, SearchState& search_state) {
        /// If the depth we've reached exceeds the max depth, then stop the search
        if (depth > MAX_DEPTH) {
            return true;
        }

        /// If the user has asked us to stop, then stop
#ifndef WIND32
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fileno(stdin), &readfds);
        struct timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        select(16, &readfds, 0, 0, &tv);

        bool ret = (FD_ISSET(fileno(stdin), &readfds));
        int bytes;
        char input[256] = "", *endc;

        if (ret) {
            do {
                bytes = read(fileno(stdin), input, 256);
            } while (bytes < 0);
            endc = strchr(input, '\n');
            if (endc) {
                *endc = 0;
            }
            if (strlen(input)> 0) {
                if (!strncmp(input, "stop", 4)) {
                    options.infinite = false;
                    return true;
                }
            }
        }
#endif // NOT WINDOWS

        /// If the engine is pondering or in an infinite search, then don't stop the search
        if (options.infinite || options.ponder) {
            return false;
        }

        /// If it's sudden death, and we've run outta time, then stop the search
        if (options.sudden_death && search_state.clock.has_exceeded_time()) {
            return true;
        }

        /// If we're supposed to search to a specific depth, and we've reached that depth, then stop the search
        if (options.search_to_depth_x == (int)depth) {
            return true;
        }

        /// If the amount of time that's transpired exceeds the amount of time for the search, then stop
        if (options.move_time != -1) {
            return (int) (search_state.clock.duration() / 1000) >= options.move_time;
        }

        return false;

    }

    void order_moves(std::vector<ChessMove>& movelist, ChessMove* hash_move) {
        MoveScore score = 0;

        for (int i = 0; i < movelist.size(); i++) {
            score += movelist[i].score;

            if (hash_move != nullptr && *hash_move == movelist[i]) {
                score += 128; // |= (1 << 7)
            }
        }

        std::sort(movelist.begin(), movelist.end());
    }
}

/**
 * determines whether a given move in a certain position is legal or not
 * @param board     instance of Board object under search
 * @param chessmove instance of ChessMove object being scrutinized
 * @return          true if the move is legal, false if the move is illegal
 */
bool is_move_legal(Board& board, ChessMove& chessmove) {
    /// create the movelist and reserve 128 moves
    std::vector<ChessMove> movelist;
    movelist.reserve(128);

    /// generate all moves
    gen_all_moves(board, movelist);

    for (int i = 0; i < movelist.size(); i++) {
        /// iterate through the moves and if a legal move equals the given move, return true
        board.make_move(movelist[i]);
        if(!board.is_king_checked(!board.side_2_move())) {
            if (chessmove == movelist[i]) {
                board.unmake_move();
                return true;
            }
        }

        board.unmake_move();
    }
    return false;
}

/**
 * wrapper for breadth-first search function
 * @param board        instance of Board object under search
 * @param options      instance of UCIOptions object informing exit conditions
 * @param search_state instance of SearchState object holding search information
 * @param eval_state   instance of EvaluationState object containing evaluation data
 * @return             the best ChessMove from the current position
 */
ChessMove think(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state) {
    ChessMove best_move;
    Centipawns_t score{0};
    search_state.reset();
//    Centipawns_t upper_window_index{0};
//    Centipawns_t lower_window_index{0};
    Centipawns_t alpha{-INF};
    Centipawns_t beta{INF};

    search_state.clock.alloc_time(options, board.side_2_move());
    search_state.clock.start();

    /// Starting at a depth of 1, perform a depth first search
    for (unsigned depth = 1; depth < MAX_DEPTH; depth++, alpha = -INF, beta = INF) {
        /// think and extract the best move and the corresponding scre
        std::tuple<ChessMove, Centipawns_t> tup = internal::search_root(board, options, search_state, eval_state, depth, alpha, beta);
        best_move = std::get<0>(tup);
        score = std::get<1>(tup);

        /// Get a split from the clock and compute the time elapsed in milliseconds
        search_state.clock.stop();
        Milliseconds_t time_elapsed = search_state.clock.duration() / 1000000;

        /// print search information
        std::cout << "info score cp " << score << " time " << time_elapsed << " depth " << depth <<
#ifndef NDEBUG
            " raw_nodes " << search_state.raw_nodes <<
            " leaf_nodes " << search_state.leaf_nodes <<
            " nps " << (double)(search_state.raw_nodes / (time_elapsed / 1000.)) <<
            " ordering " << search_state.ordering() <<
            " window_ratio " << search_state.window_ratio() <<
#endif // NDEBUG
            std::endl;

        /// if we've run out of time or the user has said "stop" or whatever, then break out of the loop and just return
        if (internal::check_stop_search(depth, options, search_state)) {
            break;
        }
    }

    return best_move;
}

/**
 * performs perft search for perft testing
 * @param board      instance of Board object under search
 * @param depth      depth to which we want to search
 * @param leaf_nodes the number of leaf nodes that have been searched so far
 */
void perft(Board& board, Depth depth, int& leaf_nodes) {
    if (!(int)depth) {
        ++leaf_nodes;
        return;
    }

    std::vector<ChessMove> movelist;
    movelist.reserve(128);
    gen_all_moves(board, movelist);

    /// loop through the moves at the current depth
    for (auto & i : movelist) {
        board.make_move(i);

        if(!board.is_king_checked(!board.side_2_move())) {
            perft(board, depth - 1, leaf_nodes);
        }

        board.unmake_move();
    }
}

