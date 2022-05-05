#pragma once

#include <bsl/empty.h>
#include <config.h>

#include <atomic>

// thread safe

namespace bsl {

// pre-declare

template <typename _Tv>
class lln_t;
template <typename _Tv>
class ll_t;
template <typename _Tp>
class ll_itr;

// linked list node
template <typename _Tv = empty_t>
class lln_t {
  using type = lln_t<_Tv>;

  template <typename _Tp>
  friend class ll_itr;
  template <typename _Tp>
  friend class ll_t;

 protected:
  std::atomic<type *> next{PTR_FAIL};

 public:
  [[no_unique_address]] _Tv val;
  lln_t() noexcept = default;
  // this might fail to instantiate if copy or move constructor is used, since
  // deleted
  template <typename... Args>
  lln_t(Args &&...args) noexcept(noexcept(_Tv(std::forward<Args>(args)...)))
      : val(std::forward<Args>(args)...) {}
  // no copy and move
  lln_t(const type &) = delete;
  lln_t(type &&) = delete;
  type &operator=(const type &) = delete;
  type &operator=(type &&) = delete;

  void push(type *node) noexcept {
    type *old = next.load(std::memory_order_relaxed);
    while (true) {
      node->next = old;
      if (next.compare_exchange_weak(old, node, std::memory_order_release,
                                     std::memory_order_relaxed)) {
        break;
      }
    }
  }
};

template <typename _Tv>
class ll_itr {
  using type = ll_itr<_Tv>;

 protected:
  _Tv *node = nullptr;

 public:
  ll_itr() noexcept = default;
  ll_itr(const type &) noexcept = default;
  type &operator=(const type &) noexcept = default;
  ll_itr(type &&) noexcept = default; 
  type &operator=(type &&) noexcept = default;
  ll_itr(_Tv *node) noexcept : node(node) {}
  [[nodiscard]] bool operator==(const type &other) const noexcept {
    return node == other.node;
  }
};

template <typename _Tp = empty_t>
class ll_t {
  using type = ll_t<_Tp>;
  using node_type = lln_t<_Tp>;
  using empty_type = lln_t<empty_t>;

 private:
  // empty_t head;
};

}  // namespace bsl