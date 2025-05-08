/**
 * @file test_tokenizer.c
 * @author Gavin Borne
 * @brief Tests for the Zen tokenizer
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

// gcc tests/parser/tokenizer/test_tokenizer.c src/parser/tokenizer/tokenizer.c src/parser/tokenizer/state.c src/parser/tokenizer/token_repr.c tests/parser/zassert.c -o tests/parser/tokenizer/test_tokenizer

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../zassert.h"
#include "../../../src/parser/tokenizer/state.h"
#include "../../../src/parser/tokenizer/tokenizer.h"
#include "../../../src/parser/tokenizer/token_repr.h"

/**
 * @brief Test the skip_whitespace function,
 *        asserting that the next character is the expected character.
 *
 * @param tokenizer Tokenizer
 * @param expected  Expected character after skipping whitespace
 * @param file      File the of the assertion
 * @param lineno    Line number of the assertion
 */
void test_skip_whitespace(Tokenizer *tokenizer, const char expected, const char *file, int lineno)
{
    zassert(tokenizer != NULL, tokenizer, NULL, ASSERT_NEQ_MSG, file, lineno);

    skip_whitespace(tokenizer);

    // Convert the chars to strings for printing
    char actualCh[2] = {current_char(tokenizer), '\0'};
    char expectedCh[2] = {expected, '\0'};
    zassert(current_char(tokenizer) == expected, actualCh, expectedCh, ASSERT_EQ_MSG, file, lineno);
}

/**
 * @brief Test the next_token function,
 *        asserting that the next token is the expected token
 *
 * @param tokenizer Tokenizer
 * @param expected  Expected character after skipping whitespace
 * @param file      File the of the assertion
 * @param lineno    Line number of the assertion
 */
void test_next_token(Tokenizer *tokenizer, Token expected, const char *file, int lineno)
{
    zassert(tokenizer != NULL, tokenizer, NULL, ASSERT_NEQ_MSG, file, lineno);

    Token token = next_token(tokenizer);

    if (token.value != NULL && expected.value != NULL)
    {
        zassert(
            strcmp(token.value, expected.value) == 0,
            &token.value,
            &expected.value,
            ASSERT_EQ_MSG,
            file,
            lineno);
    }
    else
    {
        zassert(
            token.value == expected.value,
            &token.value,
            &expected.value,
            ASSERT_EQ_MSG,
            file,
            lineno);
    }
    zassert(
        token.type == expected.type,
        token_type_to_string(token.type),
        token_type_to_string(expected.type),
        ASSERT_EQ_MSG,
        file,
        lineno);
}

int main(int argc, char const *argv[])
{
    if (argc > 2)
    {
        fprintf(stderr, "Usage: test_tokenizer <source (optional)>\n");
        return 1;
    }

    Tokenizer *tokenizer = malloc(sizeof(Tokenizer));
    if (!tokenizer)
    {
        fprintf(stderr, "Error: Failed to allocate memory for Tokenizer.\n");
        return 1;
    }

    // Source file given, run given file
    if (argc == 2)
    {
        // Check if the file is a .zen file
        size_t fileLen = strlen(argv[1]);
        int suffixLen = 4; // ".zen"
        if (strcmp(argv[1] + fileLen - suffixLen, ".zen") != 0)
        {
            fprintf(stderr, "tokenizer source input must be a .zen file");
            free(tokenizer);
            return 1;
        }

        FILE *fp = fopen(argv[1], "r");
        if (!fp)
        {
            perror("Unable to open file");
            free(tokenizer);
            return 1;
        }

        initialize_tokenizer(tokenizer, fp);

        Token token;
        while ((token = next_token(tokenizer)).type != TOKEN_EOF)
        {
            print_token(token);
        }
        print_token(token); // Print EOF

        fclose(fp);
        free(tokenizer);
        return 0;
    }

    // No source file, run pre-made tests
    printf("No source file given, running pre-made tests...\n");

    /***************************************************************************
     *                                 TESTING                                 *
     *                                 in1.zen                                 *
     ***************************************************************************/
    FILE *fp = fopen("tests/parser/input/in1.zen", "r");
    if (!fp)
    {
        perror("Unable to open file");
        free(tokenizer);
        return 1;
    }

    initialize_tokenizer(tokenizer, fp);

    test_skip_whitespace(tokenizer, 'i', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("int")); // Skip the rest of the word "int"
    test_skip_whitespace(tokenizer, 'x', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '=', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '5', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '\n', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, 'p', __FILE__, __LINE__);

    initialize_tokenizer(tokenizer, fp); // Reset tokenizer and file

    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "int"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "x"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_ASSIGN, "="}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NUMBER, "5"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NEWLINE, "\n"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "print"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_LT_PAREN, "("}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "x"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_RT_PAREN, ")"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_EOF, ""}, __FILE__, __LINE__);

    fclose(fp);

    /***************************************************************************
     *                                 TESTING                                 *
     *                                 in2.zen                                 *
     ***************************************************************************/

    fp = fopen("tests/parser/input/in2.zen", "r");
    if (!fp)
    {
        perror("Unable to open file");
        free(tokenizer);
        return 1;
    }

    initialize_tokenizer(tokenizer, fp); // Reset tokenizer and file

    test_skip_whitespace(tokenizer, 's', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("string"));
    test_skip_whitespace(tokenizer, 'g', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("greeting"));
    test_skip_whitespace(tokenizer, '=', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '"', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("\"Hello, world!\""));
    test_skip_whitespace(tokenizer, '\n', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, 'i', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("if"));
    test_skip_whitespace(tokenizer, 'g', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("greeting.length"));
    test_skip_whitespace(tokenizer, '>', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '4', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '{', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '\n', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, 'p', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("print(greeting)"));
    test_skip_whitespace(tokenizer, '\n', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '}', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, 'e', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("else"));
    test_skip_whitespace(tokenizer, '{', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '\n', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, 'p', __FILE__, __LINE__);
    move_pointer(tokenizer, strlen("print(\"Greeting too short.\")"));
    test_skip_whitespace(tokenizer, '\n', __FILE__, __LINE__);
    move_pointer(tokenizer, 1);
    test_skip_whitespace(tokenizer, '}', __FILE__, __LINE__);

    initialize_tokenizer(tokenizer, fp); // Reset tokenizer and file

    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "string"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "greeting"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_ASSIGN, "="}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_STRING, "Hello, world!"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NEWLINE, "\n"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IF, "if"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "greeting"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_DOT, "."}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "length"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_GT, ">"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NUMBER, "4"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_LT_CURLY, "{"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NEWLINE, "\n"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "print"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_LT_PAREN, "("}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "greeting"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_RT_PAREN, ")"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NEWLINE, "\n"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_RT_CURLY, "}"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_ELSE, "else"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_LT_CURLY, "{"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NEWLINE, "\n"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_IDENTIFIER, "print"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_LT_PAREN, "("}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_STRING, "Greeting too short."}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_RT_PAREN, ")"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_NEWLINE, "\n"}, __FILE__, __LINE__);
    test_next_token(tokenizer, (Token){TOKEN_RT_CURLY, "}"}, __FILE__, __LINE__);

    /***************************************************************************
     *                                 TESTING                                 *
     *                                   END                                   *
     ***************************************************************************/

    fclose(fp);
    free(tokenizer);

    printf("All tests passed\n");
    return 0;
}