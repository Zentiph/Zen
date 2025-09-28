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
      fprintf(stdout, "Test passed: %s (%s %s %s) at %s:%d\n",   \
              op, #lhs_expr, op, #rhs_expr, __FILE__, __LINE__); \
      fflush(stdout);                                            \
   } while (false)
#else
#define ZA__PASS(op, lhs_expr, rhs_expr) ((void)0)
#endif

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

// for common types
#define ZA__REPORT_NUM(op, lhs_expr, rhs_expr, fmt, lval, rval)     \
   do                                                               \
   {                                                                \
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
      za__report_hdr(op, #lhs_expr, #rhs_expr, __FILE__, __LINE__); \
      fprintf(stderr, "  left : %s\n", (la) ? (la) : "(null)");     \
      fprintf(stderr, "  right: %s\n", (ra) ? (ra) : "(null)");     \
      fflush(stderr);                                               \
      ZA__ABORT();                                                  \
   } while (false)

/// A macro to assert two chars are equal.
#define ZA_ASSERT_EQ_CHAR(lhs, rhs)                              \
   do                                                            \
   {                                                             \
      char _l = (lhs), _r = (rhs);                               \
      if (!(_l == _r))                                           \
         ZA__REPORT_NUM("==", lhs, rhs, "%d", (int)_l, (int)_r); \
      else                                                       \
         ZA__PASS("==", lhs, rhs);                               \
   } while (false)

/// A macro to assert two chars are not equal.
#define ZA_ASSERT_NE_CHAR(lhs, rhs)                              \
   do                                                            \
   {                                                             \
      char _l = (lhs), _r = (rhs);                               \
      if (_l == _r)                                              \
         ZA__REPORT_NUM("!=", lhs, rhs, "%d", (int)_l, (int)_r); \
      else                                                       \
         ZA__PASS("!=", lhs, rhs);                               \
   } while (false)

/// A macro to assert two ints are equal.
#define ZA_ASSERT_EQ_INT(lhs, rhs)                     \
   do                                                  \
   {                                                   \
      int _l = (lhs), _r = (rhs);                      \
      if (!(_l == _r))                                 \
         ZA__REPORT_NUM("==", lhs, rhs, "%d", _l, _r); \
      else                                             \
         ZA__PASS("==", lhs, rhs);                     \
   } while (false)

/// A macro to assert two ints are not equal.
#define ZA_ASSERT_NE_INT(lhs, rhs)                     \
   do                                                  \
   {                                                   \
      int _l = (lhs), _r = (rhs);                      \
      if (_l == _r)                                    \
         ZA__REPORT_NUM("!=", lhs, rhs, "%d", _l, _r); \
      else                                             \
         ZA__PASS("!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two longs are equal.
#define ZA_ASSERT_EQ_LONG(lhs, rhs)                     \
   do                                                   \
   {                                                    \
      long _l = (lhs), _r = (rhs);                      \
      if (!(_l == _r))                                  \
         ZA__REPORT_NUM("==", lhs, rhs, "%ld", _l, _r); \
      else                                              \
         ZA__PASS("==", lhs, rhs);                      \
   } while (false)

/// A macro to assert two longs are not equal.
#define ZA_ASSERT_NE_LONG(lhs, rhs)                     \
   do                                                   \
   {                                                    \
      long _l = (lhs), _r = (rhs);                      \
      if (_l == _r)                                     \
         ZA__REPORT_NUM("!=", lhs, rhs, "%ld", _l, _r); \
      else                                              \
         ZA__PASS("!=", lhs, rhs);                      \
   } while (false)

/// A macro to assert two long longs are equal.
#define ZA_ASSERT_EQ_LLONG(lhs, rhs)                     \
   do                                                    \
   {                                                     \
      long long _l = (lhs), _r = (rhs);                  \
      if (!(_l == _r))                                   \
         ZA__REPORT_NUM("==", lhs, rhs, "%lld", _l, _r); \
      else                                               \
         ZA__PASS("==", lhs, rhs);                       \
   } while (false)

/// A macro to assert two long longs are not equal.
#define ZA_ASSERT_NE_LLONG(lhs, rhs)                     \
   do                                                    \
   {                                                     \
      long long _l = (lhs), _r = (rhs);                  \
      if (_l == _r)                                      \
         ZA__REPORT_NUM("!=", lhs, rhs, "%lld", _l, _r); \
      else                                               \
         ZA__PASS("!=", lhs, rhs);                       \
   } while (false)

/// A macro to assert two unsigned ints are equal.
#define ZA_ASSERT_EQ_UINT(lhs, rhs)                    \
   do                                                  \
   {                                                   \
      unsigned int _l = (lhs), _r = (rhs);             \
      if (!(_l == _r))                                 \
         ZA__REPORT_NUM("==", lhs, rhs, "%u", _l, _r); \
      else                                             \
         ZA__PASS("==", lhs, rhs);                     \
   } while (false)

/// A macro to assert two unsigned ints are not equal.
#define ZA_ASSERT_NE_UINT(lhs, rhs)                    \
   do                                                  \
   {                                                   \
      unsigned int _l = (lhs), _r = (rhs);             \
      if (_l == _r)                                    \
         ZA__REPORT_NUM("!=", lhs, rhs, "%u", _l, _r); \
      else                                             \
         ZA__PASS("!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two unsigned longs are equal.
#define ZA_ASSERT_EQ_ULONG(lhs, rhs)                    \
   do                                                   \
   {                                                    \
      unsigned long _l = (lhs), _r = (rhs);             \
      if (!(_l == _r))                                  \
         ZA__REPORT_NUM("==", lhs, rhs, "%lu", _l, _r); \
      else                                              \
         ZA__PASS("==", lhs, rhs);                      \
   } while (false)

/// A macro to assert two unsigned longs are not equal.
#define ZA_ASSERT_NE_ULONG(lhs, rhs)                    \
   do                                                   \
   {                                                    \
      unsigned long _l = (lhs), _r = (rhs);             \
      if (_l == _r)                                     \
         ZA__REPORT_NUM("!=", lhs, rhs, "%lu", _l, _r); \
      else                                              \
         ZA__PASS("!=", lhs, rhs);                      \
   } while (false)

/// A macro to assert two unsigned long longs are equal.
#define ZA_ASSERT_EQ_ULLONG(lhs, rhs)                    \
   do                                                    \
   {                                                     \
      unsigned long long _l = (lhs), _r = (rhs);         \
      if (!(_l == _r))                                   \
         ZA__REPORT_NUM("==", lhs, rhs, "%llu", _l, _r); \
      else                                               \
         ZA__PASS("==", lhs, rhs);                       \
   } while (false)

/// A macro to assert two unsigned long longs are not equal.
#define ZA_ASSERT_NE_ULLONG(lhs, rhs)                    \
   do                                                    \
   {                                                     \
      unsigned long long _l = (lhs), _r = (rhs);         \
      if (_l == _r)                                      \
         ZA__REPORT_NUM("!=", lhs, rhs, "%llu", _l, _r); \
      else                                               \
         ZA__PASS("!=", lhs, rhs);                       \
   } while (false)

/// A macro to assert two floats are equal.
#define ZA_ASSERT_EQ_FLOAT(lhs, rhs)                                   \
   do                                                                  \
   {                                                                   \
      float _l = (lhs), _r = (rhs);                                    \
      if (!(_l == _r))                                                 \
         ZA__REPORT_NUM("==", lhs, rhs, "%g", (double)_l, (double)_r); \
      else                                                             \
         ZA__PASS("==", lhs, rhs);                                     \
   } while (false)

/// A macro to assert two floats are not equal.
#define ZA_ASSERT_NE_FLOAT(lhs, rhs)                                   \
   do                                                                  \
   {                                                                   \
      float _l = (lhs), _r = (rhs);                                    \
      if (_l == _r)                                                    \
         ZA__REPORT_NUM("!=", lhs, rhs, "%g", (double)_l, (double)_r); \
      else                                                             \
         ZA__PASS("!=", lhs, rhs);                                     \
   } while (false)

/// A macro to assert two doubles are equal.
#define ZA_ASSERT_EQ_DOUBLE(lhs, rhs)                  \
   do                                                  \
   {                                                   \
      double _l = (lhs), _r = (rhs);                   \
      if (!(_l == _r))                                 \
         ZA__REPORT_NUM("==", lhs, rhs, "%g", _l, _r); \
      else                                             \
         ZA__PASS("==", lhs, rhs);                     \
   } while (false)

/// A macro to assert two doubles are not equal.
#define ZA_ASSERT_NE_DOUBLE(lhs, rhs)                  \
   do                                                  \
   {                                                   \
      double _l = (lhs), _r = (rhs);                   \
      if (_l == _r)                                    \
         ZA__REPORT_NUM("!=", lhs, rhs, "%g", _l, _r); \
      else                                             \
         ZA__PASS("!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two long doubles are equal.
#define ZA_ASSERT_EQ_LDOUBLE(lhs, rhs)                  \
   do                                                   \
   {                                                    \
      long double _l = (lhs), _r = (rhs);               \
      if (!(_l == _r))                                  \
         ZA__REPORT_NUM("==", lhs, rhs, "%Lg", _l, _r); \
      else                                              \
         ZA__PASS("==", lhs, rhs);                      \
   } while (false)

/// A macro to assert two long doubles are not equal.
#define ZA_ASSERT_NE_LDOUBLE(lhs, rhs)                  \
   do                                                   \
   {                                                    \
      long double _l = (lhs), _r = (rhs);               \
      if (_l == _r)                                     \
         ZA__REPORT_NUM("!=", lhs, rhs, "%Lg", _l, _r); \
      else                                              \
         ZA__PASS("!=", lhs, rhs);                      \
   } while (false)

/// A macro to assert two pointers are equal.
#define ZA_ASSERT_EQ_PTR(lhs, rhs)                     \
   do                                                  \
   {                                                   \
      const void *_l = (const void *)(lhs);            \
      const void *_r = (const void *)(rhs);            \
      if (!(_l == _r))                                 \
         ZA__REPORT_NUM("==", lhs, rhs, "%p", _l, _r); \
      else                                             \
         ZA__PASS("==", lhs, rhs);                     \
   } while (false)

/// A macro tp assert two pointers are not equal.
#define ZA_ASSERT_NE_PTR(lhs, rhs)                     \
   do                                                  \
   {                                                   \
      const void *_l = (const void *)(lhs);            \
      const void *_r = (const void *)(rhs);            \
      if (_l == _r)                                    \
         ZA__REPORT_NUM("!=", lhs, rhs, "%p", _l, _r); \
      else                                             \
         ZA__PASS("!=", lhs, rhs);                     \
   } while (false)

/// A macro to assert two C-strings are equal (NULL-safe).
#define ZA_ASSERT_EQ_CSTR(lhs, rhs)                                               \
   do                                                                             \
   {                                                                              \
      const char *_l = (lhs);                                                     \
      const char *_r = (rhs);                                                     \
      bool _eq = (_l == NULL && _r == NULL) || (_l && _r && strcmp(_l, _r) == 0); \
      if (!_eq)                                                                   \
         ZA__REPORT_CSTR("==", lhs, rhs, _l, _r);                                 \
      else                                                                        \
         ZA__PASS("==", lhs, rhs);                                                \
   } while (false)

/// A macro to assert two C-strings are not equal (NULL-safe).
#define ZA_ASSERT_NE_CSTR(lhs, rhs)                                               \
   do                                                                             \
   {                                                                              \
      const char *_l = (lhs);                                                     \
      const char *_r = (rhs);                                                     \
      bool _eq = (_l == NULL && _r == NULL) || (_l && _r && strcmp(_l, _r) == 0); \
      if (_eq)                                                                    \
         ZA__REPORT_CSTR("!=", lhs, rhs, _l, _r);                                 \
      else                                                                        \
         ZA__PASS("!=", lhs, rhs);                                                \
   } while (false)

/// A macro to assert a condition is true.
#define ZA_ASSERT_TRUE(cond) ZA_ASSERT_NE_INT((cond), 0)
/// A macro to assert a condition is false.
#define ZA_ASSERT_FALSE(cond) ZA_ASSERT_EQ_INT((cond), 0)

#endif /* ZASSERT_H */