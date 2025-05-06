/**
 * @file tokenizer.h
 * @author Gavin Borne
 * @brief Tokenizer header for the Zen programming language
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

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_TOKEN_LENGTH 128
#define NEWLINE_CHAR '\n'
#define BUFFER_SIZE 1024

/**
 * @enum TokenType
 * @brief All token types.
 *        FILEEND is used instead of EOF to avoid
 *        name conflicts with stdio.h.
 */
typedef enum
{
    TOKEN_COMMENT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_BINARY_OP,
    TOKEN_IP_BINARY_OP,
    TOKEN_COMPARISON_OP,
    TOKEN_LOGIC_OP,
    TOKEN_STRING,
    TOKEN_LT_PAREN,
    TOKEN_RT_PAREN,
    TOKEN_LT_BRACK,
    TOKEN_RT_BRACK,
    TOKEN_LT_CURLY,
    TOKEN_RT_CURLY,
    TOKEN_DOT,
    TOKEN_ARROW,
    TOKEN_DBL_ARROW,
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_INVALID
} TokenType;

/**
 * @struct Token
 * @brief A token that contains its type and value.
 */
typedef struct
{
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

/**
 * @brief Convert a TokenType to a string.
 *
 * @param type   TokenType
 * @return char* String representation of the TokenType
 */
char *token_type_to_string(TokenType type);

/**
 * @brief Skip the meaningless whitespace in the
 *        file until a token is found.
 *        Newline characters as well as semicolons
 *        are valid whitespace, so track those.
 *
 * @param fp         File pointer
 * @param ptr        Pointer to the current position in the buffer
 * @param buffer     The buffer containing file data
 * @param bytesRead  Pointer to the count of valid bytes in buffer
 * @param bufferSize The size of the buffer
 */
void skip_whitespace(FILE *fp, char **ptr, char *buffer, size_t *bytesRead, size_t bufferSize);

/**
 * @brief Get the next token in the file.
 *
 * @param fp       File pointer
 * @return Token - The token found
 */
Token next_token(FILE *fp);

/**
 * @brief Print a representation of a token.
 *
 * @param token Token to print
 */
void print_token(Token token);

#endif // TOKENIZER_H