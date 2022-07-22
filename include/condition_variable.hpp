// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#ifndef _FAVONIUS_CONDITION_VARIABLE_HPP_
#define _FAVONIUS_CONDITION_VARIABLE_HPP_

#include <kernel.h>

#include "mutex.hpp"

namespace ztd {

class condition_variable {
public:
    condition_variable() noexcept {
        [[maybe_unused]]int ec = k_condvar_init(&_condvar);
    }
    condition_variable(const condition_variable&) = delete;

    void notify_one() noexcept {
        [[maybe_unused]]int ec = k_condvar_signal(&_condvar);
    }

    void notify_all() noexcept {
        [[maybe_unused]]int woken_threads = k_condvar_broadcast(&_condvar);
    }

    void wait(ztd::unique_lock<ztd::mutex>& lock) noexcept {
        k_condvar_wait(&_condvar, lock.mutex()->native_handle(), K_FOREVER);
    }

    // Not available for now until I figure out how to avoid busy waiting since API does not support waiting on predicate.
    //template <typename Predicate>
    //void wait(ztd::unique_lock<ztd::mutex>& lock, Predicate stop_waiting) noexcept {
    //    k_condvar_wait(&_condvar, lock.mutex()->native_handle(), K_FOREVER);
    //}

    bool wait_for(ztd::unique_lock<ztd::mutex>& lock, uint64_t timeout_ms) noexcept {
        k_condvar_wait(&_condvar, lock.mutex()->native_handle(), K_MSEC(timeout_ms));
    }

    k_condvar* native_handle() noexcept {
        return &_condvar;
    }

private:
    struct k_condvar _condvar;
};

} // namespace

#endif // _FAVONIUS_CONDITION_VARIABLE_HPP_