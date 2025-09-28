/// @file lexer.h
/// @author Gavin Borne
/// @brief Lexer types and functions header for the Zen programming language.
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

#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdio.h>

#ifndef LEX_BUF_SIZE
/// The size of the lexer's text IO buffer.
#define LEX_BUF_SIZE 8192
#endif

#ifndef TOK_SIZE
/// The max token size in bytes.
#define TOK_SIZE 65536
#endif

#ifndef LEX_KEEP_BACK
/// How many characters that can be ungotten safely.
#define LEX_KEEP_BACK 1
#endif

#ifndef LEX_LOOKAHEAD
/// The desired lookahead for the lexer in bytes.
#define LEX_LOOKAHEAD 1
#endif

/// @brief A representation of the lexer's state.
typedef struct
{
   FILE *fp;
   const char *filename;

   char buf[LEX_BUF_SIZE];
   /// @brief A pointer to the current char.
   char *ptr;
   /// @brief The number of valid bytes in buf.
   size_t bytesRead;

   int line;
   int col;

   // position history so unget() can restore line/col
   int _hist_line[LEX_KEEP_BACK];
   int _hist_col[LEX_KEEP_BACK];
   int _hist_len; // 0 to LEX_KEEP_BACK
} Lexer;

// Macro that makes the preprocessor generate an enum and
// matching char* array for all token types.
// Solution modified from Terrence M on stackoverflow:
// https://stackoverflow.com/a/10966395
#define _FOREACH_TOKEN(TOKEN) \
   TOKEN(TOK_KW)              \
   TOKEN(TOK_COMMENT)         \
   TOKEN(TOK_ID)              \
   TOKEN(TOK_NUM)             \
   TOKEN(TOK_STR)             \
   TOKEN(TOK_ASSIGN)          \
   TOKEN(TOK_ADD)             \
   TOKEN(TOK_SUB)             \
   TOKEN(TOK_MUL)             \
   TOKEN(TOK_DIV)             \
   TOKEN(TOK_MOD)             \
   TOKEN(TOK_ADD_ASSIGN)      \
   TOKEN(TOK_SUB_ASSIGN)      \
   TOKEN(TOK_MUL_ASSIGN)      \
   TOKEN(TOK_DIV_ASSIGN)      \
   TOKEN(TOK_MOD_ASSIGN)      \
   TOKEN(TOK_AND)             \
   TOKEN(TOK_OR)              \
   TOKEN(TOK_NOT)             \
   TOKEN(TOK_EQ)              \
   TOKEN(TOK_NE)              \
   TOKEN(TOK_LT)              \
   TOKEN(TOK_GT)              \
   TOKEN(TOK_LE)              \
   TOKEN(TOK_GE)              \
   TOKEN(TOK_LT_PAREN)        \
   TOKEN(TOK_RT_PAREN)        \
   TOKEN(TOK_LT_BRACK)        \
   TOKEN(TOK_RT_BRACK)        \
   TOKEN(TOK_LT_BRACE)        \
   TOKEN(TOK_RT_BRACE)        \
   TOKEN(TOK_ARROW)           \
   TOKEN(TOK_DBL_ARROW)       \
   TOKEN(TOK_DOT)             \
   TOKEN(TOK_COMMA)           \
   TOKEN(TOK_NEWLINE)         \
   TOKEN(TOK_EOF)             \
   TOKEN(TOK_INVALID)

#define _GENERATE_ENUM(ENUM) ENUM,
#define _GENERATE_STR(STR) #STR,

typedef enum
{
   _FOREACH_TOKEN(_GENERATE_ENUM)
} TokenType;

static const char *TOK_STR[] = {
    _FOREACH_TOKEN(_GENERATE_STR)};

/// @brief A token, containing its type and its string value.
typedef struct
{
   TokenType type;
   char val[TOK_SIZE];
} Token;

/// @brief Initialize a lexer.
/// @param fp       A pointer to the file to read from.
/// @param filename The name of the file.
/// @return The initialized lexer.
Lexer *init_lexer(FILE *fp, const char *filename);

/// @brief Get the char the lexer is currently examining.
/// @param lex The lexer.
/// @return The current char.
int cur_char(Lexer *lex);

/// @brief Peek at the next char.
/// @param lex The lexer.
/// @return    The next char.
int peek(Lexer *lex);

/// @brief Advance to the next char and return it.
/// @param lex The lexer.
/// @return The char that was advanced to.
int advance(Lexer *lex);

/// @brief Un-consume a single char.
///        This operation is guaranteed to be safe up to LEX_KEEP_BACK times
///        in a row without buffer refilling.
/// @param lex The lexer.
/// @return Whether the operation was a success.
bool unget(Lexer *lex);

/// @brief Skip ahead by n chars, ending if EOF is reached.
/// @param lex The lexer.
/// @param nThe number of chars to skip.
void skip(Lexer *lex, int n);

/// @brief Skip whitespace starting at the current char until
///        a non-whitespace char is found.
///        The newline char is not considered whitespace.
/// @param lex The lexer.
void skip_whitespace(Lexer *lex);

/// @brief Get the next token in the file.
/// @param lex The lexer.
/// @return The token found.
Token next_tok(Lexer *lex);

#endif // LEXER_H