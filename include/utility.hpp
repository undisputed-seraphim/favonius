#ifndef _FAVONIUS_UTILITY_HPP_
#define _FAVONIUS_UTILITY_HPP_

#include "type_traits.hpp"

#if defined(FAVONIUS_ALLOW_STD_HEADERS)
#if FAVONIUS_ALLOW_STD_HEADERS

#include <utility>

namespace ztd = std;

#else

namespace ztd {

template <typename T> struct remove_reference      { typedef T type; };
template <typename T> struct remove_reference<T&>  { typedef T type; };
template <typename T> struct remove_reference<T&&> { typedef T type; };

template <typename T>
typename remove_reference<T>::type&& move(T&& arg) {
    return static_cast<typename remove_reference<T>::type&&>(arg);
}

template <typename T>
inline constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename T>
inline constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
    static_assert(!ztd::is_lvalue_reference<T>::value, "Can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(t);
}

template <typename T1, typename T2>
struct pair {
public:
    using first_type = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    pair() : first(), second() {}
    pair(const T1& _first, const T2& _second) : first(_first), second(_second) {}

    pair(pair&& p) : first(move(p.first)), second(move(p.second)) {}

    template <typename U1, typename U2>
    pair(U1&& _first, U2&& _second) : first(forward<U1>(_first)), second(forward<U2>(_second)) {}

    template <typename U1, typename U2>
    pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}

    template <typename U1, typename U2Arg0, typename... U2Args>
    pair(U1&& _first, U2Arg0&& _u2arg0, U2Args&&... _u2args) : first(forward<U1>(_first)), second(forward<U2Arg0>(_u2arg0), forward<U2Args>(_u2args)...) {}

    pair& operator=(pair&& p) {
        first = move(p.first);
        second = move(p.second);
        return *this;
    }

    template <typename U1, typename U2>
    pair& operator=(const pair<U1, U2>& p) {
        first = move(p.first);
        second = move(p.second);
        return *this;
    }
};

} // namespace

#endif // FAVONIUS_ALLOW_STD_HEADERS
#endif // FAVONIUS_ALLOW_STD_HEADERS

#endif // _FAVONIUS_UTILITY_HPP_