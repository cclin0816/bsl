#include <bsl/cdll.h>
#include <iostream>

// struct tt {
//   bsl::cdlln_t node;
//   int a;
// };


int main() {
  bsl::cdll_t l1;
  l1.init();
  // bsl::cdll_t l2;
  // tt t[10];

  // for (int i = 0; i < 5; ++i) {
  //   t[i].a = i;
  //   l1.push_back(t[i].node);
  // }

  // for (int i = 5; i < 10; ++i) {
  //   t[i].a = i;
  //   l2.push_back(t[i].node);
  // }

  for (auto &it : l1) {
  }

}