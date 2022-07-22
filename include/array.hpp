#ifndef _FAVONIUS_UTILITY_HPP_
#define _FAVONIUS_UTILITY_HPP_

#include "type_traits.hpp"

#if defined(FAVONIUS_ALLOW_STD_HEADERS)
#if FAVONIUS_ALLOW_STD_HEADERS

#include <array>

namespace ztd = std;

#else

namespace ztd {

template <typename T, unsigned long long N>
class array final {
public:
    using value_type = T;
    using size_type = unsigned long long;
    using difference_type = unsigned long long;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    // TODO
    ~array() noexcept {}

    constexpr       T* data()       noexcept { return _data; }
    constexpr const T* data() const noexcept { return _data; }

    constexpr size_type     size() const noexcept { return N; }
    constexpr size_type max_size() const noexcept { return N; }

    constexpr void fill(const T& value) noexcept {
        for (size_type i = 0; i < N; ++i) {
            _data[i] = value;
        }
    }

    constexpr void swap(array& other) noexcept {
        const T *const ptr = other._data;
        other._data = _data;
        _data = ptr;
    }

private:
    T _data[N];
};

} // namespace

#endif // FAVONIUS_ALLOW_STD_HEADERS
#endif // FAVONIUS_ALLOW_STD_HEADERS

#endif // _FAVONIUS_UTILITY_HPP_