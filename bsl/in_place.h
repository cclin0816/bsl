#pragma once

namespace bsl {

// in_place type for inplace construction
struct in_place_t {
  explicit in_place_t() = default;
};
inline constexpr in_place_t in_place{};

}  // namespace bsl