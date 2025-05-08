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
   AST_WHITE,
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

#endif // AST_H