#ifndef PROMETHEUS_ATTACK_HPP
#define PROMETHEUS_ATTACK_HPP

#include "bitmask.hpp"
#include "defines.hpp"
#include "extern.hpp"

#ifdef USE_SSE
#include "magic.hpp"
#include <x86intrin.h>
#endif // USE_SSE

Bitboard bishop_moves(Square_t sq, Bitboard occupied);
Bitboard rook_moves(Square_t sq, Bitboard occupied);
Bitboard queen_moves(Square_t sq, Bitboard occupied);
Bitboard knight_moves(Bitboard knights);
Bitboard king_moves(Bitboard king);
Bitboard xray_rook_moves(Square_t rook_sq, Bitboard occupied, Bitboard blockers);
Bitboard xray_bishop_moves(Square_t bishop_sq, Bitboard occupied, Bitboard blockers);
Bitboard xray_queen_moves(Square_t queen_sq, Bitboard occupied, Bitboard blockers);

#endif // PROMETHEUS_ATTACK_HPP
