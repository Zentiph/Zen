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
typedef struct parser_s
{
   lexer_t *lex;
   token_t cur;
   token_t prev;
   token_t ahead;
   bool has_ahead;
} parser_t;

/// @brief Initialize a parser.
/// @param lex The lexer to use.
/// @return The initialized parser.
parser_t *parser_init(lexer_t *lex);

/// @brief Close and deallocate a parser.
/// @param parser  The parser.
void parser_close(parser_t *parser);

/// @brief Peek at the next token.
/// @param parser  The parser.
/// @return The next token.
token_t *parser_peek(parser_t *parser);

/// @brief Advance to the next token.
/// @param parser  The parser.
void parser_adv(parser_t *parser);

/// @brief Check if the current token matches the given token type.
///        If so, advance and return true. Otherwise, return false.
/// @param parser    The parser.
/// @param tok_type  The token type to match.
/// @return Whether the current token's type and expected type match.
bool parser_match(parser_t *parser, Token tok_type);

/// @brief Determine if the current token matches given token type without advancing.
/// @param parser    The parser.
/// @param tok_type  The token type to check.
/// @return Whether the current token's type and expected type match.
bool parser_check(parser_t *parser, Token tok_type);

/// @brief Similar to match(), but with an error message.
/// @param parser    The parser.
/// @param tok_type  The token type to check.
void parser_expect(parser_t *parser, Token tok_type);

/// @brief Report a syntax error to the parser.
/// @param parser  The parser.
/// @param msg     The error message.
void parser_error(parser_t *parser, const char *msg);

/// @brief Parse a primary expression.
/// @param parser  The parser.
/// @return The AST node representing the expression.
ast_node_t *parse_primary(parser_t *parser);

/// @brief Parse a unary expression (-x, !x).
/// @param parser  The parser.
/// @return The AST node representing the expression.
ast_node_t *parse_unary(parser_t *parser);

/// @brief Parse a factor (x * y, x / y, x % y).
/// @param parser  The parser.
/// @return The AST node representing the expression.
ast_node_t *parse_factor(parser_t *parser);

/// @brief Parse a term (x + y, x - y).
/// @param parser  The parser.
/// @return The AST node representing the expression.
ast_node_t *parse_term(parser_t *parser);

/// @brief Parse a comparison (x == y, x != y, x < y, x > y, x <= y, x >= y).
/// @param parser  The parser.
/// @return The AST node representing the expression.
ast_node_t *parse_comp(parser_t *parser);

/// @brief Parse a logical expression (x && y, x || y).
/// @param parser  The parser.
/// @return The AST node representing the expression.
ast_node_t *parse_logic(parser_t *parser);

/// @brief Parse an expression.
///        This function acts as a top-level entrypoint, but
///        is identical to parse_logic().
/// @param parser  The parser.
/// @return The AST node representing the expression.
ast_node_t *parse_expr(parser_t *parser);

/// @brief Parse a statement (return, assignment, or regex).
/// @param parser  The parser.
/// @return The AST node representing the statement.
ast_node_t *parse_stmt(parser_t *parser);

/// @brief Parse a code block.
/// @param parser  The parser.
/// @return The AST node representing the code block.
ast_node_t *parse_block(parser_t *parser);

/// @brief Parse an if statement.
/// @param parser  The parser.
/// @return The AST node representing the if statement.
ast_node_t *parse_if(parser_t *parser);

/// @brief Parse a while statement.
/// @param parser  The parser.
/// @return The AST node representing the while statement.
ast_node_t *parse_while(parser_t *parser);

/// @brief Parse a function definition.
/// @param parser  The parser.
/// @return The AST node representing the function definition.
ast_node_t *parse_func_def(parser_t *parser);

/// @brief Parse a function call.
/// @param parser  The parser.
/// @return The AST node representing the function call.
ast_node_t *parse_func_call(parser_t *parser);

// TODO MORE PARSE FUNCS

/// @brief The top-level parsing entry which parses a program,
///        which consists of a series of
///        function definitions and statements.
/// @param parser  The parser.
/// @return The parsed program.
ast_node_t *parse_program(parser_t *parser);

#endif // ZLANG_PARSER_H