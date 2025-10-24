///
/// @file ast.h
/// @author Gavin Borne
/// @brief Abstract syntax tree and AST parsing specification header for the Zen
///        programming language.
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

#include "lexer/lexer.h"

/// An enum of every type of AST node.
typedef enum {
   AST_NUMBER,
   AST_IDENTIFIER,
   AST_BINARY,
   AST_UNARY,
   AST_IF,
   AST_WHILE,
   AST_FUNC_DEF,
   AST_RETURN,
   AST_FUNC_CALL,
   AST_BLOCK,
   AST_VAR_DECL,
   AST_ASSIGN,
} ASTNode;

/// @brief An AST node that stores an ASTNode and the data related to that type.
typedef struct ast_node_s *ast_node_t;

///
/// @brief Create a number AST node.
///
/// @param val         - The numeric value.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_number(double val);

///
/// @brief Create an identifier AST node.
///
/// @param name        - The name.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_identifier(char *name);

///
/// @brief Create a binary AST node.
///
/// @param lhs         - The lefthand side.
/// @param op          - The operator to use.
/// @param rhs         - The righthand side.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_binary(ast_node_t lhs, Token op, ast_node_t rhs);

///
/// @brief Create a unary AST node.
///
/// @param op          - The operator to use.
/// @param operand     - The node to perform the operation on.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_unary(Token op, ast_node_t operand);

///
/// @brief Create an if statement AST node.
///
/// @param cond        - The condition for the statement to run.
/// @param then_block  - The then block.
/// @param else_block  - The else block (optional).
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_if(ast_node_t cond, ast_node_t then_block,
                         ast_node_t else_block);

///
/// @brief Create a while statement AST node.
///
/// @param cond        - The condition for the while statement to run.
/// @param body        - The body of the while statement.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_while(ast_node_t cond, ast_node_t body);

///
/// @brief Create a function definition AST node.
///
/// @param name        - The name of the function.
/// @param params      - The names of the function parameters.
/// @param num_params  - The number of function parameters.
/// @param body        - The body of the function.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_func_def(char *name, char **params, int num_params,
                               ast_node_t body);

///
/// @brief Create a return AST node.
///
/// @param val         - The value to return.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_return(ast_node_t val);

///
/// @brief Create a function call AST node.
///
/// @param name        - The name of the function.
/// @param args        - The values of the arguments passed to the function.
/// @param num_args    - The number of arguments passed to the function.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_func_call(char *name, ast_node_t *args, int num_args);

///
/// @brief Create a code block AST node.
///
/// @param stmts       - The statements in the block.
/// @param num_stmts   - The number of statements.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_block(ast_node_t *stmts, int num_stmts);

///
/// @brief Create an assignment AST node.
///
/// @param name        - The name of the variable to assign to.
/// @param val         - The value to assign to the variable.
/// @return ast_node_t - The AST node.
///
ast_node_t ast_create_assign(char *name, ast_node_t val);
