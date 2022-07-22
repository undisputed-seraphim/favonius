#ifndef _FAVONIUS_MAP_HPP_
#define _FAVONIUS_MAP_HPP_

#include <sys/rb.h>
#include "memory.hpp"

namespace ztd {

// std::map equivalent based on the red-black tree provided by Zephyr.
// API with std::map is similar, but there may be slight sematic differences.

// Unlike std::map, template parameter T must be implicitly comparable.
template <typename Key, typename T, size_t HeapSize = 0, template<typename, size_t> typename Allocator = ztd::allocator>
class map final {
private:
    struct Node;
    using NodeType = typename List<T>::Node;
public:
    using key_type = Key;
    using mapped_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using allocator_type = Allocator<NodeType, HeapSize>;

    class iterator {
    private:
        rbnode* _node;
    public:
        iterator(rbnode* node) noexcept : _node(node) {}

        iterator& operator++() noexcept {
            _node = _node->children
            return *this;
        }

        operator bool() const noexcept { return static_cast<bool>(_snode); }

        bool operator==(iterator other) const noexcept { return _snode == other._snode; }
        bool operator==(sys_snode_t* snode) const noexcept { return _snode == snode; }
        T operator*() noexcept {
            NodeType* node = nullptr;
            node = SYS_SLIST_CONTAINER(_snode, node, snode);
            return node->value;
        }
    };

    iterator begin() noexcept { return iterator(_list.head); }
    iterator end() noexcept { return iterator(_list.tail); }

    map() noexcept { _tree_init(); }

    template <typename... Args>
    ztd::pair<iterator, value_type> emplace(Args&&... args) noexcept {
        NodeType* node = _alloc.allocate(1);
        node->value = T(ztd::forward<Args>(args)...);

        //rb_insert(&_tree, )
    }

private:

    struct Node {
    public:
        T value;
        rbnode node;
        
        // In-place construction of T
        template <typename... Args>
        Node(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) : node({nullptr}), value(std::forward<Args>(args)...) {}

        // Copy-construct from T
        Node(const T& value_) noexcept(noexcept(T(value_))) : node({nullptr}), value(value_) {}

        // Move-construct from T
        Node(T&& value_) noexcept(noexcept(T(std::move(value_)))) : node({nullptr}), value(std::move(value_)) {}

        Node(const Node& other) = delete;

        bool operator==(const Node& other) const noexcept {
            return (&node) == (&other.node);
        }
        bool operator<(const Node& other) const noexcept {
            return value < other.value;
        }
    };
    using NodeType = Node<T>;

    struct rbtree _tree;
    allocator_type _alloc;

    static constexpr void _tree_init() noexcept {
        _tree.lessthan_fn = [](struct rbnode* left, struct rbnode* right) noexcept -> bool {
            NodeType* leftptr  = CONTAINER_OF(left, NodeType, node);
            NodeType* rightptr = CONTAINER_OF(right, NodeType, node);
            return (*leftptr) < (*rightptr);
        };
    }
};

} // namespace

#endif // _FAVONIUS_MAP_HPP_