#include <sys/select.h>
#include <libc.h>
#include "search.hpp"

/**
 * sets all member variables to nominal values
 */
SearchState::SearchState(int tt_size) : result_flag(Result::NO_RESULT), tt(tt_size), time_exit(false), height(0), killer_move({}), history_heuristic({{}})
#ifndef NDEBUG
, leaf_nodes(0), raw_nodes(0), fail_high_count(0), fail_high_first_count(0), window_widen_count(0), window_success_count(0)
#endif // NDEBUG
{
#ifdef USE_SSE
    init_sliding_attacks();
#endif // NDEBUG
}

/**
 * resets all SearchState member variables to nominal values
 */
void SearchState::reset() {
    result_flag = Result::NO_RESULT;
    time_exit = false;
    height = 0;
    killer_move = {};
    history_heuristic = {{}};
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

        /// Try to reconstruct the best move from hash table
        ChessHash* root_hash = search_state.tt.find(board.key()); // attempt to find root position's hash in table for move ordering purposes only
        ChessMove* hash_move = nullptr;
        if (root_hash != nullptr) {
            PieceType_t moved = board.piece_type(root_hash->from_sq);
            PieceType_t captured = PieceType::NO_PIECE;
            if (root_hash->m_flag() & CAPTURE_MOVE) {
                if (root_hash->m_flag() == ENPASSANT) {
                    captured = W_PAWN + !board.side_2_move();
                } else {
                    captured = board.piece_type(root_hash->to_sq);
                }
            }

            // place higher up in if-statement
            if (root_hash->key != 0) {
                hash_move = new ChessMove(root_hash->from_sq, root_hash->to_sq, root_hash->m_flag(), moved, captured); // 'new' key word. WATCH OUT!
            }
        }

        /// generate all the moves
        gen_all_moves(board, movelist);
        order_moves(movelist, search_state, board, hash_move);
        if (hash_move != nullptr) {
            delete hash_move; // nice deletion, mate!
            hash_move = nullptr;
        }

        for (long unsigned int i = 0; i < movelist.size(); i++) {
            // make the move, then determine if it's legal by playing it and seeing whether you put yourself in check
            board.make_move(movelist[i]);
            if (board.is_king_checked((!board.side_2_move()))) {
                movelist.erase(movelist.begin() + i);
                board.unmake_move();
                --i;
                continue;
            }
            ++search_state.height;

            /// in debug mode, it's useful to count raw nodes, so do so
#ifndef NDEBUG
            search_state.raw_nodes++;
#endif // NDEBUG

            /// Search the current position using negamax and null-move pruning
            root_score = -search(board, options, search_state, eval_state, depth - 1, -beta, -alpha, true);
            board.unmake_move();
            --search_state.height;

            if (check_stop_search(depth, options, search_state)) {
                return std::tuple<ChessMove, Centipawns_t>(best_move, alpha);
            }

            /// if the root score exceeds the current best move score or there's not best yet, then update those fields
            if (root_score > alpha) {
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
                    ret_score = -INF;
                } else {
                    search_state.result_flag = BLACK_IS_MATED;
                    ret_score = -INF;
                }
            } else {
                search_state.result_flag = STALEMATE;
            }

            return std::tuple<ChessMove, Centipawns_t>(ChessMove(), ret_score);
        }

        if (root_score == INF - 2) {
            if (board.side_2_move() == WHITE) {
                search_state.result_flag = BLACK_IS_MATED;
            } else {
                search_state.result_flag = WHITE_IS_MATED;
            }
        }

        if (alpha != init_alpha) {
            ChessHash pv_hash(board.key(), root_score, best_move.from_sq, best_move.to_sq, best_move.flag(), depth, EXACT, board.current_ply());
            search_state.tt.insert(pv_hash);
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

        Centipawns_t init_alpha = alpha;
        Centipawns_t score = -INF;
        Depth num_extensions = 0;
        Depth R = compute_adap_null_move_r(depth);

        /// Check the hash table for extra information
        ChessHash* hash = search_state.tt.find(board.key());
        HashFlag flag = NO_INFO;
        Centipawns_t hash_score;
        ChessMove* hash_move = nullptr;
        if (hash != nullptr) {
            flag = hash->hash_flag;
            hash_score = hash->score;

            /// this segment causes the SearchTest to fail
            if (std::abs(hash_score) > INF - 100) {
                if (hash_score > 0) {
                    hash_score -= search_state.height;
                } else {
                    hash_score += search_state.height;
                }
            }

            /// forget why these are the conditions
//            if (hash->depth >= (depth - R) && (hash_score < beta) && (flag == STORED_ALPHA)) {
////            if (flag == AVOID_NULL && hash->depth >= depth && search_state.height) {
//                do_null = false;
//                flag = AVOID_NULL;
//            }

            /// if the hashed move went deeper than we currently are in the search, then it should be adhered to
            if (hash->depth >= depth) {
                switch (flag) {
                    case STORED_ALPHA:
                        if (hash_score <= alpha) {
                            // the beta passed into this function has a tighter bound, so don't use the hash table one
                            return alpha;
                        }
                        break;
                    case STORED_BETA:
                        if (hash_score >= beta) {
                            // the beta passed into this function has a tighter bound, so don't use the hash table one
                            return beta;
                        }
                        break;
                    case EXACT: return hash_score;
                }
            }

//            score = hash_score; // alpha = hash_score
        }

        /// Check Extensions
        if (board.is_king_checked(board.side_2_move())) {
            if (depth < MAX_DEPTH) { // for stability reasons, we can never exceed MAX_DEPTH
                ++num_extensions;
            }
        } else if (do_null) {  // do null move pruning
            internal::def_stage(board, eval_state);
//            if (!(eval_state.stage & 2) && depth > R) {
            if (eval_state.stage != LATE_END_GAME && depth > R && search_state.height >= 1) {
                board.make_null_move();
                Centipawns_t null_move_score = -search(board, options, search_state, eval_state, depth - R - 1, -beta, -beta + 1, false);
                board.unmake_null_move(); // board.unmake_null_move();

                // if I have a remarkable position despite the null move, just return current upper bound
                if (null_move_score >= beta) {
                    return beta;
                }

                // if I die when I null-move, then something is amiss. let's look into this position a bit more
                if (null_move_score < -INF + 100) {
                    ++num_extensions;
                }
            }
        }

        /// if this is the last depth we want to search to, then perform the quiescence search
        if (depth + num_extensions == 0) {
            return q_search(board, options, search_state, eval_state, MAX_DEPTH, alpha, beta);
        }

        /// initialize local variables
        ChessMove best_move;
        std::vector<ChessMove> movelist;
        movelist.reserve(128); // performance

        if (hash != nullptr) {
            /// if we fail low (alpha cutoff) then we will store a dummy move in the hashtable causing moved==captured
            /// if this is the case, don't create a ChessMove pointer just keep going.
            if (hash->from_sq != hash->to_sq) {
                PieceType_t moved = board.piece_type(hash->from_sq);
                PieceType_t captured = PieceType::NO_PIECE;
                if (hash->m_flag() & CAPTURE_MOVE) {
                    if (hash->m_flag() == ENPASSANT) {
                        captured = W_PAWN + !board.side_2_move();
                    } else {
                        captured = board.piece_type(hash->to_sq);
                    }
                }

                hash_move = new ChessMove(hash->from_sq, hash->to_sq, hash->m_flag(), moved, captured);
//                best_move = ChessMove(hash->from_sq, hash->to_sq, hash->m_flag(), moved, captured);
            }
        }

        /// generate all the moves from this position
        gen_all_moves(board, movelist);
        order_moves(movelist, search_state, board, hash_move);
//        order_moves(movelist, search_state, board, &best_move);
        if (hash_move != nullptr) {
            delete hash_move;
            hash_move = nullptr;
        }

        for (std::size_t i = 0; i < movelist.size(); ++i) {
            /// for each move, determine its legality by playing it and seeing whether you put yourself in check
            board.make_move(movelist[i]);
            if (board.is_king_checked(!board.side_2_move())) {
                movelist.erase(movelist.begin() + i);
                board.unmake_move();
                --i;
                continue;
            }
            ++search_state.height;
#ifndef NDEBUG
            search_state.raw_nodes++;
#endif // NDEBUG

            /// recursively call this function, decrementing the depth and flipping the scores
            score = -search(board, options, search_state, eval_state, depth - 1 + num_extensions, -beta, -alpha, true);
            board.unmake_move();
            --search_state.height;

            /// if we've run out of time, then quit. By returning alpha, we don't give any new information
            if (internal::check_stop_search(depth, options, search_state)) {
#ifndef NDEBUG
                ++search_state.leaf_nodes;
#endif // NDEBUG
                return alpha; // return alpha?
            }

            /// if the score exceeds alpha (the best score found so far), then update the alpha value
            if (score > alpha) {
                /// if this score is better than or equal to beta, return immediately (beta-cutoff)
                if (score >= beta) {
#ifndef NDEBUG
                    if (i == 0) {
                        ++search_state.fail_high_first_count;
                    }
                    ++search_state.fail_high_count;
                    ++search_state.leaf_nodes;
#endif // NDEBUG

                    /// store non-capturing beta-cutoffs
                    if (!(movelist[i].flag() & CAPTURE_MOVE)) {
                        search_state.killer_move[search_state.height].second = search_state.killer_move[search_state.height].first;
                        search_state.killer_move[search_state.height].first = movelist[i];
                        search_state.history_heuristic[movelist[i].moved][movelist[i].to_sq] += static_cast<Centipawns_t>(depth * depth); // depth^2
                    }

                    ChessHash upper_bnd_hash(board.key(), beta, movelist[i].from_sq, movelist[i].to_sq, movelist[i].flag(), depth, STORED_BETA, board.current_ply()); // could store beta, or could store score
                    if (std::abs(beta) > INF - 100) {
                        if (beta > 0) {
                            upper_bnd_hash.score += search_state.height;
                        } else {
                            upper_bnd_hash.score -= search_state.height;
                        }
                    }
                    search_state.tt.insert(upper_bnd_hash);
                    return beta;
                }

//                /// store non-capturing alpha cutoffs
//                if (!(movelist[i].flag() & CAPTURE_MOVE)) {
//                    search_state.history_heuristic[movelist[i].moved][movelist[i].to_sq] += static_cast<Centipawns_t>(depth * depth); // depth^2
//                }

                alpha = score;
                best_move = movelist[i];
            }
        }

        /// if no moves were found, it's either checkmate or stalemate
        if (movelist.empty()) {
            /// checkmate if the king is checked
            if (board.is_king_checked(board.side_2_move())) {
                alpha = -INF + search_state.height; // this makes sure closer mates are prioritized
            } else {
                alpha = 0;
            }
        }

        if (alpha > init_alpha) {
            ChessHash exact_hash(board.key(), alpha, best_move.from_sq, best_move.to_sq, best_move.flag(), depth, EXACT, board.current_ply());
            if (std::abs(alpha) > INF - 100) {
                if (beta > 0) {
                    exact_hash.score += search_state.height;
                } else {
                    exact_hash.score -= search_state.height;
                }
            }
            search_state.tt.insert(exact_hash);
        } else {
            /// store a dummy in the transposition table because we don't know the actual move
            ChessHash lower_bnd_hash(board.key(), alpha, best_move.from_sq, best_move.to_sq, best_move.flag(), depth, STORED_ALPHA, board.current_ply());
            if (std::abs(alpha) > INF - 100) {
                if (beta > 0) {
                    lower_bnd_hash.score += search_state.height;
                } else {
                    lower_bnd_hash.score -= search_state.height;
                }
            }
            search_state.tt.insert(lower_bnd_hash);
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
        // check to see if we have run out of time
        if (((unsigned)search_state.raw_nodes & 127) == 0) { // every 127 might be a little extreme? Test moving in further
            internal::check_stop_search(depth, options, search_state);
        }
        search_state.raw_nodes++;

        /// initialize the local variables
        Centipawns_t score = -INF;
        Centipawns_t init_alpha = alpha;
        Centipawns_t standing_pat = evaluate(board, eval_state); // evaluate the current position
        assert(alpha < beta);

        if (board.is_draw()) {
//            SPDLOG_LOGGER_INFO(spdlog::get(logger_name), "Quitting q-search cuz it's a draw!");
#ifndef NDEBUG
            search_state.leaf_nodes++;
#endif // NDEBUG
            return 0;
        }

        if (depth == 0) {
//            SPDLOG_LOGGER_INFO(spdlog::get(logger_name), "Quitting q-search cuz we reached the depth limit");
            return standing_pat;
        }

        if (standing_pat >= beta) {
#ifndef NDEBUG
            search_state.leaf_nodes++;
#endif // NDEBUG
            return beta;
        }

        // Delta pruning: If the standing_pat + HUGE SCORE (like value of queen and called "delta") is still worse than alpha, then return alpha,
        // because if the standing_pat + delta < alpha, then the evaluation is so much worse than what's already available,
        // that there's no point in looking for a quieter position. Don't do it during the late game though, where there
        // could be huge fluctuations in the evaluation anyway due to mating and stuff like that
        if (eval_state.stage != LATE_END_GAME) {
            Centipawns_t delta = QUEEN_BASE_VAL;
//            if ((((board.piece_bb(W_PAWN) & RANK_MASK[RANK7]) << 8) & ~board.piece_bb(B_PIECES)) || (((board.piece_bb(B_PAWN) & RANK_MASK[RANK2]) >> 8) & ~board.piece_bb(W_PIECES))) {
//                delta += 100; // idk some value
//            }

            if (standing_pat + delta < alpha) {
#ifndef NDEBUG
                search_state.leaf_nodes++;
#endif // NDEBUG
                return alpha;
            }
        }

        // update the current best score with standing pat if the standing pat is better than the best score
        if (standing_pat > alpha) {
            alpha = standing_pat;
        }

        // if we have run out of time to search, then just quit.
        if (search_state.time_exit) {
            return 0;
        }

        ChessMove best_move;
        std::vector<ChessMove> movelist;
        movelist.reserve(128);

        /// only generate captures and promotions
        gen_all_caps(board, movelist); // captures & promotions
        order_qmoves(movelist, search_state, board);

        for (unsigned i = 0; i < movelist.size(); i++) {
            // make the move and test that it's legal
            board.make_move(movelist[i]);
            if (board.is_king_checked(!board.side_2_move())) {
                movelist.erase(movelist.begin() + i);
                board.unmake_move();
                --i;
                continue;
            }
            ++search_state.height;

            /// recursively search and return to the original position
            score = -q_search(board, options, search_state, eval_state, depth - 1, -beta, -alpha);
            board.unmake_move();
            --search_state.height;

            /// update the score if it's better than alpha, and return immediately if it's better than beta
            if (score > alpha) {
                if (score >= beta) {
#ifndef NDEBUG
                    if (i == 0) {
                        ++search_state.fail_high_first_count;
                    }
                    ++search_state.fail_high_count;
                    ++search_state.leaf_nodes;
#endif // NDEBUG
                    // don't store in transposition table because we don't have any search depth
                    return beta;
                }

                best_move = movelist[i];
                alpha = score;
            }
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
        if (search_state.height < MIN_DEPTH && depth < MIN_DEPTH) {
            return false;
        }

        /// If the depth we've reached exceeds the max depth, then stop the search
        if (depth > MAX_DEPTH) {
            search_state.time_exit = true;
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
        select(16, &readfds, nullptr, nullptr, &tv);

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
                    search_state.time_exit = true;
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
        if (search_state.clock.has_exceeded_time()) {
            search_state.time_exit = true;
            return true;
        }

        /// If we're supposed to search to a specific depth, and we've reached that depth, then stop the search
        if (options.search_to_depth_x == (int)depth) {
            search_state.time_exit = true;
            return true;
        }

        /// If the amount of time that's transpired exceeds the amount of time for the search, then stop
//        if (options.search_for_time_x != -1) {
//            search_state.clock.stop();
//            bool should_exit = search_state.clock.duration() / 1000000 > (unsigned long long)options.search_for_time_x;
//            search_state.time_exit = should_exit;
//            return should_exit;
//        }

        return false;
    }

    void order_moves(std::vector<ChessMove>& movelist, SearchState& search_state, Board& board, ChessMove* hash_move) {
        MoveScore score = 0;

        for (auto & move : movelist) {
            score = move.score; // MVV-LVA

            if (hash_move != nullptr) {
                if (move == *hash_move) {
                    score |= (1 << 31); // |= (1 << 7) (4294967295)
                }
            } else {
                if (move.flag() == CAPTURE_MOVE) { // maybe exclude promotions later
                    auto see = board.see(move.to_sq, move.captured, move.from_sq, move.moved);
                    if (see >= 0) { // only reject losing moves, not winning or tying moves
                        score += see + 100000; // put 100000 in globals
                    }
                } else {
                    if (move == search_state.killer_move[search_state.height].first) { // maybe rendered more effective if we index by PLY (board.current_ply())?
                        score += 90000; // put 90000 in globals
                    } else if (move == search_state.killer_move[search_state.height].second) {
                        score += 80000; // put 80000 in globals
                    }
//                    } else {
                    // depth = 15 => 15 * 15 = 225 => 225,000 -> tune later!
                        score += 1000 * search_state.history_heuristic[move.moved][move.to_sq]; // hmm... so we are always adding this no matter what? the element better be 0 by default
//                    }
                }
            }

            move.score = score;
//            score = 0;
        }

        std::sort(movelist.begin(), movelist.end());
#ifndef NDEBUG
        if (!movelist.empty() && hash_move != nullptr) {
            ASSERT(movelist[0] == *hash_move, "in order_moves() the hash move is not the first move");
        }
#endif // NDEBUG
    }


    /**
     * ordering qmoves is akin to ordering regular moves only we don't check the hash table, so we don't pass a hash
     * move in the function, and if the see is < 0, we don't just order it last, we don't search that position at all
     * @param movelist     the list of pseudo-legal moves
     * @param search_state contains meta-data about the search (like killers and history stuff)
     * @param board        contains data about the position (SEE function)
     */
    void order_qmoves(std::vector<ChessMove>& movelist, SearchState& search_state, Board& board) {
        MoveScore score = 0;

        for (unsigned i = 0; i < movelist.size(); ++i) {
            ASSERT(i < movelist.size(), "i > movelist.size()");
            ChessMove move = movelist[i];
//            score = move.score;

            // all qmoves are captures, no need checking whether it is
            ASSERT(&board != nullptr, "Board isn't defined!");
            ASSERT(&move != nullptr, "Move isn't defined!");

            if (move.captured != PieceType::NO_PIECE) {
                Centipawns_t see = board.see(move.to_sq, move.captured, move.from_sq, move.moved);
                if (see > 0) {
                    score += see + 100000; // put 100000 in globals (just added the >= condition from ==)
                } else {
                    /// don't consider losing captures in qsearch
                    movelist.erase(movelist.begin() + i);
                    --i;
                    continue;
                }
            }

            movelist[i].score = score;
            score = 0;
        }

        std::sort(movelist.begin(), movelist.end());
    }

    std::size_t choose_book_move(const std::vector<BookEdge>& book_moves) {
        // for now we don't look at the moves themselves. Later on we will
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<std::size_t> distribution(0, book_moves.size() - 1);
        return distribution(gen);
    }

    Depth compute_adap_null_move_r(Depth depth) {
        return NULL_MOVE_R;
//        return NULL_MOVE_R + (depth > 6) + (depth > 11) + (depth > 16) + (depth > 21);
    }
};

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

    for (long unsigned int i = 0; i < movelist.size(); i++) { // NOLINT
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
ChessMove think(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state, Book& book) {
    ChessMove best_move;
    Centipawns_t score{0};
    search_state.reset();
//    Centipawns_t upper_window_index{0};
//    Centipawns_t lower_window_index{0};
    Centipawns_t alpha{-INF};
    Centipawns_t beta{INF};

    // try to play a book move
    std::vector<BookEdge> book_moves = book.edges(board.key());
    if (!book_moves.empty()) {
        std::size_t index = internal::choose_book_move(book_moves);
        book.make_move(index);
        return book_moves[index].move;
    }

    search_state.clock.alloc_time(options, board.side_2_move());
    search_state.clock.start();

    /// Starting at a depth of 1, perform a depth first search
    for (unsigned depth = 1; depth < MAX_DEPTH; depth++, alpha = -INF, beta = INF) {
        /// think and extract the best move and the corresponding score
        std::tuple<ChessMove, Centipawns_t> tup = internal::search_root(board, options, search_state, eval_state, depth, alpha, beta);
        best_move = std::get<0>(tup);
        score = std::get<1>(tup);

        /// Get a split from the clock and compute the time elapsed in milliseconds
        search_state.clock.stop();
        Milliseconds_t time_elapsed = search_state.clock.duration() / 1000000;
#ifndef NDEBUG
        std::string game_stage_str{};
        switch (eval_state.stage) {
            case OPENING:
                game_stage_str = "opening";
                break;
            case MID_GAME:
                game_stage_str = "middle";
                break;
            case EARLY_END_GAME:
                game_stage_str = "early_end";
                break;
            case LATE_END_GAME:
                game_stage_str = "late_end";
                break;
        }

        std::string pos_type_str{};
        switch (eval_state.pos_type) {
            case CLOSED:
                pos_type_str = "closed";
                break;
            case SEMI_CLOSED:
                pos_type_str = "semi_closed";
                break;
            case SEMI_OPEN:
                pos_type_str = "semi_open";
                break;
            case OPEN:
                pos_type_str = "open";
                break;
        }
#endif // NDEBUG

        /// Move through the principal variation by making the next EXACT move in the hash table
        if (search_state.result_flag == Result::NO_RESULT) {
            // two cases: no result and we have an answer, because we finished the search or no result and we don't have
            // an answer, because we were stopped short of the search
            if (best_move == ChessMove()) {
                // no answer case: just return and don't print anything, just grab the previous depth's answer if it exists
                // - NOTE: this happens if we haven't even finished searching the next additional depth given the previous
                //         search's PV before we get a timeout or something, so we bubble up immediately with alpha values,
                //         corresponding to INF, which when they get to the search_root function never override best_move = ChessMove()
                //         since alpha hasn't been improved. The best way to fix this is just to improve searching, I think, so that we
                //         compute the PV really quickly, so this doesn't happen at move like 5/6 but at move like 20/21, where that's
                //         less significant

                ChessHash* prev_best_move = search_state.tt.find(board.key());
                if (prev_best_move != nullptr) {
                    assert(prev_best_move->hash_flag == EXACT);

                    PieceType_t moved = board.piece_type(prev_best_move->from_sq);
                    PieceType_t captured = PieceType::NO_PIECE;
                    if (prev_best_move->m_flag() & CAPTURE_MOVE) {
                        if (prev_best_move->m_flag() == ENPASSANT) {
                            captured = W_PAWN + !board.side_2_move();
                        } else {
                            captured = board.piece_type(prev_best_move->to_sq);
                        }
                    }

                    best_move =  ChessMove(prev_best_move->from_sq, prev_best_move->to_sq, prev_best_move->m_flag(), moved, captured);
                    return best_move;
                } else {
                    // in this case, we ran out of time searching the first PV at the first depth. this shouldn't happen unless you do something like
                    // "go movetime 1" or something
                    SPDLOG_LOGGER_INFO(spdlog::get(logger_name), "Previous Depth's Answer Doesn't exist");
                    return ChessMove();
                }
            }
        } else {
            // we reach this point if the search_root function found a mate or draw, meaning we are currently in a mate
            // or draw position. So we shouldn't output any info, we should just return nothing
            return ChessMove();
        }

        // we reach this point if, it's an active position (game hasn't ended due to mate/draw/stalemate) and if we
        // finished searching at least the previous search's PV up to the current depth if not finished the whole search
        // for this depth. In this case, just output the complete response!
        std::string pv_string{best_move.to_string()};
        ChessMove next_pv_move = best_move;
        int move_counter = 0;
        for (unsigned i = 0; i < depth - 1; i++) {
            board.make_move(next_pv_move);
            ++move_counter;
            ChessHash* next_move_hash = search_state.tt.find(board.key());

            if (next_move_hash != nullptr) {
                if (next_move_hash->hash_flag == EXACT) {
                    PieceType_t moved = board.piece_type(next_move_hash->from_sq);
                    PieceType_t captured = PieceType::NO_PIECE;
                    if (next_move_hash->m_flag() & CAPTURE_MOVE) {
                        if (next_move_hash->m_flag() == ENPASSANT) {
                            captured = W_PAWN + !board.side_2_move();
                        } else {
                            captured = board.piece_type(next_move_hash->to_sq);
                        }
                    }

                    /// this can happen for reasons that are totally reasonable that I can't remember the specifics of
                    if (next_move_hash->from_sq != next_move_hash->to_sq) {
                        next_pv_move = ChessMove(next_move_hash->from_sq, next_move_hash->to_sq, next_move_hash->m_flag(), moved, captured);
                        pv_string += " " + next_pv_move.to_string();
                    }
                } else {
                    // we reach this space if the position pointed to by the PV up until this point isn't EXACT, but it
                    // is the right board (maybe check to see if the depth is equal to the depth of the search to see if
                    // the thing in the table was put there by this search and not the previous search)
                    ASSERT(depth - i == next_move_hash->depth, "Next move hash in PV is not EXACT and the depths don't match, but it's right position"); // if this is triggered
                    SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "Next Move Hash in PV is not EXACT, i = {}", i);
                    break; // could happen if something has overwritten something else (shouldn't happen)
                }
            } else {
                // we reach this space if the position pointed to by the PV doesn't correspond with the current position,
                // This happens when there is a Zobrist hash collision
                SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "Zobrist Hash Collision: Next Move Hash in PV has been overwritten, i = {}", i);
                break;
            }
        }

        /// back out of the principal variation
        for (int i = 0; i < move_counter; i++) {
            board.unmake_move();
        }

        /// print search information
        std::cout << "info score cp " << score << " time " << time_elapsed << " depth " << depth << " pv " << pv_string <<
#ifndef NDEBUG
            " raw_nodes " << search_state.raw_nodes <<
            " leaf_nodes " << search_state.leaf_nodes <<
//            " nps " << (double)(search_state.raw_nodes / (time_elapsed / 1000.)) <<
            " ordering " << search_state.ordering() <<
//            " window_ratio " << search_state.window_ratio() <<
//            " position_type " << pos_type_str <<
//            " game_stage " << game_stage_str <<
            " load_factor " << static_cast<double>(search_state.tt.load_factor()) <<
            " hit_rate " << static_cast<double>(search_state.tt.hit_rate()) <<
            " overwrite_percentage " << static_cast<double>(search_state.tt.overwrite_percentage()) <<
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
