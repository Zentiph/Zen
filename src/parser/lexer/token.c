///
/// @file token.c
/// @author Gavin Borne
/// @brief Token specification for the Zen programming language.
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../../internals.h"

#include "token.h"

struct token_s {
   Token type;
   char *lexeme;
   size_t len;
};

const token_t token_create(const Token type, const char *lexeme) {
   token_t token = malloc(sizeof(*token));
   if (!token)
      return NULL;

   token->type = type;
   token->lexeme = zlang_strdup(lexeme);
   if (lexeme && !token->lexeme) { // _dupstr() failed
      free(token);
      return NULL;
   }
   token->len = token->lexeme ? strlen(token->lexeme) : 0;

   return token;
}

void token_destroy(const token_t token) {
   if (!token)
      return;

   free(token->lexeme);
   free(token);
}

const Token token_get_type(const token_t token) { return token->type; }

bool token_has_lexeme(const token_t token) { return token->lexeme != NULL; }

const char *token_get_lexeme(const token_t token) { return token->lexeme; }

size_t token_get_length(const token_t token) { return token->len; }