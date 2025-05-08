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