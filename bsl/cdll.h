#pragma once

#include <config.h>

namespace bsl {

struct cdlln_t {
  cdlln_t *prev = nullptr;
  cdlln_t *next = nullptr;
  cdlln_t &remove() noexcept {
    prev->next = next;
    next->prev = prev;
    return *this;
  }
};

struct cdll_itr {
  cdlln_t *node = nullptr;
  cdll_itr(cdlln_t *node) noexcept : node(node) {}
  cdll_itr(cdll_itr const &other) noexcept : node(other.node) {}
  cdll_itr(cdll_itr &&other) noexcept : node(other.node) {}
  cdll_itr &operator=(cdll_itr const &other) noexcept {
    node = other.node;
    return *this;
  }
  cdll_itr &operator=(cdll_itr &&other) noexcept {
    node = other.node;
    return *this;
  }
  cdll_itr &operator++() noexcept {
    node = node->next;
    return *this;
  }
  cdll_itr &operator--() noexcept {
    node = node->prev;
    return *this;
  }
  cdll_itr operator++(int) noexcept {
    cdll_itr tmp = *this;
    ++*this;
    return tmp;
  }
  cdll_itr operator--(int) noexcept {
    cdll_itr tmp = *this;
    --*this;
    return tmp;
  }
  cdll_itr &operator+(int n) noexcept {
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
  bool operator==(const cdll_itr &rhs) const noexcept {
    return node == rhs.node;
  }
  cdlln_t &operator*() const noexcept { return *node; }
  cdlln_t *operator->() const noexcept { return node; }
};

struct cdll_t {
 private:
  cdlln_t head;

 public:
  cdll_t() = default;
  void init() noexcept {
    head.prev = &head;
    head.next = &head;
  }
  [[nodiscard]] bool empty() const noexcept { return head.next == &head; }
  void push_back(cdlln_t *node) noexcept {
    node->prev = head.prev;
    node->next = &head;
    head.prev->next = node;
    head.prev = node;
  }
  void push_back(cdlln_t &node) noexcept { push_front(&node); }
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
  void push_front(cdlln_t *node) noexcept {
    node->prev = &head;
    node->next = head.next;
    head.next->prev = node;
    head.next = node;
  }
  void push_front(cdlln_t &node) noexcept { push_back(&node); }
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
  cdlln_t *pop_back() noexcept {
    if (empty()) [[unlikely]] {
      return (cdlln_t *)PTR_FAIL;
    }
    cdlln_t *node = head.prev;
    head.prev = node->prev;
    head.prev->next = &head;
    return node;
  }
  [[nodiscard]] uint64_t size() const noexcept {
    uint64_t size = 0;
    for (const cdlln_t *node = head.next; node != &head; node = node->next) {
      size++;
    }
    return size;
  }
  [[nodiscard]] cdlln_t *back() const noexcept { return head.prev; }
  [[nodiscard]] cdlln_t *front() const noexcept { return head.next; }
  [[nodiscard]] cdll_itr begin() noexcept { return cdll_itr(head.next); }
  [[nodiscard]] cdll_itr end() noexcept { return cdll_itr(&head); }
  [[nodiscard]] cdll_itr rbegin() noexcept { return cdll_itr(head.prev); }
  [[nodiscard]] cdll_itr rend() noexcept { return cdll_itr(&head); }
};

}  // namespace bsl