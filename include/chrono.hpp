#ifndef _FAVONIUS_CHRONO_HPP_
#define _FAVONIUS_CHRONO_HPP_

#include <kernel.h>

#if defined(CONFIG_SYS_CLOCK_EXISTS)
#if CONFIG_SYS_CLOCK_EXISTS
#if defined(CONFIG_SYS_CLOCK_TICKS_PER_SEC)
constexpr int SysClockTicksPerSecond = CONFIG_SYS_CLOCK_TICKS_PER_SEC;
#endif // defined(CONFIG_SYS_CLOCK_TICKS_PER_SEC)
#endif // CONFIG_SYS_CLOCK_EXISTS
#else
constexpr int SysClockTicksPerSecond = 10000;
#endif // defined(CONFIG_SYS_CLOCK_EXISTS)

// Chrono is pretty complex, this header is meant to only implement the very frequently accessed stuff.
// I might complete it more over time though.

namespace ztd {

template <typename Rep, uint64_t Num, uint64_t Denom>
class duration {
public:
    static_assert(Num > 0, "Numerator must be greater than zero.");
    static_assert(Denom > 0, "Denominator must be greater than zero.");
    static constexpr uint64_t numerator = Num;
    static constexpr uint64_t denominator = Denom;

    constexpr duration() noexcept : _ticks(0) {}
    constexpr duration(Rep ticks) noexcept : _ticks(ticks) {}

    template <typename Rep2, uint64_t Num2, uint64_t Denom2>
    constexpr explicit duration(const duration<Rep2, Num2, Denom2>& d) noexcept {
        double numRatio = static_cast<double>(d.numerator) / numerator;
        double denomRatio = static_cast<double>(denominator) / d.denominator;
        _ticks = static_cast<Rep>(d._ticks * numRatio * denomRatio);
    }

    constexpr Rep count() const noexcept {
        return _ticks;
    }

    constexpr duration& operator+(const duration& other) noexcept {
        _ticks += other._ticks;
        return *this;
    }

    constexpr duration& operator-(const duration& other) noexcept {
        _ticks -= other._ticks;
        return *this;
    }

    constexpr duration& operator++() noexcept {
        _ticks += 1;
        return *this;
    }

    constexpr duration& operator++(int i) noexcept {
        _ticks += i;
        return *this;
    }

    constexpr duration& operator--() noexcept {
        _ticks -= 1;
        return *this;
    }

    constexpr duration& operator--(int i) noexcept {
        _ticks -= i;
        return *this;
    }

    static constexpr duration zero() noexcept {
        return duration();
    }
private:
    Rep _ticks;
};

using nanoseconds  = duration<int64_t, 1, 1'000'000'000>;
using microseconds = duration<int64_t, 1, 1'000'000>;
using milliseconds = duration<int64_t, 1, 1'000>;
using seconds      = duration<int64_t, 1, 1>;
using minutes      = duration<int64_t, 60, 1>;
using hours        = duration<int64_t, 60*60, 1>;
using days         = duration<int64_t, 60*60*24, 1>;
using weeks        = duration<int64_t, 60*60*24*7, 1>;
using years        = duration<int64_t, uint64_t(60*60*24*365.2425), 1>;

} // namespace

namespace fav {

ztd::seconds FromKTime(const k_timeout_t& ktime) noexcept {
    return ztd::seconds(ktime.ticks / SysClockTicksPerSecond);
}

template <typename DurationType>
k_timeout_t ToKTime(const DurationType&) noexcept {
    static_assert(false, "Unsupported kernel time conversion.");
}

template <>
k_timeout_t ToKTime<ztd::nanoseconds>(const ztd::nanoseconds& val) noexcept {
    return K_NSEC(val.count());
}

template <>
k_timeout_t ToKTime<ztd::microseconds>(const ztd::microseconds& val) noexcept {
    return K_USEC(val.count());
}

template <>
k_timeout_t ToKTime<ztd::milliseconds>(const ztd::milliseconds& val) noexcept {
    return K_MSEC(val.count());
}

template <>
k_timeout_t ToKTime<ztd::seconds>(const ztd::seconds& val) noexcept {
    return K_SECONDS(val.count());
}

template <>
k_timeout_t ToKTime<ztd::minutes>(const ztd::minutes& val) noexcept {
    return K_MINUTES(val.count());
}

template <>
k_timeout_t ToKTime<ztd::hours>(const ztd::hours& val) noexcept {
    return K_HOURS(val.count());
}

} // namespace

#endif // _FAVONIUS_CHRONO_HPP_