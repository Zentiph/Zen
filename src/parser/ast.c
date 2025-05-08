/**
 * @file parser/ast.c
 * @author Gavin Borne
 * @brief Abstract syntax tree and AST parsing for the Zen programming language
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

#include <stdlib.h>

#include "ast.h"

/**
 * @brief Create a number node
 *
 * @param value       Number value
 * @return ASTNode* - Node
 */
ASTNode *create_number_node(float value)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_NUMBER;
   node->number.value = value;
   return node;
}

/**
 * @brief Create an identifier node
 *
 * @param name        Identifier name
 * @return ASTNode* - Node
 */
ASTNode *create_identifier_node(char *name)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_IDENTIFIER;
   node->identifier.name = name;
   return node;
}

/**
 * @brief Create a binary node
 *
 * @param left        Node to the left of the operator
 * @param op          Binary operator (e.g. TOKEN_PLUS)
 * @param right       Node to the right of the operator
 * @return ASTNode* - Node
 */
ASTNode *create_binary_node(ASTNode *left, TokenType op, ASTNode *right)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_BINARY;
   node->binary.left = left;
   node->binary.op = op;
   node->binary.right = right;
   return node;
}

/**
 * @brief Create a unary node
 *
 * @param op          Unary operator (e.g. TOKEN_MINUS)
 * @param operand     Node to perform the operation on
 * @return ASTNode* - Node
 */
ASTNode *create_unary_node(TokenType op, ASTNode *operand)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_UNARY;
   node->unary.op = op;
   node->unary.operand = operand;
   return node;
}

/**
 * @brief Create an if statement node
 *
 * @param condition   The condition for the if block to execute
 * @param then_branch Node containing the then branch code
 * @param else_branch Node containing the else branch code
 * @return ASTNode* - Node
 */
ASTNode *create_if_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_IF;
   node->if_stmt.condition = condition;
   node->if_stmt.then_branch = then_branch;
   node->if_stmt.else_branch = else_branch;
   return node;
}

/**
 * @brief Create a block node
 *
 * @param statements      Statements in the block
 * @param statement_count Number of statements in the block
 * @return ASTNode*     - Node
 */
ASTNode *create_block_node(ASTNode **statements, int statement_count)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_BLOCK;
   node->block.statements = statements;
   node->block.statement_count = statement_count;
   return node;
}

/**
 * @brief Create a function definition node
 *
 * @param name        Function name
 * @param params      Function parameters
 * @param param_count Number of parameters in the function
 * @param body        The code inside the function
 * @return ASTNode* - Node
 */
ASTNode *create_function_def_node(char *name, char **params, int param_count, ASTNode *body)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_FUNCTION_DEF;
   node->function_def.name = name;
   node->function_def.params = params;
   node->function_def.param_count = param_count;
   node->function_def.body = body;
   return node;
}

/**
 * @brief Create a function call
 *
 * @param name        Name of the function
 * @param args        Arguments passed to the function
 * @param arg_count   Number of args passed to the function
 * @return ASTNode* - Node
 */
ASTNode *create_function_call_node(char *name, ASTNode **args, int arg_count)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_FUNCTION_CALL;
   node->function_call.name = name;
   node->function_call.args = args;
   node->function_call.arg_count = arg_count;
   return node;
}

/**
 * @brief Create a assignment node
 *
 * @param var_name    The name of the variable
 * @param value       The value to assign
 * @return ASTNode* - Node
 */
ASTNode *create_assignment_node(char *var_name, ASTNode *value)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_ASSIGNMENT;
   node->assignment.var_name = var_name;
   node->assignment.value = value;
   return node;
}

/**
 * @brief Create a return node
 *
 * @param value       Value to return
 * @return ASTNode* - Node
 */
ASTNode *create_return_node(ASTNode *value)
{
   ASTNode *node = malloc(sizeof(ASTNode));
   node->type = AST_RETURN;
   node->return_stmt.value = value;
   return node;
}