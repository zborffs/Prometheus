#include "magic.hpp"

#ifdef USE_SSE
#if defined (__GNUC__)
#if defined (__BMI2__)

uint64_t rook_masks[64];
uint64_t rook_masks2[64];
uint64_t bishop_masks[64];
uint64_t bishop_masks2[64];
uint16_t* rook_attacks[64];
uint16_t* bishop_attacks[64];
static uint16_t bishop_table[5248];
static uint16_t rook_table[102400];

void init_bmi2_fancy(uint16_t table[], uint16_t* attacks[], uint64_t masks[], uint64_t masks2[], bool is_rook) {
	uint64_t edges;
	uint64_t b;

	for(Square_t s = 0; s < 64; s++) {
		attacks[s] = table;

		edges = ((RANK_MASK[RANK1] | RANK_MASK[RANK8]) & ~RANK_MASK[s / 8]) |
				((FILE_MASK[FILEA] | FILE_MASK[FILEH]) & ~FILE_MASK[s % 8]);

		if(is_rook) {
			// masks2[s] = horzvert_mask(s, C64(1) << s);
			masks2[s] = horzvert_mask(s, 0);
			// draw_bitboard(masks2[s]);
		} else {
			// masks2[s] = diag_mask(s, C64(1) << s);
			masks2[s] = diag_mask(s, 0);
			// draw_bitboard(masks2[s]);
		}

		masks[s] = masks2[s] & ~edges;

		// b = C64(1) << s;
		b = 0;
		do {
			if(is_rook) {
				attacks[s][bmi2_index_rook(s, b)] = _pext_u64(horzvert_mask(s, b), masks2[s]);
			} else {
				attacks[s][bmi2_index_bishop(s, b)] = _pext_u64(diag_mask(s, b)  , masks2[s]);
			}

			b = (b - masks[s]) & masks[s];
			table++;
		} while (b);
	}
}

void init_sliding_attacks() {
	init_bmi2_fancy(bishop_table, bishop_attacks, bishop_masks, bishop_masks2, false);
	init_bmi2_fancy(rook_table, rook_attacks, rook_masks, rook_masks2, true);
}

#endif // __BMI2__
#endif // __GNUC__
#endif // USE_SSE
