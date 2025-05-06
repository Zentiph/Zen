/**
 * @file zassert.c
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
void zassert(bool condition, void *val1, void *val2, const char *message, const char *file, int lineno)
{
    if (!condition)
    {
        if (val1 == NULL && val2 == NULL)
        {
            fprintf(
                stderr,
                "Assertion failed: Values are NULL\nFile: %s, Line: %d",
                file,
                lineno);
            exit(1);
        }
        if (val1 == NULL)
        {
            fprintf(
                stderr,
                "Assertion failed: 1st value is NULL\nFile: %s, Line: %d",
                (char *)val2,
                file,
                lineno);
            exit(1);
        }
        if (val2 == NULL)
        {
            fprintf(
                stderr,
                "Assertion failed: 2nd value is NULL\nFile: %s, Line: %d",
                (char *)val1,
                file,
                lineno);
            exit(1);
        }
        fprintf(
            stderr,
            message,
            (char *)val1,
            (char *)val2,
            file,
            lineno);
        exit(1);
    }
    printf("Test at %s:%d OK\n", file, lineno);
}