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

#ifndef ZLANG_LEXER_H
#define ZLANG_LEXER_H

#include <stdbool.h>
#include <stdio.h>

#ifndef ZLANG_LEX_BUFSIZ
/// The size of the lexer's text IO buffer.
#define ZLANG_LEX_BUFSIZ 8192
#endif

#ifndef ZLANG_TOK_SIZ
/// The max token size in bytes.
#define ZLANG_TOK_SIZ 65536
#endif

#ifndef ZLANG_LEX_KEEP_BK
/// How many characters that can be ungotten safely.
#define ZLANG_LEX_KEEP_BK 1
#endif

#ifndef ZLANG_LEX_LOOKAHEAD
/// The desired lookahead for the lexer in bytes.
#define ZLANG_LEX_LOOKAHEAD 1
#endif

/// @brief A representation of the lexer's state.
typedef struct lexer_s
{
   FILE *fp;
   const char *filename;

   char buf[ZLANG_LEX_BUFSIZ];
   /// @brief A pointer to the current char.
   char *ptr;
   /// @brief The number of valid bytes in buf.
   size_t bytes_read;

   int line;
   int col;

   char tbuf[ZLANG_TOK_SIZ];
   size_t tlen;

   // position history so unget() can restore line/col
   int _hist_line[ZLANG_LEX_KEEP_BK];
   int _hist_col[ZLANG_LEX_KEEP_BK];
   int _hist_len; // 0 to ZLANG_LEX_KEEP_BK
} lexer_t;

// Macro that makes the preprocessor generate an enum and
// matching char* array for all token types.
// Solution modified from Terrence M on stackoverflow:
// https://stackoverflow.com/a/10966395
#define ZLANG__FOREACH_TOKEN(TOKEN) \
   TOKEN(TOK_KW)                    \
   TOKEN(TOK_COMMENT)               \
   TOKEN(TOK_ID)                    \
   TOKEN(TOK_NUM)                   \
   TOKEN(TOK_STR)                   \
   TOKEN(TOK_ASSIGN)                \
   TOKEN(TOK_ADD)                   \
   TOKEN(TOK_SUB)                   \
   TOKEN(TOK_MUL)                   \
   TOKEN(TOK_DIV)                   \
   TOKEN(TOK_MOD)                   \
   TOKEN(TOK_ADD_ASSIGN)            \
   TOKEN(TOK_SUB_ASSIGN)            \
   TOKEN(TOK_MUL_ASSIGN)            \
   TOKEN(TOK_DIV_ASSIGN)            \
   TOKEN(TOK_MOD_ASSIGN)            \
   TOKEN(TOK_AND)                   \
   TOKEN(TOK_OR)                    \
   TOKEN(TOK_NOT)                   \
   TOKEN(TOK_EQ)                    \
   TOKEN(TOK_NE)                    \
   TOKEN(TOK_LT)                    \
   TOKEN(TOK_GT)                    \
   TOKEN(TOK_LE)                    \
   TOKEN(TOK_GE)                    \
   TOKEN(TOK_LT_PAREN)              \
   TOKEN(TOK_RT_PAREN)              \
   TOKEN(TOK_LT_BRACK)              \
   TOKEN(TOK_RT_BRACK)              \
   TOKEN(TOK_LT_BRACE)              \
   TOKEN(TOK_RT_BRACE)              \
   TOKEN(TOK_ARROW)                 \
   TOKEN(TOK_DBL_ARROW)             \
   TOKEN(TOK_DOT)                   \
   TOKEN(TOK_COMMA)                 \
   TOKEN(TOK_NEWLINE)               \
   TOKEN(TOK_EOF)                   \
   TOKEN(TOK_INVALID)

#define ZLANG__GENERATE_ENUM(ENUM) ENUM,
#define ZLANG__GENERATE_STR(STR) #STR,

/// @brief An enum of token types that can be lexed.
typedef enum
{
   ZLANG__FOREACH_TOKEN(ZLANG__GENERATE_ENUM)
} Token;

/// @brief An array of string representations
///        of enum values stored at their numeric value.
const char *TOK_TO_STR[] = {
    ZLANG__FOREACH_TOKEN(ZLANG__GENERATE_STR)};

/// @brief A token, containing its type and its string value.
typedef struct token_s
{
   Token type;
   char *lexeme;
   size_t len;
} token_t;

/// @brief Free a token from memory.
/// @param tok  The token to free.
static inline void tok_free(token_t *tok)
{
   free(tok->lexeme);
   tok->lexeme = NULL;
   tok->len = 0;
}

/// @brief Initialize a lexer.
/// @param fp       A pointer to the file to read from.
/// @param filename The name of the file.
/// @return The initialized lexer.
lexer_t *lex_init(FILE *fp, const char *filename);

/// @brief Get the char the lexer is currently examining.
/// @param lex The lexer.
/// @return The current char.
int lex_cur(lexer_t *lex);

/// @brief Peek at the next char.
/// @param lex The lexer.
/// @return    The next char.
int lex_peek(lexer_t *lex);

/// @brief Advance to the next char and return it.
/// @param lex The lexer.
/// @return The char that was advanced to.
int lex_adv(lexer_t *lex);

/// @brief Un-consume a single char.
///        This operation is guaranteed to be safe up to ZLANG_LEX_KEEP_BK times
///        in a row without buffer refilling.
/// @param lex The lexer.
/// @return Whether the operation was a success.
bool lex_unget(lexer_t *lex);

/// @brief Skip ahead by n chars, ending if EOF is reached.
/// @param lex The lexer.
/// @param nThe number of chars to skip.
void lex_skip(lexer_t *lex, int n);

/// @brief Skip whitespace starting at the current char until
///        a non-whitespace char is found.
///        The newline char is not considered whitespace.
/// @param lex The lexer.
void lex_skip_wsp(lexer_t *lex);

/// @brief Get the next token in the file.
/// @param lex The lexer.
/// @return The token found.
token_t lex_next(lexer_t *lex);

#endif // ZLANG_LEXER_H