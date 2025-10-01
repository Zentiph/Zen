///
/// @file ast.h
/// @author Gavin Borne
/// @brief Abstract syntax tree and AST parsing header for the Zen programming language.
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

#ifndef ZLANG_AST_H
#define ZLANG_AST_H

#include "lexer/lexer.h"

/// @brief An enum of every type of AST node.
typedef enum
{
   AST_NUM,
   AST_ID,
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
typedef struct ast_node_s
{
   ASTNode type;

   union
   {
      struct
      {
         double val;
      } num;
      struct
      {
         char *name;
      } id;
      struct
      {
         struct ast_node_s *lhs;
         Token op;
         struct ast_node_s *rhs;
      } binary;
      struct
      {
         Token op;
         struct ast_node_s *operand;
      } unary;
      struct
      {
         struct ast_node_s *cond;
         struct ast_node_s *then_block;
         struct ast_node_s *else_block; // optional
      } if_stmt;
      struct
      {
         struct ast_node_s *cond;
         struct ast_node_s *body;
      } while_stmt;
      struct
      {
         char *name;
         char **params;
         int num_params;
         struct ast_node_s *body;
      } func_def;
      struct
      {
         struct ast_node_s *val; // can be NULL
      } return_stmt;
      struct
      {
         char *name;
         struct ast_node_s **args;
         int num_args;
      } func_call;
      struct
      {
         struct ast_node_s **stmts;
         int num_stmts;
      } block;
      struct
      {
         char *name;
         struct ast_node_s *val;
      } assign;
   };
} ast_node_t;

/// @brief Create a number AST node.
/// @param val The numeric value.
/// @return The AST node.
ast_node_t *ast_init_num(double val);

/// @brief Create an identifier AST node.
/// @param val The name.
/// @return The AST node.
ast_node_t *ast_init_id(char *name);

/// @brief Create a binary AST node.
/// @param lhs The lefthand side.
/// @param op  The operator to use.
/// @param rhs The righthand side.
/// @return The AST node.
ast_node_t *ast_init_binary(ast_node_t *lhs, Token op, ast_node_t *rhs);

/// @brief Create a unary AST node.
/// @param op      The operator to use.
/// @param operand The node to perform the operation on.
/// @return The AST node.
ast_node_t *ast_init_unary(Token op, ast_node_t *operand);

/// @brief Create an if statement AST node.
/// @param cond       The condition for the statement to run.
/// @param then_block The then block.
/// @param else_block The else block (optional).
/// @return The AST node.
ast_node_t *ast_init_if(ast_node_t *cond, ast_node_t *then_block,
                        ast_node_t *else_block);

/// @brief Create a while statement AST node.
/// @param cond The condition for the while statement to run.
/// @param body The body of the while statement.
/// @return The AST node.
ast_node_t *ast_init_while(ast_node_t *cond, ast_node_t *body);

/// @brief Create a function definition AST node.
/// @param name       The name of the function.
/// @param params     The names of the function parameters.
/// @param num_params The number of function parameters.
/// @param body       The body of the function.
/// @return The AST node.
ast_node_t *ast_init_func_def(char *name, char **params,
                              int num_params, ast_node_t *body);

/// @brief Create a return AST node.
/// @param val The value to return.
/// @return The AST node.
ast_node_t *ast_init_return(ast_node_t *val);

/// @brief Create a function call AST node.
/// @param name     The name of the function.
/// @param args     The values of the arguments passed to the function.
/// @param num_args The number of arguments passed to the function.
/// @return The AST node.
ast_node_t *ast_init_func_call(char *name, ast_node_t **args, int num_args);

/// @brief Create a code block AST node.
/// @param stmts     The statements in the block.
/// @param num_stmts The number of statements.
/// @return The AST node.
ast_node_t *ast_init_block(ast_node_t **stmts, int num_stmts);

/// @brief Create an assignment AST node.
/// @param name The name of the variable to assign to.
/// @param val  The value to assign to the variable.
/// @return The AST node.
ast_node_t *ast_init_assign(char *name, ast_node_t *val);

#endif // ZLANG_AST_H