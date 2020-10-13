//
// Created by Zach Bortoff on 2019-11-23.
//

#ifndef PROMETHEUS_MAGIC_HPP
#define PROMETHEUS_MAGIC_HPP

#include "defines.hpp"
#include "extern.hpp"
#include "bitmask.hpp"
#include <x86intrin.h>

#ifdef USE_SSE
#if defined (__GNUC__)
#if defined (__BMI2__)

extern uint16_t* rook_attacks[64];
extern uint16_t* bishop_attacks[64];
extern uint64_t rook_masks[64];
extern uint64_t rook_masks2[64];
extern uint64_t bishop_masks[64];
extern uint64_t bishop_masks2[64];
// extern uint16 bishop_table[5248];
// extern uint16 rook_table[102400];

void init_bmi2_fancy(uint16_t table[], uint16_t* attacks[], uint64_t masks[], uint64_t masks2[], bool is_rook);

inline Bitboard bmi2_index_bishop(Square_t square, Bitboard occupied) {
	return (uint16_t)_pext_u64(occupied, bishop_masks[square]);
}

inline Bitboard bmi2_index_rook(Square_t square, Bitboard occupied) {
	return (uint16_t)_pext_u64(occupied, rook_masks[square]);
}

inline Bitboard attacks_bishop(Square_t square, Bitboard occupied) {
	return _pdep_u64(bishop_attacks[square][bmi2_index_bishop(square, occupied)], bishop_masks2[square]);
}

inline Bitboard attacks_rook(Square_t square, Bitboard occupied) {
	return _pdep_u64(rook_attacks[square][bmi2_index_rook(square, occupied)], rook_masks2[square]);
}

void init_sliding_attacks();

#endif // __BMI2__
#endif // __GNUC__
#endif // USE_SSE

#endif //PROMETHEUS_MAGIC_HPP
