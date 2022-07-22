#ifndef _FAVONIUS_TYPE_TRAITS_HPP_
#define _FAVONIUS_TYPE_TRAITS_HPP_

#if defined(FAVONIUS_ALLOW_STD_HEADERS)
#if FAVONIUS_ALLOW_STD_HEADERS

#include <type_traits>

namespace ztd = std;

#else

namespace ztd {

template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant; // using injected-class-name
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template<typename T, typename U> struct is_same : false_type {};
template<typename T> struct is_same<T, T> : true_type {};

template<typename T> struct is_lvalue_reference     : false_type {};
template<typename T> struct is_lvalue_reference<T&> : true_type {};

template<typename T> struct remove_cv                   { using type = T; };
template<typename T> struct remove_cv<const T>          { using type = T; };
template<typename T> struct remove_cv<volatile T>       { using type = T; };
template<typename T> struct remove_cv<const volatile T> { using type = T; };
template<typename T> struct remove_const                { using type = T; };
template<typename T> struct remove_const<const T>       { using type = T; };
template<typename T> struct remove_volatile             { using type = T; };
template<typename T> struct remove_volatile<volatile T> { using type = T; };

template<typename T>
struct is_floating_point : integral_constant<bool,
    is_same<float,       typename remove_cv<T>::type>::value  ||
    is_same<double,      typename remove_cv<T>::type>::value  ||
    is_same<long double, typename remove_cv<T>::type>::value  >
{};

template <typename T>
struct is_integral : integral_constant<bool,
    is_same<bool, typename remove_cv<T>::type>::value ||
    // signed
    is_same<char,      typename remove_cv<T>::type>::value ||
    is_same<char16_t,  typename remove_cv<T>::type>::value ||
    is_same<char32_t,  typename remove_cv<T>::type>::value ||
    is_same<wchar_t,   typename remove_cv<T>::type>::value ||
    is_same<short,     typename remove_cv<T>::type>::value ||
    is_same<int,       typename remove_cv<T>::type>::value ||
    is_same<long,      typename remove_cv<T>::type>::value ||
    is_same<long long, typename remove_cv<T>::type>::value ||
    // unsigned
    is_same<unsigned char,      typename remove_cv<T>::type>::value ||
    is_same<unsigned short,     typename remove_cv<T>::type>::value ||
    is_same<unsigned int,       typename remove_cv<T>::type>::value ||
    is_same<unsigned long,      typename remove_cv<T>::type>::value ||
    is_same<unsigned long long, typename remove_cv<T>::type>::value >
{};

template< class T >
struct is_arithmetic : integral_constant<bool,
    is_integral<T>::value ||
    is_floating_point<T>::value>
{};

template<bool B, class T = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> { typedef T type; };

} // namespace

#endif // FAVONIUS_ALLOW_STD_HEADERS
#endif // FAVONIUS_ALLOW_STD_HEADERS

#endif // _FAVONIUS_TYPE_TRAITS_HPP_