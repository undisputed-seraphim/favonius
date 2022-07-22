#ifndef _FAVONIUS_MUTEX_HPP_
#define _FAVONIUS_MUTEX_HPP_

#include <sys/mutex.h>
#include <sys/timeutil.h>

#include "utility.hpp"

namespace ztd {

// Mutex class. In zephyr, all mutexes are recursive (reentrant).
// Fulfills C++ Mutex( Lockable ( BasicLockable ), DefaultConstructible, Destructible, NonCopyable, NonMovable ) concept.
class mutex final {
public:
    mutex() noexcept;
    mutex(const mutex&) = delete;
    mutex(mutex&&) = delete;
    ~mutex() noexcept;

    // Locks the mutex.
    // If another thread has already locked the mutex, a call to lock will block execution until the lock is acquired.
    void lock() noexcept;

    // Tries to lock the mutex.
    // Returns immediately.
    // On successful lock acquisition returns true, otherwise returns false.
    bool try_lock() noexcept;

    // Unlocks the mutex.
    void unlock() noexcept;

    k_mutex* native_handle() noexcept {
        return &_mutex;
    }

private:
    struct k_mutex _mutex;
};

class timed_mutex final {
public:
    timed_mutex() noexcept;
    timed_mutex(const timed_mutex&) = delete;
    timed_mutex(timed_mutex&&) = delete;
    ~timed_mutex() noexcept;

    // Locks the mutex.
    // If another thread has already locked the mutex, a call to lock will block execution until the lock is acquired.
    void lock() noexcept;

    // Tries to lock the mutex.
    // Returns immediately.
    // On successful lock acquisition returns true, otherwise returns false.
    bool try_lock() noexcept;

    // Tries to lock the mutex.
    // Blocks until specified timeout_duration has elapsed or the lock is acquired, whichever comes first.
    // On successful lock acquisition returns true, otherwise returns false.
    // Unit is in milliseconds.
    bool try_lock_for(uint64_t timeout_duration_ms) noexcept;

    // Tries to lock the mutex.
    // Blocks until specified timeout_time has been reached or the lock is acquired, whichever comes first.
    // On successful lock acquisition returns true, otherwise returns false.
    //bool try_lock_until() noexcept;

    // Unlocks the mutex.
    void unlock() noexcept;

    k_mutex* native_handle() noexcept {
        return &_mutex;
    }

private:
    struct k_mutex _mutex;
};

// Empty structs that acts as tags for unique_lock.
struct defer_lock_t {
    explicit defer_lock_t() = default;
};
struct try_to_lock_t {
    explicit try_to_lock_t() = default;
};
struct adopt_lock_t {
    explicit adopt_lock_t() = default;
};

inline constexpr defer_lock_t defer_lock {};
inline constexpr try_to_lock_t try_to_lock {};
inline constexpr adopt_lock_t adopt_lock {};

template <typename Mutex>
struct lock_guard final {
public:
    using mutex_type = Mutex;

    explicit lock_guard(Mutex& mutex) noexcept : _mutex(mutex) {
        _mutex.lock();    
    }

    lock_guard(const lock_guard&) = delete;
    lock_guard(lock_guard&&) = delete;
    ~lock_guard() noexcept {
        _mutex.unlock();
    }

private:
    mutex_type& _mutex;
};

template <typename Mutex>
struct unique_lock final {
public:
    using mutex_type = Mutex;

    unique_lock(mutex_type& mutex) noexcept : _mutex(mutex), _deferred(false) { _mutex.lock(); }
    unique_lock(mutex_type& mutex, ztd::defer_lock_t t)  noexcept : _mutex(mutex), _deferred(true)  {}
    unique_lock(mutex_type& mutex, ztd::try_to_lock_t t) noexcept : _mutex(mutex), _deferred(false) { _mutex.try_lock(); }
    unique_lock(mutex_type& mutex, ztd::adopt_lock_t t)  noexcept : _mutex(mutex), _deferred(false) {}
    unique_lock(const unique_lock&) = delete;
    unique_lock(unique_lock&& other) noexcept { _mutex = ztd::move(other._mutex); _deferred = other._deferred; }
    ~unique_lock() noexcept {
        if (!_deferred) {
            _mutex.unlock();
        }
    }

    void lock() noexcept {
        _mutex.lock();
    }

    bool try_lock() noexcept {
        return _mutex.try_lock();
    }

    void unlock() noexcept {
        _mutex.unlock();
    }

    mutex_type* mutex() const noexcept {
        return &_mutex;
    }

    bool owns_lock() const noexcept {
        return _mutex.native_handle()->lock_count > 0;
    }

    operator bool() const noexcept {
        return owns_lock();
    }

private:
    mutex_type& _mutex;
    bool _deferred;
};

} // namespace

#endif // _FAVONIUS_MUTEX_HPP_