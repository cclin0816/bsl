#pragma once

/*
Circular Doubly Linked List
This differs from the std::list, since list don't expose underlying nodes,
operations on cdll_t (list) and cdll_(r)itr (iterator) are for cdlln_t (node),
though underlying types can still be accessed through cdlln_t

This is made since raw pointer linked list (which is extensively used in OS,
libc ...) are not viable in stdandard library

Still, the functionallity of cdll_t is similar to std::list,
the main difference is that cdll_t is not a container, since it doesn't owns the
nodes nodes need to be created and destroyed manually, or when using stack as
underlying storage, push after lifetime starts, then pop before lifetime ends,
or else it will cause undefined behavior

Since it might be used as raw pointer linked list, move and copy semantics are
not supported, and regard of bare metal usage, default construct is zero
initialized, call init before using
*/

#include <bsl/empty.h>
#include <bsl/in_place.h>
#include <config.h>

#include <concepts>
#include <utility>

namespace bsl {

// pre-declare

template <typename _Tval>
class cdlln_t;
template <typename _Tval>
class cdll_t;
template <typename _Tnode>
class cdll_itr;
template <typename _Tnode>
class cdll_ritr;
template <typename _Titr, typename _Tnode>
class cdll_itr_base;

// circular doubly linked list node
template <typename Tval = empty_t>
class cdlln_t {
  template <typename _Titr, typename _Tnode>
  friend class cdll_itr_base;
  template <typename _Tnode>
  friend class cdll_itr;
  template <typename _Tnode>
  friend class cdll_ritr;
  template <typename _Tval>
  friend class cdll_t;

 public:
  using type = cdlln_t<Tval>;
  using value_type = Tval;

 protected:
  type *prev = nullptr;
  type *next = nullptr;

 private:
  [[no_unique_address]] value_type val{};

 public:
  cdlln_t() noexcept = default;

  /**
   * @brief node constructor for value
   * @param inplace_t inplace construction
   * @param args arguments forwards to value constructor
   */
  template <typename... Args>
  cdlln_t(in_place_t, Args &&...args) noexcept(
      noexcept(value_type(std::forward<Args>(args)...)))
      : val(std::forward<Args>(args)...) {}

  cdlln_t(const type &) = delete;
  cdlln_t(type &&) = delete;
  type &operator=(const type &) = delete;
  type &operator=(type &&) = delete;

  /**
   * @brief unlink node from list
   */
  void unlink() noexcept {
    prev->next = next;
    next->prev = prev;
  }

  /**
   * @brief insert node after this node
   * @param node node to insert
   */
  void push_back(type *node) noexcept {
    node->prev = prev;
    node->next = this;
    prev->next = node;
    prev = node;
  }

  /**
   * @brief insert node before this node
   * @param node node to insert
   */
  void push_front(type *node) noexcept {
    node->prev = this;
    node->next = next;
    next->prev = node;
    next = node;
  }

  /**
   * @brief get value
   * @return value
   */
  value_type &value() noexcept { return val; }

  /**
   * @brief get base pointer
   * @return base pointer
   */
  void *base() noexcept { return reinterpret_cast<void *>(this); }
};

// iterator base for circular doubly linked list
// inherit by forward and reverse iterator
// using CRTP to avoid duplicate code
template <typename Titr, typename Tnode>
class cdll_itr_base {
  friend Titr;

 public:
  using iterator = Titr;
  using type = cdll_itr_base<iterator, Tnode>;
  using node_type = Tnode;
  using value_type = node_type;
  // using node_type_remove_const = std::remove_const<node_type>::type;

 protected:
  node_type *node = nullptr;

 public:
  cdll_itr_base() noexcept = default;
  cdll_itr_base(node_type *node) noexcept : node(node) {}

  iterator operator+(const int n) noexcept {
    iterator itr = this->node;
    if (n >= 0) {
      for (int i = 0; i < n; ++i) {
        ++itr;
      }
    } else {
      for (int i = 0; i < -n; ++i) {
        --itr;
      }
    }
    return itr;
  }
  iterator operator-(const int n) noexcept { return *this + (-n); }
  iterator &operator+=(const int n) noexcept {
    *this = *this + n;
    return *static_cast<iterator *>(this);
  }
  iterator &operator-=(const int n) noexcept {
    *this = *this - n;
    return *static_cast<iterator *>(this);
  }
  friend bool operator==(const iterator &lhs, const iterator &rhs) noexcept {
    return lhs.node == rhs.node;
  }
  bool operator==(const node_type *rhs) const noexcept { return node == rhs; }
  node_type &operator*() const noexcept { return *node; }
  node_type *operator->() const noexcept { return node; }
  operator node_type *() const noexcept { return node; }
};

// circular doubly linked list iterator
template <typename Tnode>
class cdll_itr : public cdll_itr_base<cdll_itr<Tnode>, Tnode> {
 private:
  using type = cdll_itr<Tnode>;
  using base_type = cdll_itr_base<type, Tnode>;
  using node_type = Tnode;

 public:
  cdll_itr() noexcept = default;
  cdll_itr(node_type *node) noexcept : base_type(node) {}
  type &operator=(node_type *node) noexcept {
    base_type::node = node;
    return *this;
  }
  type &operator++() noexcept {
    base_type::node = base_type::node->next;
    return *this;
  }
  type &operator--() noexcept {
    base_type::node = base_type::node->prev;
    return *this;
  }
  type operator++(int) noexcept {
    type itr = *this;
    ++*this;
    return itr;
  }
  type operator--(int) noexcept {
    type itr = *this;
    --*this;
    return itr;
  }
};

// circular doubly linked list reverse iterator
template <typename Tnode>
class cdll_ritr : public cdll_itr_base<cdll_ritr<Tnode>, Tnode> {
 private:
  using type = cdll_ritr<Tnode>;
  using base_type = cdll_itr_base<type, Tnode>;
  using node_type = Tnode;

 public:
  cdll_ritr() noexcept = default;
  cdll_ritr(node_type *node) noexcept : base_type(node) {}
  type &operator=(node_type *node) noexcept {
    base_type::node = node;
    return *this;
  }
  type &operator++() noexcept {
    base_type::node = base_type::node->prev;
    return *this;
  }
  type &operator--() noexcept {
    base_type::node = base_type::node->next;
    return *this;
  }
  type operator++(int) noexcept {
    type itr = *this;
    ++*this;
    return itr;
  }
  type operator--(int) noexcept {
    type itr = *this;
    --*this;
    return itr;
  }
};

// circular doubly linked list
template <typename Tval = empty_t>
class cdll_t {
 public:
  using type = cdll_t<Tval>;
  using node_type = cdlln_t<Tval>;
  using empty_type = cdlln_t<empty_t>;
  using iterator = cdll_itr<node_type>;
  using const_iterator = cdll_itr<const node_type>;
  using reverse_iterator = cdll_ritr<node_type>;
  using const_reverse_iterator = cdll_ritr<const node_type>;
  using value_type = Tval;

 private:
  empty_type head;

 public:
  cdll_t() noexcept = default;
  cdll_t(const type &) = delete;
  type &operator=(const type &) = delete;
  cdll_t(type &&) = delete;
  type &operator=(type &&) = delete;

  void init() noexcept {
    head.next = &head;
    head.prev = &head;
  }

  [[nodiscard]] bool empty() const noexcept { return head.next == &head; }

  void push_back(node_type *node) noexcept {
    head.push_back(reinterpret_cast<empty_type *>(node));
  }

  void push_back(node_type &node) noexcept { push_back(&node); }

  void push_back(type &list) noexcept {
    if (list.empty()) {
      return;
    }
    list.head.next->prev = head.prev;
    head.prev->next = list.head.next;
    head.prev = list.head.prev;
    list.head.prev->next = &head;
    list.init();
  }

  /**
   * @brief pop node from the back of the list
   * @return node_type* pointer to node, PTR_FAIL if list is empty
   */
  node_type *pop_back() noexcept {
    if (empty()) [[unlikely]] {
      return (node_type *)PTR_FAIL;
    }
    empty_type *node = head.prev;
    head.prev = node->prev;
    head.prev->next = &head;
    return reinterpret_cast<node_type *>(node);
  }

  void push_front(node_type *node) noexcept {
    head.push_front(reinterpret_cast<empty_type *>(node));
  }

  void push_front(node_type &node) noexcept { push_front(&node); }

  void push_front(type &list) noexcept {
    if (list.empty()) {
      return;
    }
    list.head.prev->next = head.next;
    head.next->prev = list.head.prev;
    head.next = list.head.next;
    list.head.next->prev = &head;
    list.init();
  }

  /**
   * @brief pop node from the front of the list
   * @return node_type* pointer to node, PTR_FAIL if list is empty
   */
  node_type *pop_front() noexcept {
    if (empty()) [[unlikely]] {
      return (node_type *)PTR_FAIL;
    }
    empty_type *node = head.next;
    head.next = node->next;
    head.next->prev = &head;
    return reinterpret_cast<node_type *>(node);
  }

  [[nodiscard]] node_type *back() const noexcept {
    return reinterpret_cast<node_type *>(head.prev);
  }

  [[nodiscard]] node_type *front() const noexcept {
    return reinterpret_cast<node_type *>(head.next);
  }

  [[nodiscard]] iterator begin() noexcept {
    return {reinterpret_cast<node_type *>(head.next)};
  }

  [[nodiscard]] const_iterator begin() const noexcept {
    return {reinterpret_cast<const node_type *>(head.next)};
  }

  [[nodiscard]] const_iterator cbegin() const noexcept {
    return {reinterpret_cast<const node_type *>(head.next)};
  }

  [[nodiscard]] iterator end() noexcept {
    return {reinterpret_cast<node_type *>(&head)};
  }

  [[nodiscard]] const_iterator end() const noexcept {
    return {reinterpret_cast<const node_type *>(&head)};
  }

  [[nodiscard]] const_iterator cend() const noexcept {
    return {reinterpret_cast<const node_type *>(&head)};
  }

  [[nodiscard]] reverse_iterator rbegin() noexcept {
    return {reinterpret_cast<node_type *>(head.prev)};
  }

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return {reinterpret_cast<const node_type *>(head.prev)};
  }

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return {reinterpret_cast<const node_type *>(head.prev)};
  }

  [[nodiscard]] reverse_iterator rend() noexcept {
    return {reinterpret_cast<node_type *>(&head)};
  }

  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return {reinterpret_cast<const node_type>(&head)};
  }

  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return {reinterpret_cast<const node_type>(&head)};
  }

  /**
   * @brief size of the list, O(n)
   * @return uint64_t
   */
  [[nodiscard]] uint64_t size() const noexcept {
    uint64_t size = 0;
    for (auto &itr : *this) {
      ++size;
    }
    return size;
  }
};

}  // namespace bsl
