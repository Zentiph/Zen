/**
 * @file parser.h
 * @author Gavin Borne
 * @brief Parser header for the Zen programming language
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

#include "../tokenizer/tokenizer.h"

typedef struct
{
   Token current;
   Tokenizer tokenizer;
} Parser;

/**
 * @brief Advance to the next token.
 *
 * @param parser Parser
 */
void advance(Parser *parser);

/**
 * @brief Ensure the current token matches a specific type, then advance.
 *
 * @param parser Parser
 * @param type   Type to match
 */
void expect(Parser *parser, TokenType type);

/**
 * @brief Check if the current token matches the given type.
 *        If so, advance and return true. Otherwise, return false.
 *
 * @param parser   Parser
 * @param type     Token type to check
 * @return true  - If the types match
 * @return false - Otherwise
 */
bool match(Parser *parser, TokenType type);

/**
 * @brief Determine if the current token matches the given type without advancing.
 *
 * @param parser   Parser
 * @param type     Token type to check
 * @return true  - If the types match
 * @return false - Otherwise
 */
bool check(Parser *parser, TokenType type);

/**
 * @brief Similar to expect(), but with a message,
 *        Ensure the current token matches the given type, then advance.
 *
 * @param parser  Parser
 * @param type    Token type to expect
 * @param message Error message
 */
void expect_message(Parser *parser, TokenType type, const char *message);

/**
 * @brief Peek at the next token.
 *
 * @param tokenizer Tokenizer to peek with
 * @return Token  - Next token
 */
Token peek(Tokenizer *tokenizer);