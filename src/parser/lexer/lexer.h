/// @file lexer.h
/// @author Gavin Borne
/// @brief Lexer specification header for the Zen programming language.
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

#include "token.h"

#ifndef ZLANG_LEXER_BUFSIZ
/// The size of the lexer's text IO buffer.
#define ZLANG_LEXER_BUFSIZ 8192 // 2 ^ 13
#endif

#ifndef ZLANG_LEXER_KEEP_BACK
/// How many characters that can be ungotten safely.
#define ZLANG_LEXER_KEEP_BACK 1
#endif

#ifndef ZLANG_LEXER_LOOKAHEAD
/// The desired lookahead for the lexer in bytes.
#define ZLANG_LEXER_LOOKAHEAD 1
#endif

/// A table of all keywords for the Zen programming language.
static const char *const KW_TAB[] = {"if",     "else", "while",  "for",
                                     "in",     "fn",   "class",  "extends",
                                     "import", "from", "export", "module",
                                     NULL}; // end of table marker

/// A representation of the lexer's state.
typedef struct lexer_s *lexer_t;

/// @brief Initialize a lexer.
/// @param fp       A pointer to the file to read from.
/// @param filename The name of the file.
/// @return The initialized lexer.

///
/// @brief Create a lexer.
///
/// @param fp       - The pointer to the file to read from.
/// @param filename - The filename to read from.
/// @return lexer_t - The new lexer.
///
const lexer_t lexer_create(FILE *__restrict fp, char *__restrict filename);

///
/// @brief Destroy a lexer. The lexer's file is automatically closed in the
/// process.
///
/// @param lexer - The lexer to destroy.
///
void lexer_destroy(const lexer_t lexer);

///
/// @brief Get the character the lexer is currently examining.
///
/// @param lexer - The lexer.
/// @return int  - The current character.
///
int lexer_current(lexer_t lexer);

///
/// @brief Peek at the next character.
///
/// @param lexer - The lexer.
/// @return int  - The next character.
///
int lexer_peek(lexer_t lexer);

///
/// @brief Advance to the next character and return it.
///
/// @param lexer - The lexer.
/// @return int  - The character that was advanced to.
///
int lexer_pre_advance(lexer_t lexer);

///
/// @brief Advance to the next character and return the previous character.
///
/// @param lexer - The lexer.
/// @return int  - The character that was advanced past.
///
int lexer_post_advance(lexer_t lexer);

/// @brief Un-consume a single char.
///        This operation is guaranteed to be safe up to ZLANG_LEX_KEEP_BK times
///        in a row without buffer refilling.
/// @param lexer he lexer.
/// @return Whether the operation was a success.

///
/// @brief Un-consume a single character. This operation is guaranteed to be
///        safe up to ZLANG_LEX_KEEP_BK times in a row without buffer refilling.
///
/// @param lexer  - The lexer.
/// @return true  - If the operation was a success.
/// @return false - If the operation failed.
///
bool lexer_unget(lexer_t lexer);

///
/// @brief Skip n characters.
///
/// @param lexer - The lexer.
/// @param n     - The number of characters to skip.
///
void lexer_skip(lexer_t lexer, int n);

///
/// @brief Skip meaningless whitespace until an important character is found.
///        Newlines do not count as whitespace.
///
/// @param lexer - The lexer.
///
void lexer_skip_whitespace(lexer_t lexer);

///
/// @brief Get the next token in the file.
///
/// @param lexer    - The lexer.
/// @return token_t - The next token in the file.
///
token_t lex_next(lexer_t lexer);

#endif // ZLANG_LEXER_H