#pragma once

#include <config.h>

#include <concepts>
#include <utility>

namespace bsl {

// pre-declare

class cdlln_t;
class cdll_t;
template <typename Tp>
concept is_cdlln =
    std::is_same_v<std::remove_const_t<std::remove_pointer_t<Tp>>, cdlln_t>;
template <is_cdlln Tp>
class cdll_itr;
template <is_cdlln Tp>
class cdll_ritr;
template <typename Iterator, is_cdlln Tp>
class cdll_itr_base;

// circular doubly linked list node
class cdlln_t {
  template <typename Iterator, is_cdlln Tp>
  friend class cdll_itr_base;
  template <is_cdlln Tp>
  friend class cdll_itr;
  template <is_cdlln Tp>
  friend class cdll_ritr;
  friend class cdll_t;

 protected:
  cdlln_t *prev = nullptr;
  cdlln_t *next = nullptr;

 public:
  cdlln_t() noexcept = default;
  void unlink() noexcept {
    prev->next = next;
    next->prev = prev;
  }
  void push_back(cdlln_t *node) noexcept {
    node->prev = prev;
    node->next = this;
    prev->next = node;
    prev = node;
  }
  void push_front(cdlln_t *node) noexcept {
    node->prev = this;
    node->next = next;
    next->prev = node;
    next = node;
  }
};

// using template to avoid duplicate code for const iterator
// and CRTP to avoid duplicate code for reverse iterator

template <typename Iterator, is_cdlln Tp>
class cdll_itr_base {
  friend Iterator;

 protected:
  Tp node = nullptr;

 public:
  cdll_itr_base() noexcept = default;
  cdll_itr_base(const cdll_itr_base &) noexcept = default;
  cdll_itr_base &operator=(const cdll_itr_base &) noexcept = default;
  cdll_itr_base(cdll_itr_base &&) noexcept = default;
  cdll_itr_base &operator=(cdll_itr_base &&) noexcept = default;
  cdll_itr_base(Tp node) noexcept : node(node) {}

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
  Iterator operator+(int n) noexcept {
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
  std::remove_pointer_t<Tp> &operator*() const noexcept { return *node; }
  Tp operator->() const noexcept { return node; }
};

template <is_cdlln Tp>
class cdll_itr : public cdll_itr_base<cdll_itr<Tp>, Tp> {
 private:
  using base = cdll_itr_base<cdll_itr<Tp>, Tp>;

 public:
  cdll_itr() noexcept = default;
  template <typename... Args>
  cdll_itr(Args &&...args) noexcept : base(std::forward<Args>(args)...) {}
  cdll_itr &operator++() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->next;
    return *this;
  }
  cdll_itr &operator--() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->prev;
    return *this;
  }
};

template <is_cdlln Tp>
class cdll_ritr : public cdll_itr_base<cdll_ritr<Tp>, Tp> {
 private:
  using base = cdll_itr_base<cdll_ritr<Tp>, Tp>;

 public:
  cdll_ritr() noexcept = default;
  template <typename... Args>
  cdll_ritr(Args &&...args) noexcept : base(std::forward<Args>(args)...) {}
  cdll_ritr &operator--() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->next;
    return *this;
  }
  cdll_ritr &operator++() noexcept {
    static_cast<base *>(this)->node = static_cast<base *>(this)->node->prev;
    return *this;
  }
};

// circular doubly linked list
class cdll_t {
 private:
  cdlln_t head;

 public:
  cdll_t() noexcept = default;
  cdll_t(const cdll_t &) = delete;
  cdll_t &operator=(const cdll_t &) = delete;
  void init() noexcept {
    head.next = &head;
    head.prev = &head;
  }
  [[nodiscard]] bool empty() const noexcept { return head.next == &head; }
  void push_back(cdlln_t *node) noexcept { head.push_back(node); }
  void push_back(cdlln_t &node) noexcept { push_back(&node); }
  void push_back(cdll_t &list) noexcept {
    if (list.empty()) {
      return;
    }
    list.head.next->prev = head.prev;
    head.prev->next = list.head.next;
    head.prev = list.head.prev;
    list.head.prev->next = &head;
    list.init();
  }
  cdlln_t *pop_back() noexcept {
    if (empty()) [[unlikely]] {
      return (cdlln_t *)PTR_FAIL;
    }
    cdlln_t *node = head.prev;
    head.prev = node->prev;
    head.prev->next = &head;
    return node;
  }
  void push_front(cdlln_t *node) noexcept { head.push_front(node); }
  void push_front(cdlln_t &node) noexcept { push_front(&node); }
  void push_front(cdll_t &list) noexcept {
    if (list.empty()) {
      return;
    }
    list.head.prev->next = head.next;
    head.next->prev = list.head.prev;
    head.next = list.head.next;
    list.head.next->prev = &head;
    list.init();
  }
  cdlln_t *pop_front() noexcept {
    if (empty()) [[unlikely]] {
      return (cdlln_t *)PTR_FAIL;
    }
    cdlln_t *node = head.next;
    head.next = node->next;
    head.next->prev = &head;
    return node;
  }
  [[nodiscard]] cdlln_t *back() const noexcept { return head.prev; }
  [[nodiscard]] cdlln_t *front() const noexcept { return head.next; }
  [[nodiscard]] auto begin() noexcept { return cdll_itr<cdlln_t *>(head.next); }
  [[nodiscard]] auto begin() const noexcept {
    return cdll_itr<const cdlln_t *>(head.next);
  }
  [[nodiscard]] auto end() noexcept { return cdll_itr<cdlln_t *>(&head); }
  [[nodiscard]] auto end() const noexcept {
    return cdll_itr<const cdlln_t *>(&head);
  }
  [[nodiscard]] auto rbegin() noexcept {
    return cdll_ritr<cdlln_t *>(head.prev);
  }
  [[nodiscard]] auto rbegin() const noexcept {
    return cdll_ritr<const cdlln_t *>(head.prev);
  }
  [[nodiscard]] auto rend() noexcept { return cdll_ritr<cdlln_t *>(&head); }
  [[nodiscard]] auto rend() const noexcept {
    return cdll_ritr<const cdlln_t *>(&head);
  }
  [[nodiscard]] uint64_t size() const noexcept {
    uint64_t size = 0;
    for (auto itr : *this) {
      ++size;
    }
    return size;
  }
};

}  // namespace bsl
