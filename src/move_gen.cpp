//
// Created by Zach Bortoff on 2019-11-28.
//

#include "move_gen.hpp"

namespace internal {
    /**
     * generates all pseudolegal pawn captures
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_pawn_cap(Board& board, std::vector<ChessMove>& movelist) {
        Bitboard push_pawn;
        Square_t to_sq;
        PieceType_t ptc;

        if (board.side_2_move() == WHITE) {
            push_pawn = (board.piece_bb(W_PAWN) << 9) & board.capturable(BLACK) & ~(RANK_MASK[RANK8] | FILE_MASK[FILEA]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, BLACK);
                movelist.emplace_back(ChessMove(to_sq - 9, to_sq, CAPTURE_MOVE, W_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }

            push_pawn = (board.piece_bb(W_PAWN) << 7) & (board.capturable(BLACK)) & ~(RANK_MASK[RANK8] | FILE_MASK[FILEH]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, BLACK);
                movelist.emplace_back(ChessMove(to_sq - 7, to_sq, CAPTURE_MOVE, W_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }
        } else {
            push_pawn = (board.piece_bb(B_PAWN) >> 7) & (board.capturable(WHITE)) & ~(RANK_MASK[RANK1] | FILE_MASK[FILEA]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, WHITE);
                movelist.emplace_back(ChessMove(to_sq + 7, to_sq, CAPTURE_MOVE, B_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }
            push_pawn = (board.piece_bb(B_PAWN) >> 9) & (board.capturable(WHITE)) & ~(RANK_MASK[RANK1] | FILE_MASK[FILEH]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, WHITE);
                movelist.emplace_back(ChessMove(to_sq + 9, to_sq, CAPTURE_MOVE, B_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }
        }
    }

    /**
     * generates all pseudolegal pawn promotions
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_pawn_prom(Board& board, std::vector<ChessMove>& movelist) {
        Bitboard push_pawn;
        Square_t to_sq;
        PieceType_t ptc;

        if (board.side_2_move() == WHITE) {
            push_pawn = (board.piece_bb(W_PAWN) << 9) & board.capturable(BLACK) & RANK_MASK[RANK8] & ~FILE_MASK[FILEA];
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, BLACK);
                movelist.emplace_back(ChessMove(to_sq - 9, to_sq, QUEEN_PROMO_CAPTURE, W_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq - 9, to_sq, KNIGHT_PROMO_CAPTURE, W_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq - 9, to_sq, ROOK_PROMO_CAPTURE, W_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq - 9, to_sq, BISHOP_PROMO_CAPTURE, W_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }
            push_pawn = (board.piece_bb(W_PAWN) << 7) & board.capturable(BLACK) & RANK_MASK[RANK8] & ~FILE_MASK[FILEH];
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, BLACK);
                movelist.emplace_back(ChessMove(to_sq - 7, to_sq, QUEEN_PROMO_CAPTURE, W_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq - 7, to_sq, KNIGHT_PROMO_CAPTURE, W_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq - 7, to_sq, ROOK_PROMO_CAPTURE, W_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq - 7, to_sq, BISHOP_PROMO_CAPTURE, W_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }
            push_pawn = (board.piece_bb(W_PAWN) << 8) & board.empty_bb() & RANK_MASK[RANK8];
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                movelist.emplace_back(ChessMove(to_sq - 8, to_sq, QUEEN_PROMO, W_PAWN, NO_PIECE));
                movelist.emplace_back(ChessMove(to_sq - 8, to_sq, KNIGHT_PROMO, W_PAWN, NO_PIECE));
                movelist.emplace_back(ChessMove(to_sq - 8, to_sq, ROOK_PROMO, W_PAWN, NO_PIECE));
                movelist.emplace_back(ChessMove(to_sq - 8, to_sq, BISHOP_PROMO, W_PAWN, NO_PIECE));
                push_pawn -= C64(1) << to_sq;
            }
        } else {
            push_pawn = (board.piece_bb(B_PAWN) >> 9) & (board.capturable(WHITE) & RANK_MASK[RANK1] & ~FILE_MASK[FILEH]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, WHITE);
                movelist.emplace_back(ChessMove(to_sq + 9, to_sq, QUEEN_PROMO_CAPTURE, B_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq + 9, to_sq, KNIGHT_PROMO_CAPTURE, B_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq + 9, to_sq, ROOK_PROMO_CAPTURE, B_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq + 9, to_sq, BISHOP_PROMO_CAPTURE, B_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }
            push_pawn = (board.piece_bb(B_PAWN) >> 7) & (board.capturable(WHITE) & RANK_MASK[RANK1] & ~FILE_MASK[FILEA]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                ptc = board.piece_type(to_sq, WHITE);
                movelist.emplace_back(ChessMove(to_sq + 7, to_sq, QUEEN_PROMO_CAPTURE, B_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq + 7, to_sq, KNIGHT_PROMO_CAPTURE, B_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq + 7, to_sq, ROOK_PROMO_CAPTURE, B_PAWN, ptc));
                movelist.emplace_back(ChessMove(to_sq + 7, to_sq, BISHOP_PROMO_CAPTURE, B_PAWN, ptc));
                push_pawn -= C64(1) << to_sq;
            }
            push_pawn = (board.piece_bb(B_PAWN) >> 8) & (board.empty_bb() & RANK_MASK[RANK1]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                movelist.emplace_back(ChessMove(to_sq + 8, to_sq, QUEEN_PROMO, B_PAWN, NO_PIECE));
                movelist.emplace_back(ChessMove(to_sq + 8, to_sq, KNIGHT_PROMO, B_PAWN, NO_PIECE));
                movelist.emplace_back(ChessMove(to_sq + 8, to_sq, ROOK_PROMO, B_PAWN, NO_PIECE));
                movelist.emplace_back(ChessMove(to_sq + 8, to_sq, BISHOP_PROMO, B_PAWN, NO_PIECE));
                push_pawn -= C64(1) << to_sq;
            }
        }
    }

    /**
     * generates all pseudolegal pawn moves
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_pawn_quiet(Board& board, std::vector<ChessMove>& movelist) {
        Bitboard push_pawn;
        Square_t to_sq;

        if (board.side_2_move() == WHITE) {
            push_pawn = (board.piece_bb(W_PAWN) << 8) & (board.empty_bb() & ~RANK_MASK[RANK8]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                movelist.emplace_back(ChessMove(to_sq - 8, to_sq, QUIET_MOVE, W_PAWN, NO_PIECE));
                push_pawn -= (C64(1) << to_sq);
            }

            push_pawn = (((board.piece_bb(W_PAWN) << 8) & board.empty_bb()) << 8) & RANK_MASK[RANK4] & board.empty_bb();
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                movelist.emplace_back(ChessMove(to_sq - 16, to_sq, DOUBLE_PUSH_PAWN, W_PAWN, NO_PIECE));
                push_pawn -= (C64(1) << to_sq);
            }
        } else {
            push_pawn = (board.piece_bb(B_PAWN) >> 8) & (board.empty_bb() & ~RANK_MASK[RANK1]);
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                movelist.emplace_back(ChessMove(to_sq + 8, to_sq, QUIET_MOVE, B_PAWN, NO_PIECE));
                push_pawn -= (C64(1) << to_sq);
            }

            push_pawn = (((board.piece_bb(B_PAWN) >> 8) & board.empty_bb()) >> 8) & RANK_MASK[RANK5] & board.empty_bb();
            while (push_pawn) {
                to_sq = bitscan_forward(push_pawn);
                movelist.emplace_back(ChessMove(to_sq + 16, to_sq, DOUBLE_PUSH_PAWN, B_PAWN, NO_PIECE));
                push_pawn -= (C64(1) << to_sq);
            }
        }
    }

    /**
     * generates all pseudolegal en passant captures
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_ep_cap(Board& board, std::vector<ChessMove>& movelist) {
        int to_sq = board.ep_sq();
        if (to_sq == EP_DEFAULT) {
            return;
        }

        Bitboard push_pawn;
        if (board.side_2_move() == WHITE) {
            push_pawn = (board.piece_bb(W_PAWN) << 9) & (C64(1) << board.ep_sq()) & ~FILE_MASK[FILEA];
            if (push_pawn) {
                movelist.emplace_back(ChessMove(to_sq - 9, to_sq, ENPASSANT, W_PAWN, B_PAWN));
            }

            push_pawn = (board.piece_bb(W_PAWN) << 7) & (C64(1) << board.ep_sq()) & ~FILE_MASK[FILEH];
            if (push_pawn) {
                movelist.emplace_back(ChessMove(to_sq - 7, to_sq, ENPASSANT, W_PAWN, B_PAWN));
            }
        } else {
            push_pawn = (board.piece_bb(B_PAWN) >> 9) & (C64(1) << board.ep_sq()) & ~FILE_MASK[FILEH];
            if (push_pawn) {
                movelist.emplace_back(ChessMove(to_sq + 9, to_sq, ENPASSANT, B_PAWN, W_PAWN));
            }

            push_pawn = (board.piece_bb(B_PAWN) >> 7) & (C64(1) << board.ep_sq()) & ~FILE_MASK[FILEA];
            if (push_pawn) {
                movelist.emplace_back(ChessMove(to_sq + 7, to_sq, ENPASSANT, B_PAWN, W_PAWN));
            }
        }
    }

    /**
     * generates all pseudolegal rook captures
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_rook_cap(Board& board, std::vector<ChessMove>& movelist) {
        Bitboard push_rook;
        Bitboard rook_bb;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t ptc;

        if (board.side_2_move() == WHITE) {
            rook_bb = board.piece_bb(W_ROOK);
            while (rook_bb) {
                from_sq = bitscan_forward(rook_bb);
                push_rook = rook_moves(from_sq, board.occupied_bb()) & board.capturable(BLACK);
                while (push_rook) {
                    to_sq = bitscan_forward(push_rook);
                    ptc = board.piece_type(to_sq, BLACK);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, W_ROOK, ptc));
                    push_rook -= (C64(1) << to_sq);
                }
                rook_bb -= (C64(1) << from_sq);
            }
        } else {
            rook_bb = board.piece_bb(B_ROOK);
            while (rook_bb) {
                from_sq = bitscan_forward(rook_bb);
                push_rook = rook_moves(from_sq, board.occupied_bb()) & board.capturable(WHITE);
                while (push_rook) {
                    to_sq = bitscan_forward(push_rook);
                    ptc = board.piece_type(to_sq, WHITE);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, B_ROOK, ptc));
                    push_rook -= (C64(1) << to_sq);
                }
                rook_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all pseudolegal rook moves
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_rook_quiet(Board& board, std::vector<ChessMove>& movelist) {
        Bitboard push_rook;
        Bitboard rook_bb;
        Square_t from_sq;
        Square_t to_sq;

        if (board.side_2_move() == WHITE) {
            rook_bb = board.piece_bb(W_ROOK);
            while (rook_bb) {
                from_sq = bitscan_forward(rook_bb);
                push_rook = rook_moves(from_sq, board.occupied_bb()) & board.empty_bb();
                while (push_rook) {
                    to_sq = bitscan_forward(push_rook);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, W_ROOK, NO_PIECE));
                    push_rook -= (C64(1) << to_sq);
                }
                rook_bb -= (C64(1) << from_sq);
            }
        } else {
            rook_bb = board.piece_bb(B_ROOK);
            while (rook_bb) {
                from_sq = bitscan_forward(rook_bb);
                push_rook = rook_moves(from_sq, board.occupied_bb()) & board.empty_bb();
                while (push_rook) {
                    to_sq = bitscan_forward(push_rook);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, B_ROOK, NO_PIECE));
                    push_rook -= (C64(1) << to_sq);
                }
                rook_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all knight captures
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_knight_cap(Board& board, std::vector<ChessMove>& movelist){
        Bitboard push_knight;
        Bitboard knight_bb;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t ptc;

        if (board.side_2_move() == WHITE) {
            knight_bb = board.piece_bb(W_KNIGHT);
            while (knight_bb) {
                from_sq = bitscan_forward(knight_bb);
                push_knight = knight_moves(C64(1) << from_sq) & board.capturable(BLACK);
                while (push_knight) {
                    to_sq = bitscan_forward(push_knight);
                    ptc = board.piece_type(to_sq, BLACK);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, W_KNIGHT, ptc));
                    push_knight -= (C64(1) << to_sq);
                }
                knight_bb -= (C64(1) << from_sq);
            }
        } else {
            knight_bb = board.piece_bb(B_KNIGHT);
            while (knight_bb) {
                from_sq = bitscan_forward(knight_bb);
                push_knight = knight_moves(C64(1) << from_sq) & board.capturable(WHITE);
                while (push_knight) {
                    to_sq = bitscan_forward(push_knight);
                    ptc = board.piece_type(to_sq, WHITE);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, B_KNIGHT, ptc));
                    push_knight -= (C64(1) << to_sq);
                }
                knight_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all knight moves
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_knight_quiet(Board& board, std::vector<ChessMove>& movelist){
        Bitboard push_knight;
        Bitboard knight_bb;
        Square_t from_sq;
        Square_t to_sq;

        if (board.side_2_move() == WHITE) {
            knight_bb = board.piece_bb(W_KNIGHT);
            while (knight_bb) {
                from_sq = bitscan_forward(knight_bb);
                push_knight = knight_moves(C64(1) << from_sq) & board.empty_bb(); // shouldn't i just use the empty bitboard rather than the occupied one?; save 1 clock cycle
                // push_knight = knight_moves(knight_bb) & board.empty_bb();
                while (push_knight) {
                    to_sq = bitscan_forward(push_knight);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, W_KNIGHT, NO_PIECE));
                    push_knight -= (C64(1) << to_sq);
                }
                knight_bb -= (C64(1) << from_sq);
            }
        } else {
            knight_bb = board.piece_bb(B_KNIGHT);
            while (knight_bb) {
                from_sq = bitscan_forward(knight_bb);
                push_knight = knight_moves(C64(1) << from_sq) & board.empty_bb();
                // push_knight = knight_moves(knight_bb) & board.empty_bb();
                while (push_knight) {
                    to_sq = bitscan_forward(push_knight);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, B_KNIGHT, NO_PIECE));
                    push_knight -= (C64(1) << to_sq);
                }
                knight_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all pseudolegal bishop captures
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_bishop_cap(Board& board, std::vector<ChessMove>& movelist) {
        Bitboard push_bishop;
        Bitboard bishop_bb;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t ptc;

        if (board.side_2_move() == WHITE) {
            bishop_bb = board.piece_bb(W_BISHOP);
            while (bishop_bb) {
                from_sq = bitscan_forward(bishop_bb);
                push_bishop = bishop_moves(from_sq, board.occupied_bb()) & board.capturable(BLACK);
                while (push_bishop) {
                    to_sq = bitscan_forward(push_bishop);
                    ptc = board.piece_type(to_sq, BLACK);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, W_BISHOP, ptc));
                    push_bishop -= (C64(1) << to_sq);
                }
                bishop_bb -= (C64(1) << from_sq);
            }
        } else {
            bishop_bb = board.piece_bb(B_BISHOP);
            while (bishop_bb) {
                from_sq = bitscan_forward(bishop_bb);
                push_bishop = bishop_moves(from_sq, board.occupied_bb()) & board.capturable(WHITE);
                while (push_bishop) {
                    to_sq = bitscan_forward(push_bishop);
                    ptc = board.piece_type(to_sq, WHITE);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, B_BISHOP, ptc));
                    push_bishop -= (C64(1) << to_sq);
                }
                bishop_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all pseudolegal bishop moves
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_bishop_quiet(Board& board, std::vector<ChessMove>& movelist){
        Bitboard push_bishop;
        Bitboard bishop_bb;
        Square_t from_sq;
        Square_t to_sq;

        if (board.side_2_move() == WHITE) {
            bishop_bb = board.piece_bb(W_BISHOP);
            while (bishop_bb) {
                from_sq = bitscan_forward(bishop_bb);
                push_bishop = bishop_moves(from_sq, board.occupied_bb()) & board.empty_bb();
                while (push_bishop) {
                    to_sq = bitscan_forward(push_bishop);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, W_BISHOP, NO_PIECE));
                    push_bishop -= (C64(1) << to_sq);
                }
                bishop_bb -= (C64(1) << from_sq);
            }
        } else {
            bishop_bb = board.piece_bb(B_BISHOP);
            while (bishop_bb) {
                from_sq = bitscan_forward(bishop_bb);
                push_bishop = bishop_moves(from_sq, board.occupied_bb()) & board.empty_bb();
                while (push_bishop) {
                    to_sq = bitscan_forward(push_bishop);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, B_BISHOP, NO_PIECE));
                    push_bishop -= (C64(1) << to_sq);
                }
                bishop_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all pseudolegal queen captures
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_queen_cap(Board& board, std::vector<ChessMove>& movelist){
        Bitboard push_queen;
        Bitboard queen_bb;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t ptc;

        if (board.side_2_move() == WHITE) {
            queen_bb = board.piece_bb(W_QUEEN);
            while (queen_bb) {
                from_sq = bitscan_forward(queen_bb);
                push_queen = queen_moves(from_sq, board.occupied_bb()) & board.capturable(BLACK);
                while (push_queen) {
                    to_sq = bitscan_forward(push_queen);
                    ptc = board.piece_type(to_sq, BLACK);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, W_QUEEN, ptc));
                    push_queen -= (C64(1) << to_sq);
                }
                queen_bb -= (C64(1) << from_sq);
            }
        } else {
            queen_bb = board.piece_bb(B_QUEEN );
            while (queen_bb) {
                from_sq = bitscan_forward(queen_bb);
                push_queen = queen_moves(from_sq, board.occupied_bb()) & board.capturable(WHITE);
                while (push_queen) {
                    to_sq = bitscan_forward(push_queen);
                    ptc = board.piece_type(to_sq, WHITE);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, B_QUEEN , ptc));
                    push_queen -= (C64(1) << to_sq);
                }
                queen_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all pseudolegal queen moves
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_queen_quiet(Board& board, std::vector<ChessMove>& movelist){
        Bitboard push_queen;
        Bitboard queen_bb;
        Square_t from_sq;
        Square_t to_sq;

        if (board.side_2_move() == WHITE) {
            queen_bb = board.piece_bb(W_QUEEN );
            while (queen_bb) {
                from_sq = bitscan_forward(queen_bb);
                push_queen = queen_moves(from_sq, board.occupied_bb()) & board.empty_bb();
                while (push_queen) {
                    to_sq = bitscan_forward(push_queen);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, W_QUEEN, NO_PIECE));
                    push_queen -= (C64(1) << to_sq);
                }
                queen_bb -= (C64(1) << from_sq);
            }
        } else {
            queen_bb = board.piece_bb(B_QUEEN );
            while (queen_bb) {
                from_sq = bitscan_forward(queen_bb);
                push_queen = queen_moves(from_sq, board.occupied_bb()) & board.empty_bb();
                while (push_queen) {
                    to_sq = bitscan_forward(push_queen);
                    movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, B_QUEEN, NO_PIECE));
                    push_queen -= (C64(1) << to_sq);
                }
                queen_bb -= (C64(1) << from_sq);
            }
        }
    }

    /**
     * generates all pseudolegal king captures
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_king_cap(Board& board, std::vector<ChessMove>& movelist) {
        Bitboard push_king;
        Square_t from_sq;
        Square_t to_sq;
        PieceType_t ptc;

        if(board.side_2_move() == WHITE) {
            from_sq = board.king_location(WHITE);
            push_king = king_moves(board.piece_bb(W_KING)) & board.capturable(BLACK);
            while (push_king) {
                to_sq = bitscan_forward(push_king);
                ptc = board.piece_type(to_sq, BLACK);
                movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, W_KING, ptc));
                push_king -= (C64(1) << to_sq);
            }
        } else {
            from_sq = board.king_location(BLACK);
            push_king = king_moves(board.piece_bb(B_KING)) & board.capturable(WHITE);
            while (push_king) {
                to_sq = bitscan_forward(push_king);
                ptc = board.piece_type(to_sq, WHITE);
                movelist.emplace_back(ChessMove(from_sq, to_sq, CAPTURE_MOVE, B_KING, ptc));
                push_king -= (C64(1) << to_sq);
            }
        }
    }

    /**
     * generates all pseudolegal king moves
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_king_quiet(Board& board, std::vector<ChessMove>& movelist){
        Bitboard push_king;
        Square_t from_sq;
        Square_t to_sq;

        if (board.side_2_move() == WHITE) {
            from_sq = board.king_location(WHITE);
            push_king = king_moves(board.piece_bb(W_KING)) & board.empty_bb();
            while (push_king) {
                to_sq = bitscan_forward(push_king);
                movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, W_KING, NO_PIECE));
                push_king -= (C64(1) << to_sq);
            }
        } else {
            from_sq = board.king_location(BLACK);
            push_king = king_moves(board.piece_bb(B_KING)) & board.empty_bb();
            while (push_king) {
                to_sq = bitscan_forward(push_king);
                movelist.emplace_back(ChessMove(from_sq, to_sq, QUIET_MOVE, B_KING, NO_PIECE));
                push_king -= (C64(1) << to_sq);
            }
        }
    }

    /**
     * generates all pseudolegal castling moves
     * @param board    instance of Board object
     * @param movelist instance of MoveList object
     */
    void gen_castle(Board& board, std::vector<ChessMove>& movelist) {
        if (board.side_2_move() == WHITE) {
            if ((board.castle_perm() & WKC)) {
                Bitboard bb = C64(96);
                if ((board.empty_bb() & bb) == bb) { // 96 = 2^5 + 2^6
                    if (!board.is_sq_attacked(E1, BLACK) && !board.is_sq_attacked(F1, BLACK) && !board.is_sq_attacked(G1, BLACK)) {
                        movelist.emplace_back(ChessMove(E1, G1, KING_CASTLE, W_KING, NO_PIECE));
                    }
                }
            }

            if ((board.castle_perm() & WQC)) {
                Bitboard bb = C64(14);
                if ((board.empty_bb() & bb) == bb) { // 2^1 + 2^2 + 2^3
                    if (!board.is_sq_attacked(E1, BLACK) && !board.is_sq_attacked(D1, BLACK) && !board.is_sq_attacked(C1, BLACK)) {
                        movelist.emplace_back(ChessMove(E1, C1, QUEEN_CASTLE, W_KING, NO_PIECE));
                    }
                }
            }
        } else {
            if ((board.castle_perm() & BKC)) {
                Bitboard bb = (Bitboard)6917529027641081856;
                if ((bb & board.empty_bb()) == bb) {
                    if (!board.is_sq_attacked(E8, WHITE) && !board.is_sq_attacked(F8, WHITE) && !board.is_sq_attacked(G8, WHITE)) {
                        movelist.emplace_back(ChessMove(E8, G8, KING_CASTLE, B_KING, NO_PIECE));

                    }
                }
            }

            if ((board.castle_perm() & BQC)) {
                Bitboard bb = (Bitboard)1008806316530991104;
                if ((board.empty_bb() & bb) == bb) {
                    if (!board.is_sq_attacked(E8, WHITE) && !board.is_sq_attacked(D8, WHITE) && !board.is_sq_attacked(C8, WHITE)) {
                        movelist.emplace_back(ChessMove(E8, C8, QUEEN_CASTLE, B_KING, NO_PIECE));
                    }
                }
            }
        }
    }
}


/**
 * generates all pseudolegal moves
 * @param board    instance of Board object
 * @param movelist instance of MoveList object
 */
void gen_all_moves(Board& board, std::vector<ChessMove>& movelist) {
    internal::gen_pawn_cap(board, movelist);
    internal::gen_pawn_quiet(board, movelist);
    internal::gen_pawn_prom(board, movelist);
    internal::gen_ep_cap(board, movelist);
    internal::gen_rook_cap(board, movelist);
    internal::gen_rook_quiet(board, movelist);
    internal::gen_knight_cap(board, movelist);
    internal::gen_knight_quiet(board, movelist);
    internal::gen_bishop_cap(board, movelist);
    internal::gen_bishop_quiet(board, movelist);
    internal::gen_queen_cap(board, movelist);
    internal::gen_queen_quiet(board, movelist);
    internal::gen_king_cap(board, movelist);
    internal::gen_king_quiet(board, movelist); // 5
    internal::gen_castle(board, movelist);
}

/**
 * generates all pseudolegal captures
 * @param board    instance of Board object
 * @param movelist instance of MoveList object
 */
void gen_all_caps(Board& board, std::vector<ChessMove>& movelist) {
    internal::gen_pawn_cap(board, movelist);
    internal::gen_pawn_prom(board, movelist);
    internal::gen_ep_cap(board, movelist);
    internal::gen_rook_cap(board, movelist);
    internal::gen_knight_cap(board, movelist);
    internal::gen_bishop_cap(board, movelist);
    internal::gen_queen_cap(board, movelist);
    internal::gen_king_cap(board, movelist);
}