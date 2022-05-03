#pragma once

#include <config.h>

namespace bsl {

INLINE constexpr bool islower(int ch) noexcept {
  return (ch >= 'a' && ch <= 'z');
}

INLINE constexpr bool isupper(int ch) noexcept {
  return (ch >= 'A' && ch <= 'Z');
}

INLINE constexpr bool isalpha(int ch) noexcept {
  return (islower(ch) || isupper(ch));
}

INLINE constexpr bool isdigit(int ch) noexcept {
  return (ch >= '0' && ch <= '9');
}

INLINE constexpr bool isxdigit(int ch) noexcept {
  return (isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'));
}

INLINE constexpr bool isalnum(int ch) noexcept {
  return (isalpha(ch) || isdigit(ch));
}

INLINE constexpr bool ispunct(int ch) noexcept {
  return (ch >= 0x21 && ch <= 0x2f) || (ch >= 0x3a && ch <= 0x40) ||
         (ch >= 0x5b && ch <= 0x60) || (ch >= 0x7b && ch <= 0x7e);
}

INLINE constexpr bool isspace(int ch) noexcept {
  return (ch == '\t' || (ch >= 0x0a && ch <= 0x0d) || ch == ' ');
}

INLINE constexpr bool isprint(int ch) noexcept {
  return (ch >= 0x20 && ch <= 0x7e);
}

INLINE constexpr int to_digit(int ch) noexcept {
  return (ch >= '0' && ch <= '9')   ? (ch - '0')
         : (ch >= 'a' && ch <= 'f') ? (ch - 'a' + 10)
         : (ch >= 'A' && ch <= 'F') ? (ch - 'A' + 10)
                                    : -1;
}

}  // namespace bsl