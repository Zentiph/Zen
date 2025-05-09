/**
 * @file parser/parser.h
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

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "parser_base.h"

/**
 * @brief Parse a primary expression
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_primary(Parser *parser);

/**
 * @brief Parse a unary expression, like '-' or 'not'
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_unary(Parser *parser);

/**
 * @brief Parse a factor (*, /, %).
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_factor(Parser *parser);

/**
 * @brief Parse a term (+, -).
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_term(Parser *parser);

/**
 * @brief Parse a comparison (==, !=, <, >, <=, >=)
 *
 * @param parser
 * @return ASTNode*
 */
ASTNode *parse_comparison(Parser *parser);

/**
 * @brief Parse a logical expression (and, or)
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_logic(Parser *parser);

// NOTE: Remove this at some point, if desired.

/**
 * @brief Parse an expression. This is a top-level expression entrypoint,
 *        but is identical to parse_logic().
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_expression(Parser *parser);

/**
 * @brief Parse a statement (return, assignment, or regular expression).
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_statement(Parser *parser);

/**
 * @brief Parse a code block.
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_block(Parser *parser);

/**
 * @brief Parse an if block.
 *
 * @param parser      Parser
 * @return ASTNode* - Node
 */
ASTNode *parse_if(Parser *parser);

/**
 * @brief Parse a while block.
 */
ASTNode *parse_while(Parser *parser);

/**
 * @brief Parse a function definition
 */
ASTNode *parse_function_def(Parser *parser);

// TODO: IMPLEMENT MORE PARSE FUNCTIONS. USE EBNF/SYNTAX.MD TO HELP GUIDE

/**
 * @brief The top-level parsing entry:
 *        a program, which consists of a series of
 *        function definitions and statements.
 */
ASTNode *parse_program(Parser *parser);

#endif // PARSER_H