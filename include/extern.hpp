#ifndef CLOAK_EXTERN_HPP
#define CLOAK_EXTERN_HPP

#include "defines.hpp"
#include "uci_options.hpp"
#include <string>
#include "board.hpp"

extern const std::string logger_name;
extern const std::string engine_name;
extern const std::string version;
extern const std::string author;

extern const unsigned int MAX_DEPTH;
extern const unsigned int MIN_DEPTH;

extern const Centipawns_t INF;

extern const uint8_t CASTLE_CHECK[64];

extern PositionKey POSITION_KEYS[12][64];
extern PositionKey SIDE_2_MOVE_KEYS[2];
extern PositionKey CASTLE_PERMISSION_KEYS[16];
extern PositionKey EN_PASSANT_KEYS[17];

extern const Bitboard ANTI_DIAGONAL_MASK[15];
extern const Bitboard DIAGONAL_MASK[15];
extern const Bitboard RANK_MASK[8];
extern const Bitboard FILE_MASK[8];

extern const Bitboard KING_ATTACK_ARRAY[64];
extern const Bitboard KNIGHT_ATTACK_ARRAY[64];

/// Evaluation Parameters
extern const Centipawns_t PAWN_BASE_VAL;
extern const Centipawns_t BISHOP_BASE_VAL;
extern const Centipawns_t KNIGHT_BASE_VAL;
extern const Centipawns_t ROOK_BASE_VAL;
extern const Centipawns_t QUEEN_BASE_VAL;

extern const Centipawns_t TEMPO;
extern const double QUEEN_PERC[4];
extern const double ROOK_PERC[4];
extern const double BISHOP_PERC[4];
extern const double KNIGHT_PERC[4];
extern const double PAWN_STRENGTH[4];
extern const double CONNECTIVITY[5];
extern const Centipawns_t KNIGHT_MOBILITY;
extern const Centipawns_t KNIGHT_OUTPOST_BONUS;
extern const Centipawns_t KNIGHT_PST[64];

extern const Centipawns_t OM_ROOK_MOBILITY;
extern const Centipawns_t E_ROOK_MOBILITY;
extern const Centipawns_t ROOK_ON_HALF_OPEN_FILE_BONUS;
extern const Centipawns_t ROOK_ON_OPEN_FILE_BONUS;
extern const Centipawns_t ROOK_ON_OPEN_FILE_BONUS_NO_OPPOSING_ROOK;
extern const Centipawns_t ROOK_SUPPORTING_PASSED_PAWNS;
extern const Centipawns_t CONNECTED_ROOK_BONUS;
extern const Centipawns_t ROOK_PST[64];
extern const Centipawns_t ROOK_ON_SEVENTH;

extern const Centipawns_t OM_BISHOP_MOBILITY;
extern const Centipawns_t BISHOP_PST[64];
extern const Centipawns_t BISHOP_PAIR_BONUS;
extern const Centipawns_t BISHOP_ON_LONG_DIAGONAL;
extern const Centipawns_t BISHOP_ON_OPEN_LONG_DIAGONAL;

extern const Centipawns_t QUEEN_MOBILITY;
extern const Centipawns_t QUEEN_PST[64];

extern const Centipawns_t QUEEN_OPENING_QUICK_PENALTY;

extern const Centipawns_t OM_KING_MOBILITY;
extern const Centipawns_t E_KING_MOBILITY;
extern const Centipawns_t M_KING_PST[64];

extern const Centipawns_t E_KING_PST[64];

extern const Centipawns_t PAWN_MOBILITY;
extern const Centipawns_t PAWN_PST[64];
extern const Centipawns_t PASSED_PAWN[8];

extern const Centipawns_t PAWN_UNOPPOSED_PAWN;
extern const Centipawns_t PAWN_BACKWARD;
extern const Centipawns_t PAWN_BLOCKING;
extern const Centipawns_t PAWN_DOUBLE;
extern const Centipawns_t PAWN_ISOLATED;
extern const Centipawns_t PAWN_KING_SHELTER;
extern const Centipawns_t PAWN_IN_CHAIN;
extern const Centipawns_t PAWN_AS_ROOT_OF_CHAIN;
extern const Centipawns_t PAWN_AS_HEAD_OF_CHAIN;
extern const Centipawns_t PAWN_ATTACKING_ENEMY_CAMP;
extern const Centipawns_t PAWN_HANGING;
extern const Centipawns_t PAWN_ISLAND[4];
extern const Centipawns_t HALF_ISOLATED_PAWN;

/// Search Parameters
extern const Centipawns_t WINDOW_SIZE[3];
extern const Depth NULL_MOVE_R;
extern const double NULL_MOVE_ADAPTIVITY;
extern const Depth NULL_MOVE_ADAPTIVITY_DEPTH;
extern const Depth FUTILITY_DEPTH;
extern const Centipawns_t FUTILITY_THRESHOLD_DEPTH_1;
extern const Centipawns_t FUTILITY_THRESHOLD_DEPTH_2;
extern const Centipawns_t FUTILITY_THRESHOLD_DEPTH_3;
extern const Depth MULTICUT_R;
extern const Depth MULTICUT_DEPTH;
extern const Depth MULTICUT_M;
extern const Depth MULTICUT_C;
extern const Depth CHECKOUT_EXTENSION;
extern const Depth ONE_REPLY_EXTENSION;
extern const Depth RECAPTURE_EXTENSION;
extern const Depth PASSED_PAWN_EXTENSION_SEVENTH;
extern const Depth MATE_THREAT_EXTENSION;
extern const Depth LATE_MOVE_REDUCTION_PLY_DISCOUNT;
extern const Depth LATE_MOVE_REDUCTION_DISCOUNT_NUMBER;
extern const double LATE_MOVE_REDUCTION_DISCOUNT_PERCENTAGE;
extern const int AGE_DIFFERENCE_THRESHOLD;

#endif // CLOAK_EXTERN_HPP
