#ifndef CLOAK_CHESS_HASH_HPP
#define CLOAK_CHESS_HASH_HPP

#include "defines.hpp"
#include <ostream>

struct ChessHash {
    /// Member Variables
    PositionKey key;
    Centipawns_t score;
    Square_t from_sq : 6;
    MoveFlag_t mf : 2;
    Square_t to_sq : 6;
    MoveFlag_t pf : 2;
    Depth depth : 5; // puts upper limit on search depth as 31
    HashFlag hash_flag : 3;
    Depth age; // upper limit here is 2^8-1 (255)

    /// Constructors
    ChessHash();
    ChessHash(PositionKey k, Centipawns_t s, Square_t fs, Square_t ts, MoveFlag_t moveflag, Depth d, HashFlag hashflag, Depth a);

    /// Getters
    MoveFlag_t m_flag() noexcept;
    MoveFlag_t m_flag() const noexcept;

    /// Equivalence Operators
    bool operator==(const ChessHash&) noexcept;
    bool operator==(const ChessHash&) const noexcept;
    bool operator!=(const ChessHash&) noexcept;
    bool operator!=(const ChessHash&) const noexcept;

    /**
     * Prints ChessHash object
     * @param os   reference to whatever output stream (like log file or std::cout)
     * @param hash the ChessHash object being returned
     * @return     the same reference to the output stream
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

        switch (hash.m_flag()) {
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
            case STORED_BETA: os << "Stored Beta (beta-cutoff)"; break;
            case STORED_ALPHA: os << "Stored Alpha (alpha-cutoff)"; break;
            default: break;
        }

        os << std::endl;
        os << "  Age: " << static_cast<int>(hash.age) << std::endl;

        return os;
    }
};

#endif // CLOAK_CHESS_HASH_HPP
