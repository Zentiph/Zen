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

#define MAX_TOKEN_LENGTH 100
#define NEWLINE_CHAR '\n'
#define BUFFER_SIZE 1024

/**
 * @brief All token types.
 *        FILEEND is used instead of EOF to avoid
 *        name conflicts with stdio.h.
 */
typedef enum {
    COMMENT,
    IDENTIFIER,
    NUMBER,
    BINARY_OPERATOR,
    UNARY_OPERATOR,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    LT_PAREN,
    RT_PAREN,
    LT_BRACKET,
    RT_BRACKET,
    LT_CURLY,
    RT_CURLY,
    ARROW,
    DOUBLE_ARROW,
    NEWLINE,
    FILEEND,
    INVALID,
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