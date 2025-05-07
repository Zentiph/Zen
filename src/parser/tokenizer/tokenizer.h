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
#include "state.h"

#define MAX_TOKEN_LENGTH 128
#define NEWLINE_CHAR '\n'

/**
 * @enum TokenType
 * @brief All token types.
 */
typedef enum
{
    // Keywords
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_FN,
    TOKEN_CLASS,
    TOKEN_IMPORT,
    TOKEN_FROM,
    TOKEN_EXPORT,
    TOKEN_MODULE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_IN,
    TOKEN_EXTENDS,

    // Variable size tokens
    TOKEN_COMMENT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,

    // Standard operators
    TOKEN_ASSIGN,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,

    // In-place assignment
    TOKEN_ADD_ASSIGN,
    TOKEN_SUB_ASSIGN,
    TOKEN_MUL_ASSIGN,
    TOKEN_DIV_ASSIGN,
    TOKEN_MOD_ASSIGN,

    // Comparison
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LE,
    TOKEN_GE,
    TOKEN_EQ,
    TOKEN_NE,

    // Containers
    TOKEN_LT_PAREN,
    TOKEN_RT_PAREN,
    TOKEN_LT_BRACK,
    TOKEN_RT_BRACK,
    TOKEN_LT_CURLY,
    TOKEN_RT_CURLY,

    // Arrows
    TOKEN_ARROW,
    TOKEN_DBL_ARROW,

    // Single chars
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_NEWLINE,
    TOKEN_EOF,

    // None of the above
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
 * @param fp    File pointer
 * @param state Tokenizer state
 */
void skip_whitespace(FILE *fp, TokenizerState *state);

/**
 * @brief Get the next token in the file.
 *
 * @param fp       File pointer
 * @param state    Tokenizer state
 * @return Token - The token found
 */
Token next_token(FILE *fp, TokenizerState *state);

/**
 * @brief Print a representation of a token.
 *
 * @param token Token to print
 */
void print_token(Token token);

#endif // TOKENIZER_H