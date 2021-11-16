#include "chess_clock.hpp"

#if defined(__APPLE__) && defined(__MACH__)
ChessClock::ChessClock() : start_time_(mach_absolute_time()), end_time_(start_time_), time_is_extensible_(false), allocated_time_(0) {}
#else
ChessClock::ChessClock() : start_time_(ClockType::now()), end_time_(start_time_), time_is_extensible_(false) {}
#endif

/**
 * some arbitrary number denoting the start time.
 * @return ChessClock::TimePoint representing the start time
 */
ChessClock::TimePoint ChessClock::start_time() noexcept {
    return start_time_;
}

/**
 * some arbitrary number denoting the start time.
 * @return ChessClock::TimePoint representing the start time
 */
ChessClock::TimePoint ChessClock::start_time() const noexcept {
    return start_time_;
}

/**
 * some arbitrary number denoting the end time.
 * @return ChessClock::TimePoint representing the start time
 */
ChessClock::TimePoint ChessClock::end_time() noexcept {
    return end_time_;
}

/**
 * some arbitrary number denoting the end time.
 * @return ChessClock::TimePoint representing the start time
 */
ChessClock::TimePoint ChessClock::end_time() const noexcept {
    return end_time_;
}

/**
 * boolean dictating the time allocated for can be extended
 * @return true if the time can be extended, false otherwise
 */
bool ChessClock::time_is_extensible() noexcept {
    return time_is_extensible_;
}

/**
 * boolean dictating the time allocated for can be extended
 * @return true if the time can be extended, false otherwise
 */
bool ChessClock::time_is_extensible() const noexcept {
    return time_is_extensible_;
}

/**
 * the number of milliseconds allocated for this move
 * @return the number of milliseconds (long long) allocated for this move
 */
Milliseconds_t ChessClock::allocated_time() noexcept {
    return allocated_time_;
}

/**
 * the number of milliseconds allocated for this move
 * @return the number of milliseconds (long long) allocated for this move
 */
Milliseconds_t ChessClock::allocated_time() const noexcept {
    return allocated_time_;
}

/**
 * starts the timer by setting the start_time_ member variable to the current time
 */
void ChessClock::start() noexcept {
#if defined(__APPLE__) && defined(__MACH__)
    start_time_ = mach_absolute_time();
#else
    start_time_ = ClockType::now();
#endif
}

/**
 * stops the timer by setting the end_time_ member variable to the current time.
 */
void ChessClock::stop() noexcept {
#if defined(__APPLE__) && defined(__MACH__)
    end_time_ = mach_absolute_time();
#else
    end_time_ = ClockType::now();
#endif
}

/**
 * computes the duration of time that the timer clock has been running
 * @return the duration in nanoseconds
 */
[[nodiscard]] Nanoseconds_t ChessClock::duration() const {
#if defined(__APPLE__) && defined(__MACH__)
    mach_timebase_info_data_t time_info;
    mach_timebase_info(&time_info);
    return (end_time_ - start_time_) * time_info.numer / time_info.denom;
#else
    return ChessClock::TimeUnit(end_time_ - start_time_).count();
#endif
}

/**
 * computes the number of milliseconds that may be allocated for this move and sets the allocated_time_ member variable
 * @param options the UCI_Option at play
 * @param side2move whose turn it is
 */
void ChessClock::alloc_time(UCIOptions& options, Color_t side_2_move) {
    if (options.search_to_depth_x != -1 || options.search_x_nodes != -1 || options.ponder || options.infinite) {
        /// If any of the above options are set, we don't need to time ourselves, just think until the associated
        /// condition is met
        time_is_extensible_ = false;
        allocated_time_ = -1;
        return;
    } else if (options.search_for_time_x != -1) {
        /// if the options say to search for a certain amount of time, then time isn't extensible
        time_is_extensible_ = false;
        allocated_time_ = options.search_for_time_x * 1000;
        return;
    }

    /// If we have just received milliseconds data, then we may use our own allocation algorithm:
    /// Assume that there are on average 40 moves (White and Black make moves) per game.
    /// Allow (total amount of time) / (number of moves)
    if (side_2_move == WHITE) {
        allocated_time_ = options.w_time_milli / 50;
    } else {
        allocated_time_ = options.b_time_milli / 50;
    }

    /// if there are fewer than 10 seconds left, don't extend time. Otherwise, time is extensible
    time_is_extensible_ = allocated_time_ >= (15000);
}

/**
 * the number of milliseconds that may be added on to the
 * @param num_ext the number of extensions
 */
void ChessClock::extend_time(unsigned num_ext) noexcept {
    /// if time is extensible, give 0.250 seconds for each extension
    if (time_is_extensible_) {
        time_is_extensible_ = false;
        allocated_time_ += num_ext * 100; // 0.10 seconds.
    }
}

/**
 * a boolean dictating whether the time has run out
 * @return true=has exceeded time, false=has more time
 */
bool ChessClock::has_exceeded_time() {
    Milliseconds_t delta = duration() / 1000000;
    bool ret = false;
    if (delta >= allocated_time_) {
        stop();
        ret = true;
    }
    return ret;
}
