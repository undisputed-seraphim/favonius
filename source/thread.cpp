// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#include "thread.hpp"

namespace ztd {

thread::thread() noexcept : _thread({}) {}

int thread::join() noexcept {
    return k_thread_join(&_thread, K_NO_WAIT);
}

k_tid_t thread::native_handle() noexcept {
    return &_thread;
}

};