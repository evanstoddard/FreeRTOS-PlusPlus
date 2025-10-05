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
#include <stdio.h>

#include "frpp/utils/utils.h"

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

/**
 * @brief Reconstruct va_list from buffered va_list
 *
 * @param fmt_str Pointer to format string
 * @param arg_buf Buffer of arg list
 * @param out_buf Pointer to output buffer
 * @param out_buf_size_bytes Size of output buffer
 * @return Returns size of bytes written
 */
static size_t prv_vsnprintf_from_arg_buffer(const char *fmt_str,
                                            const void *arg_buf, void *out_buf,
                                            size_t out_buf_size_bytes) {
  if (fmt_str == NULL || arg_buf == NULL || out_buf == NULL) {
    return -EINVAL;
  }

#if defined(__x86_64__)
  /*
   * Reference:
   *
   * System V Application Binary Interface
   * AMD64 Architecture Processor Supplement
   */

  struct __va_list {
    unsigned int gp_offset;
    unsigned int fp_offset;
    void *overflow_arg_area;
    void *reg_save_area;
  };

  union {
    va_list ap;
    struct __va_list __ap;
  } u = {0};

  /* create a valid va_list with our buffer */
  u.__ap.overflow_arg_area = (void *)arg_buf;
  u.__ap.reg_save_area = NULL;
  u.__ap.gp_offset = (6 * 8);
  u.__ap.fp_offset = (6 * 8 + 16 * 16);

#elif defined(__aarch64__)
  // ARM64 - va_list is a complex structure
  struct __va_list {
    void *__stack;
    void *__gr_top;
    void *__vr_top;
    int __gr_offs;
    int __vr_offs;
  };

  union {
    va_list ap;
    struct __va_list __ap;
  } u;

  // Reconstruct va_list pointing to our buffer
  u.__ap.__stack = (void *)buffer;
  u.__ap.__gr_top = NULL;
  u.__ap.__vr_top = NULL;
  u.__ap.__gr_offs = 0;
  u.__ap.__vr_offs = 0;

#else
  // Default: assume va_list is just a pointer (ARM32, many others)
  // This works for most simple architectures
  union {
    va_list ap;
    void *ptr;
  } u;

  u.ptr = (void *)arg_buf;
#endif
  return vsnprintf(out_buf, out_buf_size_bytes, fmt_str, u.ap);
}
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
      out_len += FRPP_VA_STACK_ALIGN(int);
      break;

    case 'l':
      if (*(ptr + 1) == 'l') {
        ptr++;

        FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, long long);
        FRPP_WRITE_ARG(dst, args, out_len, long long);
        out_len += FRPP_VA_STACK_ALIGN(long long);
      } else {
        FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, long);
        FRPP_WRITE_ARG(dst, args, out_len, long);
        out_len += FRPP_VA_STACK_ALIGN(long);
      }
      break;

    case 'z':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, size_t);
      FRPP_WRITE_ARG(dst, args, out_len, size_t);
      out_len += FRPP_VA_STACK_ALIGN(size_t);
      break;

    case 't':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, ptrdiff_t);
      FRPP_WRITE_ARG(dst, args, out_len, ptrdiff_t);
      out_len += FRPP_VA_STACK_ALIGN(ptrdiff_t);
      break;

    case 'j':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, intmax_t);
      FRPP_WRITE_ARG(dst, args, out_len, intmax_t);
      out_len += FRPP_VA_STACK_ALIGN(intmax_t);
      break;

    // Handle pointer and string specifiers
    case 's':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, char *);
      FRPP_WRITE_ARG(dst, args, out_len, char *);
      out_len += FRPP_VA_STACK_ALIGN(char *);
      break;

    case 'p':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, void *);
      FRPP_WRITE_ARG(dst, args, out_len, void *);
      out_len += FRPP_VA_STACK_ALIGN(void *);
      break;

    case 'n':
      FRPP_HANDLE_BUF_OVERRUN(dst, len, out_len, int *);
      FRPP_WRITE_ARG(dst, args, out_len, int *);
      out_len += FRPP_VA_STACK_ALIGN(int *);
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
      out_len += FRPP_VA_STACK_ALIGN(int);
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
      out_len += FRPP_VA_STACK_ALIGN(double);
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

int frpp_snprintf(const char *fmt_str, const void *arg_buf, void *out_buf,
                  size_t out_buf_size_bytes) {
  return prv_vsnprintf_from_arg_buffer(fmt_str, arg_buf, out_buf,
                                       out_buf_size_bytes);
}
