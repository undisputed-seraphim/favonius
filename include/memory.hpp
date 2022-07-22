#ifndef _FAVONIUS_MEMORY_HPP_
#define _FAVONIUS_MEMORY_HPP_

#include <kernel.h>

#include "new.hpp"
#include "type_traits.hpp"
#include "utility.hpp"


namespace ztd {

// A special allocator (call semantics compatible with std::allocator)
// that has its own heap (sized HeapSize), instead of allocating from the global heap.
// (i.e. use k_heap_malloc and k_heap_free)
// Note that the actual amount of memory buffered is HeapSize + Z_HEAP_MIN_SIZE,
// as there is extra space required for bookkeeping.
template <typename T, size_t HeapSize>
struct allocator {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;
    constexpr static size_t heap_size = HeapSize + Z_HEAP_MIN_SIZE;

    constexpr allocator() noexcept {
        _heap.heap.init_mem = _heapdata;
        _heap.heap.init_bytes = heap_size;
        k_heap_init(&_heap, _heapdata, heap_size);
    }

    // Returns NULL if insufficient memory is available
    [[nodiscard]] constexpr T* allocate(size_t n) noexcept {
        return static_cast<T*>(k_heap_alloc(&_heap, sizeof(T) * n, K_NO_WAIT));
    }

    constexpr void deallocate(T* p, size_t n) noexcept {
        for (size_t m = 0; m < n; ++m) {
            k_heap_free(&_heap, p + (sizeof(T) * m));
        }
    }

private:
    struct k_heap _heap;
    alignas(8) uint8_t _heapdata[heap_size];
};

// Partial specialization where HeapSize is 0.
// In this case, we dynamically allocate memory from the global heap (CONFIG_HEAP_MEM_POOL_SIZE)
// (i.e. use k_malloc and k_free instead)
template <typename T>
struct allocator<T, 0> {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;
    constexpr static size_t heap_size = 0;

    constexpr allocator() noexcept {}

    [[nodiscard]] constexpr T* allocate(size_t n) noexcept {
        return static_cast<T*>(k_malloc(sizeof(T) * n));
    }

    constexpr void deallocate(T* p, size_t n) noexcept {
        for (size_t m = 0; m < n; ++m) {
            k_free(p + (sizeof(T) * m));
        }
    }
};

} // namespace

#endif // _FAVONIUS_MEMORY_HPP_