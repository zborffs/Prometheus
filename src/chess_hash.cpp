//
// Created by Zach Bortoff on 2019-10-06.
//

#include "defines.hpp"
#include "chess_hash.hpp"

/**
 * default constructor gives nominal values for all member variables
 */
ChessHash::ChessHash()
: key(0), score(0), from_sq(A1), mf(NO_MOVE_FLAG), to_sq(A1), pf(NO_MOVE_FLAG), depth(0), hash_flag(NO_INFO), age(0) {

}

/**
 * Constructor given exact member variables values for every variable
 * @param k        Position Key (basically position index)
 * @param s        Evaluation from that position (given a certain depth)
 * @param fs       from square of move
 * @param ts       to square of move
 * @param moveflag type of move it was
 * @param d        depth search was at when it put this in
 * @param hashflag the type of move this was
 * @param a        the age (for determining how old this element is)
 */
ChessHash::ChessHash(PositionKey k, Centipawns_t s, Square_t fs, Square_t ts, MoveFlag_t moveflag, Depth d, HashFlag hashflag, Depth a)
: key(k), score(s), from_sq(fs), mf(moveflag & 3), to_sq(ts),  pf(moveflag >> 2), depth(d), hash_flag(hashflag), age(a) {

}

/**
 * the move flag as reconstructed by 'mf' and 'pf' member variables
 * @return MoveFlag_t of the ChessMove stored by this hash
 */
MoveFlag_t ChessHash::m_flag() noexcept {
    return mf | (pf << 2);
}

/**
 * the move flag as reconstructed by 'mf' and 'pf' member variables
 * @return MoveFlag_t of the ChessMove stored by this hash
 */
MoveFlag_t ChessHash::m_flag() const noexcept {
    return mf | (pf << 2);
}

/**
 * computes whether two hash values are equal by comparing purely board-state info
 * @param hash the ChessHash object being compared against
 * @return     whether the two ChessHash objects are from the same move and the same position
 */
bool ChessHash::operator==(const ChessHash& hash) noexcept {
    /// I need to decide what I mean by equal, to do that, I need to use this function.
    /// Equal could mean having the exact same member variables
    /// OR equal could mean having all member variables equal except ones that would change depending on the search information (like score, depth, hash flag, and age)
    return this->key == hash.key && this->from_sq == hash.from_sq && this->to_sq == hash.to_sq && this->m_flag() == hash.m_flag();
}

/**
 * computes whether two hash values are equal by comparing purely board-state info
 * @param hash the ChessHash object being compared against
 * @return     whether the two ChessHash objects are from the same move and the same position
 */
bool ChessHash::operator==(const ChessHash& hash) const noexcept {
    return this->key == hash.key && this->from_sq == hash.from_sq && this->to_sq == hash.to_sq && this->m_flag() == hash.m_flag();
}

/**
 * computes whether two hash values are NOT equal by comparing purely board-state info
 * @param hash the ChessHash object being compared against
 * @return     whether the two ChessHash objects are NOT from the same move and the same position
 */
bool ChessHash::operator!=(const ChessHash& hash) noexcept {
    return !(*this == hash);
}

/**
 * computes whether two hash values are NOT equal by comparing purely board-state info
 * @param hash the ChessHash object being compared against
 * @return     whether the two ChessHash objects are NOT from the same move and the same position
 */
bool ChessHash::operator!=(const ChessHash& hash) const noexcept {
    return !(*this == hash);
}

