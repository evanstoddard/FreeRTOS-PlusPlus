/*
 * Copyright (C) Ovyl
 */

/**
 * @file frpp_printf.c
 * @author Evan Stoddard
 * @brief
 */

#include "frpp/sys/frpp_printf.h"

#include <errno.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define FRPP_HANDLE_BUF_OVERRUN(dst_, len_, idx_, type_)                       \
  do {                                                                         \
    if (dst_ && ((idx_ + sizeof(type_) > len_)))                               \
      return -ENOSPC;                                                          \
  } while (0);

#define FRPP_WRITE_ARG(dst_, args_, idx_, type_)                               \
  do {                                                                         \
    if (dst_)                                                                  \
      *(type_ *)(((uint8_t *)dst_) + idx_) = va_arg(args_, type_);             \
  } while (0);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int frpp_printf_package(void *dst, size_t len, uint32_t flags,
                        const char *fmt_str, ...) {
  va_list args;

  va_start(args, fmt_str);
  int ret = frpp_vprintf_package(dst, len, flags, fmt_str, args);
  va_end(args);

  return ret;
}

int frpp_vprintf_package(void *dst, size_t len, uint32_t flags,
                         const char *fmt_str, va_list args) {
  if (dst == NULL && len != 0) {
    return -EINVAL;
  }

  if (dst != NULL && len == 0) {
    return -EINVAL;
  }

  if (fmt_str == NULL) {
    return -EINVAL;
  }

  int out_len = 0;

  const char *ptr = fmt_str;
  uint8_t *dst_buf = (uint8_t *)dst;
  // The predicate for the outer while-loop is a little tricky.
  //
  // First and foremost, we want to loop until the NULL terminator of the format
  // string is reached.
  //
  // The next part is a bit more complicated.  This function has two different
  // modes.  The first to determine the amount of space required to package
  // the arguments without actually writing the package to the buffer.
  //
  // The second mode is to actually write the package to an output buffer.
  //
  // The first mode is determined by dst == NULL and len == 0.
  //
  // The second mode is determined by dst != NULL and len != 0.
  //
  // If in first mode, the while loop should not break out early if the
  // output length is greater than len, since nothing is being written.
  //
  // If in second mode, the while loop should break out if outlen would be
  // greater than len.

  while (*ptr && ((dst != NULL && (out_len < len)) || (dst == NULL))) {
    // Ignore anything that is not a specifier
    if (*ptr != '%') {
      ptr++;
      continue;
    }

    // Increment to specifier after %
    ptr++;

    // Length and precision specifiers don't have any impact on package size
    while (*ptr && (*ptr == '-' || *ptr == '+' || *ptr == ' ' || *ptr == '#' ||
                    *ptr == '0')) {
      ptr++;
    }

    while (*ptr && (*ptr >= '0' && *ptr <= '9')) {
      ptr++;
    }

    if (*ptr == '.') {
      ptr++;
      while (*ptr && (*ptr >= '0' && *ptr <= '9')) {
        ptr++;
      }
    }

    switch (*ptr) {

    // Do nothing since double %% is escape
    case '%':
      break;

    case 'h':
      if (*(ptr + 1) == 'h') {
        ptr++;
      }

      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, int);
      FRPP_WRITE_ARG(dst, args, out_len, int);
      out_len += sizeof(int);
      break;

    case 'l':
      if (*(ptr + 1) == 'l') {
        ptr++;

        FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, long long);
        FRPP_WRITE_ARG(dst, args, out_len, long long);
        out_len += sizeof(long long);
      } else {
        FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, long);
        FRPP_WRITE_ARG(dst, args, out_len, long);
        out_len += sizeof(long);
      }
      break;

    case 'z':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, size_t);
      FRPP_WRITE_ARG(dst, args, out_len, size_t);
      out_len += sizeof(size_t);
      break;

    case 't':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, ptrdiff_t);
      FRPP_WRITE_ARG(dst, args, out_len, ptrdiff_t);
      out_len += sizeof(ptrdiff_t);
      break;

    case 'j':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, intmax_t);
      FRPP_WRITE_ARG(dst, args, out_len, intmax_t);
      out_len += sizeof(intmax_t);
      break;

    // Handle pointer and string specifiers
    case 's':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, char *);
      FRPP_WRITE_ARG(dst, args, out_len, char *);
      out_len += sizeof(char *);
      break;

    case 'p':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, void *);
      FRPP_WRITE_ARG(dst, args, out_len, void *);
      out_len += sizeof(void *);
      break;

    case 'n':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, int *);
      FRPP_WRITE_ARG(dst, args, out_len, int *);
      out_len += sizeof(int *);
      break;

    // Handle integer specifiers
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    case 'X':
    case 'c':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, int);
      FRPP_WRITE_ARG(dst, args, out_len, int);
      out_len += sizeof(int);
      break;

    // Handle float/double specifiers
    case 'f':
    case 'F':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, double);
      FRPP_WRITE_ARG(dst, args, out_len, double);
      out_len += sizeof(double);
      break;

    default:
      ptr++;
      continue;
      break;
    }
    ptr++;
  }

  return out_len;
}
