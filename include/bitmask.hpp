//
// Created by Zach Bortoff on 2019-11-16.
//

#ifndef PROMETHEUS_BITMASK_HPP
#define PROMETHEUS_BITMASK_HPP

/// STL Includes
#include <cstdint>
#include <string>
#include <vector>

/// Project Includes
#include "defines.hpp"
#include "extern.hpp"

/**
 * Courtesy of Google's CityHash
 * https://stackoverflow.com/questions/41770887/cross-platform-definition-of-byteswap-uint64-and-byteswap-ulong
 */
#ifdef _MSC_VER
#include <stdlib.h>
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)
#elif defined(__APPLE__)
// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)
#elif defined(__sun) || defined(sun)
#include <sys/byteorder.h>
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else

#include <byteswap.h>
#endif

std::string draw_bitboard(Bitboard bb) noexcept;
int bitscan_forward(Bitboard bb);
int bitscan_reverse(Bitboard bb);
int pop_count(Bitboard bb);
Bitboard north_fill(Bitboard bb);
Bitboard south_fill(Bitboard bb);
Bitboard file_fill(Bitboard bb);
Bitboard flip_vertically(Bitboard bb);
Bitboard reverse(Bitboard bb);
Bitboard horzvert_mask(Square_t sq, Bitboard occupied);
Bitboard diag_mask(Square_t sq, Bitboard occupied);
Bitboard flip_flop(Bitboard bb);
Bitboard mirror(Bitboard bb);
Centipawns_t dot_product(Bitboard bb, const Centipawns_t* weights);

#endif //PROMETHEUS_BITMASK_HPP
