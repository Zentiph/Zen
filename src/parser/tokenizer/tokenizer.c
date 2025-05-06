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

#include "tokenizer.h"

#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char BINARY_OPERATORS[] = {'+', '-', '*', '/', '%'};

/**
 * @brief Check if a char is in a char array.
 *
 * @param ch       Char
 * @param arr      Array
 * @param size     Size of arr
 * @return true  - If ch is in arr
 * @return false - If ch is not in arr
 */
bool _char_in_arr(char ch, const char *arr, int size)
{
   for (int i = 0; i < size; i++)
   {
      if (arr[i] == ch)
         return true;
   }
   return false;
}

/**
 * @brief Check if a token matches the given keyword.
 *
 * @param token    Token to check
 * @param keyword  Keyword to compare to
 * @return true  - If the token's value equals the keyword
 * @return false - Otherwise
 */
bool _token_matches_keyword(Token token, const char *keyword)
{
   return strcmp(token.value, keyword) == 0;
}

/**
 * @brief Convert a TokenType to a string.
 *
 * @param type   TokenType
 * @return char* String representation of the TokenType
 */
char *token_type_to_string(TokenType type)
{
   switch (type)
   {
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
   case TOKEN_EXTENDS:
      return "TOKEN_EXTENDS";

   case TOKEN_COMMENT:
      return "TOKEN_COMMENT";
   case TOKEN_IDENTIFIER:
      return "TOKEN_IDENTIFIER";
   case TOKEN_NUMBER:
      return "TOKEN_NUMBER";
   case TOKEN_ASSIGNMENT:
      return "TOKEN_ASSIGNMENT";
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
   case TOKEN_LT_ARROW_BRACK:
      return "TOKEN_LT_ARROW_BRACK";
   case TOKEN_RT_ARROW_BRACK:
      return "TOKEN_RT_ARROW_BRACK";
   case TOKEN_IP_BINARY_OP:
      return "TOKEN_IP_BINARY_OP";
   case TOKEN_COMPARISON_OP:
      return "TOKEN_COMPARISON_OP";
   case TOKEN_STRING:
      return "TOKEN_STRING";
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
   case TOKEN_DOT:
      return "TOKEN_DOT";
   case TOKEN_COMMA:
      return "TOKEN_COMMA";
   case TOKEN_ARROW:
      return "TOKEN_ARROW";
   case TOKEN_DBL_ARROW:
      return "TOKEN_DBL_ARROW";
   case TOKEN_NEWLINE:
      return "TOKEN_NEWLINE";
   case TOKEN_EOF:
      return "TOKEN_EOF";
   case TOKEN_INVALID:
      return "TOKEN_INVALID";
   }
   return "TOKEN_INVALID";
}

/**
 * @brief Skip the meaningless whitespace in the
 *        file until a token is found.
 *        Newline characters as well as semicolons
 *        are valid whitespace, so track those.
 *
 * @param fp         File pointer
 * @param ptr        Pointer to the current position in the buffer
 * @param buffer     The buffer containing file data
 * @param bytesRead  Pointer to the count of valid bytes in buffer
 * @param bufferSize The size of the buffer
 */
void skip_whitespace(FILE *fp, char **ptr, char *buffer, size_t *bytesRead, size_t bufferSize)
{
   while (true)
   {
      // Refill buffer if needed
      if (*ptr >= buffer + *bytesRead)
      {
         *bytesRead = fread(buffer, 1, bufferSize, fp);
         *ptr = buffer;
         if (*bytesRead == 0)
         { // EOF reached
            break;
         }
      }

      char ch = **ptr;
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
            (*ptr)++;
            if (*ptr >= buffer + *bytesRead)
            {
               *bytesRead = fread(buffer, 1, bufferSize, fp);
               *ptr = buffer;
               if (*bytesRead == 0)
               { // EOF reached
                  break;
               }
            }
            ch = **ptr;
         }
      }
      else
      {
         break;
      }
   }
}

// TODO
// If possible make helper funcs to prevent repetition (mainly for checking if buffer needs refill and refilling it)
/**
 * @brief Get the next token in the file.
 *
 * @param fp       File pointer
 * @return Token - The token found
 */
Token next_token(FILE *fp)
{
   static char buffer[BUFFER_SIZE];
   static char *ptr = buffer;
   static size_t bytesRead = 0;

   Token token;
   token.value[0] = '\0'; // Reset token value

   // Refill the buffer if needed and check for EOF
   if (ptr >= buffer + bytesRead)
   {
      bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
      ptr = buffer;
      if (bytesRead == 0)
      {
         token.type = TOKEN_EOF;
         return token;
      }
   }

   // Skip over whitespace in the buffer
   skip_whitespace(fp, &ptr, buffer, &bytesRead, BUFFER_SIZE);
   // Check if we skipped to the end of the buffer
   if (ptr >= buffer + bytesRead)
   {
      bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
      ptr = buffer;
      if (bytesRead == 0)
      {
         token.type = TOKEN_EOF;
         return token;
      }
   }

   // Read next char from the buffer
   char ch = *ptr++;

   // Handle single-line comments
   if (ch == '/' && ptr < buffer + bytesRead && *ptr == '/')
   {
      token.type = TOKEN_COMMENT;
      int i = 0;
      // Skip the second '/'
      ptr++;

      while (true)
      {
         // Refill the buffer as needed
         if (ptr >= buffer + bytesRead)
         {
            bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
            ptr = buffer;
            if (bytesRead == 0)
               break; // EOF reached
         }
         if (*ptr == NEWLINE_CHAR)
            break;
         token.value[i++] = *ptr++;
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }

      token.value[i] = '\0';
      return token;
   }

   // Handle multi-line comments (/. ... ./)
   if (ch == '/' && ptr < buffer + bytesRead && *ptr == '.')
   {
      token.type = TOKEN_COMMENT;
      int i = 0;
      // Skip the '.'
      ptr++;

      while (true)
      {
         // Refill the buffer as needed
         if (ptr >= buffer + bytesRead)
         {
            bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
            ptr = buffer;
            if (bytesRead == 0)
               break; // EOF reached
         }
         // End of comment
         if (*ptr == '.' && (ptr + 1 < buffer + bytesRead && *(ptr + 1) == '/'))
         {
            ptr += 2; // Skip the closing "./"
            break;
         }
         token.value[i++] = *ptr++;
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }
      token.value[i] = '\0';
      return token;
   }

   // Handle strings (outer quotes not included)
   if (ch == '"' || ch == '\'')
   {
      char quote = ch; // Keep quote type consistent
      token.type = TOKEN_STRING;
      int i = 0;

      while (true)
      {
         // Refill the buffer if needed
         if (ptr >= buffer + bytesRead)
         {
            bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
            ptr = buffer;
            if (bytesRead == 0)
               break; // EOF reached
         }

         // Check for escape coded quotes
         if (*ptr == '\\')
         {
            // Increment twice to skip a "\'" or "\""
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1)
               break;
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1)
               break;
         }
         else if (*ptr != quote)
         {
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1)
               break;
         }
         else
            break; // Break if we hit the end of the string
      }
      ptr++; // Move past final quote
      token.value[i] = '\0';
      return token;
   }

   // Handle keywords and identifiers
   // (First character of an identifier cannot be a digit)
   if (isalpha(ch) || ch == '_')
   {
      int i = 0;
      token.value[i++] = ch;

      while (isalnum(*ptr) || *ptr == '_')
      {
         // Refill the buffer as needed
         if (ptr >= buffer + bytesRead)
         {
            bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
            ptr = buffer;
            if (bytesRead == 0)
               break; // EOF reached
         }

         token.value[i++] = *ptr++;
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }
      token.value[i] = '\0';

      // Check for keywords
      if (_token_matches_keyword(token, "if"))
      {
         token.type = TOKEN_IF;
      }
      else if (_token_matches_keyword(token, "else"))
      {
         token.type = TOKEN_ELSE;
      }
      else if (_token_matches_keyword(token, "while"))
      {
         token.type = TOKEN_WHILE;
      }
      else if (_token_matches_keyword(token, "for"))
      {
         token.type = TOKEN_FOR;
      }
      else if (_token_matches_keyword(token, "fn"))
      {
         token.type = TOKEN_FN;
      }
      else if (_token_matches_keyword(token, "class"))
      {
         token.type = TOKEN_CLASS;
      }
      else if (_token_matches_keyword(token, "import"))
      {
         token.type = TOKEN_IMPORT;
      }
      else if (_token_matches_keyword(token, "from"))
      {
         token.type = TOKEN_FROM;
      }
      else if (_token_matches_keyword(token, "export"))
      {
         token.type = TOKEN_EXPORT;
      }
      else if (_token_matches_keyword(token, "module"))
      {
         token.type = TOKEN_MODULE;
      }
      else if (_token_matches_keyword(token, "and"))
      {
         token.type = TOKEN_AND;
      }
      else if (_token_matches_keyword(token, "or"))
      {
         token.type = TOKEN_OR;
      }
      else if (_token_matches_keyword(token, "not"))
      {
         token.type = TOKEN_NOT;
      }
      else if (_token_matches_keyword(token, "in"))
      {
         token.type = TOKEN_IN;
      }
      else if (_token_matches_keyword(token, "extends"))
      {
         token.type = TOKEN_EXTENDS;
      }
      else
      {
         token.type = TOKEN_IDENTIFIER;
      }

      return token;
   }

   // Handle numbers
   if (isdigit(ch))
   {
      token.type = TOKEN_NUMBER;
      int i = 0;
      token.value[i++] = ch;

      while (isdigit(*ptr))
      {
         // Refill the buffer as needed
         if (ptr >= buffer + bytesRead)
         {
            bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
            ptr = buffer;
            if (bytesRead == 0)
               break; // EOF reached
         }

         token.value[i++] = *ptr++;
         if (i >= MAX_TOKEN_LENGTH - 1)
            break;
      }

      token.value[i] = '\0';
      return token;
   }

   // Handle arrows
   if (ch == '-')
   {
      char next = *ptr++;
      if (next == '>')
      {
         token.type = TOKEN_ARROW;
         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }
      // If we didn't need the next char, decrement ptr
      ptr--;
   }
   if (ch == '=')
   {
      char next = *ptr++;
      if (next == '>')
      {
         token.type = TOKEN_DBL_ARROW;
         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }
      // If we didn't need the next char, decrement ptr
      *ptr--;
   }

   // Handle comparison operators
   if (ch == '=' || ch == '!' || ch == '<' || ch == '>')
   {
      char next = *ptr++;
      // '==' or '!=' or '<=' or '>='
      if (next == '=')
      {
         token.type = TOKEN_COMPARISON_OP;
         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }

      // If we didn't need the next char,
      // decrement the ptr
      *ptr--;
      // '<' or '>'
      if (ch == '<' || ch == '>')
      {
         token.type = TOKEN_COMPARISON_OP;
         token.value[0] = ch;
         token.value[1] = '\0';
         return token;
      }
      if (ch == '=')
      {
         token.type = TOKEN_ASSIGNMENT;
         token.value[0] = ch;
         token.value[1] = '\0';
         return token;
      }
   }

   // Handle in-place and regular binary operators
   // (all in-place binary operators are string of length 2)
   if (_char_in_arr(ch, BINARY_OPERATORS, sizeof BINARY_OPERATORS))
   {
      char next = *ptr++;
      // In-place binary op
      if (ch != '=' && next == '=')
      {
         token.type = TOKEN_IP_BINARY_OP;
         token.value[0] = ch;
         token.value[1] = next;
         token.value[2] = '\0';
         return token;
      }

      // If we didn't need the next char, decrement ptr
      *ptr--;

      // Regular binary op
      if (ch == '+')
      {
         token.type = TOKEN_ADD;
         token.value[0] = ch;
         token.value[2] = '\0';
      }
      else if (ch == '-')
      {
         token.type = TOKEN_SUB;
         token.value[0] = ch;
         token.value[2] = '\0';
      }
      else if (ch == '*')
      {
         token.type = TOKEN_MUL;
         token.value[0] = ch;
         token.value[2] = '\0';
      }
      else if (ch == '/')
      {
         token.type = TOKEN_DIV;
         token.value[0] = ch;
         token.value[2] = '\0';
      }
      else // if (ch == '%')
      {
         token.type = TOKEN_MOD;
         token.value[0] = ch;
         token.value[2] = '\0';
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

   if (ch == '<')
   {
      token.type = TOKEN_LT_ARROW_BRACK;
      token.value[0] = ch;
      token.value[1] = '\0';
      return token;
   }
   if (ch == '>')
   {
      token.type = TOKEN_RT_ARROW_BRACK;
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

/**
 * @brief Print a representation of a token.
 *
 * @param token Token to print
 */
void print_token(Token token)
{
   static const char *token_types[] = {
       // Make sure these align with the defined token
       // types in TokenType from tokenizer.h

       // Make these have equal length for readability
       "TOKEN_IF            ",
       "TOKEN_ELSE          ",
       "TOKEN_WHILE         ",
       "TOKEN_FOR           ",
       "TOKEN_FN            ",
       "TOKEN_CLASS         ",
       "TOKEN_IMPORT        ",
       "TOKEN_FROM          ",
       "TOKEN_EXPORT        ",
       "TOKEN_MODULE        ",
       "TOKEN_AND           ",
       "TOKEN_OR            ",
       "TOKEN_NOT           ",
       "TOKEN_IN            ",
       "TOKEN_EXTENDS       ",

       "TOKEN_COMMENT       ",
       "TOKEN_IDENTIFIER    ",
       "TOKEN_NUMBER        ",
       "TOKEN_ASSIGNMENT    ",
       "TOKEN_ADD           ",
       "TOKEN_SUB           ",
       "TOKEN_MUL           ",
       "TOKEN_DIV           ",
       "TOKEN_MOD           ",
       "TOKEN_LT_ARROW_BRACK",
       "TOKEN_RT_ARROW_BRACK",
       "TOKEN_IP_BINARY_OP  ",
       "TOKEN_COMPARISON_OP ",
       "TOKEN_LOGIC_OP      ",
       "TOKEN_STRING        ",
       "TOKEN_LT_PAREN      ",
       "TOKEN_RT_PAREN      ",
       "TOKEN_LT_BRACK      ",
       "TOKEN_RT_BRACK      ",
       "TOKEN_LT_CURLY      ",
       "TOKEN_RT_CURLY      ",
       "TOKEN_DOT           ",
       "TOKEN_COMMA         ",
       "TOKEN_ARROW         ",
       "TOKEN_DBL_ARROW     ",
       "TOKEN_NEWLINE       ",
       "TOKEN_EOF           ",
       "TOKEN_INVALID       "};

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