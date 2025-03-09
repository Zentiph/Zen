/**
 * @file tokenizer.c
 * @author Gavin Borne
 * @brief Tokenizer header for the Zen programming language
 *
 * @copyright Copyright (c) 2025 Gavin Borne
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software
 * and associated documentation files (the “Software”),
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
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_TOKEN_LENGTH 100
#define NEWLINE_CHAR '\n'
#define BUFFER_SIZE 1024

/**
 * @brief All token types.
 *        FILEEND is used instead of EOF to avoid
 *        name conflicts with stdio.h.
 */
typedef enum {
    TOKEN_COMMENT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_BINARY_OP,
    TOKEN_UNARY_OP,
    TOKEN_SGL_QTE,
    TOKEN_DBL_QTE,
    TOKEN_LT_PAREN,
    TOKEN_RT_PAREN,
    TOKEN_LT_BRACK,
    TOKEN_RT_BRACK,
    TOKEN_LT_CURLY,
    TOKEN_RT_CURLY,
    TOKEN_ARROW,
    TOKEN_DBL_ARROW,
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_INVALID
} TokenType;

/**
 * @brief A token that contains its type and value.
 */
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

void skip_whitespace(FILE *fp, char **ptr, char *buffer, size_t *bytesRead, size_t bufferSize);
Token next_token(FILE *fp);
void print_token(Token token);

#endif // TOKENIZER_H