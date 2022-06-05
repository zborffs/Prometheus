#include "attack.hpp"

/**
 * Computes all pseudo-legal moves of specified bishop
 * @param  sq square the bishop occupies
 * @param  occupied bitboard of occupied squares
 * @return bitboard of pseudo-legal bishop moves
 */
Bitboard bishop_moves(Square_t sq, Bitboard occ) {
#ifdef USE_SSE
    #if defined (__GNUC__)
#if defined (__BMI2__)
    return attacks_bishop(sq, occ);
#endif // __BMI2__
#endif // __GNUC__
#else
    return diag_mask(sq, occ);
#endif // USE_SSE
}

/**
 * Computes all pseudo-legal moves of specified rook
 * @param  sq square the rook occupies
 * @param  occupied bitboard of occupied squares
 * @return bitboard of pseudo-legal rook moves
 */
Bitboard rook_moves(Square_t sq, Bitboard occ) {
#ifdef USE_SSE
    #if defined (__GNUC__)
#if defined (__BMI2__)
    return attacks_rook(sq, occ);
#endif // __BMI2__
#endif // __GNUC__
#else
    return horzvert_mask(sq, occ);
#endif
}

/**
 * Computes all pseudo-legal moves of specified queen
 * @param  sq square the queen occupies
 * @param  occupied bitboard of occupied squares
 * @return bitboard of pseudo-legal queen moves
 */
Bitboard queen_moves(Square_t sq, Bitboard occupied) {
#ifdef USE_SSE
    #if defined (__GNUC__)
#if defined (__BMI2__)
    return attacks_rook(sq, occupied) | attacks_bishop(sq, occupied);
#endif // __BMI2__
#endif // __GNUC__
#else
    return horzvert_mask(sq, occupied) | diag_mask(sq, occupied);
#endif
}

/**
 * Computes all pseudo-legal moves of a specified knight
 * @param  knights bitboard of the specified knight
 * @return bitboard of pseudo-legal knight moves
 */
Bitboard knight_moves(Bitboard knights) {
    Square_t square = bitscan_forward(knights); // find the square the knight is on
    Bitboard ret = 0;
    unsigned test{0};
    for ( ; knights != 0; knights -= C64(1) << square) {
        ++test;
        ret |= KNIGHT_ATTACK_ARRAY[square]; // OR together precomputed all the attack bitboards
    }
    assert(test == 1); // i'm pretty sure this loop is only gone through once... ever
    return ret;
}

/**
 * Computes all pseudo-legal moves of specified king
 * @param  king_bb the king bitboard
 * @return bitboard of pseudo-legal king moves
 */
Bitboard king_moves(Bitboard king_bb) {
    Square_t sq = bitscan_forward(king_bb);
    return KING_ATTACK_ARRAY[sq];
}

/**
 *
 * @param rook_sq
 * @param occupied
 * @param blockers
 * @return
 */
Bitboard xray_rook_moves(Square_t rook_sq, Bitboard occupied, Bitboard blockers) {
    Bitboard attacks = rook_moves(rook_sq, occupied);
    blockers &= attacks;
    return attacks ^ rook_moves(rook_sq, occupied ^ blockers);
}

/**
 *
 * @param bishop_sq
 * @param occupied
 * @param blockers
 * @return
 */
Bitboard xray_bishop_moves(Square_t bishop_sq, Bitboard occupied, Bitboard blockers) {
    Bitboard attacks = bishop_moves(bishop_sq, occupied);
    blockers &= attacks;
    return attacks ^ bishop_moves(bishop_sq, occupied ^ blockers);
}

/**
 *
 * @param queen_sq
 * @param occupied
 * @param blockers
 * @return
 */
Bitboard xray_queen_moves(Square_t queen_sq, Bitboard occupied, Bitboard blockers) {
    return xray_bishop_moves(queen_sq, occupied, blockers) | xray_rook_moves(queen_sq, occupied, blockers);
}