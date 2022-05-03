#pragma once

#include <config.h>

namespace bsl {

FORCE_INLINE constexpr uint16_t bswap(uint16_t x) noexcept {
  return __builtin_bswap16(x);
}

FORCE_INLINE constexpr uint32_t bswap(uint32_t x) noexcept {
  return __builtin_bswap32(x);
}

FORCE_INLINE constexpr uint64_t bswap(uint64_t x) noexcept {
  return __builtin_bswap64(x);
}

FORCE_INLINE constexpr int16_t bswap(int16_t x) noexcept {
  return (int16_t)__builtin_bswap16((uint16_t)x);
}

FORCE_INLINE constexpr int32_t bswap(int32_t x) noexcept {
  return (int32_t)__builtin_bswap32((uint32_t)x);
}

FORCE_INLINE constexpr int64_t bswap(int64_t x) noexcept {
  return (int64_t)__builtin_bswap64((uint64_t)x);
}

}  // namespace bsl