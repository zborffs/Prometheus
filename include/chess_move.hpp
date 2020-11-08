//
// Created by Zach Bortoff on 2019-10-06.
//

#ifndef PROMETHEUS_CHESS_MOVE_HPP
#define PROMETHEUS_CHESS_MOVE_HPP

#include "defines.hpp"
#include <ostream>
#include <cassert>

static constexpr MoveScore MVVLVA[5][5] {
    // p, r, n, b, q
    {17,  8, 10,  9,  7}, // p
    {28, 20, 24, 23, 13}, // r
    {25, 14, 18, 16, 11}, // n
    {26, 15, 22, 19, 12}, // b
    {31, 27, 30, 29, 21}  // q
};

struct ChessMove {
    /// Member Variables
    Square_t from_sq : 6;
    MoveFlag_t mf : 2;
    Square_t to_sq : 6;
    MoveFlag_t pf : 2;
    PieceType_t moved : 4;
    PieceType_t captured : 4;
    MoveScore score;

    /// Constructors
    ChessMove();
    ChessMove(Square_t, Square_t, MoveFlag_t, PieceType_t, PieceType_t);

    bool operator==(const ChessMove& rhs) const;
    inline bool operator<(const ChessMove& rhs) const {
        return this->score > rhs.score;
    }

    /// Getters
    PieceType_t promoted() noexcept;
    [[nodiscard]] PieceType_t promoted() const noexcept;
    bool dpp_flag() noexcept;
    [[nodiscard]] bool dpp_flag() const noexcept;
    bool castle_flag() noexcept;
    [[nodiscard]] bool castle_flag() const noexcept;
    bool ep_flag() noexcept;
    [[nodiscard]] bool ep_flag() const noexcept;
    MoveFlag_t flag() noexcept;
    [[nodiscard]] MoveFlag_t flag() const noexcept;
    std::string to_algebraic_notation() noexcept;
    [[nodiscard]] std::string to_algebraic_notation() const noexcept;
    std::string to_string() noexcept;
    [[nodiscard]] std::string to_string() const noexcept;

    /**
     * prints the ChessMove object
     * @param os   the std::ostream& object
     * @param move the instance of the ChessMove object being printed
     * @return     a reference to the std::ostream passed into the function, now modified.
     */
    friend std::ostream& operator<<(std::ostream& os, const ChessMove& move) {
        if (move.from_sq == move.to_sq) {
            return os << "ChessMove: Dummy";
        }

        auto f = move.flag();
        if(f == KING_CASTLE) {
            return os << "O-O";
        } else if (f == QUEEN_CASTLE) {
            return os << "O-O-O";
        }

        /// Simply obtaining some basic file and rank info.
        char from_file = (move.from_sq & 7) + 'a';
        char to_file = (move.to_sq & 7) + 'a';
        int to_rank = (move.to_sq / 8) + 1;
        char ptp_char;
        char ptm_char;

        /// Obtaining the piece that was promoted
        auto p = move.promoted();
        switch(p) {
            case W_QUEEN:
            case B_QUEEN: ptp_char = 'Q'; break;
            case W_KNIGHT:
            case B_KNIGHT: ptp_char = 'N'; break;
            case W_ROOK:
            case B_ROOK: ptp_char = 'R'; break;
            case W_BISHOP:
            case B_BISHOP: ptp_char = 'B'; break;
            default: ptp_char = ' '; break;
        }

        /// Obtaining the piece that was moved
        switch(move.moved) {
            case W_QUEEN:
            case B_QUEEN: ptm_char = 'Q'; break;
            case W_KNIGHT:
            case B_KNIGHT: ptm_char = 'N'; break;
            case W_ROOK:
            case B_ROOK: ptm_char = 'R'; break;
            case W_BISHOP:
            case B_BISHOP: ptm_char = 'B'; break;
            case W_KING:
            case B_KING: ptm_char = 'K'; break;
            default: ptm_char = ' '; break;
        }

        if(move.moved == W_PAWN || move.moved == B_PAWN) {
            if(move.captured != NO_PIECE) {
                os << from_file << 'x' << to_file << to_rank;
                if(f == ENPASSANT) {
                    return os << "e.p.";
                }
            } else {
                os << to_file << to_rank;
            }

            if(p != NO_PIECE) {
                os << '=' << ptp_char;
            }

            return os;
        } else {
            if(move.captured != NO_PIECE) {
                return os << ptm_char << 'x' << to_file << to_rank;
            }
            return os << ptm_char << to_file << to_rank;
        }
    }
};

#endif //PROMETHEUS_CHESS_MOVE_HPP
