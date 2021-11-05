#ifndef CLOAK_TRANSPOSITION_TABLE_HPP
#define CLOAK_TRANSPOSITION_TABLE_HPP

/// External Library Includes
#include <spdlog/spdlog.h>

/// Internal Library Includes
#include "chess_hash.hpp"
#include "extern.hpp"

class TranspositionTable {
    using Entry = ChessHash;

    int tt_size_;
    Entry* table_;
#ifndef NDEBUG
    int num_entries_;
    int hash_hits_;
    int num_hash_probes_;
    int num_overwrites_;
    int num_newwrites_;
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
    , num_entries_(0), hash_hits_(0), num_hash_probes_(0), num_overwrites_(0), num_newwrites_(0)
#endif // NDEBUG
    {
        assert(tt_size_ > 0);

        table_ = new Entry[tt_size_];
    }

    ~TranspositionTable();

    inline void clear() {
        std::memset(table_, 0, sizeof(Entry) * tt_size_);
#ifndef NDEBUG
        num_entries_ = 0;
        reset_tracking_variables();
#endif // NDEBUG
    }

#ifndef NDEBUG
    double load_factor();
    int num_entries();
    double hit_rate();
    void reset_tracking_variables();
    double overwrite_percentage();
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
        os << "- Table Size (in entries): " << tt.tt_size_ << std::endl;
#ifndef NDEBUG
        os << "  Number of Filled Entries: " << tt.num_entries_ << std::endl;
        os << "  Load factor (Filled Entries/Total Entries): " << tt.load_factor() << std::endl;
        os << "  Number of Probes: " << tt.num_hash_probes_ << std::endl;
        os << "  Number of Successful Probes: " << tt.hash_hits_ << std::endl;
        os << "  Hit Rate (Successful Probes/Total Probes): " << tt.hit_rate() << std::endl;
        os << "  Number of New Writes: " << tt.num_newwrites_ << std::endl;
        os << "  Number of Overwrites: " << tt.num_overwrites_ << std::endl;
        os << "  Overwrite Percentage (Overwrites / Total Writes): " << tt.overwrite_percentage() << std::endl;
#endif // NDEBUG

        return os;
    }
};


#endif // CLOAK_TRANSPOSITION_TABLE_HPP
