#ifndef CLOAK_DEFINES_HPP
#define CLOAK_DEFINES_HPP

#include <cstdint>

using Bitboard = uint64_t;
using PawnMap = uint64_t;
using PositionKey = uint64_t;
using Score = int; // For move ordering
using Depth = uint8_t;
//using MoveScore = uint8_t; // ???
using MoveScore = u_int32_t; // ???
using Square_t = uint8_t;
using Color_t = uint8_t;
using CastleRights_t = uint8_t;
using PieceType_t = uint8_t;
using MoveFlag_t = uint8_t;
using Rank_t = uint8_t;
using File_t = uint8_t;
using FileSet_t = uint8_t;
using Milliseconds_t = uint64_t;
using Nanoseconds_t = uint64_t;
using Centipawns_t = int32_t; // for Evaluation

#define C64(constantuint64) constantuint64 ##ULL

enum Square : uint8_t {
    A1 = 0, A2 =  8, A3 = 16, A4 = 24, A5 = 32, A6 = 40, A7 = 48, A8 = 56,
    B1 = 1, B2 =  9, B3 = 17, B4 = 25, B5 = 33, B6 = 41, B7 = 49, B8 = 57,
    C1 = 2, C2 = 10, C3 = 18, C4 = 26, C5 = 34, C6 = 42, C7 = 50, C8 = 58,
    D1 = 3, D2 = 11, D3 = 19, D4 = 27, D5 = 35, D6 = 43, D7 = 51, D8 = 59,
    E1 = 4, E2 = 12, E3 = 20, E4 = 28, E5 = 36, E6 = 44, E7 = 52, E8 = 60,
    F1 = 5, F2 = 13, F3 = 21, F4 = 29, F5 = 37, F6 = 45, F7 = 53, F8 = 61,
    G1 = 6, G2 = 14, G3 = 22, G4 = 30, G5 = 38, G6 = 46, G7 = 54, G8 = 62,
    H1 = 7, H2 = 15, H3 = 23, H4 = 31, H5 = 39, H6 = 47, H7 = 55, H8 = 63, EP_DEFAULT = 64
};

enum CastleRights : uint8_t {
    WKC  = 1,
    WQC  = 2,
    BKC  = 4,
    BQC  = 8,
    FULL = 15,
    NONE = 0
};

enum Color : uint8_t {
    WHITE = 0,
    BLACK = 1,
    BOTH  = 2
};

enum PieceType : uint8_t {
    W_PIECES = 0, // 0b0000
    B_PIECES = 1, // 0b0001
    W_PAWN = 2, // 0b0010
    B_PAWN = 3, // 0b0011
    W_ROOK = 4, // 0b0100
    B_ROOK = 5, // 0b0101
    W_KNIGHT = 6, // 0b0110
    B_KNIGHT = 7, // 0b0111
    W_BISHOP = 8, // 0b1000
    B_BISHOP = 9, // 0b1001
    W_QUEEN = 10, // 0b1010
    B_QUEEN = 11, // 0b1011
    W_KING = 12, // 0b1100
    B_KING = 13, // 0b1101
    NO_PIECE = 14 // 0b1110
};

enum MoveFlag : uint8_t {
    QUIET_MOVE = 0,
    DOUBLE_PUSH_PAWN = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE_MOVE = 4,
    ENPASSANT = 5,
    ROOK_PROMO = 8,
    KNIGHT_PROMO = 9,
    BISHOP_PROMO = 10,
    QUEEN_PROMO = 11,
    ROOK_PROMO_CAPTURE = 12,
    KNIGHT_PROMO_CAPTURE = 13,
    BISHOP_PROMO_CAPTURE = 14,
    QUEEN_PROMO_CAPTURE = 15,
    PROMO = 8,
    NO_MOVE_FLAG = 0
};

enum Rank : uint8_t {
    RANK1 = 0,
    RANK2 = 1,
    RANK3 = 2,
    RANK4 = 3,
    RANK5 = 4,
    RANK6 = 5,
    RANK7 = 6,
    RANK8 = 7
};

enum File : uint8_t {
    FILEA = 0,
    FILEB = 1,
    FILEC = 2,
    FILED = 3,
    FILEE = 4,
    FILEF = 5,
    FILEG = 6,
    FILEH = 7,
};

enum Result : uint8_t {
    NO_RESULT,
    WHITE_IS_MATED,
    BLACK_IS_MATED,
    DRAW,
    STALEMATE
};

enum HashFlag : uint8_t { // 3 bits
    NO_INFO = 0,
    AVOID_NULL = 1,
    EXACT = 5,
    UPPER_BOUND = 3,
    LOWER_BOUND = 4
};

enum PositionType : uint8_t { // 2 bits
    CLOSED = 0,
    SEMI_CLOSED = 1,
    SEMI_OPEN = 2,
    OPEN = 3
};

enum GameStage : uint8_t { // 2 bits
    OPENING = 0,
    MID_GAME = 1,
    EARLY_END_GAME = 2,
    LATE_END_GAME = 3
};

enum Style : uint8_t {
    AGGRESSIVE,
    BALANCED,
    DEFENSIVE
};

enum MoveHistoryIndex {
    CHESSMOVE = 0,
    POSKEY = 1,
    CASTLE_PERM = 2,
    EP_SQUARE = 3,
    FIF_MOVE_COUNT = 4
};

#endif // CLOAK_DEFINES_HPP
