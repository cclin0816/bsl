// #include <bsl/ll.h>
#include <bsl/cdll.h>

#include <array>
#include <iostream>
#include <iterator>
 

int main() {
  std::array<bsl::cdlln_t<int>, 16> narr;
  bsl::cdll_t<int> l1;
  bsl::cdll_t<int> l2;

  l1.init();
  l2.init();
  int i = 0;
  for (auto &n : narr) {
    n.value() = i++;
    l1.push_back(n);
  }

}