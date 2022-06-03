#include <sys/select.h>
#include <libc.h>
#include "search.hpp"

/**
 * basic constructor for SearchState object. sets all member variables to nominal values
 */
SearchState::SearchState(int tt_size) : result_flag(Result::NO_RESULT), tt(tt_size), time_exit(false), height(0), killer_move({}), history_heuristic({{}}), raw_nodes(0)
#ifndef NDEBUG
, leaf_nodes(0), fail_high_count(0), fail_high_first_count(0), window_widen_count(0), window_success_count(0)
#endif // NDEBUG
{
#ifdef USE_SSE
    // if we are using SSE instruction sets, then call this function to initialize the sliding attacks.
    init_sliding_attacks();
#endif // USE_SSE
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
    raw_nodes = 0;
#ifndef NDEBUG
    leaf_nodes = 0;
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
        // for debugging purposes, it's useful to count the number of nodes the engine has visited.
        ++search_state.raw_nodes; // this counts the root node as 1!

        // If it's a draw from the root position, then just return a dummy and a score of 0, and change the result flag
        if (board.is_draw()) {
            search_state.result_flag = Result::DRAW; // why didn't we find anything? because it's a DRAW
            return {ChessMove(), 0};
        }

        // initialize all the local variables at the top
        Centipawns_t init_alpha = alpha;
        std::vector<ChessMove> movelist;
        movelist.reserve(128); // reserve 128 for performance (should be less than that)
        ChessMove best_move;
        Centipawns_t root_score = -INF;

        // Try to reconstruct the best move from hash table
        ChessHash* root_hash = search_state.tt.find(board.key()); // attempt to find root position's hash in the TT
        ChessMove* hash_move = nullptr; // will be set to non-nullptr if the root_hash has good info in it...
        if (root_hash != nullptr) {
            PieceType_t moved = board.piece_type(root_hash->from_sq);
            PieceType_t captured = PieceType::NO_PIECE;
            if (root_hash->m_flag() & CAPTURE_MOVE) {
                if (root_hash->m_flag() == ENPASSANT) {
                    // if the move in the TT is an EN PASSANT capture, then
                    captured = W_PAWN + !board.side_2_move(); // set the captured piece type flag correspondingly
                } else {
                    // if it's a capture move but not an EN PASSANT, then set it to whatever piece IS at the to-sq
                    captured = board.piece_type(root_hash->to_sq);
                }
            }

            // the "root_hash->hash_flag != STORED_ALPHA" is meant to capture a weird case of us putting a dummy fake
            // move into the PV just with the lower bound alpha corresponding to that position without knowing what the
            // position is. Really, we should just be checking to see if root_hash-> chess move is a dummy move not a
            // STORED_ALPHA, but STORED_ALPHA is the only time (hopefully) we should have a dummy move in the PV
            if (root_hash->key != 0) { // could do root_hash->key == board.key()???
                if (root_hash->from_sq == root_hash->to_sq) {
                    // if the "best_move" stored in the TT is a dummy move, but somehow has a key attached to it, then
                    // it must be the case that we have stored a STORED_ALPHA hash flag. If that's not the case, then
                    // crash the program. However, if it is the case, then that indicates that in a previous search of
                    // the tree, we came across this position before. We performed a search from this position up to
                    // some depth 'D' having already been informed by the search of a sister node (or something) that
                    // the 'alpha' for the search on this position was something like '32' (i.e. not '-INF'). In
                    // returning from the search on this position, we found that after reach depth 'D' NO possible
                    // combination of moves improves on the '32' score for us. Meaning, all the moves from this position
                    // have scores < '32'. Therefore, in future searches, we shouldn't pursue this branch, because we
                    // know already that there is an alternative sister (or something) move that precludes considering
                    // this position altogether. That is, of course, assuming the depth we are searching for in the
                    // future is less than or equal to the depth we searched to when we made the TT entry. All that
                    // being said, *I THINK* if we encounter this case (however unlikely), we should NOT increment
                    // alpha, even though, we have evidence that alpha should be increased. The reason is that, this is
                    // the ROOT of the tree now. Therefore, it's not as if we could avoid this position by taking an
                    // alternative route, because this is our current position. The best we can do from a software
                    // engineering perspective is make sure our assumption that root_hash->hash_flag == STORED_ALPHA
                    // is true if we have a key for the position and a dummy move is stored.
                    assert(root_hash->hash_flag == STORED_ALPHA);
                } else {
                    // if the root hash key isn't 0, then the key must correspond to our current position. For reasons
                    // explained in the comments above, it is possible for the hash move stored inside this entry in the
                    // PV to not exist. Therefore, only create a hash_move for move ordering IF we such a hash move
                    // exists in the TT.
                    hash_move = new ChessMove(root_hash->from_sq, root_hash->to_sq, root_hash->m_flag(), moved, captured); // watch for memory leaks with "new" keyword
                }
            }
        }

        // in the root position, we must generate all the moves and order them...
        gen_all_moves(board, movelist);
        order_moves(movelist, search_state, board, hash_move);

        if (hash_move != nullptr) {
            // if the hash move is not a nullptr, then delete it. only needed for move ordering purposes
            delete hash_move;
            hash_move = nullptr;
        }

        for (std::size_t i = 0; i < movelist.size(); i++) {
            // determine if it's legal by playing it and seeing whether you put yourself in check
            board.make_move(movelist[i]);
            if (board.is_king_checked((!board.side_2_move()))) {
                // if it's an illegal move, remove it from the movelist
                movelist.erase(movelist.begin() + i);
                board.unmake_move();
                --i; // must decrement because of the post loop increment (works for std::size_t and underflow+overflow)
                continue;
            }

            // pre-increment the height of the search before going deeper. the "height" variable of all recursive
            // functions to this will have a higher height
            ++search_state.height;

            // Search the current position using negamax (negamax is the '-' sign in front of "search(...)" function)
            root_score = -search(board, options, search_state, eval_state, depth - 1, -beta, -alpha, true);

            // post-decrement the height of the search after leaving lower level recursive calls...
            board.unmake_move();
            --search_state.height;

            if (check_stop_search(depth, options, search_state)) {
                // check and see if we have run out of time to search the position... if we have then return the best
                // move so far.
                return {best_move, alpha};
            }

            if (root_score > alpha) {
                // if the root score exceeds the current best move score or there's not a best yet, then update the
                // current best score and the current best move corresponding to the high score
                alpha = root_score;
                best_move = movelist[i];
            }
        }

        if (movelist.empty()) {
            // if there are no legal moves from the current position, then it's either a stalemate or a checkmate
            Centipawns_t ret_score = 0;

            // it's a checkmate if the king is checked
            if (board.is_king_checked(board.side_2_move())) {
                if (board.side_2_move() == WHITE) {
                    search_state.result_flag = WHITE_IS_MATED;
                } else {
                    search_state.result_flag = BLACK_IS_MATED;
                }

                // since we know we are in the root position (since this is the search root function), the side to move
                // is the current player. if they are mated, regardless of color, the score is the most negative number
                // possible... hence "-INF"
                ret_score = -INF;
            } else {
                // if the king is NOT mated, then it must be a stalemate (as opposed to a draw)
                search_state.result_flag = STALEMATE;
            }

            // whether it's a stalemate or checkmate, return with the score and a dummy move; search_state object tells
            // the end result.
            return {ChessMove(), ret_score};
        }

        if (root_score == INF - 2) {
            // TODO: Check correctness...
            if (board.side_2_move() == WHITE) {
                search_state.result_flag = BLACK_IS_MATED;
            } else {
                search_state.result_flag = WHITE_IS_MATED;
            }
        }

        if (alpha != init_alpha) {
            // TODO: check this object and insert
            ChessHash pv_hash(board.key(), root_score, best_move.from_sq, best_move.to_sq, best_move.flag(), depth, EXACT, board.current_ply());
            search_state.tt.insert(pv_hash);
        }

        return {best_move, alpha};
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
        // for debugging purposes, it's useful to count the number of nodes the engine has visited.
        ++search_state.raw_nodes; // this counts the root node as 1!

        // if the current position is a draw, return 0 immediately
        if (board.is_draw()) {
            return 0;
        }

        // initialize the variables at the top of the loop!
        Centipawns_t init_alpha = alpha;
        Centipawns_t score = -INF;
        Depth num_extensions = 0;
        Depth R = compute_adap_null_move_r(depth);

        // Check the hash table for extra information
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

            /// recursively call this function, decrementing the depth and flipping the scores
            score = -search(board, options, search_state, eval_state, depth - 1 + num_extensions, -beta, -alpha, true);
            board.unmake_move();
            --search_state.height;

            /// if we've run out of time, then quit. By returning alpha, we don't give any new information
            if (internal::check_stop_search(depth, options, search_state)) {
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
            // is this wrong? or are we just not checking for the ChessMove() dummy move case when we probe?
            // store a dummy in the transposition table because we don't know the actual move
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

    /**
     * function rearranges the moves in the movelist. the closer to the front of the list a move is, the more likely we
     * believe it will cause a cutoff.
     *
     * completely heuristics-based. not incredibly well tuned. should test and tune more rigorously
     * @param movelist     a vector of ChessMove objects corresponding to pseudo-legal moves eminating from 1 position
     * @param search_state the state of the search
     * @param board        the board object (contains board state information)
     * @param hash_move    a pointer to a ChessMove object (either nullptr or the move we found in the TT for this
     * search)
     */
    void order_moves(std::vector<ChessMove>& movelist, SearchState& search_state, Board& board, ChessMove* hash_move) {
        MoveScore score = 0; // MoveScore is uint32_t type (

        for (auto & move : movelist) {
            score = move.score; // MVV-LVA

            if (hash_move != nullptr) {
                if (move == *hash_move) {
                    score |= (1 << 31);
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
                        score += (1000 * search_state.history_heuristic[move.moved][move.to_sq]); // hmm... so we are always adding this no matter what? the element better be 0 by default
//                    }
                }
            }

            move.score = score;
        }

        std::sort(movelist.begin(), movelist.end());
#ifndef NDEBUG
        if (!movelist.empty() && hash_move != nullptr) {
            // make sure the hash_move is the first move in the movelist...
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
 * iterative deepening  search function wrapping function that initializes some important search variables
 * @param board        instance of Board object under search
 * @param options      instance of UCIOptions object informing exit conditions
 * @param search_state instance of SearchState object holding search information
 * @param eval_state   instance of EvaluationState object containing evaluation data
 * @return             the best ChessMove from the current position
 */
ChessMove think(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state, Book& book) {
    // initialize the all the local search variables at the top
    ChessMove best_move; // default constructor will make this a dummy move
    Centipawns_t score{0}; // score is neutrally 0 at the start of the search
    search_state.reset(); // reset all the search variables to their nominal values
    Centipawns_t alpha{-INF}; // the lower bound of the score starts at "-infinity"
    Centipawns_t beta{INF}; // the upper bound of the score starts at "+infinity"

    // before performing a full search, check to see if the current position is in the opening-book
    std::vector<BookEdge> book_moves = book.edges(board.key());
    if (!book_moves.empty()) {
        // if the opening book contains the position, then choose one of the book moves from that position and play it
        std::size_t index = internal::choose_book_move(book_moves);
        book.make_move(index); // update the opening book so that subsequent searches of it are faster
        return book_moves[index].move;
    }

    // if we haven't found a book move, then we have to perform a search. allocate time for the search
    search_state.clock.alloc_time(options, board.side_2_move());
    search_state.clock.start(); // start the clock

    // this is the iterative deepening loop...
    // it starts by performing a full breadth-first search to a tree depth of 1, then performs another full
    // breadth-first search to a tree depth of 2, then 3, and so on. Each subsequent BFS search takes advantage of
    // knowledge gained about previous searches through the transposition table (effectively memoization in the
    // dynamic programming sense). If the time has run out or we reach some other terminating condition during the
    // a search, then we cut it off there and return the results
    for (unsigned depth = 1; depth < MAX_DEPTH; depth++, alpha = -INF, beta = INF) {
        // enter the search functions through the "search_root(...)" function which has some particularities to it that
        // the "search(...)" function itself doesn't have
        std::tuple<ChessMove, Centipawns_t> tup = internal::search_root(board, options, search_state, eval_state, depth, alpha, beta);
        best_move = std::get<0>(tup); // the best move that we think can play from this position
        score = std::get<1>(tup); // the score corresponding to the best move

        // Get a split from the clock and compute the time elapsed in milliseconds
        search_state.clock.stop();
        Milliseconds_t time_elapsed = search_state.clock.duration() / 1000000;

        // Move through the principal variation by making the next EXACT move in the hash table
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

                    // this can happen for reasons that are totally reasonable that I can't remember the specifics of
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
        std::cout << std::setprecision(2) << "info score cp " << score << " time " << time_elapsed << " depth " << depth <<
#ifndef NDEBUG
            " nodes " << search_state.raw_nodes <<
//            " leaf_nodes " << search_state.leaf_nodes <<
//            " nps " << (double)(search_state.raw_nodes / (time_elapsed / 1000.)) <<
            " ordering " << search_state.ordering() <<
//            " window_ratio " << search_state.window_ratio() <<
//            " load_factor " << static_cast<double>(search_state.tt.load_factor()) <<
//            " hit_rate " << static_cast<double>(search_state.tt.hit_rate()) <<
//            " overwrite_percentage " << static_cast<double>(search_state.tt.overwrite_percentage()) <<
#endif // NDEBUG
            " pv " << pv_string << std::endl;

        // if we've run out of time or the user has said "stop" or whatever, then break out of the loop and just return
        if (internal::check_stop_search(depth, options, search_state)) {
            break;
        }

        // if we have found a forced mate in the PV, then don't continue to search, just quit. This will hopefully fix
        // a bug related to repeating multiple mate moves over and over again. Could also introduce bugs though.
        if (std::abs(score) == INF - (Centipawns_t)depth) {
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
