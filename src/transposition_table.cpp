//
// Created by Zach Bortoff on 2020-02-07.
//

#include "transposition_table.hpp"

TranspositionTable::~TranspositionTable() {
#ifndef NDEBUG
    reset_tracking_variables();
#endif // NDEBUG
    delete [] table_;
    table_ = nullptr;
}

int TranspositionTable::index(const PositionKey key) noexcept {
    return key % tt_size_;
}

int TranspositionTable::index(const PositionKey key) const noexcept {
    return key % tt_size_;
}

#ifndef NDEBUG

double TranspositionTable::load_factor() {
    // load factor = number of entries / size of hash table.
    assert(tt_size_ != 0);
    return num_entries_ / tt_size_;
}

int TranspositionTable::num_entries() {
    return num_entries_;
}

double TranspositionTable::hit_rate() {
    /// hash hits / number of hash probes.
    assert(num_hash_probes_ != 0);
    return hash_hits_ / num_hash_probes_;
}

void TranspositionTable::reset_tracking_variables() {
    num_entries_ = 0;
    hash_hits_ = 0;
    num_hash_probes_ = 0;
    num_type_1_hash_misses_ = 0;
    num_type_2_hash_misses_ = 0;
}

int TranspositionTable::size() noexcept {
    return tt_size_;
}

int TranspositionTable::size() const noexcept {
    return tt_size_;
}

TranspositionTable::Itr TranspositionTable::find(const PositionKey key) {
    int i = index(key);
    return &table_[i];
}

std::pair<TranspositionTable::Itr, bool> TranspositionTable::insert(const ChessHash& hash) {
    int i = index(hash.key);
    table_[i].insert(hash);
    return std::make_pair(&table_[0] + i, true);
}

std::pair<TranspositionTable::Itr, bool> TranspositionTable::insert(ChessHash&& hash) {
    int i = index(hash.key);
    table_[i].insert(std::forward<ChessHash>(hash));
    return std::make_pair(&table_[0] + i, true);
}

std::pair<TranspositionTable::Itr, bool> TranspositionTable::emplace(ChessHash&& hash) {
    int i = index(hash.key);
    table_[i].emplace(std::forward<ChessHash>(hash));
    return std::make_pair(&table_[0] + i, true);
}

#endif // NDEBUG

