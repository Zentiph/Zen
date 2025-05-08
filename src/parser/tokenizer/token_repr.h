/**
 * @file token_repr.h
 * @author Gavin Borne
 * @brief Representation for tokens header for the Zen programming language
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

#ifndef TOKEN_REPR_H
#define TOKEN_REPR_H

#include "tokenizer.h"

/**
 * @brief Convert a TokenType to a string.
 *
 * @param type           TokenType
 * @return const char* - String representation of the TokenType
 */
const char *token_type_to_string(TokenType type);

/**
 * @brief Print a representation of a token.
 *
 * @param token Token to print
 */
void print_token(Token token);

#endif // TOKEN_REPR_H