//
// Created by Zach Bortoff on 2020-02-07.
//

#ifndef PROMETHEUS_TRANSPOSITIONTABLE_HPP
#define PROMETHEUS_TRANSPOSITIONTABLE_HPP

/// Standard Library Includes
#include <unordered_set>
#include <set>

/// External Library Includes
#include <spdlog/spdlog.h>

/// Internal Library Includes
#include "chess_hash.hpp"

class TranspositionTable {
    using Entry = std::set<ChessHash>;
    using Itr = std::set<ChessHash>*;

    int tt_size_;
    Entry* table_;
#ifndef NDEBUG
    int num_entries_;
    int hash_hits_;
    int num_hash_probes_;
    int num_type_1_hash_misses_;
    int num_type_2_hash_misses_;
#endif // NDEBUG

    int index(PositionKey key) noexcept;
    [[nodiscard]] int index(PositionKey key) const noexcept;

public:
    explicit TranspositionTable(const int tt_size) : tt_size_(tt_size)
#ifndef NDEBUG
    , num_entries_(0), hash_hits_(0), num_hash_probes_(0), num_type_1_hash_misses_(0), num_type_2_hash_misses_(0)
#endif // NDEBUG
    {
        table_ = new Entry[tt_size_];
    }

    ~TranspositionTable();

#ifndef NDEBUG
    double load_factor();
    int num_entries();
    double hit_rate();
    void reset_tracking_variables();
#endif // NDEBUG

    int size() noexcept;
    int size() const noexcept;

    Entry* find(PositionKey key);
    std::pair<Itr, bool> insert(const ChessHash& hash);
    std::pair<Itr, bool> insert(ChessHash&& hash);
    std::pair<Itr, bool> emplace(ChessHash&& hash);
};


#endif //PROMETHEUS_TRANSPOSITIONTABLE_HPP
