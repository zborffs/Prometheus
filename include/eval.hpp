#ifndef PROMETHEUS_EVAL_HPP
#define PROMETHEUS_EVAL_HPP

/// STL includes
#include <numeric>
#include <iomanip>
#include <utility>

/// Project includes
#include "defines.hpp"
#include "board.hpp"
#include "attack.hpp"
#include "bitmask.hpp"

class EvalPrettyPrinter {
    std::vector<std::pair<std::string, Centipawns_t> > eval_params_;
    unsigned long max_str_size_{0};
    Centipawns_t max_score_{0};

public:
    void add(const std::string& eval_name, Centipawns_t score) {
        auto search = std::find_if(eval_params_.begin(), eval_params_.end(), [&](const std::pair<std::string, Centipawns_t>& p) {
            return p.first == eval_name;
        });
        if (search != eval_params_.end()) {
            search->second = score;
        } else {
            eval_params_.emplace_back(eval_name, score);
        }

        if (std::abs(score) > max_score_) {
            max_score_ = score;
        }

        if (eval_name.size() > max_str_size_) {
            max_str_size_ = eval_name.size();
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const EvalPrettyPrinter& pretty_print) {
        os.setf(std::ios::showpos);
        const std::string param_header("Param Name");
        const std::string val_header("Value");
        const unsigned long max_score_size = pretty_print.max_score_ == 0 ? val_header.size() : std::max((unsigned long)std::floor(std::log10(pretty_print.max_score_)) + 2, val_header.size()); // add 1 for mapping 0->1 and 1 for adding sign
        const unsigned long max_str_size = pretty_print.max_str_size_ == 0 ? param_header.size() : std::max(param_header.size(), pretty_print.max_str_size_);


        std::string param_buf(max_str_size - param_header.size(), ' ');
        std::string val_buf(max_score_size - val_header.size(), ' ');
        os << "| " << param_header << param_buf << " | " << val_buf << val_header << " |" << std::endl;
        param_buf = std::string(max_str_size, '-');
        val_buf = std::string(max_score_size, '-');
        os << "+-" << param_buf << "-+-" << val_buf << "-+" << std::endl;

        for (const std::pair<std::string, Centipawns_t>& elem : pretty_print.eval_params_) {
            param_buf = std::string(max_str_size - elem.first.size(), ' ');
            const unsigned long score_length = (elem.second == 0) ? 2 : std::floor(std::log10(std::abs(elem.second))) + 2;
            val_buf = std::string(max_score_size - score_length, ' ');
            os << "| " << elem.first << param_buf << " | " << val_buf << elem.second << " |" << std::endl;
        }

        os.unsetf(std::ios::showpos);
        return os;
    }
};


const Bitboard QUEEN_SIDE = FILE_MASK[FILEA] | FILE_MASK[FILEB] | FILE_MASK[FILEC] | FILE_MASK[FILED];
const Bitboard KING_SIDE = FILE_MASK[FILEE] | FILE_MASK[FILEF] | FILE_MASK[FILEG] | FILE_MASK[FILEH];
const Bitboard CENTER_FILES = FILE_MASK[FILEC] | FILE_MASK[FILED] | FILE_MASK[FILEE] | FILE_MASK[FILEF];
const Bitboard CENTER = (FILE_MASK[FILED] | FILE_MASK[FILEE]) & (RANK_MASK[RANK4] | RANK_MASK[RANK5]);

struct EvaluationState {
    GameStage stage : 2;
    PositionType pos_type : 6;
#ifndef NDEBUG
    EvalPrettyPrinter printer;
#endif // NDEBUG

    /**
     * default constructor sets member variables to what they would be at the beginning of a game
     */
    EvaluationState() : stage(OPENING), pos_type(CLOSED) {}

    /**
     * publishes an instance of an EvaluationState object
     * @param os reference to a std::ostream object
     * @param es reference to an instance of an EvaluationState object to be published
     * @return   updated reference to the input std::ostream object
     */
    friend std::ostream& operator<<(std::ostream& os, EvaluationState& es) {
        os << std::showpoint << std::showpos;
        os << "EvaluationState: " << std::endl << "- stage:";

        switch (es.stage) {
            case OPENING: os << "Opening" << std::endl; break;
            case MID_GAME: os << "Middle Game" << std::endl; break;
            case EARLY_END_GAME: os << "Early End Game" << std::endl; break;
            case LATE_END_GAME: os << "Late End Game" << std::endl; break;
        }
        os << "  pos_type: ";

        switch (es.pos_type) {
            case CLOSED: os << "Closed" << std::endl; break;
            case SEMI_CLOSED: os << "Semi-Closed" << std::endl; break;
            case SEMI_OPEN: os << "Semi-Open" << std::endl; break;
            case OPEN: os << "Open" << std::endl; break;
        }

#ifndef NDEBUG
        os << es.printer << std::endl;
#endif // NDEBUG

        return os << std::noshowpoint << std::noshowpos;
    }
};

namespace internal {
    void def_stage(const Board& board, EvaluationState& es);
    void def_pos_type(const Board& board, EvaluationState& es);

    Centipawns_t material(const Board &board, EvaluationState &es, Color_t color, bool verbose = false);
    Centipawns_t eval_pawns(const Board &board, EvaluationState &es, bool verbose = false);
    Centipawns_t eval_rooks(const Board &board, EvaluationState &es, bool verbose = false);
    Centipawns_t eval_knights(const Board &board, EvaluationState &es, bool verbose = false);
    Centipawns_t eval_bishop(const Board &board, EvaluationState &es, bool verbose = false);
    Centipawns_t eval_queens(const Board &board, EvaluationState &es, bool verbose = false);
    Centipawns_t eval_kings(const Board &board, EvaluationState &es, bool verbose = false);
    Centipawns_t eval_tempo(const Board &board, EvaluationState &es, bool verbose = false);

    /// Useful Bitmask helper functions
    Bitboard file_fill_sq(Square_t sq);
    Bitboard east_attack_file_fill(Bitboard bb);
    Bitboard west_attack_file_fill(Bitboard bb);
    Bitboard white_front_span(const Board& board);
    Bitboard black_front_span(const Board& board);
    Bitboard white_rear_span(const Board& board);
    Bitboard black_rear_span(const Board& board);
    Bitboard interspan(const Board& board);
    Bitboard closed_files(const Board& board);
    Bitboard open_files(const Board& board);
    Bitboard white_half_open_files(const Board& board);
    Bitboard black_half_open_files(const Board& board);
    Bitboard semiclosed_files(const Board& board);
    Bitboard white_open_pawns(const Board& board);
    Bitboard black_open_pawns(const Board& board);
    Bitboard open_pawns(const Board& board);
    Bitboard white_attack_front_spans(const Board& board);
    Bitboard black_attack_front_spans(const Board& board);
    Bitboard white_attack_rear_spans(const Board& board);
    Bitboard black_attack_rear_spans(const Board& board);
    FileSet_t file_set(Bitboard bb);
    FileSet_t island_east_files(FileSet_t sq);
    FileSet_t island_west_files(FileSet_t sq);
    Centipawns_t pawn_island_differential(const Board& board);
    Bitboard no_east_neighbors(Bitboard bb);
    Bitboard no_west_neighbors(Bitboard bb);
    Bitboard isolanis(Bitboard bb);
    Bitboard half_isolanis(Bitboard bb);
    Bitboard pawns_with_west_neighbors(Bitboard bb);
    Bitboard pawns_with_east_neighbors(Bitboard bb);
    Bitboard duo(Bitboard bb);
    Bitboard white_hanging_pawns(const Board& board);
    Bitboard black_hanging_pawns(const Board& board);
    Bitboard white_passed_pawns(const Board& board);
    Bitboard black_passed_pawns(const Board& board);
}

Centipawns_t evaluate(const Board& board, EvaluationState& es, bool verbose = false);

#endif // PROMETHEUS_EVAL_HPP
