// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#ifndef _FAVONIUS_ATOMIC_HPP_
#define _FAVONIUS_ATOMIC_HPP_

#include <kernel.h>

namespace ztd {

// Atomic variables in Zephyr are always 32-bit on 32-bit CPUs and 64-bit on 64-bit CPUs.
// Therefore standard behaviour of atomic variables larger than ptrdiff_t are not to be expected.

namespace _detail {

template <typename T, bool PtrSized>
struct atomic_impl {
    static_assert(false, "Compiler error: Must select one of the partially specialized templates.");
};

// For types T where the size is smaller than or equal to ptrdiff_t.
template <typename T>
struct atomic_impl<T, true> {
    using value_type = T;

    atomic_impl() noexcept = default;
    constexpr atomic_impl() noexcept(noexcept(T())) = default;
    constexpr atomic_impl(T desired) noexcept : _value(ATOMIC_INIT(desired)) {}

    atomic_impl(const atomic_impl&) = delete;

    void store(T desired) volatile noexcept {
        atomic_set(&_value, &desired);
    }

    T load() const noexcept {
        return atomic_get(_value);
    }

    operator T() const noexcept {
        return load();
    }

    bool compare_exchange_weak(T& expected, T desired) noexcept {
        return atomic_cas(&_value, desired, expected);
    }

private:
    atomic_t _value;
};

// For types T where the size is larger than ptrdiff_t.
template <typename T>
struct atomic_impl<T, false> {
public:
    using value_type = T;


private:
    ATOMIC_DEFINE(_value, sizeof(T));
};

} // namespace _detail

template <typename T, bool PtrSized = (sizeof(T) <= sizeof(ptrdiff_t))>
using atomic = _detail::atomic_impl<T, PtrSized>;

using atomic_char = atomic<char>;
using atomic_schar = atomic<signed char>;
using atomic_uchar = atomic<unsigned char>;
using atomic_short = atomic<short int>;
using atomic_ushort = atomic<unsigned short int>;
using atomic_int = atomic<int>;
using atomic_uint = atomic<unsigned int>;
using atomic_long = atomic<long>;
using atomic_ulong = atomic<unsigned long>;
using atomic_llong = atomic<long long>;
using atomic_ullong = atomic<unsigned long long>;

using atomic_int8_t = atomic<int8_t>;
using atomic_uint8_t = atomic<uint8_t>;
using atomic_int16_t = atomic<int16_t>;
using atomic_uint16_t = atomic<uint16_t>;
using atomic_int32_t = atomic<uint32_t>;
using atomic_uint32_t = atomic<uint32_t>;
using atomic_int64_t = atomic<uint64_t>;
using atomic_uint64_t = atomic<uint64_t>;

} // namespace

#endif // _FAVONIUS_ATOMIC_HPP_