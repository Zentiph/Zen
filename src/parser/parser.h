///
/// @file parser.h
/// @author Gavin Borne
/// @brief Parsing functionality header for the Zen programming language.
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

#ifndef ZLANG_PARSER_H
#define ZLANG_PARSER_H

#include "ast.h"

/// @brief A representation of the parser's state.
typedef struct
{
   lexer_t *lex;
   token_t cur;
   token_t prev;
} parser_t;

/// @brief Initialize a parser.
/// @param lex The lexer to use.
/// @return The initialized parser.
parser_t *parser_init(lexer_t *lex);

/// @brief Advance to the next token.
/// @param parser The parser.
void parser_adv(parser_t *parser);

/// @brief Check if the current token matches the given token type.
///        If so, advance and return true. Otherwise, return false.
/// @param parser   The parser.
/// @param tok_type The token type to match.
/// @return Whether the current token's type and expected type match.
bool parser_match(parser_t *parser, Token tok_type);

/// @brief Determine if the current token matches given token type without advancing.
/// @param parser   The parser.
/// @param tok_type The token type to check.
/// @return Whether the current token's type and expected type match.
bool parser_check(parser_t *parser, Token tok_type);

/// @brief Similar to match(), but with an error message.
/// @param parser   The parser.
/// @param tok_type The token type to check.
/// @param msg      The error message.
void parser_expect(parser_t *parser, Token tok_type, const char *msg);

/// @brief Peek at the next token.
/// @param lex The lexer.
/// @return The next token.
token_t parser_peek(lexer_t *lex);

/// @brief Report a syntax error to the parser.
/// @param parser The parser.
/// @param msg    The error message.
void parser_error(parser_t *parser, const char *msg);

#endif // ZLANG_PARSER_H