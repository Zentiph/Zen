///
/// @file test_lexer.c
/// @author Gavin Borne
/// @brief Tests for the Zen lexer.
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZA_VERBOSE
#include "../zassert.h"
#include "../../../src/parser/lexer/lexer.h"

#define TEST_SKIP_WSP(lex, exp) test_skip_wsp_at(__FILE__, __LINE__, (lex), (exp))
#define TEST_NEXT_TOK(lex, exp) test_next_tok_at(__FILE__, __LINE__, (lex), (exp))
#define TOKEXP(t, s) ((Token){.type = (t), .val = (s), .len = ((s) ? strlen(s) : 0)})

static void test_skip_wsp_at(const char *file, int line, Lexer *lex, const int exp)
{
    ZA_ASSERT_TRUE_AT(file, line, lex != NULL);
    skip_wsp(lex);
    ZA_ASSERT_EQ_INT_AT(file, line, cur_char(lex), exp);
}

static void test_next_tok_at(const char *file, int line, Lexer *lex, Token exp)
{
    ZA_ASSERT_TRUE_AT(file, line, lex != NULL);
    ZA_ASSERT_EQ_CSTR_AT(file, line, next_tok(lex).val, exp.val);
}

int main(int argc, char const *argv[])
{
    if (argc > 2)
    {
        fprintf(stderr, "Usage: test_tokenizer <source (optional)>\n");
        return 1;
    }

    // Source file given, run given file
    if (argc == 2)
    {
        const char *filename = argv[1];
        // Check if the file is a .zen file
        size_t file_len = strlen(filename);
        int suffix_len = 4; // ".zen"
        if (file_len < 4 || strcmp(filename + file_len - suffix_len, ".zen") != 0)
        {
            fprintf(stderr, "tokenizer source input must be a .zen file");
            return 1;
        }

        FILE *fp = fopen(filename, "r");
        if (!fp)
        {
            perror("Unable to open file");
            return 1;
        }

        Lexer *lex = init_lexer(fp, filename);
        if (!lex)
        {
            perror("Could not allocate memory for Lexer");
            return 1;
        }

        Token tok;
        while ((tok = next_tok(lex)).type != TOK_EOF)
        {
            printf("%s\n", TOK_TO_STR[tok.type]);
        }
        printf("%s\n", TOK_TO_STR[tok.type]); // Print EOF

        fclose(fp);
        free(lex);
        lex = NULL;
        return 0;
    }

    // No source file, run pre-made tests
    printf("No source file given, running pre-made tests...\n");

    /////////////////////////////////////////////////////////////////////////////
    //                                 TESTING                                 //
    //                                 in1.zen                                 //
    /////////////////////////////////////////////////////////////////////////////
    char *filename = "tests/parser/input/in1.zen";
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("Unable to open file");
        return 1;
    }

    Lexer *lex = init_lexer(fp, filename);
    if (!lex)
    {
        perror("Could not allocate memory for Lexer");
        return 1;
    }

    TEST_SKIP_WSP(lex, 'i');
    skip(lex, strlen("int")); // Skip the rest of the word "int"
    TEST_SKIP_WSP(lex, 'x');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '=');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '5');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '\n');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, 'p');

    free(lex);
    lex = NULL;
    rewind(fp);
    lex = init_lexer(fp, filename);

    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "int"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "x"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ASSIGN, "="));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NUM, "5"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NEWLINE, "\n"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "print"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_LT_PAREN, "("));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "x"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_RT_PAREN, ")"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_EOF, ""));

    free(lex);
    lex = NULL;
    fclose(fp);

    /////////////////////////////////////////////////////////////////////////////
    //                                 TESTING                                 //
    //                                 in2.zen                                 //
    /////////////////////////////////////////////////////////////////////////////
    filename = "./tests/parser/input/in2.zen";
    fp = fopen(filename, "r");
    if (!fp)
    {
        perror("Unable to open file");
        return 1;
    }

    lex = init_lexer(fp, filename);

    TEST_SKIP_WSP(lex, 's');
    skip(lex, strlen("string"));
    TEST_SKIP_WSP(lex, 'g');
    skip(lex, strlen("greeting"));
    TEST_SKIP_WSP(lex, '=');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '"');
    skip(lex, strlen("\"Hello, world!\""));
    TEST_SKIP_WSP(lex, '\n');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, 'i');
    skip(lex, strlen("if"));
    TEST_SKIP_WSP(lex, 'g');
    skip(lex, strlen("greeting.length"));
    TEST_SKIP_WSP(lex, '>');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '4');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '{');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '\n');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, 'p');
    skip(lex, strlen("print(greeting)"));
    TEST_SKIP_WSP(lex, '\n');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '}');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, 'e');
    skip(lex, strlen("else"));
    TEST_SKIP_WSP(lex, '{');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '\n');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, 'p');
    skip(lex, strlen("print(\"Greeting too short.\")"));
    TEST_SKIP_WSP(lex, '\n');
    skip(lex, 1);
    TEST_SKIP_WSP(lex, '}');

    free(lex);
    lex = NULL;
    rewind(fp);
    lex = init_lexer(fp, filename);

    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "string"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "greeting"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ASSIGN, "="));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_STR, "Hello, world!"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NEWLINE, "\n"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_KW, "if"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "greeting"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_DOT, "."));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "length"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_GT, ">"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NUM, "4"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_LT_BRACE, "{"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NEWLINE, "\n"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "print"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_LT_PAREN, "("));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "greeting"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_RT_PAREN, ")"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NEWLINE, "\n"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_RT_BRACE, "}"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_KW, "else"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_LT_BRACE, "{"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NEWLINE, "\n"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_ID, "print"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_LT_PAREN, "("));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_STR, "Greeting too short."));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_RT_PAREN, ")"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_NEWLINE, "\n"));
    TEST_NEXT_TOK(lex, TOKEXP(TOK_RT_BRACE, "}"));

    /////////////////////////////////////////////////////////////////////////////
    //                                 TESTING                                 //
    //                                   END                                   //
    /////////////////////////////////////////////////////////////////////////////
    fclose(fp);
    free(lex);
    lex = NULL;

    za_print_summary(stdout);
    return za_exit_code();
}