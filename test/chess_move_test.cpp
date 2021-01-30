#include <gtest/gtest.h>
#include "chess_move.hpp"

class ChessMoveTester : public ::testing::Test {
protected:
    /// Quiets Moves
    ChessMove white_quiet_pawn;
    ChessMove black_quiet_pawn;
    ChessMove white_quiet_rook;
    ChessMove black_quiet_rook;
    ChessMove white_quiet_knight;
    ChessMove black_quiet_knight;
    ChessMove white_quiet_bishop;
    ChessMove black_quiet_bishop;
    ChessMove white_quiet_queen;
    ChessMove black_quiet_queen;
    ChessMove white_quiet_king;
    ChessMove black_quiet_king;

    /// Double Push Pawn Moves
    ChessMove white_dpp;
    ChessMove black_dpp;

    /// Castling
    ChessMove white_king;
    ChessMove black_king;
    ChessMove white_queen;
    ChessMove black_queen;

    /// En Passant
    ChessMove white_ep_left;
    ChessMove white_ep_right;
    ChessMove black_ep_left;
    ChessMove black_ep_right;

    /// Promotions
    ChessMove white_promo_rook;
    ChessMove white_promo_rook_capture;
    ChessMove black_promo_rook;
    ChessMove black_promo_rook_capture;

    ChessMove white_promo_knight;
    ChessMove white_promo_knight_capture;
    ChessMove black_promo_knight;
    ChessMove black_promo_knight_capture;

    ChessMove white_promo_bishop;
    ChessMove white_promo_bishop_capture;
    ChessMove black_promo_bishop;
    ChessMove black_promo_bishop_capture;

    ChessMove white_promo_queen;
    ChessMove white_promo_queen_capture;
    ChessMove black_promo_queen;
    ChessMove black_promo_queen_capture;

protected:
    void SetUp() override {
        /// Initialize Quiet Moves
        white_quiet_pawn = ChessMove(A2, A3, QUIET_MOVE, W_PAWN, NO_PIECE);
        black_quiet_pawn = ChessMove(C7, C6, QUIET_MOVE, B_PAWN, NO_PIECE);
        white_quiet_rook = ChessMove(H1, H3, QUIET_MOVE, W_ROOK, NO_PIECE);
        black_quiet_rook = ChessMove(H8, H4, QUIET_MOVE, B_ROOK, NO_PIECE);
        white_quiet_knight = ChessMove(G1, H3, QUIET_MOVE, W_KNIGHT, NO_PIECE);
        black_quiet_knight = ChessMove(G8, H6, QUIET_MOVE, B_KNIGHT, NO_PIECE);
        white_quiet_bishop = ChessMove(F1, E2, QUIET_MOVE, W_BISHOP, NO_PIECE);
        black_quiet_bishop = ChessMove(F8, E7, QUIET_MOVE, B_BISHOP, NO_PIECE);
        white_quiet_queen = ChessMove(D1, B3, QUIET_MOVE, W_QUEEN, NO_PIECE);
        black_quiet_queen = ChessMove(D8, B6, QUIET_MOVE, B_QUEEN, NO_PIECE);
        white_quiet_king = ChessMove(E1, E2, QUIET_MOVE, W_KING, NO_PIECE);
        black_quiet_king = ChessMove(E8, E7, QUIET_MOVE, B_KING, NO_PIECE);

        /// Initialize Double Push Pawn
        white_dpp = ChessMove(E2, E4, DOUBLE_PUSH_PAWN, W_PAWN, NO_PIECE);
        black_dpp = ChessMove(E7, E5, DOUBLE_PUSH_PAWN, B_PAWN, NO_PIECE);

        /// Initialize Castling
        white_king = ChessMove(E1, G1, KING_CASTLE, W_KING, NO_PIECE);
        white_queen = ChessMove(E1, C1, QUEEN_CASTLE, W_KING, NO_PIECE);
        black_king = ChessMove(E8, G8, KING_CASTLE, B_KING, NO_PIECE);
        black_queen = ChessMove(E8, C8, QUEEN_CASTLE, B_KING, NO_PIECE);

        /// Initialize En Passant
        white_ep_left = ChessMove(B5, A6, ENPASSANT, W_PAWN, B_PAWN);
        white_ep_right = ChessMove(B5, C6, ENPASSANT, W_PAWN, B_PAWN);
        black_ep_left = ChessMove(D4, C3, ENPASSANT, B_PAWN, W_PAWN);
        black_ep_right = ChessMove(D4, E3, ENPASSANT, B_PAWN, W_PAWN);

        /// Initialize Promotions
        white_promo_rook = ChessMove(A7, A8, ROOK_PROMO, W_PAWN, NO_PIECE);
        white_promo_rook_capture = ChessMove(B7, C8, ROOK_PROMO_CAPTURE, W_PAWN, B_QUEEN);
        black_promo_rook = ChessMove(A2, A1, ROOK_PROMO, B_PAWN, NO_PIECE);
        black_promo_rook_capture = ChessMove(B2, C1, ROOK_PROMO_CAPTURE, B_PAWN, W_ROOK);

        white_promo_knight = ChessMove(C7, C8, KNIGHT_PROMO, W_PAWN, NO_PIECE);
        white_promo_knight_capture = ChessMove(D7, C8, KNIGHT_PROMO_CAPTURE, W_PAWN, B_ROOK);
        black_promo_knight = ChessMove(D2, D1, KNIGHT_PROMO, B_PAWN, NO_PIECE);
        black_promo_knight_capture = ChessMove(E2, D1, KNIGHT_PROMO_CAPTURE, B_PAWN, W_QUEEN);

        white_promo_bishop = ChessMove(E7, E8, BISHOP_PROMO, W_PAWN, NO_PIECE);
        white_promo_bishop_capture = ChessMove(E7, F8, BISHOP_PROMO_CAPTURE, W_PAWN, B_KNIGHT);
        black_promo_bishop = ChessMove(G2, G1, BISHOP_PROMO, B_PAWN, NO_PIECE);
        black_promo_bishop_capture = ChessMove(G2, H1, BISHOP_PROMO_CAPTURE, B_PAWN, W_BISHOP);

        white_promo_queen = ChessMove(F7, F8, QUEEN_PROMO, W_PAWN, NO_PIECE);
        white_promo_queen_capture = ChessMove(F7, G8, QUEEN_PROMO_CAPTURE, W_PAWN, B_BISHOP);
        black_promo_queen = ChessMove(F2, F1, QUEEN_PROMO, B_PAWN, NO_PIECE);
        black_promo_queen_capture = ChessMove(G2, F1, QUEEN_PROMO_CAPTURE, B_PAWN, W_KNIGHT);
    }
};

/**
 * Tests the promoted() function for every move-type for each side.
 */
TEST_F(ChessMoveTester, Promoted_PieceType) {
    /// Quiet moves must not have promoted pieces.
    EXPECT_EQ(white_quiet_pawn.promoted(), NO_PIECE);
    EXPECT_EQ(black_quiet_pawn.promoted(), NO_PIECE);
    EXPECT_EQ(white_quiet_rook.promoted(), NO_PIECE);
    EXPECT_EQ(black_quiet_rook.promoted(), NO_PIECE);
    EXPECT_EQ(white_quiet_knight.promoted(), NO_PIECE);
    EXPECT_EQ(black_quiet_knight.promoted(), NO_PIECE);
    EXPECT_EQ(white_quiet_bishop.promoted(), NO_PIECE);
    EXPECT_EQ(black_quiet_bishop.promoted(), NO_PIECE);
    EXPECT_EQ(white_quiet_queen.promoted(), NO_PIECE);
    EXPECT_EQ(black_quiet_queen.promoted(), NO_PIECE);
    EXPECT_EQ(white_quiet_king.promoted(), NO_PIECE);
    EXPECT_EQ(black_quiet_king.promoted(), NO_PIECE);

    /// DPP moves must not have promoted pieces.
    EXPECT_EQ(white_dpp.promoted(), NO_PIECE);
    EXPECT_EQ(black_dpp.promoted(), NO_PIECE);

    /// Castling moves must not have promoted pieces.
    EXPECT_EQ(white_king.promoted(), NO_PIECE);
    EXPECT_EQ(black_king.promoted(), NO_PIECE);
    EXPECT_EQ(white_queen.promoted(), NO_PIECE);
    EXPECT_EQ(black_queen.promoted(), NO_PIECE);

    /// EP moves must not have promoted pieces.
    EXPECT_EQ(white_ep_left.promoted(), NO_PIECE);
    EXPECT_EQ(white_ep_right.promoted(), NO_PIECE);
    EXPECT_EQ(black_ep_left.promoted(), NO_PIECE);
    EXPECT_EQ(black_ep_right.promoted(), NO_PIECE);

    /// Quiet moves must have promoted pieces.
    EXPECT_EQ(white_promo_rook.promoted(), W_ROOK);
    EXPECT_EQ(white_promo_rook_capture.promoted(), W_ROOK);
    EXPECT_EQ(black_promo_rook.promoted(), B_ROOK);
    EXPECT_EQ(black_promo_rook_capture.promoted(), B_ROOK);

    EXPECT_EQ(white_promo_knight.promoted(), W_KNIGHT);
    EXPECT_EQ(white_promo_knight_capture.promoted(), W_KNIGHT);
    EXPECT_EQ(black_promo_knight.promoted(), B_KNIGHT);
    EXPECT_EQ(black_promo_knight_capture.promoted(), B_KNIGHT);

    EXPECT_EQ(white_promo_bishop.promoted(), W_BISHOP);
    EXPECT_EQ(white_promo_bishop_capture.promoted(), W_BISHOP);
    EXPECT_EQ(black_promo_bishop.promoted(), B_BISHOP);
    EXPECT_EQ(black_promo_bishop_capture.promoted(), B_BISHOP);

    EXPECT_EQ(white_promo_queen.promoted(), W_QUEEN);
    EXPECT_EQ(white_promo_queen_capture.promoted(), W_QUEEN);
    EXPECT_EQ(black_promo_queen.promoted(), B_QUEEN);
    EXPECT_EQ(black_promo_queen_capture.promoted(), B_QUEEN);
}

TEST_F(ChessMoveTester, Moved_PieceType) {
    EXPECT_EQ(white_quiet_pawn.moved, W_PAWN);
    EXPECT_EQ(black_quiet_pawn.moved, B_PAWN);
    EXPECT_EQ(white_quiet_rook.moved, W_ROOK);
    EXPECT_EQ(black_quiet_rook.moved, B_ROOK);
    EXPECT_EQ(white_quiet_knight.moved, W_KNIGHT);
    EXPECT_EQ(black_quiet_knight.moved, B_KNIGHT);
    EXPECT_EQ(white_quiet_bishop.moved, W_BISHOP);
    EXPECT_EQ(black_quiet_bishop.moved, B_BISHOP);
    EXPECT_EQ(white_quiet_queen.moved, W_QUEEN);
    EXPECT_EQ(black_quiet_queen.moved, B_QUEEN);
    EXPECT_EQ(white_quiet_king.moved, W_KING);
    EXPECT_EQ(black_quiet_king.moved, B_KING);

    EXPECT_EQ(white_dpp.moved, W_PAWN);
    EXPECT_EQ(black_dpp.moved, B_PAWN);

    EXPECT_EQ(white_king.moved, W_KING);
    EXPECT_EQ(black_king.moved, B_KING);
    EXPECT_EQ(white_queen.moved, W_KING);
    EXPECT_EQ(black_queen.moved, B_KING);

    EXPECT_EQ(white_ep_left.moved, W_PAWN);
    EXPECT_EQ(white_ep_right.moved, W_PAWN);
    EXPECT_EQ(black_ep_left.moved, B_PAWN);
    EXPECT_EQ(black_ep_right.moved, B_PAWN);

    EXPECT_EQ(white_promo_rook.moved, W_PAWN);
    EXPECT_EQ(white_promo_rook_capture.moved, W_PAWN);
    EXPECT_EQ(black_promo_rook.moved, B_PAWN);
    EXPECT_EQ(black_promo_rook_capture.moved, B_PAWN);

    EXPECT_EQ(white_promo_knight.moved, W_PAWN);
    EXPECT_EQ(white_promo_knight_capture.moved, W_PAWN);
    EXPECT_EQ(black_promo_knight.moved, B_PAWN);
    EXPECT_EQ(black_promo_knight_capture.moved, B_PAWN);

    EXPECT_EQ(white_promo_bishop.moved, W_PAWN);
    EXPECT_EQ(white_promo_bishop_capture.moved, W_PAWN);
    EXPECT_EQ(black_promo_bishop.moved, B_PAWN);
    EXPECT_EQ(black_promo_bishop_capture.moved, B_PAWN);

    EXPECT_EQ(white_promo_queen.moved, W_PAWN);
    EXPECT_EQ(white_promo_queen_capture.moved, W_PAWN);
    EXPECT_EQ(black_promo_queen.moved, B_PAWN);
    EXPECT_EQ(black_promo_queen_capture.moved, B_PAWN);
}

TEST_F(ChessMoveTester, Captured_PieceType) {
    EXPECT_EQ(white_quiet_pawn.captured, NO_PIECE);
    EXPECT_EQ(black_quiet_pawn.captured, NO_PIECE);
    EXPECT_EQ(white_quiet_rook.captured, NO_PIECE);
    EXPECT_EQ(black_quiet_rook.captured, NO_PIECE);
    EXPECT_EQ(white_quiet_knight.captured, NO_PIECE);
    EXPECT_EQ(black_quiet_knight.captured, NO_PIECE);
    EXPECT_EQ(white_quiet_bishop.captured, NO_PIECE);
    EXPECT_EQ(black_quiet_bishop.captured, NO_PIECE);
    EXPECT_EQ(white_quiet_queen.captured, NO_PIECE);
    EXPECT_EQ(black_quiet_queen.captured, NO_PIECE);
    EXPECT_EQ(white_quiet_king.captured, NO_PIECE);
    EXPECT_EQ(black_quiet_king.captured, NO_PIECE);

    EXPECT_EQ(white_dpp.captured, NO_PIECE);
    EXPECT_EQ(black_dpp.captured, NO_PIECE);

    EXPECT_EQ(white_king.captured, NO_PIECE);
    EXPECT_EQ(black_king.captured, NO_PIECE);
    EXPECT_EQ(white_queen.captured, NO_PIECE);
    EXPECT_EQ(black_queen.captured, NO_PIECE);

    EXPECT_EQ(white_ep_left.captured, B_PAWN);
    EXPECT_EQ(white_ep_right.captured, B_PAWN);
    EXPECT_EQ(black_ep_left.captured, W_PAWN);
    EXPECT_EQ(black_ep_right.captured, W_PAWN);

    EXPECT_EQ(white_promo_rook.captured, NO_PIECE);
    EXPECT_EQ(white_promo_rook_capture.captured, B_QUEEN);
    EXPECT_EQ(black_promo_rook.captured, NO_PIECE);
    EXPECT_EQ(black_promo_rook_capture.captured, W_ROOK);

    EXPECT_EQ(white_promo_knight.captured, NO_PIECE);
    EXPECT_EQ(white_promo_knight_capture.captured, B_ROOK);
    EXPECT_EQ(black_promo_knight.captured, NO_PIECE);
    EXPECT_EQ(black_promo_knight_capture.captured, W_QUEEN);

    EXPECT_EQ(white_promo_bishop.captured, NO_PIECE);
    EXPECT_EQ(white_promo_bishop_capture.captured, B_KNIGHT);
    EXPECT_EQ(black_promo_bishop.captured, NO_PIECE);
    EXPECT_EQ(black_promo_bishop_capture.captured, W_BISHOP);

    EXPECT_EQ(white_promo_queen.captured, NO_PIECE);
    EXPECT_EQ(white_promo_queen_capture.captured, B_BISHOP);
    EXPECT_EQ(black_promo_queen.captured, NO_PIECE);
    EXPECT_EQ(black_promo_queen_capture.captured, W_KNIGHT);
}

TEST_F(ChessMoveTester, DPP_Flag) {
    EXPECT_EQ(white_quiet_pawn.dpp_flag(), false);
    EXPECT_EQ(black_quiet_pawn.dpp_flag(), false);
    EXPECT_EQ(white_quiet_rook.dpp_flag(), false);
    EXPECT_EQ(black_quiet_rook.dpp_flag(), false);
    EXPECT_EQ(white_quiet_knight.dpp_flag(), false);
    EXPECT_EQ(black_quiet_knight.dpp_flag(), false);
    EXPECT_EQ(white_quiet_bishop.dpp_flag(), false);
    EXPECT_EQ(black_quiet_bishop.dpp_flag(), false);
    EXPECT_EQ(white_quiet_queen.dpp_flag(), false);
    EXPECT_EQ(black_quiet_queen.dpp_flag(), false);
    EXPECT_EQ(white_quiet_king.dpp_flag(), false);
    EXPECT_EQ(black_quiet_king.dpp_flag(), false);

    EXPECT_EQ(white_dpp.dpp_flag(), true);
    EXPECT_EQ(black_dpp.dpp_flag(), true);

    EXPECT_EQ(white_king.dpp_flag(), false);
    EXPECT_EQ(black_king.dpp_flag(), false);
    EXPECT_EQ(white_queen.dpp_flag(), false);
    EXPECT_EQ(black_queen.dpp_flag(), false);

    EXPECT_EQ(white_ep_left.dpp_flag(), false);
    EXPECT_EQ(white_ep_right.dpp_flag(), false);
    EXPECT_EQ(black_ep_left.dpp_flag(), false);
    EXPECT_EQ(black_ep_right.dpp_flag(), false);

    EXPECT_EQ(white_promo_rook.dpp_flag(), false);
    EXPECT_EQ(white_promo_rook_capture.dpp_flag(), false);
    EXPECT_EQ(black_promo_rook.dpp_flag(), false);
    EXPECT_EQ(black_promo_rook_capture.dpp_flag(), false);

    EXPECT_EQ(white_promo_knight.dpp_flag(), false);
    EXPECT_EQ(white_promo_knight_capture.dpp_flag(), false);
    EXPECT_EQ(black_promo_knight.dpp_flag(), false);
    EXPECT_EQ(black_promo_knight_capture.dpp_flag(), false);

    EXPECT_EQ(white_promo_bishop.dpp_flag(), false);
    EXPECT_EQ(white_promo_bishop_capture.dpp_flag(), false);
    EXPECT_EQ(black_promo_bishop.dpp_flag(), false);
    EXPECT_EQ(black_promo_bishop_capture.dpp_flag(), false);

    EXPECT_EQ(white_promo_queen.dpp_flag(), false);
    EXPECT_EQ(white_promo_queen_capture.dpp_flag(), false);
    EXPECT_EQ(black_promo_queen.dpp_flag(), false);
    EXPECT_EQ(black_promo_queen_capture.dpp_flag(), false);
}

TEST_F(ChessMoveTester, Castle_Flag) {
    EXPECT_EQ(white_quiet_pawn.castle_flag(), false);
    EXPECT_EQ(black_quiet_pawn.castle_flag(), false);
    EXPECT_EQ(white_quiet_rook.castle_flag(), false);
    EXPECT_EQ(black_quiet_rook.castle_flag(), false);
    EXPECT_EQ(white_quiet_knight.castle_flag(), false);
    EXPECT_EQ(black_quiet_knight.castle_flag(), false);
    EXPECT_EQ(white_quiet_bishop.castle_flag(), false);
    EXPECT_EQ(black_quiet_bishop.castle_flag(), false);
    EXPECT_EQ(white_quiet_queen.castle_flag(), false);
    EXPECT_EQ(black_quiet_queen.castle_flag(), false);
    EXPECT_EQ(white_quiet_king.castle_flag(), false);
    EXPECT_EQ(black_quiet_king.castle_flag(), false);

    EXPECT_EQ(white_dpp.castle_flag(), false);
    EXPECT_EQ(black_dpp.castle_flag(), false);

    EXPECT_EQ(white_king.castle_flag(), true);
    EXPECT_EQ(black_king.castle_flag(), true);
    EXPECT_EQ(white_queen.castle_flag(), true);
    EXPECT_EQ(black_queen.castle_flag(), true);

    EXPECT_EQ(white_ep_left.castle_flag(), false);
    EXPECT_EQ(white_ep_right.castle_flag(), false);
    EXPECT_EQ(black_ep_left.castle_flag(), false);
    EXPECT_EQ(black_ep_right.castle_flag(), false);

    EXPECT_EQ(white_promo_rook.castle_flag(), false);
    EXPECT_EQ(white_promo_rook_capture.castle_flag(), false);
    EXPECT_EQ(black_promo_rook.castle_flag(), false);
    EXPECT_EQ(black_promo_rook_capture.castle_flag(), false);

    EXPECT_EQ(white_promo_knight.castle_flag(), false);
    EXPECT_EQ(white_promo_knight_capture.castle_flag(), false);
    EXPECT_EQ(black_promo_knight.castle_flag(), false);
    EXPECT_EQ(black_promo_knight_capture.castle_flag(), false);

    EXPECT_EQ(white_promo_bishop.castle_flag(), false);
    EXPECT_EQ(white_promo_bishop_capture.castle_flag(), false);
    EXPECT_EQ(black_promo_bishop.castle_flag(), false);
    EXPECT_EQ(black_promo_bishop_capture.castle_flag(), false);

    EXPECT_EQ(white_promo_queen.castle_flag(), false);
    EXPECT_EQ(white_promo_queen_capture.castle_flag(), false);
    EXPECT_EQ(black_promo_queen.castle_flag(), false);
    EXPECT_EQ(black_promo_queen_capture.castle_flag(), false);
}

TEST_F(ChessMoveTester, EP_Flag) {
    EXPECT_EQ(white_quiet_pawn.ep_flag(), false);
    EXPECT_EQ(black_quiet_pawn.ep_flag(), false);
    EXPECT_EQ(white_quiet_rook.ep_flag(), false);
    EXPECT_EQ(black_quiet_rook.ep_flag(), false);
    EXPECT_EQ(white_quiet_knight.ep_flag(), false);
    EXPECT_EQ(black_quiet_knight.ep_flag(), false);
    EXPECT_EQ(white_quiet_bishop.ep_flag(), false);
    EXPECT_EQ(black_quiet_bishop.ep_flag(), false);
    EXPECT_EQ(white_quiet_queen.ep_flag(), false);
    EXPECT_EQ(black_quiet_queen.ep_flag(), false);
    EXPECT_EQ(white_quiet_king.ep_flag(), false);
    EXPECT_EQ(black_quiet_king.ep_flag(), false);

    EXPECT_EQ(white_dpp.ep_flag(), false);
    EXPECT_EQ(black_dpp.ep_flag(), false);

    EXPECT_EQ(white_king.ep_flag(), false);
    EXPECT_EQ(black_king.ep_flag(), false);
    EXPECT_EQ(white_queen.ep_flag(), false);
    EXPECT_EQ(black_queen.ep_flag(), false);

    EXPECT_EQ(white_ep_left.ep_flag(), true);
    EXPECT_EQ(white_ep_right.ep_flag(), true);
    EXPECT_EQ(black_ep_left.ep_flag(), true);
    EXPECT_EQ(black_ep_right.ep_flag(), true);

    EXPECT_EQ(white_promo_rook.ep_flag(), false);
    EXPECT_EQ(white_promo_rook_capture.ep_flag(), false);
    EXPECT_EQ(black_promo_rook.ep_flag(), false);
    EXPECT_EQ(black_promo_rook_capture.ep_flag(), false);

    EXPECT_EQ(white_promo_knight.ep_flag(), false);
    EXPECT_EQ(white_promo_knight_capture.ep_flag(), false);
    EXPECT_EQ(black_promo_knight.ep_flag(), false);
    EXPECT_EQ(black_promo_knight_capture.ep_flag(), false);

    EXPECT_EQ(white_promo_bishop.ep_flag(), false);
    EXPECT_EQ(white_promo_bishop_capture.ep_flag(), false);
    EXPECT_EQ(black_promo_bishop.ep_flag(), false);
    EXPECT_EQ(black_promo_bishop_capture.ep_flag(), false);

    EXPECT_EQ(white_promo_queen.ep_flag(), false);
    EXPECT_EQ(white_promo_queen_capture.ep_flag(), false);
    EXPECT_EQ(black_promo_queen.ep_flag(), false);
    EXPECT_EQ(black_promo_queen_capture.ep_flag(), false);
}

TEST_F(ChessMoveTester, Flag) {
    EXPECT_EQ(white_quiet_pawn.flag(), QUIET_MOVE);
    EXPECT_EQ(black_quiet_pawn.flag(), QUIET_MOVE);
    EXPECT_EQ(white_quiet_rook.flag(), QUIET_MOVE);
    EXPECT_EQ(black_quiet_rook.flag(), QUIET_MOVE);
    EXPECT_EQ(white_quiet_knight.flag(), QUIET_MOVE);
    EXPECT_EQ(black_quiet_knight.flag(), QUIET_MOVE);
    EXPECT_EQ(white_quiet_bishop.flag(), QUIET_MOVE);
    EXPECT_EQ(black_quiet_bishop.flag(), QUIET_MOVE);
    EXPECT_EQ(white_quiet_queen.flag(), QUIET_MOVE);
    EXPECT_EQ(black_quiet_queen.flag(), QUIET_MOVE);
    EXPECT_EQ(white_quiet_king.flag(), QUIET_MOVE);
    EXPECT_EQ(black_quiet_king.flag(), QUIET_MOVE);

    EXPECT_EQ(white_dpp.flag(), DOUBLE_PUSH_PAWN);
    EXPECT_EQ(black_dpp.flag(), DOUBLE_PUSH_PAWN);

    EXPECT_EQ(white_king.flag(), KING_CASTLE);
    EXPECT_EQ(black_king.flag(), KING_CASTLE);
    EXPECT_EQ(white_queen.flag(), QUEEN_CASTLE);
    EXPECT_EQ(black_queen.flag(), QUEEN_CASTLE);

    EXPECT_EQ(white_ep_left.flag(), ENPASSANT);
    EXPECT_EQ(white_ep_right.flag(), ENPASSANT);
    EXPECT_EQ(black_ep_left.flag(), ENPASSANT);
    EXPECT_EQ(black_ep_right.flag(), ENPASSANT);

    EXPECT_EQ(white_promo_rook.flag(), ROOK_PROMO);
    EXPECT_EQ(white_promo_rook_capture.flag(), ROOK_PROMO_CAPTURE);
    EXPECT_EQ(black_promo_rook.flag(), ROOK_PROMO);
    EXPECT_EQ(black_promo_rook_capture.flag(), ROOK_PROMO_CAPTURE);

    EXPECT_EQ(white_promo_knight.flag(), KNIGHT_PROMO);
    EXPECT_EQ(white_promo_knight_capture.flag(), KNIGHT_PROMO_CAPTURE);
    EXPECT_EQ(black_promo_knight.flag(), KNIGHT_PROMO);
    EXPECT_EQ(black_promo_knight_capture.flag(), KNIGHT_PROMO_CAPTURE);

    EXPECT_EQ(white_promo_bishop.flag(), BISHOP_PROMO);
    EXPECT_EQ(white_promo_bishop_capture.flag(), BISHOP_PROMO_CAPTURE);
    EXPECT_EQ(black_promo_bishop.flag(), BISHOP_PROMO);
    EXPECT_EQ(black_promo_bishop_capture.flag(), BISHOP_PROMO_CAPTURE);

    EXPECT_EQ(white_promo_queen.flag(), QUEEN_PROMO);
    EXPECT_EQ(white_promo_queen_capture.flag(), QUEEN_PROMO_CAPTURE);
    EXPECT_EQ(black_promo_queen.flag(), QUEEN_PROMO);
    EXPECT_EQ(black_promo_queen_capture.flag(), QUEEN_PROMO_CAPTURE);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}