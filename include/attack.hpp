#ifndef CLOAK_ATTACK_HPP
#define CLOAK_ATTACK_HPP

#include "bitmask.hpp"
#include "defines.hpp"
#include "extern.hpp"

#ifdef USE_SSE
#include "magic.hpp"
#include <x86intrin.h>

__m128i but_not_A(__m128i b);
__m128i but_not_H(__m128i);
__m128i north_one(__m128i);
__m128i south_one(__m128i);
__m128i east_one(__m128i);
__m128i no_ea_one (__m128i);
__m128i so_ea_one(__m128i);
__m128i west_one(__m128i);
__m128i so_we_one(__m128i);
__m128i no_we_one(__m128i);
#endif // USE_SSE

Bitboard bishop_moves(Square_t sq, Bitboard occupied);
Bitboard rook_moves(Square_t sq, Bitboard occupied);
Bitboard queen_moves(Square_t sq, Bitboard occupied);
Bitboard knight_moves(Bitboard knights);
Bitboard king_moves(Bitboard king);
Bitboard xray_rook_moves(Square_t rook_sq, Bitboard occupied, Bitboard blockers);
Bitboard xray_bishop_moves(Square_t bishop_sq, Bitboard occupied, Bitboard blockers);
Bitboard xray_queen_moves(Square_t queen_sq, Bitboard occupied, Bitboard blockers);

#endif // CLOAK_ATTACK_HPP
