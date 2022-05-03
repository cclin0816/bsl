#pragma once

#include <cstddef>
#include <cstdint>

constexpr auto CACHELINE_SZ = 64U;
using uint128_t = __uint128_t;

#define ALIGN(x) __attribute__((aligned(x)))
#define CL_ALIGN ALIGN(CACHELINE_SZ)
#define INLINE inline
#define FORCE_INLINE inline __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#define ASSUME(x) __builtin_assume(x)
#define PACKED __attribute__((packed))
#define PURE __attribute__((pure))
#define FLATTEN __attribute__((flatten))
#define COLD __attribute__((cold))
#define HOT __attribute__((hot))
#define UNUSED __attribute__((unused))
#define MALLOC __attribute__((malloc))

#define PTR_FAIL (void *)0xFFFFFFFFFFFFFFFF