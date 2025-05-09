/**
 * @file parser/tokenizer/token_repr.c
 * @author Gavin Borne
 * @brief Representation for tokens for the Zen programming language
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

#include "token_repr.h"

/**
 * @brief Convert a TokenType to a string.
 *
 * @param type           TokenType
 * @return const char* - String representation of the TokenType
 */
const char *token_type_to_string(TokenType type)
{
   switch (type)
   {
   // Keywords
   case TOKEN_IF:
      return "TOKEN_IF";
   case TOKEN_ELSE:
      return "TOKEN_ELSE";
   case TOKEN_WHILE:
      return "TOKEN_WHILE";
   case TOKEN_FOR:
      return "TOKEN_FOR";
   case TOKEN_FN:
      return "TOKEN_FN";
   case TOKEN_CLASS:
      return "TOKEN_CLASS";
   case TOKEN_EXTENDS:
      return "TOKEN_EXTENDS";
   case TOKEN_IMPORT:
      return "TOKEN_IMPORT";
   case TOKEN_FROM:
      return "TOKEN_FROM";
   case TOKEN_EXPORT:
      return "TOKEN_EXPORT";
   case TOKEN_MODULE:
      return "TOKEN_MODULE";
   case TOKEN_AND:
      return "TOKEN_AND";
   case TOKEN_OR:
      return "TOKEN_OR";
   case TOKEN_NOT:
      return "TOKEN_NOT";
   case TOKEN_IN:
      return "TOKEN_IN";
   case TOKEN_RETURN:
      return "TOKEN_RETURN";

   // Variable size tokens
   case TOKEN_COMMENT:
      return "TOKEN_COMMENT";
   case TOKEN_IDENTIFIER:
      return "TOKEN_IDENTIFIER";
   case TOKEN_NUMBER:
      return "TOKEN_NUMBER";
   case TOKEN_STRING:
      return "TOKEN_STRING";

   // Standard operators
   case TOKEN_ASSIGN:
      return "TOKEN_ASSIGN";
   case TOKEN_ADD:
      return "TOKEN_ADD";
   case TOKEN_SUB:
      return "TOKEN_SUB";
   case TOKEN_MUL:
      return "TOKEN_MUL";
   case TOKEN_DIV:
      return "TOKEN_DIV";
   case TOKEN_MOD:
      return "TOKEN_MOD";

   // In-place assignment
   case TOKEN_ADD_ASSIGN:
      return "TOKEN_ADD_ASSIGN";
   case TOKEN_SUB_ASSIGN:
      return "TOKEN_SUB_ASSIGN";
   case TOKEN_MUL_ASSIGN:
      return "TOKEN_MUL_ASSIGN";
   case TOKEN_DIV_ASSIGN:
      return "TOKEN_DIV_ASSIGN";
   case TOKEN_MOD_ASSIGN:
      return "TOKEN_MOD_ASSIGN";

   // Comparison
   case TOKEN_LT:
      return "TOKEN_LT";
   case TOKEN_GT:
      return "TOKEN_GT";
   case TOKEN_LE:
      return "TOKEN_LE";
   case TOKEN_GE:
      return "TOKEN_GE";
   case TOKEN_EQ:
      return "TOKEN_EQ";
   case TOKEN_NE:
      return "TOKEN_NE";

   // Containers
   case TOKEN_LT_PAREN:
      return "TOKEN_LT_PAREN";
   case TOKEN_RT_PAREN:
      return "TOKEN_RT_PAREN";
   case TOKEN_LT_BRACK:
      return "TOKEN_LT_BRACK";
   case TOKEN_RT_BRACK:
      return "TOKEN_RT_BRACK";
   case TOKEN_LT_CURLY:
      return "TOKEN_LT_CURLY";
   case TOKEN_RT_CURLY:
      return "TOKEN_RT_CURLY";

   // Arrows
   case TOKEN_ARROW:
      return "TOKEN_ARROW";
   case TOKEN_DBL_ARROW:
      return "TOKEN_DBL_ARROW";

   // Single chars
   case TOKEN_DOT:
      return "TOKEN_DOT";
   case TOKEN_COMMA:
      return "TOKEN_COMMA";
   case TOKEN_NEWLINE:
      return "TOKEN_NEWLINE";
   case TOKEN_EOF:
      return "TOKEN_EOF";

   // None of the above
   case TOKEN_INVALID:
      return "TOKEN_INVALID";

   default:
      fprintf(stderr, "Error: Unhandled TokenType value: %d\n", type);
      exit(EXIT_FAILURE);
   }
}

/**
 * @brief Print a representation of a token.
 *
 * @param token Token to print
 */
void print_token(Token token)
{
   static const char *token_types[] = {
       // Make sure the order aligns with the defined token
       // types in TokenType from tokenizer.h

       // Make these have equal length for readability
       "TOKEN_IF        ",
       "TOKEN_ELSE      ",
       "TOKEN_WHILE     ",
       "TOKEN_FOR       ",
       "TOKEN_FN        ",
       "TOKEN_CLASS     ",
       "TOKEN_EXTENDS   ",
       "TOKEN_IMPORT    ",
       "TOKEN_FROM      ",
       "TOKEN_EXPORT    ",
       "TOKEN_MODULE    ",
       "TOKEN_AND       ",
       "TOKEN_OR        ",
       "TOKEN_NOT       ",
       "TOKEN_IN        ",
       "TOKEN_RETURN   ",
       "TOKEN_COMMENT   ",
       "TOKEN_IDENTIFIER",
       "TOKEN_NUMBER    ",
       "TOKEN_STRING    ",
       "TOKEN_ASSIGN    ",
       "TOKEN_ADD       ",
       "TOKEN_SUB       ",
       "TOKEN_MUL       ",
       "TOKEN_DIV       ",
       "TOKEN_MOD       ",
       "TOKEN_ADD_ASSIGN",
       "TOKEN_SUB_ASSIGN",
       "TOKEN_MUL_ASSIGN",
       "TOKEN_DIV_ASSIGN",
       "TOKEN_MOD_ASSIGN",
       "TOKEN_LT        ",
       "TOKEN_GT        ",
       "TOKEN_LE        ",
       "TOKEN_GE        ",
       "TOKEN_EQ        ",
       "TOKEN_NE        ",
       "TOKEN_LT_PAREN  ",
       "TOKEN_RT_PAREN  ",
       "TOKEN_LT_BRACK  ",
       "TOKEN_RT_BRACK  ",
       "TOKEN_LT_CURLY  ",
       "TOKEN_RT_CURLY  ",
       "TOKEN_ARROW     ",
       "TOKEN_DBL_ARROW ",
       "TOKEN_DOT       ",
       "TOKEN_COMMA     ",
       "TOKEN_NEWLINE   ",
       "TOKEN_EOF       ",
       "TOKEN_INVALID   "};

   // Newline representation is "\\n"
   // (to prevent gaps in output)
   if (token.value[0] == '\n')
   {
      printf("%s: '%s'\n", token_types[token.type], "\\n");
   }
   else
   {
      printf("%s: '%s'\n", token_types[token.type], token.value);
   }
}