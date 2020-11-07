//
// Created by Zach Bortoff on 2020-02-07.
//

#include "transposition_table.hpp"

/**
 * Conditions:
 * 1.) There may not be more than 1 instance of a key in the hash table
 * 2.) Any ChessHash whose age exceeds the threshold may be removed
 * 3.) If the two ChessHashes are equivalent
 * 3a.) If both are EXACT, then save the one inside the table regardless of depth
 * 3b.) If both are NOT EXACT, then save whichever one has a larger depth
 * 3c.) If the inserted one is EXACT, but the other isn't save the inserted one
 * 3d.) If the one inside is EXACT, but inserted isn't, don't save
 * 4.) If there are no free slots and the position is table,
 * 3a.) if both are EXACT, keep the one inside the table
 * 5b.) if neither is exact, pick the one with the bigger depth
 * 5c.) if the table one is EXACT, but the inserted one isn't, keep searching
 * 5d.) if the table one is NOT EXACT, but the inserted one is EXACT, replace
 */

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
    return num_entries_ / tt_size_;
}

int TranspositionTable::num_entries() {
    return num_entries_;
}

double TranspositionTable::hit_rate() {
    /// hash hits / number of hash probes.
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
    int x = index(key);
    TranspositionTable::Entry* entry = &table_[x];

#ifndef NDEBUG
    ++num_hash_probes_;
#endif // NDEBUG

    for (auto & hashe : entry->hashes) {
        if (hashe.key == key) {
#ifndef NDEBUG
            ++hash_hits_;
#endif // NDEBUG
            return &hashe;
        }
    }

    return nullptr;
}

/**
 *
 * @param inserted_hash
 * @return
 */
bool TranspositionTable::insert(const ChessHash& inserted_hash) {
    int x = index(inserted_hash.key);
    TranspositionTable::Entry* entry = &table_[x];

    for (auto & h : entry->hashes) {
        /// if the entry is not in the slot, then just put it in there
        if (h.key == 0) {
            h = inserted_hash;
#ifndef NDEBUG
            ++num_entries_;
#endif // NDEBUG
            return true;
        } else if (h == inserted_hash) {
            /// if the move in the table is on the PV, but it's root's ply was >= some age threshold, then replace it
            Depth hash1_age = (h.age % 255) - h.depth; // ply of root position
            Depth hash_age = (inserted_hash.age % 255) - inserted_hash.depth; // ply of root position
            if (hash_age - hash1_age >= AGE_DIFFERENCE_THRESHOLD) {
                h = inserted_hash;
                return true;
            }

            if (h.hash_flag == EXACT) {
                return false;
            } else if (inserted_hash.hash_flag == EXACT) { // we already know that h.hash_flag != EXACT at this point, so no need to include
                /// if the inserted_hash inside the table is not on the PV but the move being inserted is, then just replace it
                h = inserted_hash;
                return true;
            } else if (inserted_hash.depth >= h.depth) {
                h = inserted_hash;
                return true;
            }

            return false;
        }
#ifndef NDEBUG
        ++num_type_2_hash_misses_; // two different position mapped to the same index
#endif // NDEBUG
    }

    /// if we've reached this point, then every bucket is filled, but none of them correspond with the position-move that I'm putting in
    for (auto & h : entry->hashes) {
        /// If it's expired, replace it
        Depth hash1_age = (h.age % 255) - h.depth; // ply of root position
        Depth hash_age = (inserted_hash.age % 255) - inserted_hash.depth; // ply of root position
        if (hash_age - hash1_age >= AGE_DIFFERENCE_THRESHOLD) {
            h = inserted_hash;
            return true;
        }

        if (h.hash_flag == EXACT) {
            continue;
        } else if (inserted_hash.hash_flag == EXACT) { // we already know h.hash_flag != EXACT at this point, so no need to include
            h = inserted_hash;
            return true;
        } else if (h.depth < inserted_hash.depth) {
            /// if neither is exact, then if the inserted has a greater depth than the one inside the table, then replace it
            h = inserted_hash;
            return true;
        }
    }

    if (inserted_hash.hash_flag == EXACT) {
        spdlog::get(logger_name)->warn("Attempted to insert EXACT hash into the table, nothing in the table would give way! ");
    }

    return false;
}

