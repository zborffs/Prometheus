#include <attack.hpp>
#include "bitmask.hpp"
#include "eval.hpp"

namespace internal {
    void print_verbose(const std::string& param_name, Centipawns_t val) {
        std::setfill(' ');
        if (val < 0) {
            const unsigned num_sig_figs = std::floor(std::log10(-val));
            std::cout << "| " << param_name << std::setw(12 - param_name.size()) << "| " << val << std::setw(5 - num_sig_figs) << "|" << std::endl;
        } else {
            const unsigned num_sig_figs = std::floor(std::log10(val));
            std::cout << "| " << param_name << std::setw(12 - param_name.size()) << "| " << val << std::setw(6 - num_sig_figs) << "|" << std::endl;
        }

    }

    /**
     * defines the stage that we're in based on the current position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of an EvaluationState object to be updated
     */
    void def_stage(const Board& board, EvaluationState& es) {
        auto m = material(board, es, WHITE, false) + material(board, es, BLACK, false);

        if (m > 6600) {
            es.stage = OPENING;
        } else if (m > 4000) {
            es.stage = MID_GAME;
        } else if (m > 2000) {
            es.stage = EARLY_END_GAME;
        } else {
            es.stage = LATE_END_GAME;
        }
    }

    /**
     * defines the position type that it is based on the current position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of an EvaluationState object to be updated
     */
    void def_pos_type(const Board& board, EvaluationState& es) {
        auto num_pawns = pop_count(board.piece_bb(W_PAWN) | board.piece_bb(B_PAWN));

        if (num_pawns > 12) {
            es.pos_type = CLOSED;
        } else if (num_pawns > 8) {
            es.pos_type = SEMI_CLOSED;
        } else if (num_pawns > 4) {
            es.pos_type = SEMI_OPEN;
        } else {
            es.pos_type = OPEN;
        }
    }

    /**
     * computes the material differential in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of an EvaluationState object informing how to score
     * @param color determiner of which color to compute score for (usually BOTH is used)
     * @return      the material score differential in centipawns
     */
    Centipawns_t material(const Board &board, EvaluationState &es, Color_t color, bool verbose) {
        Centipawns_t ret{0};

        if (color == WHITE) {
            ret = BISHOP_BASE_VAL * pop_count(board.piece_bb(W_BISHOP)) +
                  PAWN_BASE_VAL * pop_count(board.piece_bb(W_PAWN)) +
                  KNIGHT_BASE_VAL * pop_count(board.piece_bb(W_KNIGHT)) +
                  QUEEN_BASE_VAL * pop_count(board.piece_bb(W_QUEEN)) +
                  ROOK_BASE_VAL * pop_count(board.piece_bb(W_ROOK));
        } else if (color == BLACK) {
            ret = BISHOP_BASE_VAL * pop_count(board.piece_bb(B_BISHOP)) +
                  PAWN_BASE_VAL * pop_count(board.piece_bb(B_PAWN)) +
                  KNIGHT_BASE_VAL * pop_count(board.piece_bb(B_KNIGHT)) +
                  QUEEN_BASE_VAL * pop_count(board.piece_bb(B_QUEEN)) +
                  ROOK_BASE_VAL * pop_count(board.piece_bb(B_ROOK));
        } else {
            ret = BISHOP_PERC[es.pos_type] * BISHOP_BASE_VAL * (pop_count(board.piece_bb(W_BISHOP)) - pop_count(board.piece_bb(B_BISHOP))) +
                  KNIGHT_PERC[es.pos_type] * KNIGHT_BASE_VAL * (pop_count(board.piece_bb(W_KNIGHT)) - pop_count(board.piece_bb(B_KNIGHT))) +
                  QUEEN_PERC[es.pos_type] * QUEEN_BASE_VAL * (pop_count(board.piece_bb(W_QUEEN)) - pop_count(board.piece_bb(B_QUEEN))) +
                  ROOK_PERC[es.pos_type] * ROOK_BASE_VAL * (pop_count(board.piece_bb(W_ROOK)) - pop_count(board.piece_bb(B_ROOK)));

            ret += (PAWN_STRENGTH[es.stage] * PAWN_BASE_VAL * (pop_count(board.piece_bb(W_PAWN)) - pop_count(board.piece_bb((B_PAWN)))));

            if (verbose) {
                print_verbose("Material", ret);
            }
        }

        return ret;
    }

    /**
     * computes the pawn differential in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of a EvaluationState object informing how to score
     * @return      the pawn sccore differential in centipawns
     */
    Centipawns_t eval_pawns(const Board &board, EvaluationState &es, bool verbose) {
        Bitboard w = board.piece_bb(W_PAWN);
        Bitboard b = board.piece_bb(B_PAWN);
        Centipawns_t ret{0};

        Bitboard push_pawn, push_pawn2, push_pawn3;
        Square_t to_sq;
        PieceType_t piece_defending;

        /// Piece Square Tables --> Positioning
        ret += dot_product(w, &PAWN_PST[0]);
        ret -= dot_product(flip_flop(b), &PAWN_PST[0]);

        /// Attacking + Mobility
        push_pawn = (w << 8) & (board.empty_bb());
        push_pawn |= (w << 16) & (board.empty_bb() & RANK_MASK[RANK4] & push_pawn);
        push_pawn2 = (w << 9) & ~(FILE_MASK[FILEA]);
        push_pawn3 = (w << 7) & ~(FILE_MASK[FILEH]);
        ret += PAWN_MOBILITY * (pop_count(push_pawn) +
                                pop_count(push_pawn2 & board.capturable(BLACK)) +
                                pop_count(push_pawn3 & board.capturable(BLACK)) +
                                pop_count(push_pawn2 & CENTER) +
                                pop_count(push_pawn3 & CENTER));

        /// Defending + Connectivity (Take Right)
        push_pawn2 &= board.capturable(WHITE);
        while (push_pawn2) {
            to_sq = bitscan_forward(push_pawn2);
            piece_defending = board.piece_type(to_sq, WHITE);
            Centipawns_t s = (CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(W_PAWN - 2) / 2]);
            ret += s;
            push_pawn2 -= C64(1) << to_sq;
        }

        /// Defending + Connectivity
        push_pawn3 &= board.capturable(WHITE);
        while (push_pawn3) {
            to_sq = bitscan_forward(push_pawn3);
            piece_defending = board.piece_type(to_sq, WHITE);
            Centipawns_t s = (CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(W_PAWN - 2) / 2]);
            ret += s;
            push_pawn3 -= C64(1) << to_sq;
        }

        /// Attacking + Mobility
        push_pawn  = (b >> 8) & (board.empty_bb());
        push_pawn |= (b >> 16) & (board.empty_bb() & RANK_MASK[RANK5] & push_pawn);
        push_pawn2 = (b >> 9) & ~(FILE_MASK[FILEH]);
        push_pawn3 = (b >> 7) & ~(FILE_MASK[FILEA]);
        ret -= PAWN_MOBILITY * (pop_count(push_pawn) +
                                pop_count(push_pawn2 & board.capturable(WHITE)) +
                                pop_count(push_pawn3 & board.capturable(WHITE)) +
                                pop_count(push_pawn2 & CENTER) +
                                pop_count(push_pawn3 & CENTER));

        /// Defending + Connectivity (Take Right)
        push_pawn2 &= board.capturable(BLACK);
        while (push_pawn2) {
            to_sq = bitscan_forward(push_pawn2);
            piece_defending = board.piece_type(to_sq, BLACK);
            Centipawns_t s = (CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(B_PAWN - 2) / 2]);
            ret -= s;
            push_pawn2 -= C64(1) << to_sq;
        }

        /// Defending + Connectivity
        push_pawn3 &= board.capturable(BLACK);
        while (push_pawn3) {
            to_sq = bitscan_forward(push_pawn3);
            piece_defending = board.piece_type(to_sq, BLACK);
            Centipawns_t s = (CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(B_PAWN - 2) / 2]);
            ret -= s;
            push_pawn3 -= C64(1) << to_sq;
        }

        /// Passed Pawn Boni
        Bitboard passed_pawn = internal::white_passed_pawns(board);
        while (passed_pawn) {
            to_sq = bitscan_forward(passed_pawn);
            ret += PASSED_PAWN[to_sq / 8];
            Bitboard to_sq_bb = C64(1) << to_sq;
            if (file_fill(to_sq_bb) & board.piece_bb(W_ROOK)) {
                ret += ROOK_SUPPORTING_PASSED_PAWNS;
            }
            passed_pawn -= to_sq_bb;
        }

        passed_pawn = internal::black_passed_pawns(board);
        while (passed_pawn) {
            to_sq = bitscan_forward(passed_pawn);
            ret -= PASSED_PAWN[7 - (to_sq / 8)];
            Bitboard to_sq_bb = C64(1) << to_sq;
            if (file_fill(to_sq_bb) & board.piece_bb(B_ROOK)) {
                ret -= ROOK_SUPPORTING_PASSED_PAWNS;
            }
            passed_pawn -= to_sq_bb;
        }

        /// Hanging and Isolated Penalties --> Positioning
        ret += (pop_count(white_hanging_pawns(board)) - pop_count(black_hanging_pawns(board))) * PAWN_HANGING;
        ret += pawn_island_differential(board);
        ret += (pop_count(isolanis(board.piece_bb(W_PAWN))) - pop_count(isolanis(board.piece_bb(B_PAWN)))) * PAWN_ISOLATED;
        ret += (pop_count(half_isolanis(board.piece_bb(W_PAWN))) - pop_count(half_isolanis(board.piece_bb(B_PAWN)))) * HALF_ISOLATED_PAWN;

        if (verbose) {
            print_verbose("Pawns", ret);
        }

        return ret;
    }

    /**
     * computes the rook differential in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of a EvaluationState object informing how to score
     * @return      the rook score differential in centipawns
     */
    Centipawns_t eval_rooks(const Board& board, EvaluationState& es, bool verbose) {
        Bitboard w = board.piece_bb(W_ROOK);
        Bitboard b = board.piece_bb(B_ROOK);
        Centipawns_t ret{0};
        Bitboard push_rook;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t piece_defending;

        /// Piece Square Tables --> Positioning
        ret += dot_product(w, &ROOK_PST[0]);
        ret -= dot_product(flip_flop(b), &ROOK_PST[0]);

        /// Open File + Half Open File Boni --> Positioning
        Bitboard of = open_files(board);
        Bitboard nsf = ~semiclosed_files(board);
        ret += (pop_count(of & w) - pop_count(of & b)) * ROOK_ON_OPEN_FILE_BONUS;
        ret += (pop_count(white_half_open_files(board) & w & nsf) - pop_count(black_half_open_files(board) & b & nsf)) * ROOK_ON_HALF_OPEN_FILE_BONUS;

        /// Attacking + Defending + Mobility + Connectivity
        for (Color_t col = WHITE; col <= BLACK; col++) {
            Bitboard rook_bb = col == WHITE ? w : b;
            Centipawns_t color_multiplier = col * -2 + 1;
            while (rook_bb) {
                from_sq = bitscan_forward(rook_bb);
                push_rook = rook_moves(from_sq, board.occupied_bb());
                ret += pop_count(push_rook & (board.empty_bb() | board.capturable(!col) | CENTER_FILES)) * color_multiplier * OM_ROOK_MOBILITY;

                push_rook &= board.capturable(col);

                while (push_rook) {
                    to_sq = bitscan_forward(push_rook);
                    piece_defending = board.piece_type(to_sq, col);
                    Centipawns_t s = ((CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(W_ROOK - 2) / 2]) * color_multiplier);
                    ret += s;

                    // bonus for connected rooks
                    if (piece_defending == W_ROOK + col) {
                        ret += CONNECTED_ROOK_BONUS * color_multiplier;
                    }
                    push_rook -= C64(1) << to_sq;
                }

                rook_bb -= C64(1) << from_sq;
            }
        }

        if (verbose) {
            print_verbose("Rook", ret);
        }

        return ret;
    }

    /**
     * computes the knight differential in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of a EvaluationState object informing how to score
     * @return      the knight score differential in centipawns
     */
    Centipawns_t eval_knights(const Board &board, EvaluationState &es, bool verbose) {
        Bitboard w = board.piece_bb(W_KNIGHT);
        Bitboard b = board.piece_bb(B_KNIGHT);
        Centipawns_t ret{0};

        Bitboard push_knight;
        Bitboard knight_bb;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t piece_defending;

        /// Piece Square Tables --> Positioning
        ret += dot_product(w, &KNIGHT_PST[0]);
        ret -= dot_product(flip_flop(b), &KNIGHT_PST[0]);

        /// Attacking + Defending + Mobility + Connectivity
        for (Color_t col = WHITE; col <= BLACK; col++) {
            knight_bb = board.piece_bb(W_QUEEN + col);
            Centipawns_t color_multiplier = col * -2 + 1;
            while (knight_bb) {
                from_sq = bitscan_forward(knight_bb);
                push_knight = knight_moves(C64(1) << from_sq);
                ret += pop_count(push_knight & (board.capturable(!col) | board.empty_bb() | CENTER)) * color_multiplier * KNIGHT_MOBILITY;

                /// Outpost --> Positioning
                if (col == WHITE) {
                    Bitboard outpost = (((board.piece_bb(W_PAWN) << 9) & ~(FILE_MASK[FILEA])) | ((board.piece_bb(W_PAWN) << 7) & ~(FILE_MASK[FILEH]))) & (C64(1) << from_sq);
                    if (outpost) {
                        outpost = ~((board.piece_bb(B_PAWN) & north_fill(((C64(1) << (from_sq + 1)) & ~(FILE_MASK[FILEA])) | ((C64(1) << (from_sq - 1)) & ~(FILE_MASK[FILEH])))));
                        if(outpost) {
                            ret += KNIGHT_OUTPOST_BONUS;
                        }
                    }
                } else {
                    Bitboard outpost = (((board.piece_bb(B_PAWN) >> 9) & ~(FILE_MASK[FILEH])) | ((board.piece_bb(B_PAWN) >> 7) & ~(FILE_MASK[FILEA]))) & (C64(1) << from_sq);
                    if(outpost) {
                        outpost = ~((board.piece_bb(W_PAWN) & south_fill(((C64(1) << (from_sq + 1)) & ~(FILE_MASK[FILEA])) | ((C64(1) << (from_sq - 1)) & ~(FILE_MASK[FILEH])))));

                        if(outpost) {
                            ret -= KNIGHT_OUTPOST_BONUS;
                        }
                    }
                }

                push_knight &= board.capturable(col);
                while (push_knight) {
                    to_sq = bitscan_forward(push_knight);
                    piece_defending = board.piece_type(to_sq, col);
                    Centipawns_t s = (CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(W_KNIGHT - 2) / 2]) * color_multiplier;
                    ret += s;
                    push_knight -= C64(1) << to_sq;
                }

                knight_bb -= C64(1) << from_sq;
            }
        }

        if (verbose) {
            print_verbose("Knight", ret);
        }

        return ret;
    }

    /**
     * computes the bishop differential in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of a EvaluationState object informing how to score
     * @return      the bishop score differential in centipawns
     */
    Centipawns_t eval_bishop(const Board &board, EvaluationState &es, bool verbose) {
        Bitboard w = board.piece_bb(W_BISHOP);
        Bitboard b = board.piece_bb(B_BISHOP);
        Centipawns_t ret{0};

        Bitboard push_bishop;
        Bitboard bishop_bb;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t piece_defending;

        /// Piece Square Tables --> Positioning
        ret += dot_product(w, &BISHOP_PST[0]);
        ret -= dot_product(flip_flop(b), &BISHOP_PST[0]);

        /// Bishop Pair Advantage
        if (pop_count(w) >= 2) {
            ret += BISHOP_PAIR_BONUS;
        }
        if (pop_count(b) >= 2) {
            ret -= BISHOP_PAIR_BONUS;
        }

        /// Attacking + Defending + Mobility + Connectivity
        for (Color_t col = WHITE; col <= BLACK; col++) {
            bishop_bb = board.piece_bb(W_BISHOP + col);
            Centipawns_t color_multiplier = col * -2 + 1;
            while (bishop_bb) {
                from_sq = bitscan_forward(bishop_bb);
                push_bishop = bishop_moves(from_sq, board.occupied_bb());

                ret += pop_count(push_bishop & (board.capturable(!col) | board.empty_bb() | CENTER)) * color_multiplier;

                push_bishop &= board.capturable(col);
                while (push_bishop) {
                    to_sq = bitscan_forward(push_bishop);
                    piece_defending = board.piece_type(to_sq, col);
                    Centipawns_t s = (CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(W_BISHOP - 2) / 2]) * color_multiplier;
                    ret += s;
                    push_bishop -= C64(1) << to_sq;
                }

                bishop_bb -= C64(1) << from_sq;
            }
        }

        if (verbose) {
            print_verbose("Bishops", ret);
        }

        return ret;
    }

    /**
     * computes the queen differential in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of a EvaluationState object informing how to score
     * @return      the queen score differential in centipawns
     */
    Centipawns_t eval_queens(const Board &board, EvaluationState &es, bool verbose) {
        Bitboard w = board.piece_bb(W_QUEEN);
        Bitboard b = board.piece_bb(B_QUEEN);
        Centipawns_t ret{0};
        Bitboard push_queen;
        Bitboard queen_bb;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t piece_defending;

        ret += dot_product(w, &QUEEN_PST[0]);
        ret -= dot_product(flip_flop(b), &QUEEN_PST[0]);

        for (Color_t col = WHITE; col <= BLACK; col++) {
            queen_bb = board.piece_bb(W_QUEEN + col);
            Centipawns_t color_multiplier = col * -2 + 1;

            while (queen_bb) {
                from_sq = bitscan_forward(queen_bb);
                push_queen = queen_moves(from_sq, board.occupied_bb());
                ret += pop_count(push_queen & (board.capturable(!col) | board.empty_bb() | CENTER)) * color_multiplier;

                push_queen &= board.capturable(col);
                while (push_queen) {
                    to_sq = bitscan_forward(push_queen);
                    piece_defending = board.piece_type(to_sq, col);
                    Centipawns_t s = (CONNECTIVITY[(piece_defending - 2) / 2] + CONNECTIVITY[(W_QUEEN - 2) / 2]) * color_multiplier;
                    ret += s;
                    push_queen -= C64(1) << to_sq;
                }

                queen_bb -= C64(1) << from_sq;
            }
        }

        /// Penalize for opening quickly
        if (es.stage == OPENING) {
            if (board.piece_bb(W_QUEEN) != (C64(1) << D1)) {
                ret += QUEEN_OPENING_QUICK_PENALTY;
            }

            if (board.piece_bb(B_QUEEN) != (C64(1) << D8)) {
                ret -= QUEEN_OPENING_QUICK_PENALTY;
            }
        }

        if (verbose) {
            print_verbose("Queens", ret);
        }

        return ret;
    }

    /**
     * computes the king differential in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of a EvaluationState object informing how to score
     * @return      the king score differential in centipawns
     */
    Centipawns_t eval_kings(const Board &board, EvaluationState &es, bool verbose) {
        Bitboard w = board.piece_bb(W_KING);
        Bitboard b = board.piece_bb(B_KING);
        Centipawns_t ret{0};

        Bitboard push_king;
        Bitboard king_bb;
        Square_t to_sq;
        PieceType_t piece_defending;

        /// if the game stage is early end game or late end game
        if (es.stage & 2) {
            ret += dot_product(w, &E_KING_PST[0]);
            ret -= dot_product(flip_flop(b), &E_KING_PST[0]);

            /// if the game stage is opening or midgame
        } else {
            ret += dot_product(w, &M_KING_PST[0]);
            ret -= dot_product(flip_flop(b), &M_KING_PST[0]);
        }

        for (Color_t col = WHITE; col <= BLACK; col++) {
            king_bb = board.piece_bb(W_KING + col);
            push_king = king_moves(king_bb);
            Centipawns_t color_multiplier = col * -2 + 1;
            ret += pop_count(push_king & (board.capturable(!col) | board.empty_bb())) * color_multiplier;
        }

        if (verbose) {
            print_verbose("Kings", ret);
        }

        return ret;
    }

    /**
     * computes the tempo score in centipawns given a certain position
     * @param board instance of a Board object being scrutinized
     * @param es    instance of a EvaluationState object informing how to score
     * @return      the tempo score in centipawns
     */
    Centipawns_t eval_tempo(const Board &board, EvaluationState &es, bool verbose) {
        /// shouldn't get the tempo initiative if in check though
        Centipawns_t ret = (board.side_2_move()) ? -TEMPO : TEMPO;
        if (verbose) {
            print_verbose("Tempo", ret);
        }
        return ret;
    }

    /**
     * computes the file corresponding to a given square
     * @param sq square we're probing
     * @return   file the square is on
     */
    Bitboard file_fill_sq(Square_t sq) {
        assert(sq % 8 == (int)sq % 8);
        return FILE_MASK[sq % 8];
    }

    /**
     * computes bitboard of with the entire file of all east attacks filled in
     * @param bb bitboard (usually pawns bitboard)
     * @return   bitboard with all east attack files filled in
     */
    Bitboard east_attack_file_fill(Bitboard bb) {
        return (file_fill(bb) << 1) & ~FILE_MASK[FILEA];
    }

    /**
     * computes bitboard of with the entire file of all west attacks filled in
     * @param bb bitboard (usually pawns bitboard)
     * @return   bitboard with all west attack files filled in
     */
    Bitboard west_attack_file_fill(Bitboard bb) {
        return (file_fill(bb) >> 1) & ~FILE_MASK[FILEH];
    }

    /**
     * computes bitboard of north fill of all white pawns moved ahead by 1 square
     * @param board instance of Board object
     * @return      white front span bitboard
     */
    Bitboard white_front_span(const Board& board) {
        return north_fill(board.piece_bb(W_PAWN) << 8);
    }

    /**
     * computes bitboard of south fill of all black pawns moved ahead by 1 square
     * @param board instancce of Board object
     * @return      black front span bitboard
     */
    Bitboard black_front_span(const Board& board) {
        return south_fill(board.piece_bb(B_PAWN) >> 8);
    }

    /**
     * computes bitboard of south fill of all white pawns moved back 1 square
     * @param board instance of Board object
     * @return      white rear span bitboard
     */
    Bitboard white_rear_span(const Board& board) {
        return south_fill(board.piece_bb(W_PAWN) >> 8);
    }

    /**
     * computes bitboard of north fill of all black pawns moved back 1 square
     * @param board instance of Board object
     * @return      black rear span bitboard
     */
    Bitboard black_rear_span(const Board& board) {
        return north_fill(board.piece_bb(B_PAWN) << 8);
    }

    /**
     * the intersection of black front and white front spans
     * @param board instance of Board object
     * @return      interspan bitboard
     */
    Bitboard interspan(const Board& board) {
        return black_front_span(board) & white_front_span(board);
    }

    /**
     * file fill of all files where there are at least 1 black pawn and 1 white pawn
     * @param board instance of Board object
     * @return      closed files bitboard
     */
    Bitboard closed_files(const Board& board) {
        return file_fill(board.piece_bb(W_PAWN)) & file_fill(board.piece_bb(B_PAWN));
    }

    /**
     * TEST!
     * file fill of all files where there are no pawns
     * @param board instance of Board object
     * @return      open files bitboard
     */
    Bitboard open_files(const Board& board) {
        return ~(file_fill(board.piece_bb(W_PAWN) | board.piece_bb(B_PAWN)));
    }

    /**
     * TEST!
     * @param board
     * @return
     */
    Bitboard white_half_open_files(const Board& board) {
        return ~(file_fill(board.piece_bb(W_PAWN))) ^ open_files(board);
    }

    /**
     * TEST!
     * @param board
     * @return
     */
    Bitboard black_half_open_files(const Board& board) {
        return ~(file_fill(board.piece_bb(B_PAWN))) ^ open_files(board);
    }

    /**
     * TEST!
     * @param board
     * @return
     */
    Bitboard semiclosed_files(const Board& board) {
        return file_fill(open_pawns(board)) & closed_files(board);
    }

    /**
     * white pawns on files not occupied by black pawns
     * @param board instance of Board object
     * @return      white open pawns bitboard
     */
    Bitboard white_open_pawns(const Board& board) {
        return board.piece_bb(W_PAWN) & ~black_front_span(board);
    }

    /**
     * black pawns on files not occupied by white pawns
     * @param board instance of Board object
     * @return      black open pawns bitboard
     */
    Bitboard black_open_pawns(const Board& board) {
        return board.piece_bb(B_PAWN) & ~white_front_span(board);
    }

    /**
     * white and black pawns on open files
     * @param board instance of Board object
     * @return      open pawns bitboard
     */
    Bitboard open_pawns(const Board& board) {
        return white_open_pawns(board) | black_open_pawns(board);
    }

    /**
     * computes file fills of all white pawn attack squares
     * @param board instance of board object
     * @return      white attack front spans bitboard
     */
    Bitboard white_attack_front_spans(const Board& board) {
        Bitboard wfs = white_front_span(board);
        return ((wfs << 1) & ~FILE_MASK[FILEH]) | ((wfs >> 1) & ~FILE_MASK[FILEA]);
    }

    /**
     * computes file fills of all black pawn attack squares
     * @param board
     * @return
     */
    Bitboard black_attack_front_spans(const Board& board) {
        Bitboard bfs = black_front_span(board);
        return ((bfs << 1) & ~FILE_MASK[FILEH]) | ((bfs >> 1) & ~FILE_MASK[FILEA]);
    }

    /**
     *
     * @param board
     * @return
     */
    Bitboard white_attack_rear_spans(const Board& board) {
        Bitboard wrs = white_rear_span(board);
        return ((wrs << 1) & ~FILE_MASK[FILEH]) | ((wrs >> 1) & ~FILE_MASK[FILEA]);
    }

    /**
     *
     * @param board
     * @return
     */
    Bitboard black_attack_rear_spans(const Board& board) {
        Bitboard brs = black_rear_span(board);
        return ((brs << 1) & ~FILE_MASK[FILEH]) | ((brs >> 1) & ~FILE_MASK[FILEA]);
    }

   /**
    * returns a FileSet_t, which encodes the boolean information of whether a given file is occupied by a pawn
    * @param bb a pawn bitboard (either black or white)
    * @return FileSet_t (uint8_t) with most significant bit being H file and least significant bit being A file
    */
   FileSet_t file_set(Bitboard bb) {
       return (FileSet_t)south_fill(bb);
   }

    /**
     * computes the east border-files of each pawn island
     * @param fs the FileSet_t of the white or black pawns
     * @return island east files FileSet_t
     */
    FileSet_t island_east_files(FileSet_t fs) {
        return fs & ~(fs >> 1);
    }

    /**
     * compues the west border-files of each pawn island
     * @param fs the FileSet_t of the white or black pawns
     * @return island west files FileSet_t
     */
    FileSet_t island_west_files(FileSet_t fs) {
        return fs & ~(fs << 1);
    }

    /**
     *
     * @param board
     * @return
     */
    Centipawns_t pawn_island_differential(const Board& board) {
        FileSet_t wfs = internal::file_set(board.piece_bb(W_PAWN));
        FileSet_t bfs = internal::file_set(board.piece_bb(B_PAWN));
        Centipawns_t diff = pop_count((Bitboard)wfs) - pop_count((Bitboard)bfs);
        return (diff > 0) ? PAWN_ISLAND[diff] : -PAWN_ISLAND[-diff];
    }

    /**
     *
     * @param board
     * @param bb
     * @return
     */
    Bitboard no_east_neighbors(Bitboard bb) {
        return bb & ~internal::west_attack_file_fill(bb);
    }

    /**
     *
     * @param board
     * @param bb
     * @return
     */
    Bitboard no_west_neighbors(Bitboard bb) {
        return bb & ~internal::east_attack_file_fill(bb);
    }

    /**
     *
     * @param board
     * @param bb
     * @return
     */
    Bitboard isolanis(Bitboard bb) {
        return internal::no_east_neighbors(bb) & internal::no_west_neighbors(bb);
    }

    /**
     *
     * @param board
     * @param bb
     * @return
     */
    Bitboard half_isolanis(Bitboard bb) {
        return internal::no_east_neighbors(bb) ^ internal::no_west_neighbors(bb);
    }

    /**
     *
     * @param board
     * @param bb
     * @return
     */
    Bitboard pawns_with_west_neighbors(Bitboard bb) {
        return bb & ((bb << 1) & ~FILE_MASK[FILEA]);
    }

    /**
     *
     * @param board
     * @param bb
     * @return
     */
    Bitboard pawns_with_east_neighbors(Bitboard bb) {
        return bb & ((bb >> 1) & ~FILE_MASK[FILEH]);
    }

    /**
     *
     * @param board
     * @param bb
     * @return
     */
    Bitboard duo(Bitboard bb) {
        Bitboard withWestNeighbors = internal::pawns_with_west_neighbors(bb);
        Bitboard withEastNeighbors = withWestNeighbors >> 1;

        Bitboard withOneExclusiveNeighbor  = withWestNeighbors ^ withEastNeighbors;
        Bitboard withExclusiveWestNeighbor = withWestNeighbors & withOneExclusiveNeighbor;
        Bitboard withExclusiveEastNeighbor = withEastNeighbors & withOneExclusiveNeighbor;

        Bitboard duoWestOne = withExclusiveEastNeighbor & (withExclusiveWestNeighbor >> 1);
        Bitboard duoEastOne = duoWestOne << 1;
        return duoWestOne | duoEastOne;
    }

    /**
     *
     * @param board
     * @return
     */
    Bitboard white_hanging_pawns(const Board& board) {
        return internal::white_open_pawns(board) & internal::half_isolanis(board.piece_bb(W_PAWN)) & internal::duo(board.piece_bb(W_PAWN));
    }

    /**
     *
     * @param board
     * @return
     */
    Bitboard black_hanging_pawns(const Board& board) {
        return internal::black_open_pawns(board) & internal::half_isolanis(board.piece_bb(B_PAWN)) & internal::duo(board.piece_bb(B_PAWN));
    }

    /**
     *
     * @param board
     * @return
     */
    Bitboard white_passed_pawns(const Board& board) {
        Bitboard all_front_spans = black_front_span(board);
        all_front_spans |= ((all_front_spans >> 1) & ~FILE_MASK[FILEH]) | ((all_front_spans << 1) & ~FILE_MASK[FILEA]);
        return board.piece_bb(W_PAWN) & ~all_front_spans;
    }

    /**
     *
     * @param board
     * @return
     */
    Bitboard black_passed_pawns(const Board& board) {
        Bitboard all_front_spans = white_front_span(board);
        all_front_spans |= ((all_front_spans >> 1) & ~FILE_MASK[FILEH]) | ((all_front_spans << 1) & ~FILE_MASK[FILEA]);
        return board.piece_bb(B_PAWN) & ~all_front_spans;
    }
}


/**
 * computes the entire score of a given position and updates the EvaluationState object
 * @param board instance of a Board object being scrutinized
 * @param es    instance of a EvaluationState object informing how to score
 * @return      the overall score of the position
 */
Centipawns_t evaluate(const Board& board, EvaluationState& es, bool verbose) {
    internal::def_stage(board, es);
    internal::def_pos_type(board, es);

    /// if verbose has been activated, print the EvaluationState after it's been updated
    if (verbose) {
        std::cout << es << std::endl;
        std::cout << "| Parameter | Score |" << std::endl;
        std::cout << "+-----------+-------+" << std::endl;
    }

    Centipawns_t ret = internal::material(board, es, Color::BOTH, verbose);
    ret += internal::eval_pawns(board, es, verbose);
    ret += internal::eval_rooks(board, es, verbose);
    ret += internal::eval_knights(board, es, verbose);
    ret += internal::eval_bishop(board, es, verbose);
    ret += internal::eval_queens(board, es, verbose);
    ret += internal::eval_kings(board, es, verbose);
    ret += internal::eval_tempo(board, es, verbose);

    return ret;
}
