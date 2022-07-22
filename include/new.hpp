#ifndef _FAVONIUS_NEW_HPP_
#define _FAVONIUS_NEW_HPP_

#include <kernel.h>

// Operator new cannot throw exceptions. If insufficient memory, return NULL instead.
[[nodiscard]] void* operator new  (size_t count) noexcept { return k_malloc(count); }
[[nodiscard]] void* operator new[](size_t count) noexcept { return k_malloc(count); }

// Placement new
[[nodiscard]] void* operator new  (size_t, void* ptr) noexcept { return ptr; }
[[nodiscard]] void* operator new[](size_t, void* ptr) noexcept { return ptr; }

#endif // _FAVONIUS_NEW_HPP_