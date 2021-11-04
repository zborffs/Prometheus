#ifndef PROMETHEUS_STOPWATCH_HPP
#define PROMETHEUS_STOPWATCH_HPP

/// Standard Library Includes
#include <ios>
#include <iostream>

/// OS-Specific Includes
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
#else
#include <chrono>
#endif

namespace zaamath {
    using Milliseconds_t = uint64_t;
    using Nanoseconds_t = uint64_t;

    class StopWatch {
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

    public:
#if defined(__APPLE__) && defined(__MACH__)
        explicit StopWatch() : start_time_(mach_absolute_time()), end_time_(start_time_){}
#else
        explicit StopWatch() : start_time_(ClockType::now()), end_time_(start_time_) {}
#endif

        TimePoint start_time() noexcept {return start_time_;}
        [[nodiscard]] TimePoint start_time() const noexcept {return start_time_;}
        TimePoint end_time() noexcept {return end_time_;}
        [[nodiscard]] TimePoint end_time() const noexcept {return end_time_;}
        [[nodiscard]] Nanoseconds_t duration() const {
#if defined(__APPLE__) && defined(__MACH__)
            mach_timebase_info_data_t time_info;
            mach_timebase_info(&time_info);
            return (end_time_ - start_time_) * time_info.numer / time_info.denom;
#else
            return ChessClock::TimeUnit(end_time_ - start_time_).count();
#endif
        }
        void start() noexcept {
#if defined(__APPLE__) && defined(__MACH__)
            start_time_ = mach_absolute_time();
#else
            start_time_ = ClockType::now();
#endif
        }
        void stop() noexcept {
#if defined(__APPLE__) && defined(__MACH__)
            end_time_ = mach_absolute_time();
#else
            end_time_ = ClockType::now();
#endif
        }

        /**
         * publishes the ChessClock information to an output stream
         * @param os a reference to the output stream
         * @param cc a reference to an instance of the ChessClock class
         * @return   a reference to the output stream that's just been updated.
         */
        friend std::ostream& operator<<(std::ostream& os, const StopWatch& cc) {
            Milliseconds_t time = cc.duration() * 1e-6; // nanoseconds / milliseconds = 1e-6
            os << time;
            return os;
        }
    };
};

#endif //PROMETHEUS_STOPWATCH_HPP
