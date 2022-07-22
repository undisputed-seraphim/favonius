// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#ifndef _FAVONIUS_RINGBUFFER_HPP_
#define _FAVONIUS_RINGBUFFER_HPP_

#include <sys/ring_buffer.h>
#include <kernel.h>

#include "memory.hpp"

namespace fav {

// Fix-sized ring buffer.
// Template parameter T should be default-nothrow-constructible and copiable. (Can lead to undefined behaviours in Peek() if not copiable)
// Template parameter N is the maximum number of entries.
// TODO incomplete.
template <typename T = uint32_t, uint32_t Size = 16>
struct RingBuffer final {
public:
    using ValueType = T;
    constexpr static uint32_t BufferSizeBytes = sizeof(T) * Size;

    RingBuffer() noexcept {
        ring_buf_init(&_ring_buf, Size, _ring_buf_data);
    }
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&& other) noexcept : _buf(other._buf) {
        other = RingBuffer();
    }

    void Reset() noexcept {
        _buf = ring_buf_reset(_buf);
    }

    // Get the maximum capacity of the buffer, in terms of number of entries,
    uint32_t Capacity() const noexcept {
        return ring_buf_capacity_get(_buf) / sizeof(T);
    }

    // Get the number of entries filled so far.
    uint32_t Size() const noexcept {
        return ring_buf_size_get(_buf) / sizeof(T);
    }

    // Get the amount of free entry slots.
    uint32_t FreeSpace() const noexcept {
        return ring_buf_space_get(_buf) / sizeof(T);
    }

    bool Empty() const noexcept {
        return ring_buf_is_empty(_buf);
    }

    // Copy data into this ring buffer.
    void Push(const T& data) noexcept {
        uint32_t bytes_put = ring_buf_put(_buf, static_cast<uint8_t*>(&data), sizeof(T));
        __ASSERT(bytes_put == sizeof(T), "Insufficient space in ring buffer.");
    }

    // Write the first value from the read end of this RingBuffer into the provided object.
    // The value in this RingBuffer is then removed.
    void Pop(T& value) const noexcept {
        uint32_t bytes_get = ring_buf_get(_buf, static_cast<uint8_t*>(&value), sizeof(T));
        __ASSERT(bytes_get == sizeof(T), "Fewer bytes were fetched than expected.");
    }

    // Discard the first read value.
    void Pop() const noexcept {
        uint32_t bytes_get = ring_buf_get(_buf, NULL, sizeof(T));
    }

    // Retrieve the entry from the reading end, without removal.
    uint32_t Peek(T* data) const noexcept {
        return ring_buf_peek(_buf, static_cast<uint8_t*>(data), sizeof(T));
    }

    // Retrieve a copy of the entry from the reading end, without removal.
    T Peek() const noexcept {
        T value;
        uint32_t bytes_peek = ring_buf_peek(_buf, static_cast<uint8_t*>(&value), sizeof(T));
        __ASSERT(bytes_peek == sizeof(T), "Fewer bytes were fetched than expected.");
        return T(value); // Explicit copy, we do not want to move
    }

    // Constructs T and then calls Push.
    // No special in-place construction semantics, this is for convenience only.
    template <typename... Args>
    void Emplace(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) {
        T value(std::forward<Args>(args)...);
        Push(value);
    }
    
private:
    struct ring_buf _ring_buf;
    uint32_t _ring_buf_data[1U << Size];
};

} // namespace

#endif // _FAVONIUS_RINGBUFFER_HPP_