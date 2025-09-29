///
/// @file parser.c
/// @author Gavin Borne
/// @brief Parsing functionality for the Zen programming language.
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

#include "parser.h"

// ----- internals -----
char *__generate_err_msg(const char *msg, const char *filename, int line)
{
   size_t buf_size = 512;
   char *err_msg = (char *)malloc(buf_size);
   if (!err_msg)
      return NULL;

   snprintf(err_msg, buf_size, "Syntax error at %s:%d - %s", filename, line, msg);
   return err_msg;
}

// ----- public api -----

Parser *init_parser(Lexer *lex)
{
   Parser *parser = (Parser *)malloc(sizeof(Parser));
   parser->lex = lex;
   parser->cur = next_tok(&parser->lex);

   Token tok;
   tok.type = TOK_INVALID;
   tok.val = "";
   tok.len = 0;
   parser->prev = tok;

   return parser;
}

void advance(Parser *parser)
{
   parser->prev = parser->cur;
   parser->cur = next_tok(&parser->lex);
}

bool match(Parser *parser, TokenType tok_type)
{
   if (parser->cur.type == tok_type)
   {
      advance(parser);
      return true;
   }
   return false;
}

bool check(Parser *parser, TokenType tok_type)
{
   return parser->cur.type == tok_type;
}

void expect(Parser *parser, TokenType tok_type, const char *msg)
{
   if (parser->cur.type != tok_type)
   {
      fprintf(stderr, "Syntax error: %s (found %s)\n",
              msg, TOK_TO_STR[parser->cur.type]);
      exit(EXIT_FAILURE);
   }
   advance(parser);
}

Token peek_tok(Lexer *lex)
{
   // TODO
}

void error(Parser *parser, const char *msg)
{
   fprintf(stderr, __generate_err_msg(msg, parser->lex->filename, parser->lex->line));
}