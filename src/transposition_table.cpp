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
//    assert(num_hash_probes_ != 0);
    return hash_hits_ / num_hash_probes_;
}

void TranspositionTable::reset_tracking_variables() {
    num_entries_ = 0;
    hash_hits_ = 0;
    num_hash_probes_ = 0;
    num_type_1_hash_misses_ = 0;
    num_type_2_hash_misses_ = 0;
}

#endif // NDEBUG

ChessHash* TranspositionTable::find(const PositionKey key) {
    int i = index(key);
    TranspositionTable::Entry* entry = &table_[i];

#ifndef NDEBUG
    ++num_hash_probes_;
#endif // NDEBUG

    for (int i = 0; i < HASHES_LENGTH; i++) {
        if (entry->hashes[i].key == key) {
#ifndef NDEBUG
            ++hash_hits_;
#endif // NDEBUG
            return &entry->hashes[i];
        }
    }

    return nullptr;
}

/**
 *
 * @param hash
 * @return
 */
bool TranspositionTable::insert(const ChessHash& hash) {
    int i = index(hash.key);
    TranspositionTable::Entry* entry = &table_[i];

    for (int i = 0; i < HASHES_LENGTH; i++) {
        /// if the entry is not in the slot, then just put it in there
        if (entry->hashes[i].key == 0) {
            entry->hashes[i] = hash;
#ifndef NDEBUG
            ++num_entries_;
#endif // NDEBUG
            return true;
        } else if (entry->hashes[i] == hash) {
            if (entry->hashes[i].hash_flag == hash.hash_flag) {
                /// if the move in the table is on the PV, but it's root's ply was >= some age threshold, then replace it
                int hash1_age = ((int)entry->hashes[i].age % 255) - entry->hashes[i].depth; // ply of root position
                int hash_age = ((int)hash.age % 255) - hash.depth; // ply of root position
                if (hash_age - hash1_age >= AGE_DIFFERENCE_THRESHOLD || entry->hashes[i].depth < hash.depth) {
                    entry->hashes[i] = hash;
                    return true;
                }

                return false;
            } else if (entry->hashes[i].hash_flag == HashFlag::EXACT && hash.hash_flag != HashFlag::EXACT) {
                /// if the move in the table is on the PV, but it's root's ply was >= some age threshold, then replace it
                int hash1_age = ((int)entry->hashes[i].age % 255) - entry->hashes[i].depth; // ply of root position
                int hash_age = ((int)hash.age % 255) - hash.depth; // ply of root position
                if (hash_age - hash1_age >= AGE_DIFFERENCE_THRESHOLD) {
                    entry->hashes[i] = hash;
                    return true;
                }

                return false;
            } else if (entry->hashes[i].hash_flag != HashFlag::EXACT && hash.hash_flag == HashFlag::EXACT) {
                /// if the hash inside the table is not on the PV but the move being inserted is, then just replace it
                entry->hashes[i] = hash;
                return true;
            }
        }
#ifndef NDEBUG
        ++num_type_2_hash_misses_; // two different position mapped to the same index
#endif // NDEBUG
    }

    return false;
}

