#pragma once

#include <config.h>

#include <concepts>

namespace bsl {

FORCE_INLINE constexpr bool collide(uint64_t addr1, uint64_t size1, uint64_t addr2,
                              uint64_t size2) noexcept {
  return !(addr1 + size1 <= addr2 || addr2 + size2 <= addr1);
}

template <std::unsigned_integral T>
FORCE_INLINE constexpr bool has_attr(T attr, T mask, T check) noexcept {
  return (attr & mask) == check;
}

template <std::unsigned_integral T>
FORCE_INLINE constexpr bool has_attr(T attr, T check) noexcept {
  return has_attr(attr, check, check);
}

}  // namespace bsl