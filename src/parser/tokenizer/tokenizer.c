/**
 * @file tokenizer.c
 * @author Gavin Borne
 * @brief Tokenizer for the Zen programming language
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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "tokenizer.h"

/**
 * @brief Representation of a keyword token.
 */
typedef struct
{
   const char *keyword;
   TokenType type;
} Keyword;

static const Keyword keyword_table[] = {
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"in", TOKEN_IN},
    {"fn", TOKEN_FN},
    {"class", TOKEN_CLASS},
    {"extends", TOKEN_EXTENDS},
    {"import", TOKEN_IMPORT},
    {"from", TOKEN_FROM},
    {"export", TOKEN_EXPORT},
    {"module", TOKEN_MODULE},
    {"and", TOKEN_AND},
    {"or", TOKEN_OR},
    {"not", TOKEN_NOT},
    {NULL, TOKEN_INVALID}};

/**
 * @brief Skip the meaningless whitespace in the
 *        file until a token is found.
 *        Newline characters as well as semicolons
 *        are valid whitespace, so track those.
 *
 * @param fp    File pointer
 * @param state Tokenizer state
 */
void skip_whitespace(FILE *fp, TokenizerState *state)
{
   while (true)
   {
      if (buffer_full(state))
      {
         if (refill_buffer(fp, state) == 0)
         { // EOF reached
            break;
         }
      }

      char ch = current_char(state);
      if (isspace(ch))
      {
         if (ch == NEWLINE_CHAR)
         {
            // Preserve newline as a token, don't skip
            break;
         }
         // Otherwise skip the whitespace
         while (isspace(ch) && ch != NEWLINE_CHAR)
         {
            move_pointer(state, 1);
            if (buffer_full(state))
            {
               if (refill_buffer(fp, state) == 0)
               { // EOF reached
                  break;
               }
            }
            ch = current_char(state);
         }
      }
      else
      {
         break;
      }
   }
}

/**
 * @brief Get the next token in the file.
 *
 * @param fp       File pointer
 * @param state    Tokenizer state
 * @return Token - The token found
 */
Token next_token(FILE *fp, TokenizerState *state)
{
   Token token;
   token.value[0] = '\0'; // Reset token value

   if (buffer_full(state))
   {
      if (refill_buffer(fp, state) == 0)
      { // No new data read, reached EOF
         token.type = TOKEN_EOF;
         return token;
      }
   }

   skip_whitespace(fp, state);

   if (buffer_full(state))
   {
      if (refill_buffer(fp, state) == 0)
      {
         token.type = TOKEN_EOF;
         return token;
      }
   }

   char ch = advance(state);

   // Handle single-line comments
   if (ch == '/' && !buffer_full(state) && current_char(state) == '/')
   {
      token.type = TOKEN_COMMENT;
      int i = 0;
      // Skip the second '/'
      move_pointer(state, 1);

      while (true)
      {
         // Refill the buffer as needed
         if (buffer_full(state))
         {
            if (refill_buffer(fp, state) == 0)
               break; // EOF reached
         }

         if (current_char(state) == NEWLINE_CHAR)
            break;

         token.value[i++] = advance(state);
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }

      token.value[i] = '\0';
      return token;
   }

   // Handle multi-line comments (/. ... ./)
   if (ch == '/' && !buffer_full(state) && current_char(state) == '.')
   {
      token.type = TOKEN_COMMENT;
      int i = 0;
      // Skip the '.'
      move_pointer(state, 1);

      while (true)
      {
         // Refill the buffer as needed
         if (buffer_full(state))
         {
            if (refill_buffer(fp, state) == 0)
               break; // EOF reached
         }
         // End of comment
         if (current_char(state) == '.' && (next_char_in_bounds(state) && peek(state) == '/'))
         {
            move_pointer(state, 2); // Skip the closing "./"
            break;
         }
         token.value[i++] = advance(state);
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }
      token.value[i] = '\0';
      return token;
   }

   // Handle strings (outer quotes not included in token)
   if (ch == '"' || ch == '\'')
   {
      char quote = ch; // Keep quote type consistent
      token.type = TOKEN_STRING;
      int i = 0;

      while (true)
      {
         // Refill the buffer if needed
         if (buffer_full(state))
         {
            if (refill_buffer(fp, state) == 0)
               break; // EOF reached
         }

         if (current_char(state) == '\\')
         {
            // Handle escape sequences
            move_pointer(state, 1); // Skip the backslash
            if (buffer_full(state))
            {
               if (refill_buffer(fp, state) == 0)
                  break; // EOF reached
            }

            // Append the escaped character if valid
            char escaped_char = advance(state);
            switch (escaped_char)
            {
            case 'n':
               token.value[i++] = '\n';
               break;
            case 't':
               token.value[i++] = '\t';
               break;
            case '\\':
               token.value[i++] = '\\';
               break;
            case '"':
               token.value[i++] = '"';
               break;
            case '\'':
               token.value[i++] = '\'';
               break;
            default:
               // Handle invalid escape sequences by appending as-is
               token.value[i++] = '\\';
               token.value[i++] = escaped_char;
               break;
            }

            if (i >= MAX_TOKEN_LENGTH - 1)
               break;
         }
         else if (current_char(state) != quote)
         {
            token.value[i++] = advance(state);
            if (i >= MAX_TOKEN_LENGTH - 1)
               break;
         }
         else
            break; // Break if we hit the end of the string
      }
      move_pointer(state, 1); // Move past final quote
      token.value[i] = '\0';
      return token;
   }

   // Handle keywords and identifiers
   // (First character of an identifier cannot be a digit)
   if (isalpha(ch) || ch == '_')
   {
      int i = 0;
      token.value[i++] = ch;

      while (isalnum(current_char(state)) || current_char(state) == '_')
      {
         // Refill the buffer as needed
         if (buffer_full(state))
         {
            if (refill_buffer(fp, state) == 0)
               break; // EOF reached
         }

         token.value[i++] = advance(state);
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }
      token.value[i] = '\0';

      // Check for keywords
      for (int i = 0; keyword_table[i].keyword != NULL; i++)
      {
         if (strcmp(token.value, keyword_table[i].keyword) == 0)
         {
            token.type = keyword_table[i].type;
            return token;
         }
      }

      token.type = TOKEN_IDENTIFIER;
      return token;
   }

   // Handle numbers
   if (isdigit(ch))
   {
      token.type = TOKEN_NUMBER;
      int i = 0;
      token.value[i++] = ch;

      while (isdigit(current_char(state)))
      {
         // Refill the buffer as needed
         if (buffer_full(state))
         {
            if (refill_buffer(fp, state) == 0)
               break; // EOF reached
         }

         token.value[i++] = advance(state);
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }

      token.value[i] = '\0';
      return token;
   }

   // Handle arrows
   if (ch == '-')
   {
      char next = advance(state);
      if (next == '>')
      {
         token.type = TOKEN_ARROW;
         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }
      // If we didn't need the next char, go back
      move_pointer(state, -1);
   }
   if (ch == '=')
   {
      char next = advance(state);
      if (next == '>')
      {
         token.type = TOKEN_DBL_ARROW;
         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }
      // If we didn't need the next char, decrement ptr
      move_pointer(state, -1);
   }

   // Handle comparison operators
   if (ch == '=' || ch == '!' || ch == '<' || ch == '>')
   {
      char next = advance(state);
      // '==' or '!=' or '<=' or '>='
      if (next == '=')
      {
         switch (ch)
         {
         case '=':
            token.type = TOKEN_EQ;
            break;
         case '!':
            token.type = TOKEN_NE;
            break;
         case '<':
            token.type = TOKEN_LE;
            break;
         case '>':
            token.type = TOKEN_GE;
            break;

         default:
            fprintf(stderr, "Error: Invalid token: '%c%c'", ch, next);
            exit(EXIT_FAILURE);
         }

         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }

      // If we didn't need the next char,
      // decrement the ptr
      move_pointer(state, -1);
      // '<', '>', or '='
      switch (ch)
      {
      case '<':
         token.type = TOKEN_LT;
         break;
      case '>':
         token.type = TOKEN_GT;
         break;
      case '=':
         token.type = TOKEN_ASSIGN;
         break;

      default:
         fprintf(stderr, "Error: Invalid operator: '%c'", ch);
         exit(EXIT_FAILURE);
      }

      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }

   // Handle in-place and regular binary operators
   // (all in-place binary operators are string of length 2)
   if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%')
   {
      char next = advance(state);
      // In-place binary op
      if (next == '=')
      {
         switch (ch)
         {
         case '+':
            token.type = TOKEN_ADD_ASSIGN;
            break;
         case '-':
            token.type = TOKEN_ADD_ASSIGN;
            break;
         case '*':
            token.type = TOKEN_ADD_ASSIGN;
            break;
         case '/':
            token.type = TOKEN_ADD_ASSIGN;
            break;
         case '%':
            token.type = TOKEN_ADD_ASSIGN;
            break;

         default:
            fprintf(stderr, "Error: Undefined in-place binary operator token '%c%c'", ch, next);
            exit(EXIT_FAILURE);
         }

         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }

      // If we didn't need the next char, decrement ptr
      move_pointer(state, -1);

      // Regular binary op
      if (ch == '+')
      {
         token.type = TOKEN_ADD;
         token.value[0] = ch;
         token.value[1] = '\0';
      }
      else if (ch == '-')
      {
         token.type = TOKEN_SUB;
         token.value[0] = ch;
         token.value[1] = '\0';
      }
      else if (ch == '*')
      {
         token.type = TOKEN_MUL;
         token.value[0] = ch;
         token.value[1] = '\0';
      }
      else if (ch == '/')
      {
         token.type = TOKEN_DIV;
         token.value[0] = ch;
         token.value[1] = '\0';
      }
      else // if (ch == '%')
      {
         token.type = TOKEN_MOD;
         token.value[0] = ch;
         token.value[1] = '\0';
      }

      return token;
   }

   // Handle single characters
   if (ch == '.')
   {
      token.type = TOKEN_DOT;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }

   if (ch == ',')
   {
      token.type = TOKEN_COMMA;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }

   if (ch == '(')
   {
      token.type = TOKEN_LT_PAREN;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }
   if (ch == ')')
   {
      token.type = TOKEN_RT_PAREN;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }
   if (ch == '[')
   {
      token.type = TOKEN_LT_BRACK;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }
   if (ch == ']')
   {
      token.type = TOKEN_RT_BRACK;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }
   if (ch == '{')
   {
      token.type = TOKEN_LT_CURLY;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }
   if (ch == '}')
   {
      token.type = TOKEN_RT_CURLY;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }

   // Handle new code lines
   if (ch == '\n' || ch == ';')
   {
      token.type = TOKEN_NEWLINE;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }

   // If none of the above, the token is invalid
   token.type = TOKEN_INVALID;
   token.value[0] = ch;
   token.value[1] = '\0';
   return token;
}