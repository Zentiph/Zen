///
/// @file zassert.h
/// @author Gavin Borne
/// @brief Better assert function header.
/// @copyright Copyright (C) 2025  Gavin Borne
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.
///

#ifndef ZASSERT_H
#define ZASSERT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ZA_ABORT_ON_FAIL
#define ZA__ABORT() abort()
#else
#define ZA__ABORT() ((void)0)
#endif

#if defined(ZA_ASSERT_VERBOSE) || defined(ZA_VERBOSE)
#define ZA__PASS(op, lhs_expr, rhs_expr)                         \
   do                                                            \
   {                                                             \
      za_tests_run++;                                            \
      fprintf(stdout, "Test passed: %s (%s %s %s) at %s:%d\n",   \
              op, #lhs_expr, op, #rhs_expr, __FILE__, __LINE__); \
      fflush(stdout);                                            \
   } while (false)
#else
#define ZA__PASS(op, lhs_expr, rhs_expr) ((void)0)
#endif

unsigned long za_tests_run;
unsigned long za_tests_failed;

// core reporters
static inline void za__report_hdr(const char *op,
                                  const char *lhs_expr,
                                  const char *rhs_expr,
                                  const char *file,
                                  int line)
{
   fprintf(stderr, "Assertion failed: %s (%s %s %s)\n",
           op, lhs_expr, op, rhs_expr);
   fprintf(stderr, "  at %s:%d\n", file, line);
}

static inline void za_print_summary(FILE *out)
{
   fprintf(out, "\n[zassert] ran %lu, failed %lu\n", za_tests_run, za_tests_failed);
   fflush(out);
}

static inline int za_exit_code(void)
{
   return (za_tests_failed != 0) ? 1 : 0;
}

// for common types
#define ZA__REPORT_NUM(op, lhs_expr, rhs_expr, fmt, lval, rval)     \
   do                                                               \
   {                                                                \
      za_tests_failed++;                                            \
      za__report_hdr(op, #lhs_expr, #rhs_expr, __FILE__, __LINE__); \
      fprintf(stderr, "  left : " fmt "\n", (lval));                \
      fprintf(stderr, "  right: " fmt "\n", (rval));                \
      fflush(stderr);                                               \
      ZA__ABORT();                                                  \
   } while (false)

// for C-strings
#define ZA__REPORT_CSTR(op, lhs_expr, rhs_expr, la, ra)             \
   do                                                               \
   {                                                                \
      za_tests_failed++;                                            \
      za__report_hdr(op, #lhs_expr, #rhs_expr, __FILE__, __LINE__); \
      fprintf(stderr, "  left : %s\n", (la) ? (la) : "(null)");     \
      fprintf(stderr, "  right: %s\n", (ra) ? (ra) : "(null)");     \
      fflush(stderr);                                               \
      ZA__ABORT();                                                  \
   } while (false)

// location-aware reporters
#define ZA__REPORT_NUM_AT(file, line, op, lhs_expr, rhs_expr, fmt, lval, rval) \
   do                                                                          \
   {                                                                           \
      za_tests_failed++;                                                       \
      za__report_hdr(op, #lhs_expr, #rhs_expr, (file), (line));                \
      fprintf(stderr, "  left : " fmt "\n", (lval));                           \
      fprintf(stderr, "  right: " fmt "\n", (rval));                           \
      fflush(stderr);                                                          \
      ZA__ABORT();                                                             \
   } while (0)

#define ZA__REPORT_CSTR_AT(file, line, op, lhs_expr, rhs_expr, la, ra) \
   do                                                                  \
   {                                                                   \
      za_tests_failed++;                                               \
      za__report_hdr(op, #lhs_expr, #rhs_expr, (file), (line));        \
      fprintf(stderr, "  left : '%s'\n", (la) ? (la) : "(null)");      \
      fprintf(stderr, "  right: '%s'\n", (ra) ? (ra) : "(null)");      \
      fflush(stderr);                                                  \
      ZA__ABORT();                                                     \
   } while (0)

#if defined(ZA_ASSERT_VERBOSE) || defined(ZA_VERBOSE)
#define ZA__PASS_AT(file, line, op, lhs_expr, rhs_expr)        \
   do                                                          \
   {                                                           \
      fprintf(stdout, "Test passed: %s (%s %s %s) at %s:%d\n", \
              op, #lhs_expr, op, #rhs_expr, (file), (line));   \
      fflush(stdout);                                          \
   } while (0)
#else
#define ZA__PASS_AT(file, line, op, lhs_expr, rhs_expr) ((void)0)
#endif

/// A macro to assert two chars are equal.
#define ZA_ASSERT_EQ_CHAR_AT(file, line, lhs, rhs)                              \
   do                                                                           \
   {                                                                            \
      za_tests_run++;                                                           \
      char _l = (lhs), _r = (rhs);                                              \
      if (!(_l == _r))                                                          \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%d", (int)_l, (int)_r); \
      else                                                                      \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                               \
   } while (false)

/// A macro to assert two chars are not equal.
#define ZA_ASSERT_NE_CHAR_AT(file, line, lhs, rhs)                              \
   do                                                                           \
   {                                                                            \
      za_tests_run++;                                                           \
      char _l = (lhs), _r = (rhs);                                              \
      if (_l == _r)                                                             \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%d", (int)_l, (int)_r); \
      else                                                                      \
         ZA__PASS_AT("!=", lhs, rhs);                                           \
   } while (false)

/// A macro to assert two ints are equal.
#define ZA_ASSERT_EQ_INT_AT(file, line, lhs, rhs)                     \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      int _l = (lhs), _r = (rhs);                                     \
      if (!(_l == _r))                                                \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%d", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                     \
   } while (false)

/// A macro to assert two ints are not equal.
#define ZA_ASSERT_NE_INT_AT(file, line, lhs, rhs)                     \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      int _l = (lhs), _r = (rhs);                                     \
      if (_l == _r)                                                   \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%d", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two longs are equal.
#define ZA_ASSERT_EQ_LONG_AT(file, line, lhs, rhs)                     \
   do                                                                  \
   {                                                                   \
      za_tests_run++;                                                  \
      long _l = (lhs), _r = (rhs);                                     \
      if (!(_l == _r))                                                 \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%ld", _l, _r); \
      else                                                             \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                      \
   } while (false)

/// A macro to assert two longs are not equal.
#define ZA_ASSERT_NE_LONG_AT(file, line, lhs, rhs)                     \
   do                                                                  \
   {                                                                   \
      za_tests_run++;                                                  \
      long _l = (lhs), _r = (rhs);                                     \
      if (_l == _r)                                                    \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%ld", _l, _r); \
      else                                                             \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                      \
   } while (false)

/// A macro to assert two long longs are equal.
#define ZA_ASSERT_EQ_LLONG_AT(file, line, lhs, rhs)                     \
   do                                                                   \
   {                                                                    \
      za_tests_run++;                                                   \
      long long _l = (lhs), _r = (rhs);                                 \
      if (!(_l == _r))                                                  \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%lld", _l, _r); \
      else                                                              \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                       \
   } while (false)

/// A macro to assert two long longs are not equal.
#define ZA_ASSERT_NE_LLONG_AT(file, line, lhs, rhs)                     \
   do                                                                   \
   {                                                                    \
      za_tests_run++;                                                   \
      long long _l = (lhs), _r = (rhs);                                 \
      if (_l == _r)                                                     \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%lld", _l, _r); \
      else                                                              \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                       \
   } while (false)

/// A macro to assert two unsigned ints are equal.
#define ZA_ASSERT_EQ_UINT_AT(file, line, lhs, rhs)                    \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      unsigned int _l = (lhs), _r = (rhs);                            \
      if (!(_l == _r))                                                \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%u", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                     \
   } while (false)

/// A macro to assert two unsigned ints are not equal.
#define ZA_ASSERT_NE_UINT_AT(file, line, lhs, rhs)                    \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      unsigned int _l = (lhs), _r = (rhs);                            \
      if (_l == _r)                                                   \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%u", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two unsigned longs are equal.
#define ZA_ASSERT_EQ_ULONG_AT(file, line, lhs, rhs)                    \
   do                                                                  \
   {                                                                   \
      za_tests_run++;                                                  \
      unsigned long _l = (lhs), _r = (rhs);                            \
      if (!(_l == _r))                                                 \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%lu", _l, _r); \
      else                                                             \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                      \
   } while (false)

/// A macro to assert two unsigned longs are not equal.
#define ZA_ASSERT_NE_ULONG_AT(file, line, lhs, rhs)                    \
   do                                                                  \
   {                                                                   \
      za_tests_run++;                                                  \
      unsigned long _l = (lhs), _r = (rhs);                            \
      if (_l == _r)                                                    \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%lu", _l, _r); \
      else                                                             \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                      \
   } while (false)

/// A macro to assert two unsigned long longs are equal.
#define ZA_ASSERT_EQ_ULLONG_AT(file, line, lhs, rhs)                    \
   do                                                                   \
   {                                                                    \
      za_tests_run++;                                                   \
      unsigned long long _l = (lhs), _r = (rhs);                        \
      if (!(_l == _r))                                                  \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%llu", _l, _r); \
      else                                                              \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                       \
   } while (false)

/// A macro to assert two unsigned long longs are not equal.
#define ZA_ASSERT_NE_ULLONG_AT(file, line, lhs, rhs)                    \
   do                                                                   \
   {                                                                    \
      za_tests_run++;                                                   \
      unsigned long long _l = (lhs), _r = (rhs);                        \
      if (_l == _r)                                                     \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%llu", _l, _r); \
      else                                                              \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                       \
   } while (false)

/// A macro to assert two floats are equal.
#define ZA_ASSERT_EQ_FLOAT_AT(file, line, lhs, rhs)                                   \
   do                                                                                 \
   {                                                                                  \
      za_tests_run++;                                                                 \
      float _l = (lhs), _r = (rhs);                                                   \
      if (!(_l == _r))                                                                \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%g", (double)_l, (double)_r); \
      else                                                                            \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                                     \
   } while (false)

/// A macro to assert two floats are not equal.
#define ZA_ASSERT_NE_FLOAT_AT(file, line, lhs, rhs)                                   \
   do                                                                                 \
   {                                                                                  \
      za_tests_run++;                                                                 \
      float _l = (lhs), _r = (rhs);                                                   \
      if (_l == _r)                                                                   \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%g", (double)_l, (double)_r); \
      else                                                                            \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                                     \
   } while (false)

/// A macro to assert two doubles are equal.
#define ZA_ASSERT_EQ_DOUBLE_AT(file, line, lhs, rhs)                  \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      double _l = (lhs), _r = (rhs);                                  \
      if (!(_l == _r))                                                \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%g", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                     \
   } while (false)

/// A macro to assert two doubles are not equal.
#define ZA_ASSERT_NE_DOUBLE_AT(file, line, lhs, rhs)                  \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      double _l = (lhs), _r = (rhs);                                  \
      if (_l == _r)                                                   \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%g", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two long doubles are equal.
#define ZA_ASSERT_EQ_LDOUBLE_AT(file, line, lhs, rhs)                  \
   do                                                                  \
   {                                                                   \
      za_tests_run++;                                                  \
      long double _l = (lhs), _r = (rhs);                              \
      if (!(_l == _r))                                                 \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%Lg", _l, _r); \
      else                                                             \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                      \
   } while (false)

/// A macro to assert two long doubles are not equal.
#define ZA_ASSERT_NE_LDOUBLE_AT(file, line, lhs, rhs)                  \
   do                                                                  \
   {                                                                   \
      za_tests_run++;                                                  \
      long double _l = (lhs), _r = (rhs);                              \
      if (_l == _r)                                                    \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%Lg", _l, _r); \
      else                                                             \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                      \
   } while (false)

/// A macro to assert two pointers are equal.
#define ZA_ASSERT_EQ_PTR_AT(file, line, lhs, rhs)                     \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      const void *_l = (const void *)(lhs);                           \
      const void *_r = (const void *)(rhs);                           \
      if (!(_l == _r))                                                \
         ZA__REPORT_NUM_AT(file, line, "==", lhs, rhs, "%p", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                     \
   } while (false)

/// A macro tp assert two pointers are not equal.
#define ZA_ASSERT_NE_PTR_AT(file, line, lhs, rhs)                     \
   do                                                                 \
   {                                                                  \
      za_tests_run++;                                                 \
      const void *_l = (const void *)(lhs);                           \
      const void *_r = (const void *)(rhs);                           \
      if (_l == _r)                                                   \
         ZA__REPORT_NUM_AT(file, line, "!=", lhs, rhs, "%p", _l, _r); \
      else                                                            \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two C-strings are equal (NULL-safe).
#define ZA_ASSERT_EQ_CSTR_AT(file, line, lhs, rhs)                                \
   do                                                                             \
   {                                                                              \
      za_tests_run++;                                                             \
      const char *_l = (lhs);                                                     \
      const char *_r = (rhs);                                                     \
      bool _eq = (_l == NULL && _r == NULL) || (_l && _r && strcmp(_l, _r) == 0); \
      if (!_eq)                                                                   \
         ZA__REPORT_CSTR_AT(file, line, "==", lhs, rhs, _l, _r);                  \
      else                                                                        \
         ZA__PASS_AT(file, line, "==", lhs, rhs);                                 \
   } while (false)

/// A macro to assert two C-strings are not equal (NULL-safe).
#define ZA_ASSERT_NE_CSTR_AT(file, line, lhs, rhs)                                \
   do                                                                             \
   {                                                                              \
      za_tests_run++;                                                             \
      const char *_l = (lhs);                                                     \
      const char *_r = (rhs);                                                     \
      bool _eq = (_l == NULL && _r == NULL) || (_l && _r && strcmp(_l, _r) == 0); \
      if (_eq)                                                                    \
         ZA__REPORT_CSTR_AT(file, line, "!=", lhs, rhs, _l, _r);                  \
      else                                                                        \
         ZA__PASS_AT(file, line, "!=", lhs, rhs);                                 \
   } while (false)

/// A macro to assert a condition is true.
#define ZA_ASSERT_TRUE_AT(file, line, cond) ZA_ASSERT_EQ_INT_AT(file, line, (cond), true)
/// A macro to assert a condition is false.
#define ZA_ASSERT_FALSE_AT(file, line, cond) ZA_ASSERT_EQ_INT_AT(file, line, (cond), false)

/// A macro to assert two chars are equal.
#define ZA_ASSERT_EQ_CHAR(lhs, rhs) ZA_ASSERT_EQ_CHAR_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two chars are not equal.
#define ZA_ASSERT_NE_CHAR(lhs, rhs) ZA_ASSERT_NE_CHAR_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two ints are equal.
#define ZA_ASSERT_EQ_INT(lhs, rhs) ZA_ASSERT_EQ_INT_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two ints are not equal.
#define ZA_ASSERT_NE_INT(lhs, rhs) ZA_ASSERT_NE_INT_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two longs are equal.
#define ZA_ASSERT_EQ_LONG(lhs, rhs) ZA_ASSERT_EQ_LONG_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two longs are not equal.
#define ZA_ASSERT_NE_LONG(lhs, rhs) ZA_ASSERT_NE_LONG_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two long longs are equal.
#define ZA_ASSERT_EQ_LLONG(lhs, rhs) ZA_ASSERT_EQ_LLONG_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two long longs are not equal.
#define ZA_ASSERT_NE_LLONG(lhs, rhs) ZA_ASSERT_NE_LLONG_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two unsigned ints are equal.
#define ZA_ASSERT_EQ_UINT(lhs, rhs) ZA_ASSERT_EQ_UINT_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two unsigned ints are not equal.
#define ZA_ASSERT_NE_UINT(lhs, rhs) ZA_ASSERT_NE_UINT_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two unsigned longs are equal.
#define ZA_ASSERT_EQ_ULONG(lhs, rhs) ZA_ASSERT_EQ_ULONG_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two unsigned longs are not equal.
#define ZA_ASSERT_NE_ULONG(lhs, rhs) ZA_ASSERT_NE_ULLONG_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two unsigned long longs are equal.
#define ZA_ASSERT_EQ_ULLONG(lhs, rhs) ZA_ASSERT_EQ_ULLONG_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two unsigned long longs are not equal.
#define ZA_ASSERT_NE_ULLONG(lhs, rhs) ZA_ASSERT_NE_ULLONG_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two floats are equal.
#define ZA_ASSERT_EQ_FLOAT(lhs, rhs) ZA_ASSERT_EQ_FLOAT_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two floats are not equal.
#define ZA_ASSERT_NE_FLOAT(lhs, rhs) ZA_ASSERT_NE_FLOAT_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two doubles are equal.
#define ZA_ASSERT_EQ_DOUBLE(lhs, rhs) ZA_ASSERT_EQ_DOUBLE_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two doubles are not equal.
#define ZA_ASSERT_NE_DOUBLE(lhs, rhs) ZA_ASSERT_NE_DOUBLE_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two long doubles are equal.
#define ZA_ASSERT_EQ_LDOUBLE(lhs, rhs) ZA_ASSERT_EQ_LDOUBLE_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two long doubles are not equal.
#define ZA_ASSERT_NE_LDOUBLE(lhs, rhs) ZA_ASSERT_NE_LDOUBLE_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two pointers are equal.
#define ZA_ASSERT_EQ_PTR(lhs, rhs) ZA_ASSERT_EQ_PTR_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro tp assert two pointers are not equal.
#define ZA_ASSERT_NE_PTR(lhs, rhs) ZA_ASSERT_NE_PTR_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert two C-strings are equal (NULL-safe).
#define ZA_ASSERT_EQ_CSTR(lhs, rhs) ZA_ASSERT_EQ_CSTR_AT(__FILE__, __LINE__, (lhs), (rhs))
/// A macro to assert two C-strings are not equal (NULL-safe).
#define ZA_ASSERT_NE_CSTR(lhs, rhs) ZA_ASSERT_NE_CSTR_AT(__FILE__, __LINE__, (lhs), (rhs))

/// A macro to assert a condition is true.
#define ZA_ASSERT_TRUE(cond) ZA_ASSERT_TRUE_AT(__FILE__, __LINE__, (cond))
/// A macro to assert a condition is false.
#define ZA_ASSERT_FALSE(cond) ZA_ASSERT_FALSE_AT(__FILE__, __LINE__, (cond))

#endif // ZASSERT_H