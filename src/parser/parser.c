/**
 * @file parser/parser.c
 * @author Gavin Borne
 * @brief Parser for the Zen programming language
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

#include <string.h>

#include "parser.h"

#include "parser_base.h"

ASTNode *parse_primary(Parser *parser)
{
   if (parser->current.type == TOKEN_NUMBER)
   {
      double value = atof(parser->current.value);
      advance(parser);
      return create_number_node(value);
   }
   else if (parser->current.type == TOKEN_IDENTIFIER)
   {
      char *name = strdup(parser->current.value);
      advance(parser);
      return create_identifier_node(name); // TODO: IMPLEMENT THIS FUNCTION
   }
   perror("Expected expression");
   exit(EXIT_FAILURE);
}