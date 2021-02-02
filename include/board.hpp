#ifndef CLOAK_BOARD_HPP
#define CLOAK_BOARD_HPP

/// STL Includes
#include <algorithm>
#include <array>
#include <exception>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

/// Third-Party Includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

/// Project Includes
#include "bitmask.hpp"
#include "chess_move.hpp"
#include "defines.hpp"
#include "extern.hpp"
#include "string_manip.hpp"

class Board {
    /// Member Variables
    PositionKey key_;
    std::array<Bitboard, 14> piece_bb_;
    Bitboard occupied_bb_;
    Bitboard empty_bb_;
    std::array<Bitboard, 2> capturable_; // (all pieces &~ kingbb)
    Depth fif_move_counter_;
    Square_t ep_sq_;
    CastleRights_t castle_perm_;
    Color_t side_2_move_;
    Depth current_ply_;
    ChessMove move_; // move that was just made to achieve the current position
    std::string best_move_;

    using MoveHistoryEntry = std::tuple<ChessMove, PositionKey, CastleRights_t, Square_t, Depth>;
    using MoveHistory = std::stack<MoveHistoryEntry>;

    MoveHistory move_history_; // STACK DATASTRUCTURE!

public:
    /// Constructors
    Board();
    Board(const std::string& fen);
    Board(const std::string& file_name, const int line_number);
    Board(std::vector<std::string>::const_iterator first, std::vector<std::string>::const_iterator last);

    /// Initialization
    void set_board(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    void set_board(std::vector<std::string>::const_iterator start, std::vector<std::string>::const_iterator last);

    /// Getters
    PositionKey key() noexcept;
    PositionKey key() const noexcept;
    Bitboard piece_bb(const PieceType_t) noexcept;
    Bitboard piece_bb(const PieceType_t) const noexcept;
    Bitboard occupied_bb() noexcept;
    Bitboard occupied_bb() const noexcept;
    Bitboard empty_bb() noexcept;
    Bitboard empty_bb() const noexcept;
    Bitboard capturable(const Color_t color) noexcept;
    Bitboard capturable(const Color_t color) const noexcept;
    Depth fif_move_counter() noexcept;
    Depth fif_move_counter() const noexcept;
    Square_t ep_sq() noexcept;
    Square_t ep_sq() const noexcept;
    CastleRights_t castle_perm() noexcept;
    CastleRights_t castle_perm() const noexcept;
    Color_t side_2_move() noexcept;
    Color_t side_2_move() const noexcept;
    Depth current_ply() noexcept;
    Depth current_ply() const noexcept;
    Square_t king_location(const Color_t color) noexcept;
    Square_t king_location(const Color_t color) const noexcept;
    PieceType_t piece_type(const Square_t sq) noexcept;
    PieceType_t piece_type(const Square_t sq) const noexcept;
    PieceType_t piece_type(const Square_t sq, const Color_t col) noexcept;
    PieceType_t piece_type(const Square_t sq, const Color_t col) const noexcept;
    PositionKey last_key() noexcept;
    PositionKey last_key() const noexcept;
    bool is_sq_attacked(const Square_t sq, const Color_t attacking_color) noexcept;
    bool is_sq_attacked(const Square_t sq, const Color_t attacking_color) const noexcept;
    bool is_draw() noexcept;
    bool is_draw() const noexcept;
    bool is_king_checked(Color_t color) noexcept;
    bool is_king_checked(Color_t color) const noexcept;
    std::string best_move() noexcept;
    std::string best_move() const noexcept;

    /// Setters
    void store_current_state(const ChessMove& new_move) noexcept;
    void reset_fif_move_counter() noexcept;
    void set_ep_sq(const Square_t new_ep_sq) noexcept;
    void swap_sides() noexcept;
    ChessMove restore_last_state() noexcept;
    void add_cap_move(const Square_t from_sq, const Square_t to_sq, const PieceType_t captured, const PieceType_t moved) noexcept;
    void add_qui_move(const Square_t from_sq, const Square_t to_sq, const PieceType_t moved) noexcept;
    void add_cas_move(const Square_t from_sq, const Square_t to_sq) noexcept;
    void add_ep_move(const Square_t from_sq, const Square_t to_sq) noexcept;
    void add_dpp_move(const Square_t from_sq, const Square_t to_sq) noexcept;
    void add_pro_move(const Square_t from_sq, const Square_t to_sq, const PieceType_t captured, const PieceType_t promoted) noexcept;
    void set_curr_ply(const Depth new_current_ply) noexcept;
    void clear() noexcept;

    /// Make Move functions
    void make_move(ChessMove& move) noexcept;
    void unmake_move() noexcept;
    void make_null_move() noexcept;
    void unmake_null_move() noexcept;

    /**
     * prints the board variables
     * @param os    reference to std::ostream object
     * @param board reference to the board object to be printed
     * @return      updated reference to std::ostream& object
     */
    friend std::ostream& operator<<(std::ostream& os, Board& board) {
        os << std::boolalpha;
        int i;
        os << "========================= Board =========================\n";
        for(i=56;i>-1;i++) {
            if(i % 8 == 0){
                switch (i) {
                    case 56: os << "8 "; break;
                    case 48: os << "7 "; break;
                    case 40: os << "6 "; break;
                    case 32: os << "5 "; break;
                    case 24: os << "4 "; break;
                    case 16: os << "3 "; break;
                    case 8: os << "2 "; break;
                    case 0: os << "1 "; break;
                }
            }
            if((board.piece_bb(W_PAWN) >> i) & 0x1) {
                os << "P ";
            } else if((board.piece_bb(W_ROOK)>>i)&1) {
                os << "R ";
            } else if((board.piece_bb(W_KNIGHT)>>i)&1) {
                os << "N ";
            } else if((board.piece_bb(W_BISHOP)>>i)&1) {
                os << "B ";
            } else if((board.piece_bb(W_QUEEN)>>i)&1) {
                os << "Q ";
            } else if((board.piece_bb(W_KING)>>i)&1) {
                os << "K ";
            } else if((board.piece_bb(B_PAWN)>>i)&1) {
                os << "p ";
            } else if((board.piece_bb(B_ROOK)>>i)&1) {
                os << "r ";
            } else if((board.piece_bb(B_KNIGHT)>>i)&1) {
                os << "n ";
            } else if((board.piece_bb(B_BISHOP)>>i)&1) {
                os << "b ";
            } else if((board.piece_bb(B_QUEEN)>>i)&1) {
                os << "q ";
            } else if((board.piece_bb(B_KING)>>i)&1) {
                os << "k ";
            } else {
                os << "* ";
            }

            if(i % 8 == 7){
                os << "\n";
                i -= 16;
            }
        }

        os << "  A " << "B " << "C " << "D " << "E " << "F " << "G " << "H\n";

        switch(board.side_2_move()) {
            case WHITE:
                os << "\nWHITE ";
                break;
            case BLACK:
                os << "\nBLACK ";
                break;
        }

        os << "to move.\n";

        os << "Position Key: " << board.key() << std::endl;

        os << "En Passant Square: ";
        switch(board.ep_sq()) {
            case A3: os << "A3\n"; break;
            case B3: os << "B3\n"; break;
            case C3: os << "C3\n"; break;
            case D3: os << "D3\n"; break;
            case E3: os << "E3\n"; break;
            case F3: os << "F3\n"; break;
            case G3: os << "G3\n"; break;
            case H3: os << "H3\n"; break;

            case A6: os << "A6\n"; break;
            case B6: os << "B6\n"; break;
            case C6: os << "C6\n"; break;
            case D6: os << "D6\n"; break;
            case E6: os << "E6\n"; break;
            case F6: os << "F6\n"; break;
            case G6: os << "G6\n"; break;
            case H6: os << "H6\n"; break;

            case EP_DEFAULT: os << "No Square\n"; break;
            default: os << "ERROR\n"; break;
        }

        os << "Castle Permission: ";

        if(board.castle_perm() & WKC) {
            os << "K";
        }

        if(board.castle_perm() & WQC) {
            os << "Q";
        }

        if(board.castle_perm() & BKC) {
            os << "k";
        }

        if(board.castle_perm() & BQC) {
            os << "q";
        }

        os << std::endl << "Fifty Move Counter: " << (int)board.fif_move_counter() << std::endl;
        os << "Best Move: " << board.best_move_ << std::endl;
        os << "Is Draw: " << board.is_draw() << std::endl;

        os << "Move History: ";

        if (board.move_history_.size() >= 1) {
            std::vector<MoveHistoryEntry> move_history_copy; move_history_copy.reserve(board.move_history_.size() - 1);
            os << board.move_ << " ";
            while (board.move_history_.size() > 1) {
                auto move_history_entry = board.move_history_.top();
                move_history_copy.push_back(move_history_entry);
                os << std::get<MoveHistoryIndex::CHESSMOVE>(move_history_entry) << " ";
                board.move_history_.pop();
            }

            for (int k = (int)move_history_copy.size() - 1; k >= 0; k--) {
                board.move_history_.push(move_history_copy[k]);
            }
        } else {
            os << "Nothing";
        }

        os << std::noboolalpha;
        return os;
    }

protected:
    /// protoected setters
    void parse_fen(const std::string& fen);
    void parse_fen(std::vector<std::string>::const_iterator first, std::vector<std::string>::const_iterator last);
    void def_piece_loc(const std::string& pl);
    void def_side2move(const std::string& s2m);
    void def_castle_perm(const std::string& cp);
    void def_ep_sq(const std::string& es);
    void def_fif_move_count(const std::string& fmc);
    void def_curr_ply(const std::string& cp);
    void def_best_move(const std::string& bm);

    /// Initialization functions.
    void init_pos_keys() noexcept;
};

#endif // CLOAK_BOARD_HPP
