/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file frpp_printf.h
 * @author Evan Stoddard
 * @brief Module to allow deferrment of format string processing.  Currently,
 * does not replace libc implementation, but rather "augments" it by allowing
 * the actual processing to be deferred.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#ifndef frpp_printf_h
#define frpp_printf_h

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Similar to regular printf commands, but instead of processing an
 * output string, packages up the format string and arguments based on the
 * format string for deferred processing.
 *
 * @param dst Destination buffer to write package to.  If NULL and len == 0,
 * will return required buffer space for package.
 * @param len Length of destination buffer.  If dst NULL, then len MUST be 0
 * @param flags Currently unused and ignored
 * @param fmt_str Format string.  Must be in RO memory
 * @retval Non-negative Length of package in bytes (will not exceed length if
 * dst != NULL && len != 0)
 * @retval -EINVAL Invalid input arguments
 * @retval -ENOSPC If dst != NULL and package exceeds len
 */
int frpp_printf_package(void *dst, size_t len, uint32_t flags,
                        const char *fmt_str, ...);

/**
 * @brief Same as frpp_printf_package, but takes arguments uses elements in
 * argument list specific by va_list args
 *
 * @param dst Destination buffer to write package to.  If NULL and len == 0,
 * will return required buffer space for package.
 * @param len Length of destination buffer.  If dst NULL, then len MUST be 0
 * @param flags Currently unused and ignored
 * @param fmt_str Format string.  Must be in RO memory
 * @param args va_list instance
 * @retval Non-negative Length of package in bytes (will not exceed length if
 * dst != NULL && len != 0)
 * @retval -EINVAL Invalid input arguments
 * @retval -ENOSPC If dst != NULL and package exceeds len
 */
int frpp_vprintf_package(void *dst, size_t len, uint32_t flags,
                         const char *fmt_str, va_list args);

#ifdef __cplusplus
}
#endif
#endif /* frpp_printf_h */
