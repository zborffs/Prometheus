#include "chess_move.hpp"

/**
 * generic constructor that sets all member variables to default values
 */
ChessMove::ChessMove() : from_sq(A1), mf(NO_MOVE_FLAG), to_sq(A1), pf(NO_MOVE_FLAG), moved(NO_PIECE), captured(NO_PIECE), score(0) {

}

/**
 * constructor that sets all member variables to argument values
 * @param from_sq  square that the moved piece came from
 * @param to_sq    square that the moved piece went to
 * @param flag     MoveFlag (Quiet move, capture, promotion, castle, etc.)
 * @param moved    PieceType that was moved
 * @param captured PieceType that was captured (if one was)
 */
ChessMove::ChessMove(Square_t from_sq, Square_t to_sq, MoveFlag_t flag, PieceType_t moved, PieceType_t captured) :
        from_sq(from_sq), mf(flag & (MoveFlag_t)3), to_sq(to_sq), pf(flag >> (MoveFlag_t)2), moved(moved), captured(captured), score(0) {

    if (captured != NO_PIECE) {
        // if a piece was captured then compute a move score using the MVVLVA table
        score = MVVLVA[captured / 2 - 1][moved / 2 - 1];
    }

    // make sure we didn't move to the same square
    if (from_sq == to_sq) {
        assert(false);
    }
    assert(from_sq != to_sq);

    // make sure neither king was captured
    assert(captured != W_KING && captured != B_KING);
#ifndef NDEBUG
    // get the type of move
    MoveFlag_t fl = this->flag();

    if (captured != NO_PIECE) {
        // so long as a piece was captured, make sure the moved piece is a different color than the captured piece
        assert(moved % 2 != captured % 2);

        // if there is a piece being captured, make sure the capture flag is set.
        assert(fl & CAPTURE_MOVE);
    } else {
        // if no piece was captured, then the flag must not be a capture move.
        assert(!(fl & CAPTURE_MOVE));
    }

    if (fl == KING_CASTLE || fl == QUEEN_CASTLE) {
        // if the flag type was a castle, then no piece should have been captured
        assert(captured == NO_PIECE);

        // if the flag type was a castle, then either the black or white king should have moved
        assert(moved == W_KING || moved == B_KING);
    }

    if (flag == ENPASSANT) {
        // if it was an en passant move, then either a whie pawn or a black pawn should have moved
        assert(moved == W_PAWN || moved == B_PAWN);

        if (moved % 2 == WHITE) {
            // if a black pawn performed en passant it must have moved from a square on row 3 and to a square on row 2
            assert(from_sq / 8 == 4);
            assert(to_sq / 8 == 5);

            // if white pawn performed en passant, then it must have captured a black pawn
            assert(captured == B_PAWN);
        } else {
            // if the get to this point, we can infer that the piece that got moved is the black pawn

            // if a black pawn performed en passant it must have moved from a square on row 3 and to a square on row 2
            assert(from_sq / 8 == 3);
            assert(to_sq / 8 == 2);

            // if black pawn performed en passant, then it must have captured a white pawn
            assert(captured == W_PAWN);
        }
    }
#endif // NDEBUG
}

/**
 * getter method for promoted piece from MoveFlag member variables
 * @return the PieceType that was promoted
 */
PieceType_t ChessMove::promoted() noexcept {
    MoveFlag_t move_info = flag();
    if (!(move_info & PROMO)) {
        // if the 'PROMO' bit of the move flag isn't set, then no piece was promoted
        return NO_PIECE;
    }
    // equation converts any promoted MOVEFLAG into PieceType corresponding to the thing being promoted to
    return 2 * (move_info & ~CAPTURE_MOVE) - 12 + (!(to_sq / 8)); // NOLINT
}

/**
 * getter method for promoted piece from MoveFlag member variables
 * @return the PieceType that was promoted
 */
PieceType_t ChessMove::promoted() const noexcept {
    auto move_info = flag();
    if (!(move_info & PROMO)) {
        // if the 'PROMO' bit of the move flag isn't set, then no piece was promoted
        return NO_PIECE;
    }
    // equation converts any promoted MOVEFLAG into PieceType corresponding to the thing being promoted to
    return 2 * (move_info & ~CAPTURE_MOVE) - 12 + (!(to_sq / 8)); // NOLINT
}

/**
 * getter method the MoveFlag from mf and pf member variables
 * @return the Moveflag of this ChessMove
 */
MoveFlag_t ChessMove::flag() noexcept { // NOLINT
    return mf | (pf << 2); // NOLINT
}

/**
 * getter method the MoveFlag from mf and pf member variables
 * @return the Moveflag of this ChessMove
 */
MoveFlag_t ChessMove::flag() const noexcept {
    return mf | (pf << 2); // NOLINT
}

/**
 * getter for whether this was a Double push pawn move
 * @return true means that it WAS a dpp move, false means it wasn't a dpp move
 */
bool ChessMove::dpp_flag() noexcept {
    return flag() == DOUBLE_PUSH_PAWN;
}

/**
 * getter for whether this was a Double push pawn move
 * @return true means that it WAS a dpp move, false means it wasn't a dpp move
 */
bool ChessMove::dpp_flag() const noexcept {
    return flag() == DOUBLE_PUSH_PAWN;
}

/**
 * getter for whether this was a castling move
 * @return true means it WAS a castling moving, false means it wasn't a castling move
 */
bool ChessMove::castle_flag() noexcept {
    auto f = flag();
    return f == KING_CASTLE || f == QUEEN_CASTLE;
}

/**
 * getter for whether this was a castling move
 * @return true means it WAS a castling moving, false means it wasn't a castling move
 */
bool ChessMove::castle_flag() const noexcept {
    auto f = flag();
    return f == KING_CASTLE || f == QUEEN_CASTLE;
}

/**
 * getter for whether this was an enpassant move
 * @return true means it WAS an enpassant move, false means it wasn't an enpassant move
 */
bool ChessMove::ep_flag() noexcept {
    return flag() == ENPASSANT;
}

/**
 * getter for whether this was an enpassant move
 * @return true means it WAS an enpassant move, false means it wasn't an enpassant move
 */
bool ChessMove::ep_flag() const noexcept {
    return flag() == ENPASSANT;
}

/**
 * defines what it means for two ChessMove objects to be equal;
 * they are equal if all their member variables except for the score is equal, because the score can be different based
 * upon where the the search tree it is.
 * @param rhs a reference to the other ChessMove object
 * @return    true if the two ChessMove objects are equal, false otherwise
 */
bool ChessMove::operator==(const ChessMove& rhs) const {
    return from_sq == rhs.from_sq && to_sq == rhs.to_sq && mf == rhs.mf && pf == rhs.pf && moved == rhs.moved && captured == rhs.captured;
}

std::string ChessMove::to_algebraic_notation() noexcept {
    if (this->from_sq == this->to_sq) {
        return "ChessMove: Dummy";
    }

    auto f = this->flag();
    if(f == KING_CASTLE) {
        return "O-O";
    } else if (f == QUEEN_CASTLE) {
        return "O-O-O";
    }

    /// Simply obtaining some basic file and rank info.
    char from_file = (this->from_sq & 7) + 'a';
    char to_file = (this->to_sq & 7) + 'a';
    char to_rank = (this->to_sq / 8) + '1';
    char ptp_char;
    char ptm_char;

    /// Obtaining the piece that was promoted
    auto p = this->promoted();
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
    switch(this->moved) {
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

    std::string ret{};
    if (this->moved == W_PAWN || this->moved == B_PAWN) {
        if (this->captured != NO_PIECE) {
            ret = {from_file, 'x', to_file, to_rank};
            if (f == ENPASSANT) {
                return ret += "e.p.";
            }
        } else {
            ret = {to_file, to_rank};
        }

        if (p != NO_PIECE) {
            ret += '=' + ptp_char;
        }

        return ret;
    } else {
        if (this->captured != NO_PIECE) {
            ret = {ptm_char, 'x', to_file, to_rank};
            return ret;
        }
        ret = {ptm_char, to_file, to_rank};
        return ret;
    }
}

[[nodiscard]] std::string ChessMove::to_algebraic_notation() const noexcept {
    if (this->from_sq == this->to_sq) {
        return "ChessMove: Dummy";
    }

    auto f = this->flag();
    if(f == KING_CASTLE) {
        return "O-O";
    } else if (f == QUEEN_CASTLE) {
        return "O-O-O";
    }

    /// Simply obtaining some basic file and rank info.
    char from_file = (this->from_sq & 7) + 'a';
    char to_file = (this->to_sq & 7) + 'a';
    char to_rank = (this->to_sq / 8) + '1';
    char ptp_char;
    char ptm_char;

    /// Obtaining the piece that was promoted
    auto p = this->promoted();
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
    switch(this->moved) {
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

    std::string ret{};
    if (this->moved == W_PAWN || this->moved == B_PAWN) {
        if (this->captured != NO_PIECE) {
            ret = {from_file, 'x', to_file, to_rank};
            if (f == ENPASSANT) {
                return ret += "e.p.";
            }
        } else {
            ret = {to_file, to_rank};
        }

        if (p != NO_PIECE) {
            ret += '=' + ptp_char;
        }

        return ret;
    } else {
        if (this->captured != NO_PIECE) {
            ret = {ptm_char, 'x', to_file, to_rank};
            return ret;
        }
        ret = {ptm_char, to_file, to_rank};
        return ret;
    }
}

std::string ChessMove::to_string() noexcept {
    if (*this == ChessMove()) {
        return "";
    }

    char from_file = (this->from_sq & 7) + 'a';
    char to_file = (this->to_sq & 7) + 'a';
    char from_rank = (this->from_sq / 8) + '1';
    char to_rank = (this->to_sq / 8) + '1';

    auto p = this->promoted();
    char ptp_char;
    switch(p) {
        case W_QUEEN: //ptp_char = 'Q'; break;
        case B_QUEEN: ptp_char = 'q'; break;
        case W_KNIGHT: //ptp_char = 'N'; break;
        case B_KNIGHT: ptp_char = 'n'; break;
        case W_ROOK: //ptp_char = 'R'; break;
        case B_ROOK: ptp_char = 'r'; break;
        case W_BISHOP: //ptp_char = 'B'; break;
        case B_BISHOP: ptp_char = 'b'; break;
        default: ptp_char = ' '; break;
    }

    if (ptp_char == ' ') {
        std::string ret{from_file, from_rank, to_file, to_rank};
        return ret;
    } else {
        std::string ret{from_file, from_rank, to_file, to_rank, ptp_char};
        return ret;
    }
}

[[nodiscard]] std::string ChessMove::to_string() const noexcept {
    if (*this == ChessMove()) {
        return "";
    }

    char from_file = (this->from_sq & 7) + 'a';
    char to_file = (this->to_sq & 7) + 'a';
    char from_rank = (this->from_sq / 8) + '1';
    char to_rank = (this->to_sq / 8) + '1';

    auto p = this->promoted();
    char ptp_char;
    switch(p) {
        case W_QUEEN: //ptp_char = 'Q'; break;
        case B_QUEEN: ptp_char = 'q'; break;
        case W_KNIGHT: //ptp_char = 'N'; break;
        case B_KNIGHT: ptp_char = 'n'; break;
        case W_ROOK: //ptp_char = 'R'; break;
        case B_ROOK: ptp_char = 'r'; break;
        case W_BISHOP: //ptp_char = 'B'; break;
        case B_BISHOP: ptp_char = 'b'; break;
        default: ptp_char = ' '; break;
    }

    if (ptp_char == ' ') {
        std::string ret{from_file, from_rank, to_file, to_rank};
        return ret;
    } else {
        std::string ret{from_file, from_rank, to_file, to_rank, ptp_char};
        return ret;
    }
}
