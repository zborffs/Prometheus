//
// Created by Zach Bortoff on 2019-11-03.
//

#ifndef PROMETHEUS_GLOBALS_HPP
#define PROMETHEUS_GLOBALS_HPP

#include "defines.hpp"
#include "extern.hpp"
#include "uci_options.hpp"
#include <string>
#include "board.hpp"
#include <shared_mutex>

const std::string logger_name("The Ledger");
const std::string engine_name("Prometheus");
const std::string version("0.1");
const std::string author("Zach Bortoff");

const unsigned int MAX_DEPTH = 6;
const unsigned int MIN_DEPTH = 2;

const Centipawns_t INF = 1000000;

const uint8_t CASTLE_CHECK[64] {
        13,15,15,15,12,15,15,14,
        15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,
        7 ,15,15,15, 3,15,15,11
};

const Bitboard ANTI_DIAGONAL_MASK[15] {
        C64(0x80),
        C64(0x8040),
        C64(0x804020),
        C64(0x80402010),
        C64(0x8040201008),
        C64(0x804020100804),
        C64(0x80402010080402),
        C64(0x8040201008040201),
        C64(0x4020100804020100),
        C64(0x2010080402010000),
        C64(0x1008040201000000),
        C64(0x804020100000000),
        C64(0x402010000000000),
        C64(0x201000000000000),
        C64(0x100000000000000)
};

const Bitboard DIAGONAL_MASK[15] {
        C64(0x1),
        C64(0x102),
        C64(0x10204),
        C64(0x1020408),
        C64(0x102040810),
        C64(0x10204081020),
        C64(0x1020408102040),
        C64(0x102040810204080),
        C64(0x204081020408000),
        C64(0x408102040800000),
        C64(0x810204080000000),
        C64(0x1020408000000000),
        C64(0x2040800000000000),
        C64(0x4080000000000000),
        C64(0x8000000000000000)
};

const Bitboard RANK_MASK[8] {
        C64(0xFF),
        C64(0xFF00),
        C64(0xFF0000),
        C64(0xFF000000),
        C64(0xFF00000000),
        C64(0xFF0000000000),
        C64(0xFF000000000000),
        C64(0xFF00000000000000)
};

const Bitboard FILE_MASK[8] {
        C64(0x101010101010101),
        C64(0x202020202020202),
        C64(0x404040404040404),
        C64(0x808080808080808),
        C64(0x1010101010101010),
        C64(0x2020202020202020),
        C64(0x4040404040404040),
        C64(0x8080808080808080)
};

const Bitboard KING_ATTACK_ARRAY[64] {
        C64(770), C64(1797), C64(3594), C64(7188), C64(14376), C64(28752),
        C64(57504), C64(49216), C64(197123), C64(460039), C64(920078),
        C64(1840156), C64(3680312), C64(7360624), C64(14721248), C64(12599488),
        C64(50463488), C64(117769984), C64(235539968), C64(471079936),
        C64(942159872), C64(1884319744), C64(3768639488), C64(3225468928),
        C64(12918652928), C64(30149115904), C64(60298231808), C64(120596463616),
        C64(241192927232), C64(482385854464), C64(964771708928), C64(825720045568),
        C64(3307175149568), C64(7718173671424), C64(15436347342848),
        C64(30872694685696), C64(61745389371392), C64(123490778742784),
        C64(246981557485568), C64(211384331665408), C64(846636838289408),
        C64(1975852459884544), C64(3951704919769088), C64(7903409839538176),
        C64(15806819679076352), C64(31613639358152704), C64(63227278716305408),
        C64(54114388906344448), C64(216739030602088448), C64(505818229730443264),
        C64(1011636459460886528), C64(2023272918921773056),
        C64(4046545837843546112), C64(8093091675687092224),
        C64(16186183351374184448), C64(13853283560024178688),
        C64(144959613005987840), C64(362258295026614272), C64(724516590053228544),
        C64(1449033180106457088), C64(2898066360212914176),
        C64(5796132720425828352), C64(11592265440851656704),
        C64(4665729213955833856)
};

const Bitboard KNIGHT_ATTACK_ARRAY[64] {
        C64(132096), C64(329728), C64(659712), C64(1319424), C64(2638848),
        C64(5277696), C64(10489856), C64(4202496), C64(33816580), C64(84410376),
        C64(168886289), C64(337772578), C64(675545156), C64(1351090312),
        C64(2685403152), C64(1075839008), C64(8657044482), C64(21609056261),
        C64(43234889994), C64(86469779988), C64(172939559976), C64(345879119952),
        C64(687463207072), C64(275414786112), C64(2216203387392),
        C64(5531918402816), C64(11068131838464), C64(22136263676928),
        C64(44272527353856), C64(88545054707712), C64(175990581010432),
        C64(70506185244672), C64(567348067172352), C64(1416171111120896),
        C64(2833441750646784), C64(5666883501293568), C64(11333767002587136),
        C64(22667534005174272), C64(45053588738670592), C64(18049583422636032),
        C64(145241105196122112), C64(362539804446949376), C64(725361088165576704),
        C64(1450722176331153408), C64(2901444352662306816),
        C64(5802888705324613632), C64(11533718717099671552),
        C64(4620693356194824192), C64(288234782788157440), C64(576469569871282176),
        C64(1224997833292120064), C64(2449995666584240128),
        C64(4899991333168480256), C64(9799982666336960512),
        C64(1152939783987658752), C64(2305878468463689728), C64(1128098930098176),
        C64(2257297371824128), C64(4796069720358912), C64(9592139440717824),
        C64(19184278881435648), C64(38368557762871296), C64(4679521487814656),
        C64(9077567998918656)
};

PositionKey POSITION_KEYS[12][64];
PositionKey SIDE_2_MOVE_KEYS[2];
PositionKey CASTLE_PERMISSION_KEYS[16];
PositionKey EN_PASSANT_KEYS[17];

/// Evaluation Parameters
const Centipawns_t PAWN_BASE_VAL   = 100;
const Centipawns_t BISHOP_BASE_VAL = 330;
const Centipawns_t KNIGHT_BASE_VAL = 310;
const Centipawns_t ROOK_BASE_VAL = 500;
const Centipawns_t QUEEN_BASE_VAL = 910;

const Centipawns_t TEMPO = 5;

const double QUEEN_PERC[4] = {
        1.0, 1.05, 1.15, 1.30
};
const double ROOK_PERC[4] = {
        0.8, 1.0, 1.2, 1.5
};
const double BISHOP_PERC[4] = {
        1.0, 1.0, 1.1, 1.2
};
const double KNIGHT_PERC[4] = {
        1.2, 1.1, 1.0, 0.85
};
const double PAWN_STRENGTH[4] = {
        1.0, 1.0, 1.1, 1.15
};
const double CONNECTIVITY[5] = {
        9.1, 5.0, 3.3, 3.1, 1.0
};
const Centipawns_t KNIGHT_MOBILITY = 4;
const Centipawns_t KNIGHT_OUTPOST_BONUS = 5;
const Centipawns_t KNIGHT_PST[64] = {
        -5, -2, -2, -2, -2, -2, -2, -5,
        -2, 0, 0, 0, 0, 0, 0, -2,
        -2, 0, 1, 2, 2, 1, 0, -2,
        -2, 0, 2, 4, 4, 2, 0, -2,
        -2, 0, 2, 4, 4, 2, 0, -2,
        -2, 0, 1, 2, 2, 1, 0, -2,
        -2, 0, 0, 0, 0, 0, 0, -2,
        -5, -2, -2, -2, -2, -2, -2, -5
};

const Centipawns_t OM_ROOK_MOBILITY = 3;
const Centipawns_t E_ROOK_MOBILITY = 9;
const Centipawns_t ROOK_ON_HALF_OPEN_FILE_BONUS = 8;
const Centipawns_t ROOK_ON_OPEN_FILE_BONUS = 15;
const Centipawns_t ROOK_ON_OPEN_FILE_BONUS_NO_OPPOSING_ROOK = 25;
const Centipawns_t ROOK_SUPPORTING_PASSED_PAWNS = 20;
const Centipawns_t CONNECTED_ROOK_BONUS = 10;
const Centipawns_t ROOK_PST[64] = {
        -4, -2, 0, 2, 2, 0, -2, -4,
        -2, 0, 0, 2, 2, 0, 0, -2,
        0, 0, 1, 2, 2, 1, 0, 0,
        2, 2, 2, 3, 3, 2, 0, 2,
        2, 2, 2, 3, 3, 2, 0, 2,
        0, 0, 1, 2, 2, 1, 0, 0,
        -2, 0, 0, 0, 0, 0, 0, -2,
        -4, -2, 0, 2, 2, 0, -2, -4
};
const Centipawns_t ROOK_ON_SEVENTH = 9;

const Centipawns_t OM_BISHOP_MOBILITY = 5;
const Centipawns_t BISHOP_PST[64] = {
        -2,-1,-1,-1,-1,-1,-1,-2,
        -1,5,0,0,0,0,5,-1,
        -1,2,5,2,2,5,2,-1,
        -1,  1,  1, 1, 1, 1, 1,-1,
        -1,  0, 1, 1, 1, 1,  0,-1,
        -1, 1, 1, 1, 1, 1, 1,-1,
        -1,  1,  0,  0,  0,  0,  1,-1,
        -2,-1,-4,-1,-1,-4,-1,-2
};
const Centipawns_t BISHOP_PAIR_BONUS = 10;
const Centipawns_t BISHOP_ON_LONG_DIAGONAL = 7;
const Centipawns_t BISHOP_ON_OPEN_LONG_DIAGONAL = 12;

const Centipawns_t QUEEN_MOBILITY = 5;
const Centipawns_t QUEEN_PST[64] = {
        -6, -3, -1, 0, 0, -1, -3, -6,
        -3, 3, 0, 2, 2, 0, 3, -3,
        -1, 2, 6, 4, 4, 6, 2, -1,
        0, 3, 3, 7, 7, 3, 3, 0,
        0, 3, 3, 7, 7, 3, 3, 0,
        -1, 2, 6, 4, 4, 6, 2, -1,
        -3, 3, 0, 2, 2, 0, 3, -3,
        -6, -3, -1, 0, 0, -1, -3, -6
};

const Centipawns_t QUEEN_OPENING_QUICK_PENALTY = -15;

const Centipawns_t OM_KING_MOBILITY = 11;
const Centipawns_t E_KING_MOBILITY = 15;
const Centipawns_t M_KING_PST[64] = {
        2,  3,  1,   0,   0,  1,  3,  2,
        2,  2,   0,   0,   0,   0,  2,  2,
        -10, -10, -20, -20, -20, -20, -20, -10,
        -30, -30, -30, -100, -100, -30, -30, -30,
        -40, -40, -40, -100, -100, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30
};

const Centipawns_t E_KING_PST[64] = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
};

const Centipawns_t PAWN_MOBILITY = 12;
const Centipawns_t PAWN_PST[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10,-25,-25, 10, 10,  5,
        5, -5,-10,  0,  0,-10, -5,  5,
        0,  0,  0, 25, 25,  0,  0,  0,
        5,  5, 10, 27, 27, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
};

const Centipawns_t PASSED_PAWN[8] = {
        0, 0, 0, 12, 25, 50, 100, 0
};

const Centipawns_t PAWN_UNOPPOSED_PAWN = 5;
const Centipawns_t PAWN_BACKWARD = -5;
const Centipawns_t PAWN_BLOCKING = -5;
const Centipawns_t PAWN_DOUBLE = -8;
const Centipawns_t PAWN_ISOLATED = -9;
const Centipawns_t PAWN_KING_SHELTER = 10;
const Centipawns_t PAWN_IN_CHAIN = 7;
const Centipawns_t PAWN_AS_ROOT_OF_CHAIN = 5;
const Centipawns_t PAWN_AS_HEAD_OF_CHAIN = -2;
const Centipawns_t PAWN_ATTACKING_ENEMY_CAMP = 20;
const Centipawns_t PAWN_HANGING = -15;
const Centipawns_t PAWN_ISLAND[4] = {
        0, -1, -2, -3
};
const Centipawns_t HALF_ISOLATED_PAWN = -5;

/// Search Parameters
const Centipawns_t WINDOW_SIZE[3] = {
        150, 550, 1100
};
const Depth NULL_MOVE_R = 7;
const double NULL_MOVE_ADAPTIVITY = 0.5;
const Depth NULL_MOVE_ADAPTIVITY_DEPTH = 7;
const Depth FUTILITY_DEPTH = 2;
const Centipawns_t FUTILITY_THRESHOLD_DEPTH_1 = 500;
const Centipawns_t FUTILITY_THRESHOLD_DEPTH_2 = 600;
const Centipawns_t FUTILITY_THRESHOLD_DEPTH_3 = 850;
const Depth MULTICUT_R = 3;
const Depth MULTICUT_DEPTH = 3;
const Depth MULTICUT_M = 15;
const Depth MULTICUT_C = 5;
const Depth CHECKOUT_EXTENSION = 2;
const Depth ONE_REPLY_EXTENSION = 2;
const Depth RECAPTURE_EXTENSION = 2;
const Depth PASSED_PAWN_EXTENSION_SEVENTH = 2;
const Depth MATE_THREAT_EXTENSION = 2;
const Depth LATE_MOVE_REDUCTION_PLY_DISCOUNT = 1;
const Depth LATE_MOVE_REDUCTION_DISCOUNT_NUMBER = 6;
const double LATE_MOVE_REDUCTION_DISCOUNT_PERCENTAGE = 0.25;
const int AGE_DIFFERENCE_THRESHOLD = 16;

#endif //PROMETHEUS_GLOBALS_HPP
