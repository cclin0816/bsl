#pragma once

#include <config.h>

extern "C" {

[[noreturn]] void _abort() noexcept;

void abort_msg(const char* msg, const char* file, unsigned line,
               const char* func) noexcept;
}

#define __assert_fail(msg, file, line, func) \
  do {                                       \
    abort_msg(msg, file, line, func);        \
    _abort();                                \
  } while (0)

#define assert(expr)                                                 \
  do {                                                               \
    if (!static_cast<bool>(expr)) [[unlikely]]                       \
      __assert_fail(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
  } while (0)

namespace bsl {

#define panic(expr) \
  __assert_fail("painc: " #expr, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define unreachable(expr) \
  __assert_fail("unreachable: " #expr, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define todo(expr) \
  __assert_fail("todo: " #expr, __FILE__, __LINE__, __PRETTY_FUNCTION__)

}  // namespace bsl
