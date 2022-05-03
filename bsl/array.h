#pragma once

#include <config.h>

#include <array>

namespace bsl {

template <typename T, uint64_t N>
using array_t = std::array<T, N>;

}  // namespace bsl