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

#include <string.h>

#include "../internals.h"
#pragma GCC dependency "../internals.c"
#include "lexer/token.h"
#pragma GCC dependency "lexer/token.c"
#include "lexer/lexer.h"
#pragma GCC dependency "lexer/lexer.c"

#include "parser.h"

struct parser_s {
   lexer_t lexer;
   token_t cur;
   token_t prev;
   token_t ahead;
   bool has_ahead;
};

static char *generate_err_msg(const char *msg, const char *filename, int line) {
   size_t bufsiz = 512;
   char *err_msg = malloc(bufsiz);
   if (!err_msg)
      return NULL;

   snprintf(err_msg, bufsiz, "Syntax error at %s:%d - %s", filename, line, msg);
   return err_msg;
}

parser_t parser_create(lexer_t lexer) {
   parser_t parser = malloc(sizeof(parser_t));
   if (!parser)
      return NULL;

   parser->lexer = lexer;
   parser->prev = (token_t){0};
   parser->cur = lexer_next_token(parser->lexer);
   parser->has_ahead = false;

   return parser;
}

void parser_close(parser_t parser) {
   lexer_destroy(parser->lexer);
   token_destroy(parser->prev);
   token_destroy(parser->cur);
   if (parser->has_ahead) {
      token_destroy(parser->ahead);
   }
}

token_t *parser_peek(parser_t parser) {
   if (!parser->has_ahead) {
      parser->ahead = lexer_next_token(parser->lexer);
      parser->has_ahead = true;
   }
   return &parser->ahead;
}

void parser_adv(parser_t parser) {
   token_destroy(parser->prev);
   parser->prev = parser->cur;

   if (parser->has_ahead) {
      parser->cur = parser->ahead;
      parser->has_ahead = false;
   } else {
      parser->cur = lexer_next_token(parser->lexer);
   }
}

bool parser_match(parser_t parser, Token token_type) {
   if (token_get_type(parser->cur) == token_type) {
      parser_adv(parser);
      return true;
   }
   return false;
}

bool parser_check(parser_t parser, Token token_type) {
   return token_get_type(parser->cur) == token_type;
}

void parser_expect(parser_t parser, Token token_type) {
   if (token_get_type(parser->cur) != token_type) {
      size_t msgsiz = 256;
      char *msg = (char *)malloc(msgsiz);
      if (!msg) {
         fprintf(stderr, "%s",
                 generate_err_msg("Got unexpected token",
                                  lexer_get_filename(parser->lexer),
                                  lexer_get_line(parser->lexer)));
      } else {
         snprintf(msg, msgsiz, "Expected token %s but got %s",
                  TOKEN_TO_STRING[token_type],
                  TOKEN_TO_STRING[token_get_type(parser->cur)]);
         fprintf(stderr, "%s\n",
                 generate_err_msg(msg, lexer_get_filename(parser->lexer),
                                  lexer_get_line(parser->lexer)));
      }
      free(msg);
      exit(EXIT_FAILURE);
   }
   parser_adv(parser);
}

void parser_error(parser_t parser, const char *msg) {
   char *m = generate_err_msg(msg, lexer_get_filename(parser->lexer),
                              lexer_get_line(parser->lexer));
   if (!m)
      fprintf(stderr, "Parser error at %s:%d",
              lexer_get_filename(parser->lexer), lexer_get_line(parser->lexer));
   fprintf(stderr, "%s\n", m);
   free(m);
}

ast_node_t parse_primary(parser_t parser) {
   if (parser_match(parser, TOKEN_NUMBER)) {
      double val = strtod(token_get_lexeme(parser->prev), NULL);
      return ast_create_number(val);
   }

   if (parser_match(parser, TOKEN_IDENTIFIER)) {
      char *name = zlang_strdup(token_get_lexeme(parser->prev));
      // is it a function call?
      if (parser_match(parser, TOKEN_LT_PAREN)) {
         ast_node_t *args = NULL;
         int arg_count = 0;
         if (!parser_check(parser, TOKEN_RT_PAREN)) {
            do {
               ast_node_t arg = parse_expr(parser);
               args = realloc(args, sizeof(ast_node_t) * ++arg_count);
               args[arg_count - 1] = arg;
            } while (parser_match(parser, TOKEN_COMMA));
         }
         parser_expect(parser, TOKEN_RT_PAREN);
         return ast_create_func_call(name, args, arg_count);
      }
      return ast_create_identifier(name);
   }

   if (parser_match(parser, TOKEN_LT_PAREN)) {
      ast_node_t expr = parse_expr(parser);
      parser_expect(parser, TOKEN_RT_PAREN);
   }

   parser_error(parser, "Expected expression.");
   return NULL;
}