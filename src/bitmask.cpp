#include "bitmask.hpp"

/**
 * Converts bitboard to a nicely formated std::string
 * @param bb bitboard to draw
 * @return   std::string that represents the bitboard
 */
std::string draw_bitboard(const Bitboard bb) noexcept {
    std::string ret("8 ");
    int i = 56;
    int k = 7;
    for (; i >= 0; i++) {
        ret += ((bb >> i) & 0x1) ? ("1 ") : ("0 ");

        if (i % 8 == 7) {
            ret += "\n";
            i -= 16;

            if (k > 0) {
                ret += std::to_string(k);
                ret += " ";
                k--;
            }
        }
    }

    ret += "  A B C D E F G H\n";
    return ret;
}

/**
 * Determines the index of the least significant 1 bit (LSB1)
 * @param bb bitboard to scan
 * @return index of the LSB1
 */
int bitscan_forward(const Bitboard bb) {
#if defined (__GNUC__)
    return __builtin_ctzll(bb);
#else
    /**
     * @author Gerd Isenberg
     */
    unsigned int lsb;
    assert (bb != 0);
    bb &= -bb; // LS1B-Isolation
    lsb = (unsigned)bb | (unsigned)(bb>>32);
    return (((((((((((unsigned)(bb>>32) !=0)  * 2)
                 + ((lsb & 0xffff0000) !=0)) * 2)
                 + ((lsb & 0xff00ff00) !=0)) * 2)
                 + ((lsb & 0xf0f0f0f0) !=0)) * 2)
                 + ((lsb & 0xcccccccc) !=0)) * 2)
                 + ((lsb & 0xaaaaaaaa) !=0);
#endif // __GNUC__
}

/**
 * Determines the index of the most significant 1 bit (MSB1)
 * @param bb bb bitboard to scan
 * @return index of the MSB1
 */
int bitscan_reverse(const Bitboard bb) {
#if defined (__GNUC__)
    return __builtin_clzll(bb);
#else
    /**
     * @author Eugene Nalimov
     */
    int result = 0;
    if (bb > 0xFFFFFFFF) {
        bb >>= 32;
        result = 32;
    }
    if (bb > 0xFFFF) {
        bb >>= 16;
        result += 16;
    }
    if (bb > 0xFF) {
        bb >>= 8;
        result += 8;
    }
    return result + ms1bTable[bb];
#endif // __GNUC__
}

/**
 * Counts the number of 1 bits in the bitboard
 * @param bb bitboard to count
 * @return number of 1 bits
 */
int pop_count(const Bitboard bb) {
#if defined (__GNUC__)
    return __builtin_popcountll(bb);
#else
    /// need to implement my own version.
        return -1;
#endif // __GNUC__
}

/**
 * Fills all the squares north of the squares occupied in the bitboard 1s
 * @param bb bitboard to be filled
 * @return bitboard with squares filled
 */
Bitboard north_fill(Bitboard bb) {
    bb |= (bb << (unsigned)8);
    bb |= (bb << (unsigned)16);
    bb |= (bb << (unsigned)32);
    return bb;
}

/**
 * Fills all the squares south of the squares occupied in the bitboard with 1s
 * @param bb bitboard to be filled
 * @return bitboard with squares filled
 */
Bitboard south_fill(Bitboard bb) {
    bb |= (bb >> (unsigned)8);
    bb |= (bb >> (unsigned)16);
    bb |= (bb >> (unsigned)32);
    return bb;
}

/**
 * Fills all the squares both north and south of the squares occupied in the bitboard with 1s
 * @param bb bitboard to be filled
 * @return bitboard with squares filled
 */
Bitboard file_fill(Bitboard bb) {
    return north_fill(bb) | south_fill(bb);
}

/**
 * Flips the given bitboard vertically, so A1 goes to A8 and H8 goes to H1
 * @param bb bitboard to be flipped
 * @return flipped bitboard
 */
Bitboard flip_vertically(Bitboard bb) {
    return bswap_64(bb);
}

/**
 * Reverses the order of the bits of the bitboard (also akin to "folding" A1 to H8)
 * @param bb bitboard to be reversed
 * @return reversed bitboard
 */
Bitboard reverse(Bitboard bb) {
    for (int i = 0; i < 6; i++) {
        int s(1 << i);
        Bitboard f((Bitboard)(1) << s);
        Bitboard k((Bitboard)(-1) / (f + 1) );
        bb = ((bb >> s) & k) + f * (bb & k);
    }
    return bb;
}

/**
 * Computes the horizontal and vertical mask of the given square and occupied bitboard
 * @param sq square of the piece for which function computes horizontal and vertical mask
 * @param occupied occupied bitboard
 * @return east-west and north-south pseudo-legal moves bitboard
 */
Bitboard horzvert_mask(Square_t sq, Bitboard occupied) {
    Bitboard bb = C64(1) << sq;
    Bitboard occ = occupied;
    Bitboard shifted_bb = 2 * bb;
    Bitboard reverse_bb = reverse(bb);
    Bitboard shifted_rev_bb = 2 * reverse_bb;
    Bitboard file_bb = FILE_MASK[sq % 8];
    Bitboard rank_bb = RANK_MASK[sq / 8];

    Bitboard horiz_bb = (occ - shifted_bb) ^ reverse(reverse(occ) - shifted_rev_bb);
    Bitboard vert_bb = ((occ & file_bb) - (shifted_bb)) ^ reverse(reverse(occ & file_bb) - (shifted_rev_bb));
    return (horiz_bb & rank_bb) | (vert_bb & file_bb);
}

/**
 * Computes the diagonal mask of the given square and occupied bitboard
 * @param sq square of the piece for which function computes diagonal mask
 * @param occupied occupied bitboard
 * @return northeast-southwest and northwest-southeast pseudo-legal moves bitboard
 */
Bitboard diag_mask(Square_t sq, Bitboard occupied) {
    Bitboard bb = C64(1) << sq;
    Bitboard occ = occupied;
    Bitboard shifted_bb = 2 * bb;
    Bitboard reverse_bb = reverse(bb);
    Bitboard shifted_rev_bb = 2 * reverse_bb;
    Bitboard diag_mask = DIAGONAL_MASK[(sq / 8) + (sq % 8)];
    Bitboard anti_diag_mask = ANTI_DIAGONAL_MASK[(sq / 8) + 7 - (sq % 8)];

    Bitboard diagonalBB = ((occ & diag_mask) - (shifted_bb)) ^ reverse(reverse(occ & diag_mask) - (shifted_rev_bb));
    Bitboard antiDiagonalBB = ((occ & anti_diag_mask) - (shifted_bb)) ^ reverse(reverse(occ & anti_diag_mask) - (shifted_rev_bb));
    return (diagonalBB & diag_mask) | (antiDiagonalBB & anti_diag_mask);
}

/**
 * Reverses the order of the bits of the bitboard (also akin to "folding" A1 to H8)
 * @param bb bitboard to be flop-flopped
 * @return flip-flopped bitboard
 */
Bitboard flip_flop(Bitboard bb) {
    bb = flip_vertically(bb);
    bb = mirror(bb);
    return bb;
}

/**
 * Flips the board horizontally (A1 becomes H1 and A8 becomes H8)
 * @param bb bitboard to be mirrored
 * @return mirrored bitboard
 */
Bitboard mirror(Bitboard bb) {
    const Bitboard k1 = C64(0x5555555555555555);
    const Bitboard k2 = C64(0x3333333333333333);
    const Bitboard k4 = C64(0x0f0f0f0f0f0f0f0f);
    bb = ((bb >> 1) & k1) +  2*(bb & k1);
    bb = ((bb >> 2) & k2) +  4*(bb & k2);
    bb = ((bb >> 4) & k4) + 16*(bb & k4);
    return bb;
}

/**
 * Computes the dot product of a bitboard with weight vector
 * @param bb bitboard (represents existence of piece on certain square)
 * @param weights weight vector (represents score of piece being on a certain square)
 * @return score of bitboard / weight-vector combination
 */
Centipawns_t dot_product(Bitboard bb, const Centipawns_t* weights) {
    Bitboard bit = C64(1);
    int accu = 0;

    for (int sq = 0; sq < 64; ++sq, bit += bit) {
        accu += (bb & bit) ? (weights[sq] & -((bb & bit) == bit)) : (0);
    }

    return accu;
}

Bitboard in_between(Square_t sq1, Square_t sq2) {
    const Bitboard m1   = C64(-1);
    const Bitboard a2a7 = C64(0x0001010101010100);
    const Bitboard b2g7 = C64(0x0040201008040200);
    const Bitboard h1b7 = C64(0x0002040810204080); /* Thanks Dustin, g2b7 did not work for c1-a3 */
    Bitboard btwn, line, rank, file;

    btwn  = (m1 << sq1) ^ (m1 << sq2);
    file  =   (sq2 & 7) - (sq1   & 7);
    rank  =  ((sq2 | 7) -  sq1) >> 3 ;
    line  =      (   (file  &  7) - 1) & a2a7; /* a2a7 if same file */
    line += 2 * ((   (rank  &  7) - 1) >> 58); /* b1g1 if same rank */
    line += (((rank - file) & 15) - 1) & b2g7; /* b2g7 if same diagonal */
    line += (((rank + file) & 15) - 1) & h1b7; /* h1b7 if same antidiag */
    line *= btwn & -btwn; /* mul acts like shift by smaller square */
    return line & btwn;   /* return the bits on that line in-between */
}