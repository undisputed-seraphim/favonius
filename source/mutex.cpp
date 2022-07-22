// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#include "mutex.hpp"

namespace ztd {

mutex::mutex() noexcept {
    int ec = k_mutex_init(&_mutex);
    if (ec != 0) {
        // TODO: Error-handling, but don't throw
    }
}

mutex::~mutex() noexcept {
    unlock();
}

void mutex::lock() noexcept {
    [[maybe_unused]] int ec = k_mutex_lock(&_mutex, K_FOREVER);
}

bool mutex::try_lock() noexcept {
    int ec = k_mutex_lock(&_mutex, K_NO_WAIT);
    return (ec == 0);
}

void mutex::unlock() noexcept {
    [[maybe_unused]] int ec = k_mutex_unlock(&_mutex);
}

timed_mutex::timed_mutex() noexcept {
    [[maybe_unused]] int ec = k_mutex_init(&_mutex);
}

timed_mutex::~timed_mutex() noexcept {
    unlock();
}

void timed_mutex::lock() noexcept {
    [[maybe_unused]] int ec = k_mutex_lock(&_mutex, K_FOREVER);
}

bool timed_mutex::try_lock() noexcept {
    int ec = k_mutex_lock(&_mutex, K_NO_WAIT);
    return (ec == 0);
}

bool timed_mutex::try_lock_for(uint64_t timeout_duration_ms) noexcept {
    int ec = k_mutex_lock(&_mutex, K_MSEC(timeout_duration_ms));
    return (ec == 0);
}

void timed_mutex::unlock() noexcept {
    [[maybe_unused]] int ec = k_mutex_unlock(&_mutex);
}

} // namespace