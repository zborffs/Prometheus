//
// Created by Zach Bortoff on 2020-06-03.
//

#include <random>
#include "board.hpp"


/**
 * default constructor sets all member variables to nominal values
 */
Board::Board() : key_(0), piece_bb_({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}), occupied_bb_(0), empty_bb_(0), capturable_({{0, 0}}), fif_move_counter_(0), ep_sq_(A1), castle_perm_(FULL), side_2_move_(WHITE), current_ply_(0) {
    set_board();
    init_all_keys();
}

/**
 * constructor with input being the starting position FEN string
 * @param fen
 */
Board::Board(const std::string& fen) : key_(0), piece_bb_({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}), occupied_bb_(0), empty_bb_(0), capturable_({{0, 0}}), fif_move_counter_(0), ep_sq_(A1), castle_perm_(FULL), side_2_move_(WHITE), current_ply_(0) {
    set_board(fen);
    init_all_keys();
}

/**
 * constructor with the file name and line number pointing to some FEN string to start position
 * @param file_name   path to file containing FENs
 * @param line_number line of desired FEN
 */
Board::Board(const std::string& file_name, const int line_number) : key_(0), piece_bb_({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}), occupied_bb_(0), empty_bb_(0), capturable_({{0, 0}}), fif_move_counter_(0), ep_sq_(A1), castle_perm_(FULL), side_2_move_(WHITE), current_ply_(0) {
    std::string fen;
    try {
        fen = read_fen_from_file(file_name, line_number);
    } catch(const std::exception& e) {
        throw std::logic_error(e.what());
    }
    set_board(fen);
    init_all_keys();
}

/**
 * initializes all keys to a random number
 */
void Board::init_all_keys() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0);

    for (int i = 0 ; i < 12 ; i++) {
        for (int j = 0 ; j < 64 ; j++) {
            POSITION_KEYS[i][j] = dis(gen);
        }
    }

    for (int i = 0 ; i < 2 ; i++) {
        SIDE_2_MOVE_KEYS[i] = dis(gen);
    }

    for (int i = 0 ; i < 16 ; i++) {
        CASTLE_PERMISSION_KEYS[i] = dis(gen);
    }

    for (int i = 0 ; i < 17 ; i++) {
        EN_PASSANT_KEYS[i] = dis(gen);
    }
}

/**
 * sets board to given fen string
 * @param fen input fen string
 */
void Board::set_board(const std::string& fen) {
    try {
        parse_fen(fen);
    } catch(std::exception& e) {
        throw std::logic_error(e.what());
    }

    init_pos_keys();
}

/**
 * sets board to given fen
 * @param first iterator to first element of fen string split by ' '
 * @param last  iterator to last element of fen string split by ' '
 */
void Board::set_board(std::vector<std::string>::const_iterator first, std::vector<std::string>::const_iterator last) {
    try {
        parse_fen(first, last);
    } catch(std::exception& e) {
        throw std::logic_error(e.what());
    }

    init_pos_keys();
}

/**
 * getter for board's PositionKey
 * @return PositionKey of the board
 */
PositionKey Board::key() noexcept {
    return key_;
}

/**
 * getter for board's PositionKey
 * @return PositionKey of the board
 */
PositionKey Board::key() const noexcept {
    return key_;
}

/**
 * getter for bitboard corresponding to requested piece type, which denotes location of all pieces of that piece type
 * @param piece_type desired piece type
 * @return           bitboard of desired piece type
 */
Bitboard Board::piece_bb(const PieceType_t piece_type) noexcept {
    return piece_bb_[piece_type];
}

/**
 * getter for bitboard corresponding to requested piece type, which denotes location of all pieces of that piece type
 * @param piece_type desired piece type
 * @return           bitboard of desired piece type
 */
Bitboard Board::piece_bb(const PieceType_t piece_type) const noexcept {
    return piece_bb_[piece_type];
}

/**
 * getter for bitboard denoting all occupied squares
 * @return occupied bitboard
 */
Bitboard Board::occupied_bb() noexcept {
    return occupied_bb_;
}

/**
 * getter for bitboard denoting all occupied squares
 * @return occupied bitboard
 */
Bitboard Board::occupied_bb() const noexcept {
    return occupied_bb_;
}

/**
 * getter for bitboard denoting all empty squares
 * @return empty bitboard
 */
Bitboard Board::empty_bb() noexcept {
    return empty_bb_;
}

/**
 * getter for bitboard denoting all empty squares
 * @return empty bitboard
 */
Bitboard Board::empty_bb() const noexcept {
    return empty_bb_;
}

/**
 * getter for bitboard denoting location of all capturable pieces by color
 * @param color given color
 * @return      capturable bitboard of given color
 */
Bitboard Board::capturable(const Color_t color) noexcept {
    return capturable_[color];
}

/**
 * getter for bitboard denoting location of all capturable pieces by color
 * @param color given color
 * @return      capturable bitboard of given color
 */
Bitboard Board::capturable(const Color_t color) const noexcept {
    return capturable_[color];
}

/**
 * getter for 50 move counter, denoting how many plys have been played since a capture, promotion, pawn move, or castle
 * @return 50 move counter
 */
uint8_t Board::fif_move_counter() noexcept {
    return fif_move_counter_;
}

/**
 * getter for 50 move counter, denoting how many plys have been played since a capture, promotion, pawn move, or castle
 * @return 50 move counter
 */
uint8_t Board::fif_move_counter() const noexcept {
    return fif_move_counter_;
}

/**
 * getter for en passant square denoting the square on which an en passant could happen this turn
 * @return en passant square
 */
Square_t Board::ep_sq() noexcept {
    return ep_sq_;
}

/**
 * getter for en passant square denoting the square on which an en passant could happen this turn
 * @return en passant square
 */
Square_t Board::ep_sq() const noexcept {
    return ep_sq_;
}

/**
 * getter for castle permission denoting which side can perform which sort of castle
 * @return castle permission
 */
CastleRights_t Board::castle_perm() noexcept {
    return castle_perm_;
}

/**
 * getter for castle permission denoting which side can perform which sort of castle
 * @return castle permission
 */
CastleRights_t Board::castle_perm() const noexcept {
    return castle_perm_;
}

/**
 * getter for side to move denoting whose turn it is to move in the current position
 * @return side to move
 */
Color_t Board::side_2_move() noexcept {
    return side_2_move_;
}

/**
 * getter for side to move denoting whose turn it is to move in the current position
 * @return side to move
 */
Color_t Board::side_2_move() const noexcept {
    return side_2_move_;
}

/**
 * getter for the ply we're currently on (how many moves have been played x2)
 * @return current ply
 */
Depth Board::current_ply() noexcept {
    return current_ply_;
}

/**
 * getter for the ply we're currently on (how many moves have been played x2)
 * @return current ply
 */
Depth Board::current_ply() const noexcept {
    return current_ply_;
}

/**
 * getter for the location of a given color's king
 * @param color given color
 * @return      Square the king of a given color is on
 */
Square_t Board::king_location(const Color_t color) noexcept {
    return bitscan_forward(piece_bb_[W_KING + color]);
}

/**
 * getter for the location of a given color's king
 * @param color given color
 * @return      Square the king of a given color is on
 */
Square_t Board::king_location(const Color_t color) const noexcept {
    return bitscan_forward(piece_bb_[W_KING + color]);
}

/**
 * getter for the piece type located a given square
 * @param sq the given square
 * @return   the piece type on the given square
 */
PieceType_t Board::piece_type(const Square_t sq) noexcept {
    for (int i = W_PAWN; i < NO_PIECE; i++) {
        if ((piece_bb_[i] >> sq) & 1) {
            return i;
        }
    }

    // if we didn't find the piece on the board, then return NO_PIECE
    return PieceType::NO_PIECE;
}

/**
 * getter for the piece type located a given square
 * @param sq the given square
 * @return   the piece type on the given square
 */
PieceType_t Board::piece_type(const Square_t sq) const noexcept {
    for (int i = W_PAWN; i < NO_PIECE; i++) {
        if ((piece_bb_[i] >> sq) & 1) {
            return i;
        }
    }

    // if we didn't find the piece on the board, then return NO_PIECE
    return PieceType::NO_PIECE;
}

/**
 * getter for the piece type located a given square and color (this narrows search space)
 * @param sq  given square to search
 * @param col given color of piece
 * @return    the piece type on the given square and of the given color
 */
PieceType_t Board::piece_type(const Square_t sq, const Color_t col) noexcept {
    for (int i = W_PAWN + col; i < NO_PIECE; i+=2) {
        if ((piece_bb_[i] >> sq) & 1) {
            return i;
        }
    }

    return PieceType::NO_PIECE;
}

/**
 * getter for the piece type located a given square and color (this narrows search space)
 * @param sq  given square to search
 * @param col given color of piece
 * @return    the piece type on the given square and of the given color
 */
PieceType_t Board::piece_type(const Square_t sq, const Color_t col) const noexcept {
    for (int i = W_PAWN + col; i < NO_PIECE; i+=2) {
        if ((piece_bb_[i] >> sq) & 1) {
            return i;
        }
    }

    return PieceType::NO_PIECE;
}

/**
 * getter for the key of the previous position
 * @return PositionKey of the previous position
 */
PositionKey Board::last_key() noexcept {
    return std::get<1>(move_history_.top());
}

/**
 * getter for the key of the previous position
 * @return PositionKey of the previous position
 */
PositionKey Board::last_key() const noexcept {
    return std::get<1>(move_history_.top());
}

/**
 * determines whether a given square is under attack by a given side: this is done by pretending that the attacking
 * piece is on the square in question and taking the intersection of the hypothetical piece's attack squares with the
 * attacking color's corresponding bitboards
 * @param sq              square that may or may not be under attack
 * @param attacking_color the color attacking the square
 * @return                true means it's being attacked, false means it's NOT being attacked
 */
bool Board::is_sq_attacked(const Square_t sq, const Color_t attacking_color) noexcept {
    /// if the diagmask intersecting with the opponent's bishop or queen contains a 1, then it's being attacked
    if (diag_mask(sq, occupied_bb_) & (piece_bb_[W_BISHOP + attacking_color] | piece_bb_[W_QUEEN + attacking_color])) {
        return true;

    /// if the horzvertmask intersecting with the opponent's rook or queen contains a 1, then it's being attacked
    } else if(horzvert_mask(sq, occupied_bb_) & (piece_bb_[W_ROOK + attacking_color] | piece_bb_[W_QUEEN + attacking_color])) {
        return true;

    /// if the knightmask intersecting with the opponent's knight contains a 1, then it's being attacked
    } else if(KNIGHT_ATTACK_ARRAY[sq] & piece_bb_[W_KNIGHT + attacking_color]) {
        return true;

    /// if the kingmask intersecting with the opponent's king contains a 1, then it's being attacked
    } else if(KING_ATTACK_ARRAY[sq] & piece_bb_[W_KING + attacking_color]) {
        return true;
    }

    /// if the attacking color is WHITE, then see if WHITE pawns are attacking the given square
    if (attacking_color == WHITE) {
        if(piece_bb_[W_PAWN] & (((C64(1) << (sq - 9)) & ~(RANK_MASK[RANK8] | FILE_MASK[FILEH])) | ((C64(1) << (sq - 7)) & ~(RANK_MASK[RANK8] | FILE_MASK[FILEA])))){
            return true;
        }
        /// if the attacking color is BLACK, then see if BLACK pawns are attacking the given square
    } else {
        if(piece_bb_[B_PAWN] & (((C64(1) << (sq + 9)) & ~(RANK_MASK[RANK1] | FILE_MASK[FILEA])) | ((C64(1) << (sq + 7)) & ~(RANK_MASK[RANK1] | FILE_MASK[FILEH])))){
            return true;
        }
    }

    /// otherwise return false
    return false;
}

/**
 * determines whether a given square is under attack by a given side: this is done by pretending that the attacking
 * piece is on the square in question and taking the intersection of the hypothetical piece's attack squares with the
 * attacking color's corresponding bitboards
 * @param sq              square that may or may not be under attack
 * @param attacking_color the color attacking the square
 * @return                true means it's being attacked, false means it's NOT being attacked
 */
bool Board::is_sq_attacked(const Square_t sq, const Color_t attacking_color) const noexcept {
    /// if the diagmask intersecting with the opponent's bishop or queen contains a 1, then it's being attacked
    if (diag_mask(sq, occupied_bb_) & (piece_bb_[W_BISHOP + attacking_color] | piece_bb_[W_QUEEN + attacking_color])) {
        return true;

        /// if the horzvertmask intersecting with the opponent's rook or queen contains a 1, then it's being attacked
    } else if(horzvert_mask(sq, occupied_bb_) & (piece_bb_[W_ROOK + attacking_color] | piece_bb_[W_QUEEN + attacking_color])) {
        return true;

        /// if the knightmask intersecting with the opponent's knight contains a 1, then it's being attacked
    } else if(KNIGHT_ATTACK_ARRAY[sq] & piece_bb_[W_KNIGHT + attacking_color]) {
        return true;

        /// if the kingmask intersecting with the opponent's king contains a 1, then it's being attacked
    } else if(KING_ATTACK_ARRAY[sq] & piece_bb_[W_KING + attacking_color]) {
        return true;
    }

    /// if the attacking color is WHITE, then see if WHITE pawns are attacking the given square
    if (attacking_color == WHITE) {
        if(piece_bb_[W_PAWN] & (((C64(1) << (sq - 9)) & ~(RANK_MASK[RANK8] | FILE_MASK[FILEH])) | ((C64(1) << (sq - 7)) & ~(RANK_MASK[RANK8] | FILE_MASK[FILEA])))){
            return true;
        }
        /// if the attacking color is BLACK, then see if BLACK pawns are attacking the given square
    } else {
        if(piece_bb_[B_PAWN] & (((C64(1) << (sq + 9)) & ~(RANK_MASK[RANK1] | FILE_MASK[FILEA])) | ((C64(1) << (sq + 7)) & ~(RANK_MASK[RANK1] | FILE_MASK[FILEH])))){
            return true;
        }
    }

    /// otherwise return false
    return false;
}

/**
 * determines whether the position is a draw by 50 move rule, by 3-Fold Repetition, or Insufficient Material (NOT
 * STALEMATE)
 * @return true means it's a draw, false means it's NOT a draw
 */
bool Board::is_draw() noexcept {
    /// Draw by fifty move rule
    if (fif_move_counter_ >= 100) {
        return true;
    }

    /// Draw by 3-Fold Repetition
    /// First check to make sure that we have made more than 8 fif_move_counter moves, indicating no pawn moves or
    /// and therefore indicate positions that can't come about again.
    if (fif_move_counter_ >= 8) {
        unsigned num_repititions{0};
        std::vector<MoveHistoryEntry> move_history_copy; move_history_copy.reserve(move_history_.size() - 1);
        /// Back track eight moves,
        for (unsigned i = 0; i < 8; i++) {
            auto move_history_entry = move_history_.top();
            move_history_copy.push_back(move_history_entry);
            PositionKey former_key = std::get<MoveHistoryIndex::POSKEY>(move_history_entry);
            Square_t former_ep_sq = std::get<MoveHistoryIndex::EP_SQUARE>(move_history_entry);
            CastleRights_t former_castle_perm = std::get<MoveHistoryIndex::CASTLE_PERM>(move_history_entry);

            former_key ^= CASTLE_PERMISSION_KEYS[former_castle_perm];

            if ((i % 4) == 3) {
                if (key_ == former_key) {
                    num_repititions++;
                }
            }
            move_history_.pop();
        }

        for (int k = (int)move_history_copy.size() - 1; k >= 0; k--) {
            move_history_.push(move_history_copy[k]);
        }

        if (num_repititions == 2) {
            return true;
        }
    }

    /// Draw by Insufficient Material
    if(pop_count(piece_bb_[W_PIECES]) <= 2 && pop_count(piece_bb_[B_PIECES]) <= 2) {
        // both sides have a king and a bishop, the bishops being the same color
        if(pop_count(piece_bb_[W_PIECES] | piece_bb_[B_PIECES]) == 4) {
            if((piece_bb_[W_PIECES] == (piece_bb_[W_KING] | piece_bb_[W_BISHOP])) &&
               (piece_bb_[B_PIECES] == (piece_bb_[B_KING] | piece_bb_[B_BISHOP])) &&
               (bitscan_forward(piece_bb_[W_BISHOP]) % 2 == bitscan_forward(piece_bb_[B_BISHOP]) % 2)) {
                return true;
            }
        } else {
            // If you have 1 minor piece + King vs. King, return true
            if((piece_bb_[W_KNIGHT] | piece_bb_[B_KNIGHT] | piece_bb_[W_BISHOP] | piece_bb_[B_BISHOP])) {
                return true;
            }
        }
    }

    return false;
}

/**
 * determines whether the king of a given color is checked
 * @param color the color the king which is being scrutinized
 * @return      true means it's in check, false means it's NOT in check
 */
bool Board::is_king_checked(Color_t color) noexcept {
    return is_sq_attacked(king_location(color), !color);
}

/**
 * determines whether the king of a given color is checked
 * @param color the color the king which is being scrutinized
 * @return      true means it's in check, false means it's NOT in check
 */
bool Board::is_king_checked(Color_t color) const noexcept {
    return is_sq_attacked(king_location(color), !color);
}

/**
 * stores the current position information in the move_history_ member variable, and replaces move with the move that's
 * just been played (i.e. new_move)
 * @param new_move the move that has just been played
 */
void Board::store_current_state(const ChessMove& new_move) noexcept {
    /// Take away the castle permission key from the key_
    key_ ^= CASTLE_PERMISSION_KEYS[castle_perm_];

    /// Take away the en passant key, if the en passant flag is set
    if (ep_sq_ != EP_DEFAULT) {
        key_ ^= EN_PASSANT_KEYS[ep_sq_];
    }

    /// store the current position information, which cannot be recovered strictly by undoing the move into the
    /// move_history_ object
    MoveHistoryEntry entry = std::make_tuple(move_, key_, castle_perm_, ep_sq_, fif_move_counter_);
    move_history_.push(entry);

    /// Update the current position's member variables based on nominal ChessMove data from new_move
    castle_perm_ &= CASTLE_CHECK[new_move.from_sq];
    castle_perm_ &= CASTLE_CHECK[new_move.to_sq];
    ep_sq_ = EP_DEFAULT; // we are assuming that the new en passant square is nothing unless otherwise told
    move_ = new_move;
    ++fif_move_counter_;

    /// reintegrate the castle permission hash into the key after the variable itself has been updated
    key_ ^= CASTLE_PERMISSION_KEYS[castle_perm_];

    /// increment the ply and the ply number
    ++current_ply_;
}

/**
 * resets the fifty move counter to 0: this happens on any move that can't be undone on the next move (ex: promotion,
 * pawn moves, captures, and castles)
 */
void Board::reset_fif_move_counter() noexcept {
    fif_move_counter_ = 0;
}

/**
 * sets the en passant square to the given en passant square
 * @param new_ep_sq the new en passant square
 */
void Board::set_ep_sq(const Square_t new_ep_sq) noexcept {
    /// if the en passant square currently has some value, then unmask the key_
    if(ep_sq_ != EP_DEFAULT) {
        key_ ^= EN_PASSANT_KEYS[ep_sq_];
    }

    /// update the en passant square variable and mask it on the key_
    ep_sq_ = new_ep_sq;
    key_ ^= EN_PASSANT_KEYS[ep_sq_];
}

/**
 * updates the side_2_move variable and updates the key
 */
void Board::swap_sides() noexcept {
    /// unmask the current side from the key
    key_ ^= SIDE_2_MOVE_KEYS[side_2_move_];

    /// set the side to move
    side_2_move_ = !side_2_move_;

    /// mask the current side to move using updated side to move variable
    key_ ^= SIDE_2_MOVE_KEYS[side_2_move_];
}

/**
 * restores the last position
 * @return the move that has just been undon
 */
ChessMove Board::restore_last_state() noexcept {
    /// save the move that is being undone
    ChessMove last_move = move_;

    /// swap the side to move
    swap_sides();

    /// decrement the number of plies and the current ply
    --current_ply_;

    /// if the en passant square is set, then unmask the key
    if (ep_sq_ != EP_DEFAULT) {
        key_ ^= EN_PASSANT_KEYS[ep_sq_];
    }

    /// unmask the castle permissions from the key
    key_ ^= CASTLE_PERMISSION_KEYS[castle_perm_];

    /// retrieve the data from the top of the stack and pop it off, saving the data saved therein into the member
    /// variables
    auto[m, checksum, cp, ep, fm] = move_history_.top();
    move_ = m;
    castle_perm_ = cp;
    ep_sq_ = ep;
    fif_move_counter_ = fm;
    move_history_.pop();

    /// remask the key with the updated en passant square if it's been set
    if (ep_sq_ != EP_DEFAULT) {
        key_ ^= EN_PASSANT_KEYS[ep_sq_];
    }

    /// remask the key with the updated castle permission
    key_ ^= CASTLE_PERMISSION_KEYS[castle_perm_];

    /// the move that we are undoing - this is needed to finish undoing the position
    return last_move;
}

/**
 * add (and undoes) capture moves from board
 * @param from_sq  from square of piece moved
 * @param to_sq    to square of piece moved
 * @param captured piece type that's been captured
 * @param moved    piece type that's been moved
 */
void Board::add_cap_move(const Square_t from_sq, const Square_t to_sq, const PieceType_t captured, const PieceType_t moved) noexcept {
    /// Zobrist Hashing
    key_ ^= POSITION_KEYS[moved - 2][from_sq];
    key_ ^= POSITION_KEYS[captured - 2][to_sq];
    key_ ^= POSITION_KEYS[moved - 2][to_sq];

    /// Incrementally updating bitboards
    Bitboard to_bb = C64(1) << to_sq;
    Bitboard from_bb = C64(1) << from_sq;
    Bitboard from2bb = from_bb ^ to_bb;
    piece_bb_[moved] ^= from2bb;
    piece_bb_[side_2_move_] ^= from2bb;
    piece_bb_[captured] ^= to_bb;
    piece_bb_[!side_2_move_] ^= to_bb;
    occupied_bb_ ^= from_bb;
    empty_bb_ ^= from_bb;

    if (moved != (W_KING + side_2_move_)) {
        capturable_[side_2_move_] ^= from2bb;
    }
    capturable_[!side_2_move_] ^= to_bb;
}

/**
 * adds (and undoes) quiet moves from board
 * @param from_sq from square of piece moved
 * @param to_sq   to square of piece moved
 * @param moved   piece type that's been moved
 */
void Board::add_qui_move(const Square_t from_sq, const Square_t to_sq, const PieceType_t moved) noexcept {
    /// Zobrist hashing
    key_ ^= POSITION_KEYS[moved - 2][from_sq];
    key_ ^= POSITION_KEYS[moved - 2][to_sq];

    /// incrementally updating bitboards
    Bitboard from2bb = C64(1) << from_sq ^ C64(1) << to_sq;
    piece_bb_[moved] ^= from2bb;
    piece_bb_[side_2_move_] ^= from2bb;
    occupied_bb_ ^= from2bb;
    empty_bb_ ^= from2bb;

    if(moved != (W_KING + side_2_move_)){
        capturable_[side_2_move_] ^= from2bb;
    }
}

/**
 * adds (and undoes) castling moves
 * @param from_sq the from square of the castle
 * @param to_sq   the to square of the castle
 */
void Board::add_cas_move(const Square_t from_sq, const Square_t to_sq) noexcept {
    /// Zobrist Keys
    key_ ^= POSITION_KEYS[W_KING + side_2_move_ - 2][from_sq];
    key_ ^= POSITION_KEYS[W_KING + side_2_move_ - 2][to_sq];

    /// Incrementally Update Bitboards
    Bitboard from2bb = C64(1) << from_sq ^ C64(1) << to_sq;
    piece_bb_[W_KING + side_2_move_] ^= from2bb;
    piece_bb_[side_2_move_] ^= from2bb;
    occupied_bb_ ^= from2bb;
    empty_bb_ ^= from2bb;

    switch(to_sq) {
        case C1:
            key_ ^= POSITION_KEYS[W_ROOK - 2][A1];
            key_ ^= POSITION_KEYS[W_ROOK - 2][D1];
            from2bb = C64(1) << A1 ^ C64(1) << D1;
            piece_bb_[W_ROOK] ^= from2bb;
            break;
        case C8:
            key_ ^= POSITION_KEYS[B_ROOK-2][A8];
            key_ ^= POSITION_KEYS[B_ROOK-2][D8];
            from2bb = C64(1) << A8 ^ C64(1) << D8;
            piece_bb_[B_ROOK] ^= from2bb;
            break;
        case G1:
            key_ ^= POSITION_KEYS[W_ROOK-2][H1];
            key_ ^= POSITION_KEYS[W_ROOK-2][F1];
            from2bb = C64(1) << H1 ^ C64(1) << F1;
            piece_bb_[W_ROOK] ^= from2bb;
            break;
        case G8:
            key_ ^= POSITION_KEYS[B_ROOK][H8];
            key_ ^= POSITION_KEYS[B_ROOK][F8];
            from2bb = C64(1) << H8 ^ C64(1) << F8;
            piece_bb_[B_ROOK] ^= from2bb;
            break;
    }

    piece_bb_[side_2_move_] ^= from2bb;
    occupied_bb_ ^= from2bb;
    empty_bb_ ^= from2bb;
    capturable_[side_2_move_] ^= from2bb;
}

/**
 * adds (and undoes) en passant moves
 * @param from_sq from square of the en passant move
 * @param to_sq   to square of the en passant move
 */
void Board::add_ep_move(const Square_t from_sq, const Square_t to_sq) noexcept {
    /// Zobrist hashing
    key_ ^= POSITION_KEYS[W_PAWN+side_2_move_-2][from_sq];
    key_ ^= POSITION_KEYS[W_PAWN+side_2_move_-2][to_sq];

    /// Incrementally update Bitboards
    Bitboard from2bb = C64(1) << from_sq ^ C64(1) << to_sq;
    piece_bb_[W_PAWN+side_2_move_] ^= from2bb;
    piece_bb_[side_2_move_] ^= from2bb;
    occupied_bb_ ^= from2bb;
    empty_bb_ ^= from2bb;

    capturable_[side_2_move_] ^= from2bb;

    Bitboard capture_bb;
    if(side_2_move_ == WHITE){
        key_ ^= POSITION_KEYS[B_PAWN - 2][to_sq - 8];
        capture_bb = C64(1) << (to_sq - 8);
        piece_bb_[B_PAWN] ^= capture_bb;
        piece_bb_[B_PIECES] ^= capture_bb;
        capturable_[BLACK] ^= capture_bb;
    } else {
        key_ ^= POSITION_KEYS[W_PAWN - 2][to_sq + 8];
        capture_bb = C64(1) << (to_sq + 8);
        piece_bb_[W_PAWN] ^= capture_bb;
        piece_bb_[W_PIECES] ^= capture_bb;
        capturable_[WHITE] ^= capture_bb;
    }

    occupied_bb_ ^= capture_bb;
    empty_bb_ ^= capture_bb;
}

/**
 * adds (and undoes) double push pawn moves
 * @param from_sq from square of pushed pawn
 * @param to_sq   to square of pushed pawn
 */
void Board::add_dpp_move(const Square_t from_sq, const Square_t to_sq) noexcept {
    /// Zobrist Hashing
    key_ ^= POSITION_KEYS[W_PAWN + side_2_move_ - 2][from_sq];
    key_ ^= POSITION_KEYS[W_PAWN + side_2_move_ - 2][to_sq];

    /// Incrementally Update Bitboards
    Bitboard from2bb = C64(1) << from_sq ^ C64(1) << to_sq;
    piece_bb_[W_PAWN + side_2_move_] ^= from2bb;
    piece_bb_[side_2_move_] ^= from2bb;
    occupied_bb_ ^= from2bb;
    empty_bb_ ^= from2bb;
    capturable_[side_2_move_] ^= from2bb;
}

void Board::add_pro_move(const Square_t from_sq, const Square_t to_sq, const PieceType_t captured, const PieceType_t promoted) noexcept {
    /// Zobrist Hashing
    key_ ^= POSITION_KEYS[W_PAWN + side_2_move_ - 2][from_sq];
    key_ ^= POSITION_KEYS[promoted - 2][to_sq];

    /// Incrementally Update Bitboards
    Bitboard from_bb = C64(1) << from_sq;
    Bitboard to_bb = C64(1) << to_sq;
    Bitboard from2bb = from_bb ^ to_bb;
    piece_bb_[W_PAWN + side_2_move_] ^= from_bb;
    piece_bb_[promoted] ^= to_bb;
    piece_bb_[side_2_move_] ^= from2bb;
    capturable_[side_2_move_] ^= from2bb;

    if(captured != NO_PIECE) {
        key_ ^= POSITION_KEYS[captured - 2][to_sq];
        piece_bb_[captured] ^= to_bb;
        piece_bb_[!side_2_move_] ^= to_bb;
        capturable_[!side_2_move_] ^= to_bb;
        occupied_bb_ ^= from_bb;
        empty_bb_ ^= from_bb;
        return;
    }

    occupied_bb_ ^= from2bb;
    empty_bb_ ^= from2bb;
}

/**
 * setter for the current ply
 * @param new_current_ply the new ply
 */
void Board::set_curr_ply(const Depth new_current_ply) noexcept {
    current_ply_ = new_current_ply;
}

/**
 * resets the board's member variables to their nominal values
 */
void Board::clear() noexcept {
    for (unsigned i = 0; i < piece_bb_.size(); i++) {
        piece_bb_[i] = 0;
    }
    empty_bb_ = 0;
    occupied_bb_ = 0;
    for (unsigned i = 0; i < capturable_.size(); i++) {
        capturable_[i] = 0;
    }

    move_ = ChessMove();
    fif_move_counter_ = 0;
    ep_sq_ = EP_DEFAULT;
    castle_perm_ = NONE;
    side_2_move_ = WHITE;
    key_ = 0;
    current_ply_ = 0;
    while (!move_history_.empty()) {
        move_history_.pop();
    }
}

/**
 * updates the internals of the position given a ChessMove
 * @param move the move that's just been made
 */
void Board::make_move(ChessMove& move) noexcept {
    /// store the current position
    store_current_state(move);

    /// if it's a castle move
    if (move.castle_flag()) {
        add_cas_move(move.from_sq, move.to_sq);
        swap_sides();
        return;
    }

    /// if it's an en passant move
    if (move.ep_flag()) {
        add_ep_move(move.from_sq, move.to_sq);
        swap_sides();
        reset_fif_move_counter();
        return;
    }

    /// if it's a double push pawn move
    if(move.dpp_flag()) {
        add_dpp_move(move.from_sq, move.to_sq);

        if (side_2_move_ == WHITE) {
            set_ep_sq(move.to_sq - 8);
        } else {
            set_ep_sq(move.to_sq + 8);
        }

        swap_sides();
        reset_fif_move_counter();
        return;
    }

    /// if it's a promotion
    PieceType_t promoted = move.promoted();
    if (promoted != NO_PIECE) {
        add_pro_move(move.from_sq, move.to_sq, move.captured, promoted);
        swap_sides();
        reset_fif_move_counter();
        return;
    }

    /// if it's a capture
    if (move.captured != NO_PIECE) {
        add_cap_move(move.from_sq, move.to_sq, move.captured, move.moved);
        swap_sides();
        reset_fif_move_counter();
        return;
    }

    /// otherwise it must be a quiet move
    add_qui_move(move.from_sq, move.to_sq, move.moved);
    swap_sides();

    /// if the moved piece was a pawn, reset the fifty move counter
    if (move.moved == W_PAWN || move.moved == B_PAWN) {
        reset_fif_move_counter();
    }
}

/**
 * unmake the move that was just played
 */
void Board::unmake_move() noexcept {
    /// restore the last position
    auto last_move = restore_last_state();

    /// if it was a castle
    if (last_move.castle_flag()) {
        add_cas_move(last_move.from_sq, last_move.to_sq);
        return;
    }

    /// if it was an en passant move
    if (last_move.ep_flag()) {
        add_ep_move(last_move.from_sq, last_move.to_sq);
        return;
    }

    /// if it was an double push pawn move
    if (last_move.dpp_flag()) {
        add_dpp_move(last_move.from_sq, last_move.to_sq);
        return;
    }

    /// if it was a promoted move
    auto promoted = last_move.promoted();
    if (promoted != NO_PIECE) {
        add_pro_move(last_move.from_sq, last_move.to_sq, last_move.captured, promoted);
        return;
    }

    /// if it was a capture
    if (last_move.captured != NO_PIECE) {
        add_cap_move(last_move.from_sq, last_move.to_sq, last_move.captured, last_move.moved);
        return;
    }


    /// otherwise it was a quiet move
    add_qui_move(last_move.from_sq, last_move.to_sq, last_move.moved);
}

/**
 * makes a null move, which is just not moving
 */
void Board::make_null_move() noexcept {
    ChessMove dummy;
    store_current_state(dummy);
    swap_sides();
}

/**
 * unmake a null move, which is just not moving
 */
void Board::unmake_null_move() noexcept {
    restore_last_state();
}

/**
 * parse the given fen string
 * @param fen the given fen string
 */
void Board::parse_fen(const std::string &fen) {
    std::vector<std::string> v = split(fen);
    parse_fen(v.begin(), v.end());
}

/**
 * parse the given fen string
 * @param first an iterator to the first element of a vector containing the fen string split by ' '
 * @param last  an iterator to the last element of a vector containing the fen string split by ' '
 */
void Board::parse_fen(std::vector<std::string>::const_iterator first, std::vector<std::string>::const_iterator last) {
    /// How do we know that fen_vec[n] is defined. Add Conditions!
    try {
        auto size = std::distance(first, last);
        clear();

        if(size >= 4) {
            def_piece_loc(*first);
            def_side2move(*(first + 1));
            def_castle_perm(*(first + 2));
            def_ep_sq(*(first + 3));

            if (size > 4) {
                auto itr = std::find(first, last, "bm");
                if (itr != last) {
                    /// ADD CONDITIONS SO THIS IS ONLY INCLUDED WHEN RUNNING TESTS.
                    best_move_ = *(itr + 1); // bm is next string
                    best_move_.erase(best_move_.end() - 1); // remove ';'
                }

                itr = std::min(itr, std::find(first, last, "id"));

                if (itr - first == 6) {
                    def_fif_move_count(*(itr - 2));
                    def_curr_ply(*(itr - 1));
                }
            }
        }
    } catch (std::runtime_error& e) {
        throw std::logic_error(e.what());
    }
}

/**
 * defines the piece locations given the piece location portion of the fen string
 * @param pl piece location portion of the fen string
 */
void Board::def_piece_loc(const std::string& pl) {
    int square = 56;
    uint64_t bit = (C64(1) << square);

    for(auto itr = pl.begin() ; itr != pl.end();
        bit = (C64(1) << square), ++itr) {
        switch (*itr) {
            case 'r':
                piece_bb_[B_ROOK] |= bit;
                piece_bb_[B_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[BLACK] |= bit;
                square++;
                break;
            case 'n':
                piece_bb_[B_KNIGHT] |= bit;
                piece_bb_[B_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[BLACK] |= bit;
                square++;
                break;
            case 'b':
                piece_bb_[B_BISHOP] |= bit;
                piece_bb_[B_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[BLACK] |= bit;
                square++;
                break;
            case 'q':
                piece_bb_[B_QUEEN] |= bit;
                piece_bb_[B_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[BLACK] |= bit;
                square++;
                break;
            case 'k':
                piece_bb_[B_KING] |= bit;
                piece_bb_[B_PIECES] |= bit;
                occupied_bb_ |= bit;
                square++;
                break;
            case 'p':
                piece_bb_[B_PAWN] |= bit;
                piece_bb_[B_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[BLACK] |= bit;
                square++;
                break;
            case 'R':
                piece_bb_[W_ROOK] |= bit;
                piece_bb_[W_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[WHITE] |= bit;
                square++;
                break;
            case 'N':
                piece_bb_[W_KNIGHT] |= bit;
                piece_bb_[W_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[WHITE] |= bit;
                square++;
                break;
            case 'B':
                piece_bb_[W_BISHOP] |= bit;
                piece_bb_[W_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[WHITE] |= bit;
                square++;
                break;
            case 'Q':
                piece_bb_[W_QUEEN] |= bit;
                piece_bb_[W_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[WHITE] |= bit;
                square++;
                break;
            case 'K':
                piece_bb_[W_KING] |= bit;
                piece_bb_[W_PIECES] |= bit;
                occupied_bb_ |= bit;
                square++;
                break;
            case 'P':
                piece_bb_[W_PAWN] |= bit;
                piece_bb_[W_PIECES] |= bit;
                occupied_bb_ |= bit;
                capturable_[WHITE] |= bit;
                square++;
                break;
            case '8':
                square += 8;
                break;
            case '7':
                square += 7;
                break;
            case '6':
                square += 6;
                break;
            case '5':
                square += 5;
                break;
            case '4':
                square += 4;
                break;
            case '3':
                square += 3;
                break;
            case '2':
                square += 2;
                break;
            case '1':
                square += 1;
                break;
            case '/':
                square -= 16;
                break;
            default:
                throw std::runtime_error("Board::def_piece_loc(const std::string&): Invalid Character!");
        }
    }
    empty_bb_ = ~occupied_bb_;
}

/**
 * defines the side to move from the side to move portion of the fen string
 * @param s2m the side to move portion of the fen string
 */
void Board::def_side2move(const std::string& s2m) {
    if(s2m[0] == 'w') {
        side_2_move_ = WHITE;
    } else if(s2m[0] == 'b') {
        side_2_move_ = BLACK;
    } else {
        throw std::runtime_error("Board::def_side2move(const std::string&): Invalid color character (choose \'w\' or \'b\')!");
    }
}

/**
 * defines the castle permissions given the castle permission portion of the fen string
 * @param cp castle permission portion of the fen string
 */
void Board::def_castle_perm(const std::string &cp) {
    for (int i = 0; i != cp.size(); i++) {
        switch(cp[i]) {
            case 'k':
                castle_perm_ |= BKC;
                break;
            case 'q':
                castle_perm_ |= BQC;
                break;
            case 'K':
                castle_perm_ |= WKC;
                break;
            case 'Q':
                castle_perm_ |= WQC;
                break;
            case '-':
                castle_perm_ = NONE;
                return;
            default:
                throw std::runtime_error("Board::def_castle_perm(const std::string&): Invalid Castle character!");
        }
    }
}

/**
 * defines the en passant square given the en passant portion of the FEN string
 * @param es en passant portion of the FEN string
 */
void Board::def_ep_sq(const std::string &es) {
    if(es[0] == '-') {
        ep_sq_ = EP_DEFAULT;
    } else {
        int file = es[0] - 'a';
        int rank = es[1] - '1';

        ep_sq_ = file + 8 * rank;
    }
}

/**
 * defines the fifty move counter given the fifty move counter portion of the fen string
 * @param fmc fifty move counter portion of the fen string
 */
void Board::def_fif_move_count(const std::string& fmc) {
    fif_move_counter_ = (Depth)std::stoi(fmc);
}

/**
 * defines the current ply given the current ply portion of the fen string
 * @param cp current ply portion of the fen string
 */
void Board::def_curr_ply(const std::string& cp) {
    current_ply_ = (Depth)std::stoi(cp);
}

/**
 * defines the best move from the current position from the best move portion of the FEN string (optional)
 * @param bm best move portion of the FEN string (optional)
 */
void Board::def_best_move(const std::string& bm) {
    best_move_ = bm.substr(0, bm.size() - 1);
}

/**
 * initializes the key from set member variables
 */
void Board::init_pos_keys() noexcept {
    key_ = 0;
    Bitboard board;

    for (int piece_type = W_PAWN, sq; piece_type <= B_KING; piece_type++) {
        board = piece_bb_[piece_type];
        for (;board; board -= C64(1) << sq) {
            sq = bitscan_forward(board);
            key_ ^= POSITION_KEYS[piece_type - 2][sq];
        }
    }

    key_ ^= SIDE_2_MOVE_KEYS[side_2_move_];
    key_ ^= CASTLE_PERMISSION_KEYS[castle_perm_];
    key_ ^= EN_PASSANT_KEYS[ep_sq_];
}