/**
 * @file zassert.h
 * @author Gavin Borne
 * @brief Better assert function for testing
 * @copyright Copyright (C) 2025  Gavin Borne
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ZASSERT_H
#define ZASSERT_H

#include <stdbool.h>

const char *ASSERT_EQ_MSG = "Assertion failed: '%s' != '%s'\nFailure at: %s:%d";
const char *ASSERT_NEQ_MSG = "Assertion failed: '%s' == '%s'\nFailure at: %s:%d";

/**
 * @brief A modified assert function that prints a better output
 *        including the actual values of the variables compared.
 *
 * @param condition Condition to assert
 * @param val1      Value of the first variable
 * @param val2      Value of the second variable
 * @param message   Message to print if the assertion fails
 * @param file      File the of the assertion
 * @param lineno    Line number of the assertion
 */
void zassert(bool condition, const void *val1, const void *val2, const char *message, const char *file, int lineno);

#endif // ZASSERT_H