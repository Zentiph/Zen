/**
 * @file test_tokenizer.c
 * @author Gavin Borne
 * @brief Tests for the Zen tokenizer
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
#include <string.h>
#include "../src/tokenizer/tokenizer.h"
#include "zassert.h"

/**
 * Global buffer to use when reading from files
 * (identical to the buffer used in tokenizer.c)
 */
static char buffer[BUFFER_SIZE];
static char *ptr = buffer;
static size_t bytesRead = 0;

/**
 * @brief Test the skip_whitespace function,
 *        asserting that the next character is the expected character
 *
 * @param fp       File pointer
 * @param expected Expected character after skipping whitespace
 * @param file     File the of the assertion
 * @param lineno   Line number of the assertion
 */
void test_skip_whitespace(FILE *fp, const char expected, const char* file, int lineno) {
    _assert(fp != NULL, fp, NULL, ASSERT_NEQ_MSG, file, lineno);
    skip_whitespace(fp, &ptr, buffer, &bytesRead, BUFFER_SIZE);
    // Convert the chars to strings for printing
    char actualCh[2] = {*ptr, '\0'};
    char expectedCh[2] = {expected, '\0'};
    _assert(*ptr == expected, actualCh, expectedCh, ASSERT_EQ_MSG, file, lineno);
}

/**
 * @brief Test the next_token function,
 *        asserting that the next token is the expected token
 *
 * @param fp       File pointer
 * @param expected Expected character after skipping whitespace
 * @param file     File the of the assertion
 * @param lineno   Line number of the assertion
 */
void test_next_token(FILE *fp, Token expected, const char* file, int lineno) {
    _assert(fp != NULL, fp, NULL, ASSERT_NEQ_MSG, file, lineno);
    Token token = next_token(fp);
    if (token.value != NULL && expected.value != NULL) {
        _assert(
            strcmp(token.value, expected.value) == 0,
            &token.value,
            &expected.value,
            ASSERT_EQ_MSG,
            file,
            lineno
        );
    } else {
        _assert(
            token.value == expected.value,
            &token.value,
            &expected.value,
            ASSERT_EQ_MSG,
            file,
            lineno
        );
    }
    _assert(
        token.type == expected.type,
        token_type_to_string(token.type),
        token_type_to_string(expected.type),
        ASSERT_EQ_MSG,
        file,
        lineno
    );
}

/**
 * We do not have a test_print_token function here because
 * print_token is simply for debugging and not related to
 * the functionality of the tokenizer.
 */

int main(int argc, char const *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: test_tokenizer <source (optional)>\n");
        return 1;
    }

    // Source file given, run given file
    if (argc == 2) {
        // Check if the file is a .zen file
        size_t fileLen = strlen(argv[1]);
        int suffixLen = 4; // ".zen"
        if (strcmp(argv[1] + fileLen - suffixLen, ".zen") != 0) {
            fprintf(stderr, "tokenizer source input must be a .zen file");
            return 1;
        }

        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            perror("Unable to open file");
            return 1;
        }

        Token token;
        while ((token = next_token(fp)).type != TOKEN_EOF) {
            print_token(token);
        }
        print_token(token); // Print EOF

        fclose(fp);
        return 0;
    }

    // No source file, run pre-made tests
    // Test in1.zen
    FILE *fp = fopen("tests/input/in1.zen", "r");
    if (!fp) {
        perror("Unable to open file");
        return 1;
    }

    test_skip_whitespace(fp, 'i', __FILE__, __LINE__);
    ptr += 3; // Skip the rest of the word "int"
    test_skip_whitespace(fp, 'x', __FILE__, __LINE__);
    ptr++;
    test_skip_whitespace(fp, '=', __FILE__, __LINE__);
    ptr++;
    test_skip_whitespace(fp, '5', __FILE__, __LINE__);
    ptr++;
    test_skip_whitespace(fp, '\n', __FILE__, __LINE__);
    ptr++;
    test_skip_whitespace(fp, 'p', __FILE__, __LINE__);
    ptr += 5; // Skip the rest of the word "print"
    test_skip_whitespace(fp, '(', __FILE__, __LINE__);

    // Reset the file pointer and buffer
    fseek(fp, 0, SEEK_SET);
    memset(buffer, 0, BUFFER_SIZE);
    ptr = buffer;

    test_next_token(fp, (Token){TOKEN_IDENTIFIER, "int"}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_IDENTIFIER, "x"}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_BINARY_OP, "="}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_NUMBER, "5"}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_NEWLINE, "\n"}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_IDENTIFIER, "print"}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_LT_PAREN, "("}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_IDENTIFIER, "x"}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_RT_PAREN, ")"}, __FILE__, __LINE__);
    test_next_token(fp, (Token){TOKEN_EOF, ""}, __FILE__, __LINE__);

    fclose(fp);

    printf("All tests passed\n");
    return 0;
}