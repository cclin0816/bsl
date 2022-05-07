#include <bsl/cdll.h>

#include <array>
#include <cassert>
#include <iostream>
#include <iterator>
#include <type_traits>

template <typename T>
concept cdlln_t_unlink = requires() {
  T().unlink();
};

template <typename T>
concept cdlln_t_push_back = requires() {
  T().push_back(&T());
};

template <typename T>
concept cdlln_t_push_front = requires() {
  T().push_front(&T());
};

template <typename T>
concept cdlln_t_base = requires() {
  T().base();
};

template <typename T>
void cdlln_t_test() {
  static_assert(
      !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T> &&
          !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>,
      "no move or copy");
  static_assert(!cdlln_t_unlink<T> && !cdlln_t_push_back<T> &&
                    !cdlln_t_push_front<T> && !cdlln_t_base<T>,
                "no rvalue on these functions");
}

bool check_integrity(auto const &l, unsigned long size) {
  // forwards check
  auto sz = l.size();
  // backwards check
  auto ritr = l.rbegin();
  for (unsigned long i = 0; i < sz; ++i, ++ritr)
    ;
  assert(ritr == l.rend());
  return sz == size;
}

void check_contents(auto const &l, auto const &a) {
  auto l_itr = l.begin();
  auto a_itr = a.begin();
  while (l_itr != l.end()) {
    assert((l_itr->value()) == *a_itr);
    ++l_itr;
    ++a_itr;
  }
}

int main() {
  static_assert(sizeof(bsl::cdll_t<int>) == 16U);
  static_assert(sizeof(bsl::cdlln_t<int>) == 24U);

  std::array<bsl::cdlln_t<unsigned>, 16> narr;
  bsl::cdll_t<unsigned> l1;
  bsl::cdll_t<unsigned> l2;
  l1.init();
  l2.init();

  // cdlln_t tests
  for (unsigned i = 0; i < 8; ++i) {
    narr[i].value() = i;
    l1.push_back(narr[i]);
    narr[i + 8].value() = i + 8;
    l2.push_front(narr[i + 8]);
  }
  narr[4].unlink();
  const auto &n4 = narr[4];
  [[maybe_unused]] auto n4v = n4.value();
  narr[12].unlink();
  narr[0].push_back(&(narr[4]));
  narr[8].push_front(&(narr[12]));
  assert((void *)narr.data() == narr[0].base());
  assert(check_integrity(l1, 8) && check_integrity(l2, 8));
  std::array<unsigned, 8> l1_chk = {4, 0, 1, 2, 3, 5, 6, 7};
  std::array<unsigned, 8> l2_chk = {15, 14, 13, 11, 10, 9, 8, 12};
  check_contents(l1, l1_chk);
  check_contents(l2, l2_chk);
  cdlln_t_test<bsl::cdlln_t<>>();
  auto l1_itr = l1.begin();
  auto l2_itr = l2.crbegin();
  l2_itr = l1_itr;
}