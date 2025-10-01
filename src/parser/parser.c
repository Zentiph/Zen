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

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

// ----- internals -----
char *generate_err_msg(const char *msg, const char *filename, int line)
{
   size_t bufsiz = 512;
   char *err_msg = (char *)malloc(bufsiz);
   if (!err_msg)
      return NULL;

   snprintf(err_msg, bufsiz, "Syntax error at %s:%d - %s", filename, line, msg);
   return err_msg;
}

// ----- public api -----

parser_t *parser_init(lexer_t *lex)
{
   parser_t *parser = (parser_t *)malloc(sizeof(parser_t));
   if (!parser)
      return NULL;

   parser->lex = lex;
   parser->prev = (token_t){0};
   parser->cur = lex_next(parser->lex);
   parser->has_ahead = false;

   return parser;
}

void parser_close(parser_t *parser)
{
   lex_close(parser->lex);
   free(parser->prev.lexeme);
   free(parser->cur.lexeme);
   if (parser->has_ahead)
   {
      free(parser->ahead.lexeme);
   }
}

token_t *parser_peek(parser_t *parser)
{
   if (!parser->has_ahead)
   {
      parser->ahead = lex_next(parser->lex);
      parser->has_ahead = true;
   }
   return &parser->ahead;
}

void parser_adv(parser_t *parser)
{
   tok_free(&parser->prev);
   parser->prev = parser->cur;

   if (parser->has_ahead)
   {
      parser->cur = parser->ahead;
      parser->has_ahead = false;
   }
   else
   {
      parser->cur = lex_next(parser->lex);
   }
}

bool parser_match(parser_t *parser, Token tok_type)
{
   if (parser->cur.type == tok_type)
   {
      parser_adv(parser);
      return true;
   }
   return false;
}

bool parser_check(parser_t *parser, Token tok_type)
{
   return parser->cur.type == tok_type;
}

void parser_expect(parser_t *parser, Token tok_type)
{
   if (parser->cur.type != tok_type)
   {
      size_t msgsiz = 256;
      char *msg = (char *)malloc(msgsiz);
      if (!msg)
      {
         fprintf(stderr, generate_err_msg("Got unexpected token",
                                          parser->lex->filename, parser->lex->line));
      }
      else
      {
         snprintf(msg, msgsiz,
                  "Expected token %s but got %s",
                  TOK_TO_STR[tok_type], TOK_TO_STR[parser->cur.type]);
         fprintf(stderr, "%s\n",
                 generate_err_msg(msg, parser->lex->filename, parser->lex->line));
      }
      free(msg);
      exit(EXIT_FAILURE);
   }
   parser_adv(parser);
}

void parser_error(parser_t *parser, const char *msg)
{
   char *m = generate_err_msg(msg, parser->lex->filename, parser->lex->line);
   if (!m)
      fprintf(stderr, "Parser error at %s:%d",
              parser->lex->filename, parser->lex->line);
   fprintf(stderr, "%s\n", m);
   free(m);
}