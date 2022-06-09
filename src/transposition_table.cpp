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

std::optional<ChessHash> TranspositionTable::find(const PositionKey key) {
    int x = index(key);
    TranspositionTable::Entry* entry = &table_[x];

#ifndef NDEBUG
    ++num_hash_probes_;
#endif // NDEBUG

    if (entry->key == key) {
#ifndef NDEBUG
        ++hash_hits_;
#endif // NDEBUG
        return *entry;
    }

    return std::nullopt;
}

/**
 *
 * @param inserted_hash
 * @return
 */
bool TranspositionTable::insert(const ChessHash& inserted_hash) {
    int x = index(inserted_hash.key);

    if (table_[x].key == 0) {
        // if either no position is being stored at the index 'x', then insert the new entry
#ifndef NDEBUG
        ++num_entries_;
        ++num_newwrites_;
#endif // NDEBUG
        table_[x] = inserted_hash;
    } else  if (table_[x].hash_flag != EXACT ||
                (table_[x].hash_flag == EXACT && inserted_hash.hash_flag == EXACT)) {
        // if there is another position being stored here, but the position being stored here is not exact, then always
        // replace it. if there is another position being stored here, but the position IS EXACT AND we are inserting an
        // exact, then replace it. So never replace an exact with an BETA_CUTOFF, for example.
            table_[x] = inserted_hash;
#ifndef NDEBUG
            ++num_overwrites_;
#endif // NDEBUG
    } else if ((table_[x].age + 20) % 256 < inserted_hash.age) {
        // if the age of the position at the index we are trying to insert a new position is 20 plies greater than the
        // inserted index, then we can be confident that we put this entry into the table awhile ago... So long ago
        // that, we would never come across this position again. We are pretty much assuming that the 'keys' of the two
        // positions are different
        assert(table_[x].key != inserted_hash.key);
        table_[x] = inserted_hash;
#ifndef NDEBUG
        ++num_overwrites_;
#endif // NDEBUG
    }

    return true;
}

