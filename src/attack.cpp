#include "attack.hpp"

#ifdef USE_SSE

__m128i but_not_A(__m128i b) {
    b = _mm_srli_epi64 (b, 1);
    b = _mm_add_epi8(b, b);
    return b;
}

__m128i but_not_H(__m128i b) {
    b = _mm_add_epi8   (b, b);
    b = _mm_srli_epi64 (b, 1);
    return b;
}

__m128i north_one(__m128i b) {
    b = _mm_slli_epi64 (b, 8);
    return b;
}

__m128i south_one(__m128i b) {
    b = _mm_srli_epi64 (b, 8);
    return b;
}

__m128i east_one(__m128i b) {
    b = _mm_add_epi8(b, b);
    return b;
}

__m128i no_ea_one (__m128i b) {
    b = _mm_add_epi8   (b, b);
    b = _mm_slli_epi64 (b, 8);
    return b;
}

__m128i so_ea_one (__m128i b) {
    b = _mm_add_epi8   (b, b);
    b = _mm_srli_epi64 (b, 8);
    return b;
}

__m128i west_one(__m128i b) {
    b = _mm_srli_epi64 (b, 1);
    b = _mm_add_epi8   (b, b);
    b = _mm_srli_epi64 (b, 1);
    return b;
}

__m128i so_we_one (__m128i b) {
    b = _mm_srli_epi64 (b, 1);
    b = _mm_add_epi8   (b, b);
    b = _mm_srli_epi64 (b, 9);
    return b;
}

__m128i no_we_one (__m128i b) {
    b = _mm_srli_epi64(b, 1);
    b = _mm_add_epi8(b, b);
    b = _mm_slli_epi64(b, 7);
    return b;
}

#endif // USE_SSE

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
 * Computes all pseudo-legal moves of specified knight
 * @param  sq square the knight occupies
 * @param  occupied bitboard of occupied squares
 * @return bitboard of pseudo-legal knight moves
 */
Bitboard knight_moves(Bitboard knights) {
    Square_t square = bitscan_forward(knights);
    Bitboard ret = 0;
    for ( ; knights != 0; knights -= C64(1) << square) {
        ret |= KNIGHT_ATTACK_ARRAY[square];
    }
    return ret;
}

/**
 * Computes all pseudo-legal moves of specified king
 * @param  sq square the king occupies
 * @param  occupied bitboard of occupied squares
 * @return bitboard of pseudo-legal king moves
 */
Bitboard king_moves(Bitboard king_bb) {
    Square_t sq = bitscan_forward(king_bb);
    return KING_ATTACK_ARRAY[sq];
}
