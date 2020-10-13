//
// Created by Zach Bortoff on 2019-10-06.
//

#ifndef PROMETHEUS_CHESS_HASH_HPP
#define PROMETHEUS_CHESS_HASH_HPP

#include "defines.hpp"
#include <ostream>

struct ChessHash {
    /// Member Variables
    PositionKey key;
    Score score;
    Square_t from_sq : 6;
    MoveFlag_t mf : 2;
    Square_t to_sq : 6;
    MoveFlag_t pf : 2;
    Depth depth : 5;
    HashFlag hash_flag : 3;
    Depth age;

    /// Constructors
    ChessHash();
    ChessHash(PositionKey, Score, Square_t, Square_t, MoveFlag_t, Depth, HashFlag, Depth);

    /// Getters
    MoveFlag_t flag() noexcept;
    MoveFlag_t flag() const noexcept;

    /// Equivalence Operators
    bool operator==(const ChessHash&) noexcept;
    bool operator==(const ChessHash&) const noexcept;
    bool operator!=(const ChessHash&) noexcept;
    bool operator!=(const ChessHash&) const noexcept;

    /**
     * Prints ChessHash object
     * @param os
     * @param hash
     * @return
     */
    friend std::ostream& operator<<(std::ostream& os, const ChessHash& hash) {
        if (hash.from_sq == hash.to_sq) {
            return os << "ChessHash: Dummy";
        }

        char from_file = (hash.from_sq & 7) + 'a';
        int from_rank = (hash.from_sq / 8) + 1;
        char to_file = (hash.to_sq & 7) + 'a';
        int to_rank = (hash.to_sq / 8) + 1;

        os << "ChessHash:" << std::endl;
        os << "- Position Key: " << hash.key << std::endl;
        os << "  Score: " << hash.score << std::endl;
        os << "  From Square: " << from_file << from_rank << std::endl;
        os << "  To Square: " << to_file << to_rank << std::endl;
        os << "  Move Flag: ";

        switch (hash.flag()) {
            case QUIET_MOVE: os << "Quiet Move"; break;
            case DOUBLE_PUSH_PAWN: os << "Double Push Pawn"; break;
            case KING_CASTLE: os << "King Castle"; break;
            case QUEEN_CASTLE: os << "Queen Castle"; break;
            case CAPTURE_MOVE: os << "Capture"; break;
            case ENPASSANT: os << "En Passant"; break;
            case ROOK_PROMO: os << "Rook Promotion"; break;
            case KNIGHT_PROMO: os << "Knight Promotion"; break;
            case BISHOP_PROMO: os << "Bishop Promotion"; break;
            case QUEEN_PROMO: os << "Queen Promotion"; break;
            case ROOK_PROMO_CAPTURE: os << "Rook Promotion and Capture"; break;
            case KNIGHT_PROMO_CAPTURE: os << "Knight Promotion and Capture"; break;
            case BISHOP_PROMO_CAPTURE: os << "Bishop Promotion and Capture"; break;
            case QUEEN_PROMO_CAPTURE: os << "Queen Promotion and Capture"; break;
        }

        os << std::endl;

        os << "  Depth: " << static_cast<int>(hash.depth) << std::endl;
        os << "  Hash Flag: ";

        switch (hash.hash_flag) {
            case NO_INFO: os << "No Information"; break;
            case AVOID_NULL: os << "Avoid Null Move"; break;
            case EXACT: os << "Exact"; break;
            case UPPER_BOUND: os << "Upper Bound"; break;
            case LOWER_BOUND: os << "Lower Bound"; break;
            default: break;
        }

        os << std::endl;
        os << "  Age: " << static_cast<int>(hash.age) << std::endl;

        return os;
    }
};

#endif //PROMETHEUS_CHESS_HASH_HPP
