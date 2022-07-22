// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Tan Li Boon

#ifndef _FAVONIUS_LIST_HPP_
#define _FAVONIUS_LIST_HPP_

#include <sys/slist.h>
#include <kernel.h>

#include "memory.hpp"

namespace fav {

// Doubly-linked list of type T.
// Heap memory is statically allocated, specified by HeapSize.
// If HeapSize is 0, then memory will be allocated from the global system heap (CONFIG_HEAP_MEM_POOL_SIZE)
// Upon allocation failure, member functions that allocate will return false.
// Member functions are NOT thread safe.
template <typename T, size_t HeapSize = 0, template<typename, size_t> typename Allocator = ztd::allocator>
class List final {
private:
    struct Node;
    using NodeType = typename List<T>::Node;
public:
    using ValueType = T;
    using AllocatorType = Allocator<NodeType, HeapSize>;

    List() noexcept : _alloc() { sys_dlist_init(&_list); }

    bool Empty() noexcept {
        return sys_dlist_is_empty(&_list);
    }

    // \brief Retrieves the value at the index of this list. Note that this list wraps around, i.e.
    // if requested index is larger than the size of the list, indexing will rotate to the beginning.
    // Therefore if the list is non-empty, a value will always be returned.
    // If the list is empty, a default-constructed T will be returned.
    // \return A copy.
    const T& At(size_t index) const noexcept {
        if (Empty()) {
            return T();
        }
        sys_dnode_t* curentry = _list.head;
        size_t counter = 0;
        while (counter < index && curentry->next != _list.head) {
            curentry = curentry->next;
            counter++;
        }
        if (counter != index) {
            // Not found, requested index exceeds size of list
        }
        return CONTAINER_OF(curentry, NodeType, dnode)->value;
    }

    // Runtime: O(n)
    size_t Size() const noexcept {
        size_t length = 0;
        sys_dnode_t* ptr = _list.head;
        while (ptr != NULL && ptr->next != _list.head) {
            length++;
            ptr = ptr->next;
        }
        return length;
    }

    const T& Front() const noexcept {
        NodeType* node = CONTAINER_OF(_list.head, NodeType, dnode);
        return node->value;
    }

    const T& Back() const noexcept {
        NodeType* node = CONTAINER_OF(_list.tail, NodeType, dnode);
        return node->value;
    }

    void Clear() noexcept {
        while (!Empty()) {
            PopFront();
        }
    }

    // Move-constructs T at the back of the list.
    T& EmplaceBack(T&& value) noexcept {
        NodeType* node = _AllocateNode();
        node->value = ztd::move(value);
        sys_dlist_append(&_list, &node->dnode);
        return node->value;
    }

    // Constructs T in-place at the back of the list.
    // Effectively calls EmplaceBack(T&&).
    template <typename... Args>
    T& EmplaceBack(Args&&... args) noexcept {
        T value(ztd::forward<Args>(args)...);
        return EmplaceBack(ztd::move(value));
    }

    // Copy-constructs T at the back of the list.
    void PushBack(const T& val) noexcept {
        NodeType* node = _AllocateNode();
        node->value = val;
        sys_dlist_append(&_list, &node->dnode);
    }

    // Move-constructs T at the front of the list.
    T& EmplaceFront(T&& value) noexcept {
        NodeType* node = _AllocateNode();
        node->value = ztd::move(value);
        sys_dlist_prepend(&_list, &node->dnode);
        return node->value;
    }

    // Constructs T in-place at the front of the list.
    // Effectively calls EmplaceFront(T&&).
    template <typename... Args>
    T& EmplaceFront(Args&&... args) noexcept {
        T value(ztd::forward<Args>(args)...);
        return EmplaceFront(ztd::move(value));
    }

    // Copy-constructs T at the front of the list.
    void PushFront(const T& val) noexcept {
        NodeType* node = _AllocateNode();
        node->value = val;
        sys_dlist_prepend(&_list, &node->dnode);
    }

    // Removes and returns the first entry of the list.
    T PopFront() noexcept {
        NodeType* node = CONTAINER_OF(_list.head, NodeType, dnode);
        T value = ztd::move(node->value);
        sys_dlist_remove(_list.head);
        _DeallocateNode(node);
        return value;
    }

    // Removes and returns the last entry of the list.
    T PopBack() noexcept {
        NodeType* node = CONTAINER_OF(_list.tail, NodeType, dnode);
        T value = ztd::move(node->value);
        sys_dlist_remove(_list.tail);
        _DeallocateNode(node);
        return value;
    }

    // Retrieves the index of value if it belongs to this list, otherwise returns -1.
    int32_t IndexOf(const T& value) const noexcept {
        sys_dnode_t* next = _list.head;
        int32_t counter = 0;
        do {
            NodeType* node = CONTAINER_OF(next, NodeType, dnode);
            if (value == node->value) {
                return counter;
            }
            counter++;
        } while (static_cast<bool>(next->next));
        return -1;
    }

    class iterator {
    private:
        // A single link is made of two pointers, which we will use to store the state of this iterator.
        sys_dnode_t* _cursor;
        sys_dnode_t* _start;
    public:
        explicit iterator(sys_dnode_t* cursor) noexcept : _cursor(cursor), _start(cursor) {}

        // Prefix
        iterator& operator++() noexcept {
            _cursor = _cursor->next;
            if (_cursor->next == _start) {
                _cursor = nullptr;
            }
            return *this;
        }

        iterator& operator--() noexcept {
            _cursor = _cursor->prev;
            if (_cursor->prev == _start->prev) {
                _cursor = nullptr;
            }
            return *this;
        }

        // Postfix
        iterator operator++(int) noexcept {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator operator--(int) noexcept {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const iterator other) const noexcept { return _cursor == other._cursor; }
        bool operator==(const sys_dnode_t* dnode) const noexcept { return _cursor == dnode; }
        bool operator!=(const iterator other) const noexcept { return _cursor != other._cursor; }
        bool operator!=(const sys_dnode_t* dnode) const noexcept { return _cursor != dnode; }
        T& operator*() const noexcept {
            NodeType* node = CONTAINER_OF(((_cursor == nullptr) ? _start : _cursor), NodeType, dnode);
            return node->value;
        }
    };
    using const_iterator = const iterator;

    iterator begin() noexcept { return iterator(_list.head); }
    iterator end() noexcept { return iterator(nullptr); }
    const_iterator cbegin() const noexcept { return iterator(_list.head); }
    const_iterator cend() const noexcept { return iterator(nullptr); }
    
private:
    // Each Node class is specialized depending on the parent List type.
    // The life cycle of Nodes are entirely controlled by the parent List, including its deallocation.
    // Nodes never exist outside of a List. This allows us to predictably control its allocation and deallocation:
    // When Push*() or Emplace*() is called, a node is allocated; when Pop*() or Clear() is called, a node is deallocated.
    // When the user needs to obtain the value T, a copy or reference of T is returned, not the Node.
    // If ref<T> is returned, user needs to ensure that the corresponding Node is not deleted while ref<T> is in use.
    struct Node {
    public:
        T value; // Underlying value
        sys_dnode_t dnode; // Link to next entry

        // In-place construction of T
        template <typename... Args>
        Node(Args&&... args) noexcept(noexcept(T(ztd::forward<Args>(args)...))) : dnode({nullptr}), value(ztd::forward<Args>(args)...) {}

        // Copy-construct from T
        Node(const T& value_) noexcept(noexcept(T(value_))) : dnode({nullptr}), value(value_) {}

        // Move-construct from T
        Node(T&& value_) noexcept(noexcept(T(ztd::move(value_)))) : dnode({nullptr}), value(ztd::move(value_)) {}

        Node(const Node& other) = delete;

        // Move constructor moves the underlying value, as well as the underlying link.
        // The original link is set to nullptr.
        Node(Node&& other) noexcept(noexcept(T(ztd::move(other.value)))) : value(ztd::move(other.value)), dnode(ztd::move(other.dnode)) {
            other.dnode.next = nullptr;
        }

        bool operator==(const Node& other) const noexcept { return (&dnode) == (&other.dnode); }

        //static void* operator new(size_t, AllocatorType& alloc) noexcept { return alloc.allocate(1); }
        //static void operator delete(void* ptr, AllocatorType& alloc) noexcept { alloc.deallocate(ptr, 1); }
    };

    sys_dlist_t _list;
    AllocatorType _alloc;

    // Convenience function to allocate memory for one NodeType.
    // It is OK to make a pointer and then ignore it, because memory is managed by the list.
    // Deletions should occur through the list.
    [[nodiscard]] constexpr NodeType* _AllocateNode() noexcept {
        return _alloc.allocate(1);
    }

    constexpr void _DeallocateNode(NodeType* ptr) noexcept {
        _alloc.deallocate(ptr, 1);
    }
};

} // namespace

#endif // _FAVONIUS_LIST_HPP_