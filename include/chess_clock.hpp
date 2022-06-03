#ifndef PROMETHEUS_CHESS_CLOCK_HPP
#define PROMETHEUS_CHESS_CLOCK_HPP

/// Standard Library Includes
#include <ios>
#include <iostream>

/// Third-Party Includes
#include <spdlog/spdlog.h>

/// Project Includes
#include "defines.hpp"
#include "extern.hpp"
#include "uci_options.hpp"

/// OS-Specific Includes
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
#else
#include <chrono>
#endif

class ChessClock {
#if defined(__APPLE__) && defined(__MACH__)
    using TimePoint = uint64_t;
#else
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
#endif
#if defined(__APPLE__) && defined(__MACH__)
#else
    using ClockType = std::chrono::high_resolution_clock;
	using TimeUnit  = std::chrono::nanoseconds;
#endif
    TimePoint start_time_;
    TimePoint end_time_;
    bool time_is_extensible_;
    Milliseconds_t allocated_time_;

public:
    explicit ChessClock();

    TimePoint start_time() noexcept;
    [[nodiscard]] TimePoint start_time() const noexcept;
    TimePoint end_time() noexcept;
    [[nodiscard]] TimePoint end_time() const noexcept;
    bool time_is_extensible() noexcept;
    [[nodiscard]] bool time_is_extensible() const noexcept;
    Milliseconds_t allocated_time() noexcept;
    [[nodiscard]] Milliseconds_t allocated_time() const noexcept;
    [[nodiscard]] Nanoseconds_t duration() const;
    void start() noexcept;
    void stop() noexcept;
    void alloc_time(UCIOptions& options, Color_t side_2_move);
    void extend_time(unsigned num_extensions) noexcept;
    bool has_exceeded_time();

    /**
     * publishes the ChessClock information to an output stream
     * @param os a reference to the output stream
     * @param cc a reference to an instance of the ChessClock class
     * @return   a reference to the output stream that's just been updated.
     */
    friend std::ostream& operator<<(std::ostream& os, const ChessClock& cc) {
        os << std::boolalpha;

        Milliseconds_t time = cc.duration() / 1000000;

        os << "ChessClock:" << std::endl <<
           "- Split:" << time << " [ms]" << std::endl <<
           "  Time is Extensible: " << cc.time_is_extensible() << std::endl <<
           "  Allocated Time: " << cc.allocated_time() << " [ms]" << std::endl;
        os << std::noboolalpha;

        return os;
    }
};

#endif // PROMETHEUS_CHESS_CLOCK_HPP
