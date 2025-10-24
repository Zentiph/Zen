///
/// @file token.h
/// @author Gavin Borne
/// @brief Token specification header for the Zen programming language.
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

#pragma once

#include <stdbool.h>

#ifndef ZLANG_TOKSIZ
/// The max token size in bytes.
#define ZLANG_TOKSIZ 65536 // 2 ^ 16
#endif

// Macro that makes the preprocessor generate an enum and
// matching char* array for all token types.
// Solution modified from Terrence M on stackoverflow:
// https://stackoverflow.com/a/10966395
#define ZLANG__FOREACH_TOKEN(TOKEN)                                            \
   TOKEN(TOKEN_KEYWORD)                                                        \
   TOKEN(TOKEN_COMMENT)                                                        \
   TOKEN(TOKEN_IDENTIFIER)                                                     \
   TOKEN(TOKEN_NUMBER)                                                         \
   TOKEN(TOKEN_STRING)                                                         \
   TOKEN(TOKEN_ASSIGN)                                                         \
   TOKEN(TOKEN_ADD)                                                            \
   TOKEN(TOKEN_SUB)                                                            \
   TOKEN(TOKEN_MUL)                                                            \
   TOKEN(TOKEN_DIV)                                                            \
   TOKEN(TOKEN_MOD)                                                            \
   TOKEN(TOKEN_ADD_ASSIGN)                                                     \
   TOKEN(TOKEN_SUB_ASSIGN)                                                     \
   TOKEN(TOKEN_MUL_ASSIGN)                                                     \
   TOKEN(TOKEN_DIV_ASSIGN)                                                     \
   TOKEN(TOKEN_MOD_ASSIGN)                                                     \
   TOKEN(TOKEN_AND)                                                            \
   TOKEN(TOKEN_OR)                                                             \
   TOKEN(TOKEN_NOT)                                                            \
   TOKEN(TOKEN_EQ)                                                             \
   TOKEN(TOKEN_NE)                                                             \
   TOKEN(TOKEN_LT)                                                             \
   TOKEN(TOKEN_GT)                                                             \
   TOKEN(TOKEN_LE)                                                             \
   TOKEN(TOKEN_GE)                                                             \
   TOKEN(TOKEN_LT_PAREN)                                                       \
   TOKEN(TOKEN_RT_PAREN)                                                       \
   TOKEN(TOKEN_LT_BRACK)                                                       \
   TOKEN(TOKEN_RT_BRACK)                                                       \
   TOKEN(TOKEN_LT_BRACE)                                                       \
   TOKEN(TOKEN_RT_BRACE)                                                       \
   TOKEN(TOKEN_ARROW)                                                          \
   TOKEN(TOKEN_DBL_ARROW)                                                      \
   TOKEN(TOKEN_DOT)                                                            \
   TOKEN(TOKEN_COMMA)                                                          \
   TOKEN(TOKEN_NEWLINE)                                                        \
   TOKEN(TOKEN_EOF)                                                            \
   TOKEN(TOKEN_INVALID)

#define ZLANG__GENERATE_ENUM(ENUM) ENUM,
#define ZLANG__GENERATE_STR(STR) #STR,

/// All token types that can be lexed.
typedef enum { ZLANG__FOREACH_TOKEN(ZLANG__GENERATE_ENUM) } Token;

/// String representations of Token enum values stored at their numeric value.
static const char *TOKEN_TO_STRING[] = {
   ZLANG__FOREACH_TOKEN(ZLANG__GENERATE_STR)};

/// A token, which consists of a type and potentially a string value.
typedef struct token_s *token_t;

///
/// @brief Create a new token.
///
/// @param type     - The type of the token.
/// @param lexeme   - The lexeme, if applicable.
/// @return token_t - The token, or NULL if mallocation fails.
///
const token_t token_create(Token type, const char *lexeme);

///
/// @brief Destroy a token.
///
/// @param token - The token to destroy.
///
void token_destroy(const token_t token);

///
/// @brief Get the type of a token.
///
/// @param token        - The token to get the type of.
/// @return const Token - The token type.
///
const Token token_get_type(const token_t token);

///
/// @brief Check if a token has a variable lexeme.
///
/// @param token  - The token to check.
/// @return true  - If the token has a variable lexeme.
/// @return false - If the token does not have a variable lexeme.
///
bool token_has_lexeme(const token_t token);

///
/// @brief Get the lexeme of a token.
///
/// @param token        - The token to get the lexeme of.
/// @return const char* - The lexeme of the token, or NULL if it does not exist.
///
const char *token_get_lexeme(const token_t token);

///
/// @brief Get the length of a token.
///
/// @param token   - The token to get the length of.
/// @return size_t - The length of the token.
///
size_t token_get_length(const token_t token);
