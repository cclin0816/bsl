#pragma once

#include <bsl/empty.h>
#include <config.h>

#include <concepts>
#include <utility>

namespace bsl {

// pre-declare

template <typename _Tv>
class cdlln_t;
template <typename _Tv>
class cdll_t;
template <typename _Tp>
class cdll_itr;
template <typename _Tp>
class cdll_ritr;
template <typename Iterator, typename _Tp>
class cdll_itr_base;

// circular doubly linked list node
template <typename _Tv = empty_t>
class cdlln_t {
  using type = cdlln_t<_Tv>;

  template <typename Iterator, typename _Tp>
  friend class cdll_itr_base;
  template <typename _Tp>
  friend class cdll_itr;
  template <typename _Tp>
  friend class cdll_ritr;
  template <typename _Tp>
  friend class cdll_t;

 protected:
  type *prev = nullptr;
  type *next = nullptr;

 public:
  [[no_unique_address]] _Tv val;
  cdlln_t() noexcept = default;
  // this might fail to instantiate if copy or move constructor is used, since
  // deleted
  template <typename... Args>
  cdlln_t(Args &&...args) noexcept(noexcept(_Tv(std::forward<Args>(args)...)))
      : val(std::forward<Args>(args)...) {}
  // no copy and move
  cdlln_t(const type &) = delete;
  cdlln_t(type &&) = delete;
  type &operator=(const type &) = delete;
  type &operator=(type &&) = delete;

  void unlink() noexcept {
    prev->next = next;
    next->prev = prev;
  }
  void push_back(type *node) noexcept {
    node->prev = prev;
    node->next = this;
    prev->next = node;
    prev = node;
  }
  void push_front(type *node) noexcept {
    node->prev = this;
    node->next = next;
    next->prev = node;
    next = node;
  }
};

// use CRTP to avoid duplicate code for reverse iterator

// iterator base for circular doubly linked list
template <typename Iterator, typename _Tp>
class cdll_itr_base {
  using type = cdll_itr_base<Iterator, _Tp>;
  friend Iterator;

 protected:
  _Tp *node = nullptr;

 public:
  cdll_itr_base() noexcept = default;
  cdll_itr_base(const type &) noexcept = default;
  cdll_itr_base &operator=(const type &) noexcept = default;
  cdll_itr_base(type &&) noexcept = default;
  cdll_itr_base &operator=(type &&) noexcept = default;
  cdll_itr_base(_Tp *node) noexcept : node(node) {}

  Iterator operator++(int) noexcept {
    Iterator tmp = *this;
    ++*this;
    return tmp;
  }
  Iterator operator--(int) noexcept {
    Iterator tmp = *this;
    --*this;
    return tmp;
  }
  Iterator operator+(const int n) noexcept {
    if (n >= 0) {
      for (int i = 0; i < n; ++i) {
        ++*this;
      }
    } else {
      for (int i = 0; i < -n; ++i) {
        --*this;
      }
    }
    return *this;
  }
  bool operator==(const Iterator &rhs) const noexcept {
    return node == rhs.node;
  }
  _Tp &operator*() const noexcept { return *node; }
  _Tp *operator->() const noexcept { return node; }
};

template <typename _Tp>
class cdll_itr : public cdll_itr_base<cdll_itr<_Tp>, _Tp> {
 private:
  using type = cdll_itr<_Tp>;
  using base = cdll_itr_base<cdll_itr<_Tp>, _Tp>;

 public:
  cdll_itr() noexcept = default;
  template <typename... Args>
  cdll_itr(Args &&...args) noexcept : base(std::forward<Args>(args)...) {}
  type &operator++() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->next;
    return *this;
  }
  type &operator--() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->prev;
    return *this;
  }
};

template <typename _Tp>
class cdll_ritr : public cdll_itr_base<cdll_ritr<_Tp>, _Tp> {
 private:
  using type = cdll_ritr<_Tp>;
  using base = cdll_itr_base<cdll_ritr<_Tp>, _Tp>;

 public:
  cdll_ritr() noexcept = default;
  template <typename... Args>
  cdll_ritr(Args &&...args) noexcept : base(std::forward<Args>(args)...) {}
  type &operator++() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->prev;
    return *this;
  }
  type &operator--() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->next;
    return *this;
  }
};

// circular doubly linked list
template <typename _Tv = empty_t>
class cdll_t {
  using type = cdll_t<_Tv>;
  using node_type = cdlln_t<_Tv>;
  using empty_type = cdlln_t<empty_t>;

 private:
  empty_type head;

 public:
  cdll_t() noexcept = default;
  cdll_t(const type &) = delete;
  type &operator=(const type &) = delete;
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
  [[nodiscard]] auto begin() noexcept {
    return cdll_itr<node_type>(reinterpret_cast<node_type *>(head.next));
  }
  [[nodiscard]] auto begin() const noexcept {
    return cdll_itr<const node_type>(
        reinterpret_cast<const node_type *>(head.next));
  }
  [[nodiscard]] auto end() noexcept {
    return cdll_itr<node_type>(reinterpret_cast<node_type *>(&head));
  }
  [[nodiscard]] auto end() const noexcept {
    return cdll_itr<const node_type>(
        reinterpret_cast<const node_type *>(&head));
  }
  [[nodiscard]] auto rbegin() noexcept {
    return cdll_ritr<node_type>(reinterpret_cast<node_type *>(head.prev));
  }
  [[nodiscard]] auto rbegin() const noexcept {
    return cdll_ritr<const node_type>(
        reinterpret_cast<const node_type *>(head.prev));
  }
  [[nodiscard]] auto rend() noexcept {
    return cdll_ritr<node_type>(reinterpret_cast<node_type *>(&head));
  }
  [[nodiscard]] auto rend() const noexcept {
    return cdll_ritr<const node_type>(reinterpret_cast<const node_type>(&head));
  }
  [[nodiscard]] uint64_t size() const noexcept {
    uint64_t size = 0;
    for (auto &itr : *this) {
      ++size;
    }
    return size;
  }
};

}  // namespace bsl
