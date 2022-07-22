// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#ifndef _FAVONIUS_THREAD_HPP_
#define _FAVONIUS_THREAD_HPP_

#include <kernel.h>
#include <kernel/thread.h>
#include <kernel/thread_stack.h>

#include "utility.hpp"

namespace ztd {

class thread final {
public:
    // Threads have 1 megabyte of stack. In the future this may become adjustable.
    static constexpr uint32_t stack_size = 1024 * 1024;

    // Creates new thread object which does not represent a thread.
    thread() noexcept;

    // Creates new ztd::thread object and associates it with a thread of execution.
    // Threads in Zephyr are limited to max 3 parameters.
    template <typename Function, typename Arg1, typename Arg2, typename Arg3>
    explicit thread(Function&& fn, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3) noexcept {
        // k_thread_entry_t is a function pointer with signature void(*)(void*, void*, void*)
        // So we must adapt the input fn to that signature.
        auto thread_entry = [fn = ztd::forward<Function>(fn)] (void* p1, void* p2, void* p3) -> void {
            return fn(*p1, *p2, *p3);
        };
        k_tid_t threadptr = k_thread_create(&_thread, &_stack, stack_size, thread_entry, &arg1, &arg2, &arg3, 0, K_USER, K_NO_WAIT);
    }

    // Creates new ztd::thread object and associates it with a thread of execution.
    // Threads in Zephyr are limited to max 3 parameters.
    template <typename Function, typename Arg1, typename Arg2>
    explicit thread(Function&& fn, Arg1&& arg1, Arg2&& arg2) noexcept {
        // k_thread_entry_t is a function pointer with signature void(*)(void*, void*, void*)
        // So we must adapt the input fn to that signature.
        auto thread_entry = [fn = ztd::forward<Function>(fn)] (void* p1, void* p2, void* p3) -> void {
            return fn(*p1, *p2);
        };
        k_tid_t threadptr = k_thread_create(&_thread, &_stack, stack_size, thread_entry, &arg1, &arg2, NULL, 0, K_USER, K_NO_WAIT);
    }

    // Creates new ztd::thread object and associates it with a thread of execution.
    // Threads in Zephyr are limited to max 3 parameters.
    template <typename Function, typename Arg1>
    explicit thread(Function&& fn, Arg1&& arg1) noexcept {
        // k_thread_entry_t is a function pointer with signature void(*)(void*, void*, void*)
        // So we must adapt the input fn to that signature.
        auto thread_entry = [fn = ztd::forward<Function>(fn)] (void* p1, void* p2, void* p3) -> void {
            return fn(*p1);
        };
        k_tid_t threadptr = k_thread_create(&_thread, &_stack, stack_size, thread_entry, &arg1, NULL, NULL, 0, K_USER, K_NO_WAIT);
    }

    // Creates new ztd::thread object and associates it with a thread of execution.
    // Threads in Zephyr are limited to max 3 parameters.
    template <typename Function>
    explicit thread(Function&& fn) noexcept {
        // k_thread_entry_t is a function pointer with signature void(*)(void*, void*, void*)
        // So we must adapt the input fn to that signature.
        auto thread_entry = [fn = ztd::forward<Function>(fn)] (void* p1, void* p2, void* p3) -> void {
            return fn();
        };
        k_tid_t threadptr = k_thread_create(&_thread, &_stack, stack_size, thread_entry, NULL, NULL, NULL, 0, K_USER, K_NO_WAIT);
    }

    // The copy constructor is deleted; threads are not copyable.
    thread(const thread&) = delete;
    thread(thread&&) noexcept;

    int join() noexcept;
    k_tid_t native_handle() noexcept;

    // API inherited from Zephyr
    const char* GetName() noexcept {
        return k_thread_name_get(&_thread);
    }

    int SetName(const char* str) noexcept {
        return k_thread_name_set(&_thread, str);
    }

    int GetPriority() noexcept {
        return k_thread_priority_get(&_thread);
    }

    void SetPriority(int priority) noexcept {
        k_thread_priority_set(&_thread, priority);
    }

    void Resume() noexcept {
        k_thread_resume(&_thread);
    }

    void Start() noexcept {
        k_thread_start(&_thread);
    }

    void Suspend() noexcept {
        k_thread_suspend(&_thread);
    }

    bool StateString(char* buffer, size_t buffer_size) noexcept {
        return k_thread_state_str(&_thread);
    }

    // Time delta in cycle units
    void SetDeadline(int deadline) noexcept {
        k_thread_deadline_set(&_thread, deadline);
    }

private:
    k_thread _thread;
    K_KERNEL_STACK_MEMBER(_stack, stack_size);
};

namespace this_thread {

void yield() noexcept { k_yield(); }

void sleep_for(uint64_t us) noexcept { k_usleep(us); }

k_tid_t get_id() noexcept { return k_current_get(); }

// \brief Sets the custom data for the current thread.
template <typename T>
void SetData(T& value) noexcept {
    k_thread_custom_data_set(&value);
}

// \brief Returns the custom data for the current thread.
template <typename T>
T& GetData() noexcept {
    return *static_cast<T*>(k_thread_custom_data_get());
}

} // namespace

} // namespace

#endif // _FAVONIUS_THREAD_HPP_