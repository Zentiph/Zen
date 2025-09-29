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

#ifndef AST_H
#define AST_H

#include "lexer/lexer.h"

/// @brief An enum of every type of ASTNode.
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
} ASTNodeType;

/// @brief An AST node that stores an ASTNodeType and the data related to that type.
typedef struct ASTNode
{
   ASTNodeType type;

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
         struct ASTNode *lhs;
         TokenType op;
         struct ASTNode *rhs;
      } binary;
      struct
      {
         TokenType op;
         struct ASTNode *operand;
      } unary;
      struct
      {
         struct ASTNode *cond;
         struct ASTNode *then_block;
         struct ASTNode *else_block; // optional
      } if_stmt;
      struct
      {
         struct ASTNode *cond;
         struct ASTNode *body;
      } while_stmt;
      struct
      {
         char *name;
         char **params;
         int num_params;
         struct ASTNode *body;
      } func_def;
      struct
      {
         struct ASTNode *val; // can be NULL
      } return_stmt;
      struct
      {
         char *name;
         struct ASTNode **args;
         int num_args;
      } func_call;
      struct
      {
         struct ASTNode **stmts;
         int num_stmts;
      } block;
      struct
      {
         char *name;
         struct ASTNode *val;
      } assign;
   };
} ASTNode;

/// @brief Create a number ASTNode.
/// @param val The numeric value.
/// @return The ASTNode.
ASTNode *create_num_node(float val);

/// @brief Create an identifier ASTNode.
/// @param val The name.
/// @return The ASTNode.
ASTNode *create_id_node(char *name);

/// @brief Create a binary ASTNode.
/// @param lhs The lefthand side.
/// @param op  The operator to use.
/// @param rhs The righthand side.
/// @return The ASTNode.
ASTNode *create_binary_node(ASTNode *lhs, TokenType op, ASTNode *rhs);

/// @brief Create a unary ASTNode.
/// @param op      The operator to use.
/// @param operand The node to perform the operation on.
/// @return The ASTNode.
ASTNode *create_unary_node(TokenType op, ASTNode *operand);

/// @brief Create an if statement ASTNode.
/// @param cond       The condition for the statement to run.
/// @param then_block The then block.
/// @param else_block The else block (optional).
/// @return The ASTNode.
ASTNode *create_if_node(ASTNode *cond, ASTNode *then_block, ASTNode *else_block);

/// @brief Create a while statement ASTNode.
/// @param cond The condition for the while statement to run.
/// @param body The body of the while statement.
/// @return The ASTNode.
ASTNode *create_while_node(ASTNode *cond, ASTNode *body);

/// @brief Create a function definition ASTNode.
/// @param name       The name of the function.
/// @param params     The names of the function parameters.
/// @param num_params The number of function parameters.
/// @param body       The body of the function.
/// @return The ASTNode.
ASTNode *create_func_def_node(char *name, char **params,
                              int num_params, ASTNode *body);

/// @brief Create a return ASTNode.
/// @param val The value to return.
/// @return The ASTNode.
ASTNode *create_return_node(ASTNode *val);

/// @brief Create a function call ASTNode.
/// @param name     The name of the function.
/// @param args     The values of the arguments passed to the function.
/// @param num_args The number of arguments passed to the function.
/// @return The ASTNode.
ASTNode *create_func_call_node(char *name, ASTNode **args, int num_args);

/// @brief Create a code block ASTNode.
/// @param stmts     The statements in the block.
/// @param num_stmts The number of statements.
/// @return The ASTNode.
ASTNode *create_block_node(ASTNode **stmts, int num_stmts);

/// @brief Create an assignment ASTNode.
/// @param name The name of the variable to assign to.
/// @param val  The value to assign to the variable.
/// @return The ASTNode.
ASTNode *create_assign_node(char *name, ASTNode *val);

#endif // AST_H