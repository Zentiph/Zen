/**
 * @file parser/parser_base.c
 * @author Gavin Borne
 * @brief Base parsing functionality for the Zen programming language
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

// TODO: UPDATE PRACTICES SO ALL VAR NAMES ARE snake_case

#include <stdbool.h>
#include <string.h>

#include "parser_base.h"

#include "../tokenizer/tokenizer_base.h"
#include "../tokenizer/token_repr.h"

Parser *create_parser(Tokenizer *tokenizer)
{
   Parser *parser = malloc(sizeof(parser));
   parser->tokenizer = tokenizer;
   parser->current = next_token(&parser->tokenizer);

   Token token;
   token.type = TOKEN_INVALID;
   token.value[0] = '\0';
   parser->previous = token;

   return parser;
}

/**
 * @brief Advance to the next token.
 *
 * @param parser Parser
 */
void advance(Parser *parser)
{
   parser->previous = parser->current;
   parser->current = next_token(&parser->tokenizer);
}

/**
 * @brief Check if the current token matches the given type.
 *        If so, advance and return true. Otherwise, return false.
 *
 * @param parser   Parser
 * @param type     Token type to check
 * @return true  - If the types match
 * @return false - Otherwise
 */
bool match(Parser *parser, TokenType type)
{
   if (parser->current.type == type)
   {
      advance(parser);
      return true;
   }
   return false;
}

/**
 * @brief Determine if the current token matches the given type without advancing.
 *
 * @param parser   Parser
 * @param type     Token type to check
 * @return true  - If the types match
 * @return false - Otherwise
 */
bool check(Parser *parser, TokenType type)
{
   return parser->current.type == type;
}

/**
 * @brief Ensure the current token matches the given type, then advance.
 *
 * @param parser  Parser
 * @param type    Token type to expect
 * @param message Error message
 */
void expect(Parser *parser, TokenType type, const char *message)
{
   if (parser->current.type != type)
   {
      fprintf(stderr, "Syntax error: %s (found %s)\n", message, token_type_to_string(parser->current.type));
      exit(EXIT_FAILURE);
   }
   advance(parser);
}

/**
 * @brief Peek at the next token.
 *
 * @param tokenizer Tokenizer to peek with
 * @return Token  - Next token
 */
Token peek_token(Tokenizer *tokenizer)
{
   TokenizerSnapshot *snapshot;
   save_tokenizer_state(tokenizer, snapshot);
   Token lookahead = next_token(tokenizer);
   load_tokenizer_state(tokenizer, snapshot);
   return lookahead;
}

/**
 * @brief Generate a syntax error message.
 *
 * @param message  Error message
 * @param filename Filename the error occurred in
 * @param line     Line number the error occurred at
 * @return char* - Error message
 */
char *_generate_error_message(const char *message, const char *filename, int line)
{
   size_t buffer_size = 512;
   char *error_message = malloc(buffer_size);
   if (!error_message)
      return NULL;

   snprintf(error_message, buffer_size, "Syntax error at %s:%d - %s", filename, line, message);
   return error_message;
}

/**
 * @brief Report a syntax error to the parser.
 *
 * @param parser  Parser
 * @param message Error message
 */
void error(Parser *parser, const char *message)
{
   // TODO: ADD BETTER HANDLING EVENTUALLY
   fprintf(stderr, _generate_error_message(message, parser->tokenizer->filename, parser->tokenizer->line));
}