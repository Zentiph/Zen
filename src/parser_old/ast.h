/**
 * @file parser/ast.h
 * @author Gavin Borne
 * @brief Abstract syntax tree and AST parsing header for the Zen programming language
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef AST_H
#define AST_H

#include "../tokenizer/tokenizer.h"

/**
 * @enum  ASTNodeType
 * @brief All AST node types.
 */
typedef enum
{
   AST_NUMBER,
   AST_IDENTIFIER,
   AST_BINARY,
   AST_UNARY,
   AST_IF,
   AST_BLOCK,
   AST_WHILE,
   AST_FUNCTION_DEF,
   AST_FUNCTION_CALL,
   AST_VAR_DECL,
   AST_ASSIGNMENT,
   AST_RETURN
} ASTNodeType;

/**
 * @brief An AST node, that stores a type and the data related with that type.
 */
typedef struct ASTNode
{
   ASTNodeType type;

   union
   {
      struct
      {
         double value;
      } number;

      struct
      {
         char *name;
      } identifier;

      struct
      {
         struct ASTNode *left;
         TokenType op;
         struct ASTNode *right;
      } binary;

      struct
      {
         TokenType op;
         struct ASTNode *operand;
      } unary;

      struct
      {
         struct ASTNode *condition;
         struct ASTNode *then_branch;
         struct ASTNode *else_branch; // Optional
      } if_stmt;

      struct
      {
         struct ASTNode **statements;
         int statement_count;
      } block;

      struct
      {
         struct ASTNode *condition;
         struct ASTNode *body;
      } while_stmt;

      struct
      {
         char *name;
         char **params;
         int param_count;
         struct ASTNode *body;
      } function_def;

      struct
      {
         char *name;
         struct ASTNode **args;
         int arg_count;
      } function_call;

      struct
      {
         char *var_name;
         struct ASTNode *value;
      } assignment;

      struct
      {
         struct ASTNode *value; // Can be NULL
      } return_stmt;
   };
} ASTNode;

/**
 * @brief Create a number node
 *
 * @param value       Number value
 * @return ASTNode* - Node
 */
ASTNode *create_number_node(float value);

/**
 * @brief Create an identifier node
 *
 * @param name        Identifier name
 * @return ASTNode* - Node
 */
ASTNode *create_identifier_node(char *name);

/**
 * @brief Create a binary node
 *
 * @param left        Node to the left of the operator
 * @param op          Binary operator (e.g. TOKEN_PLUS)
 * @param right       Node to the right of the operator
 * @return ASTNode* - Node
 */
ASTNode *create_binary_node(ASTNode *left, TokenType op, ASTNode *right);

/**
 * @brief Create a unary node
 *
 * @param op          Unary operator (e.g. TOKEN_MINUS)
 * @param operand     Node to perform the operation on
 * @return ASTNode* - Node
 */
ASTNode *create_unary_node(TokenType op, ASTNode *operand);

/**
 * @brief Create an if statement node
 *
 * @param condition   The condition for the if block to execute
 * @param then_branch Node containing the then branch code
 * @param else_branch Node containing the else branch code
 * @return ASTNode* - Node
 */
ASTNode *create_if_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);

/**
 * @brief Create a while node
 *
 * @param condition   Condition for the while block to execute
 * @param body        Body of the while block
 * @return ASTNode* - Node
 */
ASTNode *create_while_node(ASTNode *condition, ASTNode *body);

/**
 * @brief Create a block node
 *
 * @param statements      Statements in the block
 * @param statement_count Number of statements in the block
 * @return ASTNode*     - Node
 */
ASTNode *create_block_node(ASTNode **statements, int statement_count);

/**
 * @brief Create a function definition node
 *
 * @param name        Function name
 * @param params      Function parameters
 * @param param_count Number of parameters in the function
 * @param body        The code inside the function
 * @return ASTNode* - Node
 */
ASTNode *create_function_def_node(char *name, char **params, int param_count, ASTNode *body);

/**
 * @brief Create a function call
 *
 * @param name        Name of the function
 * @param args        Arguments passed to the function
 * @param arg_count   Number of args passed to the function
 * @return ASTNode* - Node
 */
ASTNode *create_function_call_node(char *name, ASTNode **args, int arg_count);

/**
 * @brief Create a assignment node
 *
 * @param var_name    The name of the variable
 * @param value       The value to assign
 * @return ASTNode* - Node
 */
ASTNode *create_assignment_node(char *var_name, ASTNode *value);

/**
 * @brief Create a return node
 *
 * @param value       Value to return
 * @return ASTNode* - Node
 */
ASTNode *create_return_node(ASTNode *value);

#endif // AST_H