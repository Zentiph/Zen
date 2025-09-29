///
/// @file ast.c
/// @author Gavin Borne
/// @brief Abstract syntax tree and AST parsing for the Zen programming language.
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

#include <stdlib.h>

#include "ast.h"

ASTNode *create_num_node(float val)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_NUM;
   node->num.val = val;
   return node;
}

ASTNode *create_id_node(char *name)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_ID;
   node->id.name = name;
   return node;
}

ASTNode *create_binary_node(ASTNode *lhs, TokenType op, ASTNode *rhs)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_BINARY;
   node->binary.lhs = lhs;
   node->binary.op = op;
   node->binary.rhs = rhs;
   return node;
}

ASTNode *create_unary_node(TokenType op, ASTNode *operand)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_UNARY;
   node->unary.op = op;
   node->unary.operand = operand;
   return node;
}

ASTNode *create_if_node(ASTNode *cond, ASTNode *then_block, ASTNode *else_block)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_IF;
   node->if_stmt.cond = cond;
   node->if_stmt.then_block = then_block;
   node->if_stmt.else_block = else_block;
   return node;
}

ASTNode *create_while_node(ASTNode *cond, ASTNode *body)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_WHILE;
   node->while_stmt.cond = cond;
   node->while_stmt.body = body;
   return node;
}

ASTNode *create_func_def_node(char *name, char **params,
                              int num_params, ASTNode *body)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_FUNC_DEF;
   node->func_def.name = name;
   node->func_def.params = params;
   node->func_def.num_params = num_params;
   node->func_def.body = body;
   return node;
}

ASTNode *create_return_node(ASTNode *val)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_RETURN;
   node->return_stmt.val = val;
   return node;
}

ASTNode *create_func_call_node(char *name, ASTNode **args, int num_args)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_FUNC_CALL;
   node->func_call.name = name;
   node->func_call.args = args;
   node->func_call.num_args = num_args;
   return node;
}

ASTNode *create_block_node(ASTNode **stmts, int num_stmts)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_BLOCK;
   node->block.stmts = stmts;
   node->block.num_stmts = num_stmts;
   return node;
}

ASTNode *create_assign_node(char *name, ASTNode *val)
{
   ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
   node->type = AST_ASSIGN;
   node->assign.name = name;
   node->assign.val = val;
   return node;
}