#ifndef CLOAK_TRANSPOSITION_TABLE_HPP
#define CLOAK_TRANSPOSITION_TABLE_HPP

/// External Library Includes
#include <spdlog/spdlog.h>

/// Internal Library Includes
#include "chess_hash.hpp"
#include "extern.hpp"

#define HASHES_LENGTH 4

/// The reason there are four is because my cacheline size is 64 bytes, and a chesshash is 16 bytes, so that fits 4
struct TranspositionTableEntry {
    ChessHash hashes[HASHES_LENGTH];
};

class TranspositionTable {
    using Entry = TranspositionTableEntry;

    int tt_size_;
    Entry* table_;
#ifndef NDEBUG
    int num_entries_;
    int hash_hits_;
    int num_hash_probes_;
    int num_type_1_hash_misses_;
    int num_type_2_hash_misses_;
#endif // NDEBUG

    inline int index(PositionKey key) noexcept {
        return key % tt_size_;
    }

    inline int index(PositionKey key) const noexcept {
        return key % tt_size_;
    }

public:
    explicit TranspositionTable(const int tt_size) : tt_size_(tt_size)
#ifndef NDEBUG
    , num_entries_(0), hash_hits_(0), num_hash_probes_(0), num_type_1_hash_misses_(0), num_type_2_hash_misses_(0)
#endif // NDEBUG
    {
        assert(tt_size_ > 0);
        table_ = new Entry[tt_size_];
    }

    ~TranspositionTable();

    inline void clear() {
        std::memset(table_, 0, tt_size_ * sizeof(Entry));
#ifndef NDEBUG
        reset_tracking_variables();
#endif // NDEBUG
    }

#ifndef NDEBUG
    double load_factor();
    int num_entries();
    double hit_rate();
    void reset_tracking_variables();
#endif // NDEBUG

    inline int size() noexcept {
        return tt_size_;
    }
    inline int size() const noexcept {
        return tt_size_;
    }

    ChessHash* find(PositionKey key);
    bool insert(const ChessHash& inserted_hash);

    friend std::ostream& operator<<(std::ostream& os, TranspositionTable& tt) {
        os << "Transposition Table: " << std::endl;
        os << "- tt_size: " << tt.tt_size_ << std::endl;
#ifndef NDEBUG
        os << "  num_entries: " << tt.num_entries_ << std::endl;
        os << "  hash_hits: " << tt.hash_hits_ << std::endl;
        os << "  num_hash_probes: " << tt.num_hash_probes_ << std::endl;
        os << "  num_type_1_hash_misses: " << tt.num_type_1_hash_misses_ << std::endl;
        os << "  num_type_2_hash_misses: " << tt.num_type_2_hash_misses_ << std::endl;
#endif // NDEBUG

        return os;
    }
};


#endif // CLOAK_TRANSPOSITION_TABLE_HPP
