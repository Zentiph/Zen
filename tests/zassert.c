/**
 * @file zassert.c
 * @author Gavin Borne
 * @brief Better assert function for testing
 *
 * @copyright Copyright (c) 2025 Gavin Borne
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software
 * and associated documentation files (the "Software"),
 * to deal in the Software without restriction,
 * including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
void zassert(bool condition, void* val1, void* val2, const char* message, const char* file, int lineno) {
    if (!condition) {
        if (val1 == NULL && val2 == NULL) {
            fprintf(
                stderr,
                "Assertion failed: Values are NULL\nFile: %s, Line: %d",
                file,
                lineno
            );
            exit(1);
        }
        if (val1 == NULL) {
            fprintf(
                stderr,
                "Assertion failed: 1st value is NULL\nFile: %s, Line: %d",
                (char*)val2,
                file,
                lineno
            );
            exit(1);
        }
        if (val2 == NULL) {
            fprintf(
                stderr,
                "Assertion failed: 2nd value is NULL\nFile: %s, Line: %d",
                (char*)val1,
                file,
                lineno
            );
            exit(1);
        }
        fprintf(
            stderr,
            message,
            (char*)val1,
            (char*)val2,
            file,
            lineno
        );
        exit(1);
    }
    printf("Test at %s:%d OK\n", file, lineno);
}