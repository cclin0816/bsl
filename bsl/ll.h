#pragma once

#include <bsl/empty.h>
#include <bsl/in_place.h>
#include <config.h>

#include <atomic>
#include <utility>

// thread safe

namespace bsl {

// pre-declare

template <typename _Tval>
class lln_t;
template <typename _Tval>
class ll_t;
template <typename _Tnode>
class ll_itr;

// linked list node
template <typename Tval = empty_t>
class lln_t {
  template <typename _Tnode>
  friend class ll_itr;
  template <typename _Tval>
  friend class ll_t;

 public:
  using value_type = Tval;
  using type = lln_t<Tval>;

 private:
  std::atomic<type *> next{nullptr};
  [[no_unique_address]] value_type val{};

 public:
  lln_t() noexcept = default;

  template <typename... Args>
  lln_t(in_place_t, Args &&...args) noexcept(
      noexcept(value_type(std::forward<Args>(args)...)))
      : val(std::forward<Args>(args)...) {}

  lln_t(const type &) = delete;
  lln_t(type &&) = delete;
  type &operator=(const type &) = delete;
  type &operator=(type &&) = delete;

  [[nodiscard]] value_type &value() noexcept { return val; }
  [[nodiscard]] const value_type &value() const noexcept { return val; }

  [[nodiscard]] void *base() &noexcept {
    return reinterpret_cast<void *>(this);
  }
};

template <typename Tnode>
class ll_itr {
  using type = ll_itr<Tnode>;
  using node_type = Tnode;
  using value_type = node_type;

 protected:
  node_type *node = nullptr;

 public:
  ll_itr() noexcept = default;
  ll_itr(node_type *node) noexcept : node(node) {}
  type &operator=(node_type *node) noexcept {
    this->node = node;
    return *this;
  }
  type &operator++() noexcept {
    node = node->next.load(std::memory_order_relaxed);
  }
  type operator++(int) &noexcept {
    type tmp(*this);
    ++(*this);
    return tmp;
  }
  type operator+(const int n) noexcept {
    type tmp(*this);
    for (int i = 0; i < n; ++i) {
      ++tmp;
    }
    return tmp;
  }
  type &operator+=(const int n) noexcept {
    for (int i = 0; i < n; ++i) {
      ++(*this);
    }
    return *this;
  }
  friend bool operator==(const type &lhs, const type &rhs) noexcept {
    return lhs.node == rhs.node;
  }
  node_type &operator*() const noexcept { return *node; }
  node_type *operator->() const noexcept { return node; }
  operator node_type *() const noexcept { return node; }
};

template <typename Tval = empty_t>
class ll_t {
  using type = ll_t<Tval>;
  using node_type = lln_t<Tval>;
  using empty_type = lln_t<empty_t>;
  using iterator = ll_itr<node_type>;
  using const_iterator = ll_itr<const node_type>;
  using value_type = Tval;

 private:
  empty_type head;

 public:
  ll_t() noexcept {
    head.next.store((empty_type *)PTR_FAIL, std::memory_order_relaxed);
  }
  ll_t(const type &) = delete;
  ll_t(type &&) = delete;
  type &operator=(const type &) = delete;
  type &operator=(type &&) = delete;

  [[nodiscard]] bool empty() const noexcept {
    return head.next.load(std::memory_order_relaxed) == PTR_FAIL;
  }

  void push(node_type *node) &noexcept {
    auto *node_cast = reinterpret_cast<empty_type *>(node);
    auto *old = head.next.load(std::memory_order_relaxed);
    while (true) {
      node->next.store(old, std::memory_order_relaxed);
      if (head.next.compare_exchange_weak(old, node_cast,
                                          std::memory_order_release,
                                          std::memory_order_relaxed)) {
        break;
      }
    }
  }
  void push(node_type &node) &noexcept { push(&node); }
  void push(void *node) &noexcept { push(reinterpret_cast<node_type *>(node)); }

  node_type *pop() &noexcept {
    auto *old = head.next.load(std::memory_order_relaxed);
    while (true) {
      if (old == (empty_type *)PTR_FAIL) {
        return PTR_FAIL;
      }
      auto *next = old->next.load(std::memory_order_relaxed);
    }
  }
};

}  // namespace bsl