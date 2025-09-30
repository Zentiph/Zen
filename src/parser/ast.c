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

ast_node_t *ast_init_num(float val)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_NUM;
   node->num.val = val;
   return node;
}

ast_node_t *ast_init_id(char *name)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_ID;
   node->id.name = name;
   return node;
}

ast_node_t *ast_init_binary(ast_node_t *lhs, Token op, ast_node_t *rhs)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_BINARY;
   node->binary.lhs = lhs;
   node->binary.op = op;
   node->binary.rhs = rhs;
   return node;
}

ast_node_t *ast_init_unary(Token op, ast_node_t *operand)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_UNARY;
   node->unary.op = op;
   node->unary.operand = operand;
   return node;
}

ast_node_t *ast_init_if(ast_node_t *cond, ast_node_t *then_block, ast_node_t *else_block)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_IF;
   node->if_stmt.cond = cond;
   node->if_stmt.then_block = then_block;
   node->if_stmt.else_block = else_block;
   return node;
}

ast_node_t *ast_init_while(ast_node_t *cond, ast_node_t *body)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_WHILE;
   node->while_stmt.cond = cond;
   node->while_stmt.body = body;
   return node;
}

ast_node_t *ast_init_func_def(char *name, char **params,
                              int num_params, ast_node_t *body)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_FUNC_DEF;
   node->func_def.name = name;
   node->func_def.params = params;
   node->func_def.num_params = num_params;
   node->func_def.body = body;
   return node;
}

ast_node_t *ast_init_return(ast_node_t *val)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_RETURN;
   node->return_stmt.val = val;
   return node;
}

ast_node_t *ast_init_func_call(char *name, ast_node_t **args, int num_args)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_FUNC_CALL;
   node->func_call.name = name;
   node->func_call.args = args;
   node->func_call.num_args = num_args;
   return node;
}

ast_node_t *ast_init_block(ast_node_t **stmts, int num_stmts)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_BLOCK;
   node->block.stmts = stmts;
   node->block.num_stmts = num_stmts;
   return node;
}

ast_node_t *ast_init_assign(char *name, ast_node_t *val)
{
   ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
   node->type = AST_ASSIGN;
   node->assign.name = name;
   node->assign.val = val;
   return node;
}