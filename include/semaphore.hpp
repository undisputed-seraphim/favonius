// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#ifndef _FAVONIUS_SEMAPHORE_HPP_
#define _FAVONIUS_SEMAPHORE_HPP_

#include <kernel.h>

// This header implements the C++20 counting_semaphore on top of semaphore primitives from Zephyr.
// See https://en.cppreference.com/w/cpp/thread/counting_semaphore

namespace ztd {

template <int32_t LeastMaxValue>
class counting_semaphore {
public:
    constexpr explicit counting_semaphore(int32_t desired) noexcept {
        [[maybe_unused]] int ec = k_sem_init(&_sem, desired, LeastMaxValue);
    }
    counting_semaphore(const counting_semaphore&) = delete;
    ~counting_semaphore() noexcept {
        k_sem_reset(&_sem);
    }

    void release(int32_t update = 1) noexcept {
        while (update > 0) {
            k_sem_give(&_sem);
            update--;
        }
    }

    // Blocks until successful.
    void acquire() noexcept {
        k_sem_take(&_sem, K_FOREVER);
    }

    bool try_acquire() noexcept {
        return k_sem_take(&_sem, K_NO_WAIT) == 0;
    }

    //bool try_acquire_for(/*time*/) noexcept;

    //bool try_acquire_until(/*time*/) noexcept;

    // Non-std extensions available in Zephyr
    unsigned int Count() const noexcept {
        return k_sem_count_get(&_sem);
    }

    constexpr static int32_t max() noexcept {
        return LeastMaxValue;
    }

private:
    struct k_sem _sem;
};

using binary_semaphore = counting_semaphore<1>;

} // namespace

#endif // _FAVONIUS_SEMAPHORE_HPP_