/*
 Formatting library for C++ - time formatting

 Copyright (c) 2012 - 2016, Victor Zverovich
 All rights reserved.

 For the license information refer to format.h.
 */

#ifndef FMT_TIME_H_
#define FMT_TIME_H_

#include "fmt/format.h"
#include <ctime>

namespace fmt {

void format_value(buffer &buf, const std::tm &tm, context &ctx) {
  const char *&s = ctx.ptr();
  if (*s == ':')
    ++s;
  const char *end = s;
  while (*end && *end != '}')
    ++end;
  if (*end != '}')
    FMT_THROW(format_error("missing '}' in format string"));
  internal::MemoryBuffer<char, internal::INLINE_BUFFER_SIZE> format;
  format.append(s, end + 1);
  format[format.size() - 1] = '\0';
  std::size_t start = buf.size();
  for (;;) {
    std::size_t size = buf.capacity() - start;
    std::size_t count = std::strftime(&buf[start], size, &format[0], &tm);
    if (count != 0) {
      buf.resize(start + count);
      break;
    }
    if (size >= format.size() * 256) {
      // If the buffer is 256 times larger than the format string, assume
      // that `strftime` gives an empty result. There doesn't seem to be a
      // better way to distinguish the two cases:
      // https://github.com/fmtlib/fmt/issues/367
      break;
    }
    const std::size_t MIN_GROWTH = 10;
    buf.reserve(buf.capacity() + (size > MIN_GROWTH ? size : MIN_GROWTH));
  }
  s = end;
}
}

#endif  // FMT_TIME_H_
