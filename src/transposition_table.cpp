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
    return static_cast<double>(num_entries_) / static_cast<double>(tt_size_);
}

int TranspositionTable::num_entries() {
    return num_entries_;
}

double TranspositionTable::hit_rate() {
    /// hash hits / number of hash probes.
    return static_cast<double>(hash_hits_) / static_cast<double>(num_hash_probes_);
}

double TranspositionTable::overwrite_percentage() {
    return static_cast<double>(num_overwrites_ / static_cast<double>(num_newwrites_ + num_overwrites_));
}

void TranspositionTable::reset_tracking_variables() {
    hash_hits_ = 0;
    num_hash_probes_ = 0;
    num_overwrites_ = 0;
    num_newwrites_ = 0;
}

#endif // NDEBUG

ChessHash* TranspositionTable::find(const PositionKey key) {
    int x = index(key);
    TranspositionTable::Entry* entry = &table_[x];

#ifndef NDEBUG
    ++num_hash_probes_;
#endif // NDEBUG

    if (entry->key == key) {
#ifndef NDEBUG
        ++hash_hits_;
#endif // NDEBUG
        return entry;
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


    if (table_[x].key == 0) {
        // if the position key associated with the hash at the position which we are seeking to insert the new hash
        // then, we are not overwriting anything
#ifndef NDEBUG
        ++num_entries_;
        ++num_newwrites_;
#endif // NDEBUG
        table_[x] = inserted_hash;
    } else {
        if (table_[x].hash_flag != EXACT || (table_[x].hash_flag == EXACT && inserted_hash.hash_flag == EXACT)) { // this fixes the overwrite problem of previous hashes, b
            table_[x] = inserted_hash;

#ifndef NDEBUG
            ++num_overwrites_;
#endif // NDEBUG
        }
    }

    return true;
}

