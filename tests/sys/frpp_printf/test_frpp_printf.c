/*
 * Copyright (C) Ovyl
 */

/**
 * @file test_frpp_printf.c
 * @author Evan Stoddard
 * @brief Tests for frpp_printf
 */

#include "unity.h"

#include <errno.h>
#include <string.h>

#include "frpp/sys/frpp_printf.h"
#include "frpp/utils/utils.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Setup/Teardown
 *****************************************************************************/

/**
 * @brief Setup Code called before every test
 */
void setUp(void) {}

/**
 * @brief Tear down code run after each test
 */
void tearDown(void) {}

/*****************************************************************************
 * Setup/Teardown
 *****************************************************************************/

/*****************************************************************************
 * Tests
 *****************************************************************************/

/**
 * @brief Test case where dst is null but length > 0
 */
void test_null_out_buf(void) {
  int ret = frpp_printf_package(NULL, 12, 0, "");
  TEST_ASSERT_EQUAL(ret, -EINVAL);
}

/**
 * @brief Test case where dst != NULL and len == 0
 */
void test_zero_buf_len(void) {
  uint8_t buf[12] = {0};
  int ret = frpp_printf_package(buf, 0, 0, "");
  TEST_ASSERT_EQUAL(ret, -EINVAL);
}

/**
 * @brief Test case where format str == NULL
 */
void test_null_format_str(void) {
  uint8_t buf[12] = {0};
  int ret = frpp_printf_package(buf, sizeof(buf), 0, NULL);
  TEST_ASSERT_EQUAL(ret, -EINVAL);
}

/**
 * @brief Test calculate mode (NULL dst, zero len)
 */
void test_calculate_mode_single_int(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "%d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);
}

/**
 * @brief Test multiple integer format specifiers in calculate mode
 */
void test_calculate_mode_multiple_ints(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "%d %i %u %x %X %o", 1, 2, 3, 4, 5, 6);
  TEST_ASSERT_EQUAL(6 * FRPP_VA_STACK_ALIGN(int), ret);
}

/**
 * @brief Test character format specifier
 */
void test_char_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(int)] = {0};
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%c", 'A');
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);
}

/**
 * @brief Test string format specifier
 */
void test_string_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(char *)] = {0};
  const char *str = "test";
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%s", str);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(char *), ret);
  TEST_ASSERT_EQUAL_PTR(str, *(char **)buf);
}

/**
 * @brief Test pointer format specifier
 */
void test_pointer_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(void *)] = {0};
  void *ptr = (void *)0x12345678;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%p", ptr);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(void *), ret);
  TEST_ASSERT_EQUAL_PTR(ptr, *(void **)buf);
}

/**
 * @brief Test long format specifier
 */
void test_long_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(long)] = {0};
  long val = 123456789L;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%ld", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(long), ret);
  TEST_ASSERT_EQUAL(val, *(long *)buf);
}

/**
 * @brief Test long long format specifier
 */
void test_long_long_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(long long)] = {0};
  long long val = 123456789012345LL;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%lld", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(long long), ret);
  TEST_ASSERT_EQUAL(val, *(long long *)buf);
}

/**
 * @brief Test size_t format specifier
 */
void test_size_t_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(size_t)] = {0};
  size_t val = 42;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%zd", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(size_t), ret);
  TEST_ASSERT_EQUAL(val, *(size_t *)buf);
}

/**
 * @brief Test ptrdiff_t format specifier
 */
void test_ptrdiff_t_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(ptrdiff_t)] = {0};
  ptrdiff_t val = -42;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%td", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(ptrdiff_t), ret);
  TEST_ASSERT_EQUAL(val, *(ptrdiff_t *)buf);
}

/**
 * @brief Test intmax_t format specifier
 */
void test_intmax_t_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(intmax_t)] = {0};
  intmax_t val = 999999999;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%jd", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(intmax_t), ret);
  TEST_ASSERT_EQUAL(val, *(intmax_t *)buf);
}

/**
 * @brief Test double format specifier
 */
void test_double_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(double)] = {0};
  double val = 3.14159;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%f", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(double), ret);
  // Check binary representation instead of using TEST_ASSERT_EQUAL_DOUBLE
  // since Unity may have double precision disabled
  TEST_ASSERT_EQUAL_MEMORY(&val, buf, sizeof(double));
}

/**
 * @brief Test various float format specifiers
 */
void test_float_formats(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(double)] = {0};
  double val = 2.71828;

  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%e", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(double), ret);

  ret = frpp_printf_package(buf, sizeof(buf), 0, "%E", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(double), ret);

  ret = frpp_printf_package(buf, sizeof(buf), 0, "%g", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(double), ret);

  ret = frpp_printf_package(buf, sizeof(buf), 0, "%G", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(double), ret);
}

/**
 * @brief Test %n format specifier
 */
void test_n_format(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(int *)] = {0};
  int written = 0;
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%n", &written);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int *), ret);
  TEST_ASSERT_EQUAL_PTR(&written, *(int **)buf);
}

/**
 * @brief Test escaped %% format
 */
void test_escaped_percent(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "%%");
  TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test mixed format with literals
 */
void test_mixed_format_with_literals(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "Value: %d, String: %s, Hex: %x", 42, "test", 0xFF);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int) + FRPP_VA_STACK_ALIGN(char *) + FRPP_VA_STACK_ALIGN(int), ret);
}

/**
 * @brief Test format with width specifiers (should be ignored)
 */
void test_width_specifiers(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "%10d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_printf_package(NULL, 0, 0, "%-20s", "test");
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(char *), ret);
}

/**
 * @brief Test format with precision specifiers (should be ignored)
 */
void test_precision_specifiers(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "%.2f", 3.14159);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(double), ret);

  ret = frpp_printf_package(NULL, 0, 0, "%.10s", "test");
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(char *), ret);
}

/**
 * @brief Test format with flags (should be ignored)
 */
void test_format_flags(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "%+d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_printf_package(NULL, 0, 0, "%#x", 255);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_printf_package(NULL, 0, 0, "% d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_printf_package(NULL, 0, 0, "%010d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);
}

/**
 * @brief Test buffer overrun detection
 */
void test_buffer_overrun(void) {
  uint8_t buf[sizeof(long long) - 1] = {0};
  // Try to write a long long into buffer that's too small
  // This will trigger FRPP_HANDLE_BUF_OVERRUN when checking if sizeof(long long) fits
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%lld", 123456789012345LL);
  TEST_ASSERT_EQUAL(-ENOSPC, ret);
}

/**
 * @brief Test exact buffer size
 */
void test_exact_buffer_size(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(int) * 3] = {0};
  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%d %d %d", 1, 2, 3);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int) * 3, ret);
}

/**
 * @brief Test empty format string
 */
void test_empty_format_string(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "");
  TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test format string with no specifiers
 */
void test_no_specifiers(void) {
  int ret = frpp_printf_package(NULL, 0, 0, "Hello World");
  TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test short format specifiers (hh and h)
 */
void test_short_format(void) {
  // Note: short types are promoted to int
  int ret = frpp_printf_package(NULL, 0, 0, "%hd", (short)42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_printf_package(NULL, 0, 0, "%hhd", (char)42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);
}

/**
 * @brief Test complex format string
 */
void test_complex_format(void) {
  int ret = frpp_printf_package(NULL, 0, 0,
    "Int: %d, Long: %ld, LongLong: %lld, Size: %zu, Ptr: %p, Str: %s, Float: %f",
    42, 123L, 456LL, (size_t)789, (void *)0x1234, "test", 3.14);

  size_t expected = FRPP_VA_STACK_ALIGN(int) + FRPP_VA_STACK_ALIGN(long) + FRPP_VA_STACK_ALIGN(long long) +
                    FRPP_VA_STACK_ALIGN(size_t) + FRPP_VA_STACK_ALIGN(void *) + FRPP_VA_STACK_ALIGN(char *) + FRPP_VA_STACK_ALIGN(double);
  TEST_ASSERT_EQUAL(expected, ret);
}

/**
 * @brief Test data integrity for multiple values
 */
void test_data_integrity_multiple_values(void) {
  uint8_t buf[FRPP_VA_STACK_ALIGN(int) * 3 + FRPP_VA_STACK_ALIGN(char *)] = {0};
  int val1 = 123;
  int val2 = 456;
  int val3 = 789;
  const char *str = "test";

  int ret = frpp_printf_package(buf, sizeof(buf), 0, "%d %s %d %d", val1, str, val2, val3);

  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int) * 3 + FRPP_VA_STACK_ALIGN(char *), ret);
  TEST_ASSERT_EQUAL(val1, *(int *)(buf));
  TEST_ASSERT_EQUAL_PTR(str, *(char **)(buf + FRPP_VA_STACK_ALIGN(int)));
  TEST_ASSERT_EQUAL(val2, *(int *)(buf + FRPP_VA_STACK_ALIGN(int) + FRPP_VA_STACK_ALIGN(char *)));
  TEST_ASSERT_EQUAL(val3, *(int *)(buf + FRPP_VA_STACK_ALIGN(int) * 2 + FRPP_VA_STACK_ALIGN(char *)));
}

/*****************************************************************************
 * frpp_snprintf Tests
 *****************************************************************************/

/**
 * @brief Test frpp_snprintf with NULL format string
 */
void test_snprintf_null_format_str(void) {
  uint8_t arg_buf[16] = {0};
  char out_buf[64] = {0};
  int ret = frpp_snprintf(NULL, arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test frpp_snprintf with NULL arg buffer
 */
void test_snprintf_null_arg_buf(void) {
  char out_buf[64] = {0};
  int ret = frpp_snprintf("test %d", NULL, out_buf, sizeof(out_buf));
  TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test frpp_snprintf with NULL output buffer
 */
void test_snprintf_null_out_buf(void) {
  uint8_t arg_buf[16] = {0};
  int ret = frpp_snprintf("test %d", arg_buf, NULL, 64);
  TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test simple integer formatting with frpp_snprintf
 */
void test_snprintf_simple_int(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_snprintf("%d", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("42", out_buf);
}

/**
 * @brief Test negative integer with frpp_snprintf
 */
void test_snprintf_negative_int(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%d", -123);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_snprintf("%d", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("-123", out_buf);
}

/**
 * @brief Test hexadecimal formatting with frpp_snprintf
 */
void test_snprintf_hex_format(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "0x%x", 0xDEAD);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_snprintf("0x%x", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("0xdead", out_buf);
}

/**
 * @brief Test string formatting with frpp_snprintf
 */
void test_snprintf_string_format(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};
  const char *test_str = "Hello";

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%s", test_str);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(char *), ret);

  ret = frpp_snprintf("%s", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("Hello", out_buf);
}

/**
 * @brief Test pointer formatting with frpp_snprintf
 */
void test_snprintf_pointer_format(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};
  char expected[64] = {0};
  void *ptr = (void *)0x12345678;

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%p", ptr);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(void *), ret);

  ret = frpp_snprintf("%p", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);

  snprintf(expected, sizeof(expected), "%p", ptr);
  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}

/**
 * @brief Test long long formatting with frpp_snprintf
 */
void test_snprintf_long_long_format(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};
  long long val = 9876543210LL;

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%lld", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(long long), ret);

  ret = frpp_snprintf("%lld", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("9876543210", out_buf);
}

/**
 * @brief Test float formatting with frpp_snprintf
 */
void test_snprintf_float_format(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};
  double val = 3.14159;

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%.2f", val);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(double), ret);

  ret = frpp_snprintf("%.2f", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("3.14", out_buf);
}

/**
 * @brief Test multiple format specifiers with frpp_snprintf
 */
void test_snprintf_multiple_specifiers(void) {
  uint8_t arg_buf[64] = {0};
  char out_buf[128] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0,
    "%d %s %x", 42, "test", 0xFF);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int) + FRPP_VA_STACK_ALIGN(char *) + FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_snprintf("%d %s %x", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("42 test ff", out_buf);
}

/**
 * @brief Test complex format string with frpp_snprintf
 */
void test_snprintf_complex_format(void) {
  uint8_t arg_buf[128] = {0};
  char out_buf[256] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0,
    "Int: %d, String: %s, Hex: 0x%X", 123, "FreeRTOS", 0xABCD);

  ret = frpp_snprintf("Int: %d, String: %s, Hex: 0x%X",
    arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("Int: 123, String: FreeRTOS, Hex: 0xABCD", out_buf);
}

/**
 * @brief Test format with width specifiers with frpp_snprintf
 */
void test_snprintf_width_specifiers(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%5d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_snprintf("%5d", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("   42", out_buf);
}

/**
 * @brief Test format with zero padding with frpp_snprintf
 */
void test_snprintf_zero_padding(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%05d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_snprintf("%05d", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("00042", out_buf);
}

/**
 * @brief Test format with left alignment with frpp_snprintf
 */
void test_snprintf_left_alignment(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "%-5d", 42);
  TEST_ASSERT_EQUAL(FRPP_VA_STACK_ALIGN(int), ret);

  ret = frpp_snprintf("%-5d", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("42   ", out_buf);
}

/**
 * @brief Test empty format string with frpp_snprintf
 */
void test_snprintf_empty_format(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "");
  TEST_ASSERT_EQUAL(0, ret);

  ret = frpp_snprintf("", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_EQUAL(0, ret);
  TEST_ASSERT_EQUAL_STRING("", out_buf);
}

/**
 * @brief Test format with no specifiers with frpp_snprintf
 */
void test_snprintf_no_specifiers(void) {
  uint8_t arg_buf[32] = {0};
  char out_buf[64] = {0};

  int ret = frpp_printf_package(arg_buf, sizeof(arg_buf), 0, "Hello");
  TEST_ASSERT_EQUAL(0, ret);

  ret = frpp_snprintf("Hello", arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, ret);
  TEST_ASSERT_EQUAL_STRING("Hello", out_buf);
}

/**
 * @brief Test end-to-end workflow with frpp_snprintf
 */
void test_snprintf_end_to_end(void) {
  uint8_t arg_buf[128] = {0};
  char out_buf[256] = {0};
  const char *fmt = "Status: %d, Message: %s, Value: 0x%X";

  int package_size = frpp_printf_package(arg_buf, sizeof(arg_buf), 0,
    fmt, 200, "OK", 0xBEEF);
  TEST_ASSERT_GREATER_THAN(0, package_size);

  int output_len = frpp_snprintf(fmt, arg_buf, out_buf, sizeof(out_buf));
  TEST_ASSERT_GREATER_THAN(0, output_len);

  TEST_ASSERT_EQUAL_STRING("Status: 200, Message: OK, Value: 0xBEEF", out_buf);
}

/**
 * @brief Runner
 *
 * @return Return status (non-zero if any test failed)
 */
int main(void) {
  UNITY_BEGIN();

  // Error condition tests
  RUN_TEST(test_null_out_buf);
  RUN_TEST(test_zero_buf_len);
  RUN_TEST(test_null_format_str);

  // Calculate mode tests
  RUN_TEST(test_calculate_mode_single_int);
  RUN_TEST(test_calculate_mode_multiple_ints);

  // Basic format specifier tests
  RUN_TEST(test_char_format);
  RUN_TEST(test_string_format);
  RUN_TEST(test_pointer_format);
  RUN_TEST(test_n_format);

  // Integer format tests
  RUN_TEST(test_long_format);
  RUN_TEST(test_long_long_format);
  RUN_TEST(test_short_format);

  // Size-specific format tests
  RUN_TEST(test_size_t_format);
  RUN_TEST(test_ptrdiff_t_format);
  RUN_TEST(test_intmax_t_format);

  // Floating point tests
  RUN_TEST(test_double_format);
  RUN_TEST(test_float_formats);

  // Format modifier tests
  RUN_TEST(test_escaped_percent);
  RUN_TEST(test_width_specifiers);
  RUN_TEST(test_precision_specifiers);
  RUN_TEST(test_format_flags);

  // Complex format tests
  RUN_TEST(test_mixed_format_with_literals);
  RUN_TEST(test_complex_format);

  // Buffer handling tests
  RUN_TEST(test_buffer_overrun);
  RUN_TEST(test_exact_buffer_size);

  // Edge case tests
  RUN_TEST(test_empty_format_string);
  RUN_TEST(test_no_specifiers);

  // Data integrity tests
  RUN_TEST(test_data_integrity_multiple_values);

  // frpp_snprintf error condition tests
  RUN_TEST(test_snprintf_null_format_str);
  RUN_TEST(test_snprintf_null_arg_buf);
  RUN_TEST(test_snprintf_null_out_buf);

  // frpp_snprintf basic format tests
  RUN_TEST(test_snprintf_simple_int);
  RUN_TEST(test_snprintf_negative_int);
  RUN_TEST(test_snprintf_hex_format);
  RUN_TEST(test_snprintf_string_format);
  RUN_TEST(test_snprintf_pointer_format);
  RUN_TEST(test_snprintf_long_long_format);
  RUN_TEST(test_snprintf_float_format);

  // frpp_snprintf complex format tests
  RUN_TEST(test_snprintf_multiple_specifiers);
  RUN_TEST(test_snprintf_complex_format);

  // frpp_snprintf format modifier tests
  RUN_TEST(test_snprintf_width_specifiers);
  RUN_TEST(test_snprintf_zero_padding);
  RUN_TEST(test_snprintf_left_alignment);

  // frpp_snprintf edge case tests
  RUN_TEST(test_snprintf_empty_format);
  RUN_TEST(test_snprintf_no_specifiers);

  // frpp_snprintf integration test
  RUN_TEST(test_snprintf_end_to_end);

  return UNITY_END();
}
