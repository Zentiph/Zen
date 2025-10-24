/// @file lexer.c
/// @author Gavin Borne
/// @brief Lexer specification for the Zen programming language.
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

#define MIN_(a, b) ((a) < (b) ? (a) : (b))

struct lexer_s {
   FILE *fp;
   char *filename;

   char buf[ZLANG_LEXER_BUFSIZ];
   char *bufptr;
   size_t bytes_read;

   int line;
   int col;

   char tbuf[ZLANG_TOKSIZ];
   size_t tlen;

   // position history so unget() can restore line/col
   int _hist_line[ZLANG_LEXER_KEEP_BACK];
   int _hist_col[ZLANG_LEXER_KEEP_BACK];
   int _hist_len; // 0 to ZLANG_LEXER_KEEP_BACK
};

static inline bool _buf_full(const lexer_t lexer) {
   return lexer->bufptr >= lexer->buf + lexer->bytes_read;
}

static inline size_t _idx(const lexer_t lexer) {
   return (size_t)(lexer->bufptr - lexer->buf);
}

static inline size_t _ahead(const lexer_t lexer) {
   // bytes in the buffer after current char
   return (size_t)(lexer->buf + lexer->bytes_read - lexer->bufptr - 1);
}

static void _push_pos(const lexer_t lexer) {
   // push current (line, col) into history for lexer_unget()
   if (lexer->_hist_len == ZLANG_LEXER_KEEP_BACK) {
      memmove(&lexer->_hist_line[0], &lexer->_hist_line[1],
              sizeof(int) * (ZLANG_LEXER_KEEP_BACK - 1));
      memmove(&lexer->_hist_col[0], &lexer->_hist_col[1],
              sizeof(int) * (ZLANG_LEXER_KEEP_BACK - 1));
      lexer->_hist_len--;
   }
   lexer->_hist_line[lexer->_hist_len] = lexer->line;
   lexer->_hist_col[lexer->_hist_len] = lexer->col;
   lexer->_hist_len++;
}

static int _slide_refill(const lexer_t lexer) {
   // carry up to ZLANG_LEXER_KEEP_BACK bytes ending at current char
   // tail all unread bytes after current char
   size_t i = _idx(lexer);
   size_t back = MIN_(ZLANG_LEXER_KEEP_BACK, i + 1); // bytes to keep
   size_t tail = lexer->bytes_read - (i + 1);        // unread after current

   // move back so the current char ends up at index ZLANG_LEXER_KEEP_BACK
   if (back)
      memmove(lexer->buf + (ZLANG_LEXER_KEEP_BACK + 1 - back),
              lexer->buf + (i + 1 - back), back);

   // move tail right after current (at ZLANG_LEXER_KEEP_BACK + 1)
   if (tail)
      memmove(lexer->buf + (ZLANG_LEXER_KEEP_BACK + 1), lexer->buf + (i + 1),
              tail);

   // update ptr
   lexer->bufptr = lexer->buf + ZLANG_LEXER_KEEP_BACK;

   // fill rest of buffer
   size_t filled = ZLANG_LEXER_KEEP_BACK + 1 + tail;
   size_t space =
      (ZLANG_LEXER_BUFSIZ > filled) ? (ZLANG_LEXER_BUFSIZ - filled) : 0;
   size_t got =
      (space > 0) ? fread(lexer->buf + filled, 1, space, lexer->fp) : 0;

   lexer->bytes_read = filled + got;
   return (int)got;
}

static bool _ensure_ahead(const lexer_t lexer, size_t need) {
   if (_ahead(lexer) >= need)
      return true;
   if (lexer->bytes_read == 0)
      return false; // nothing loaded
   if (_slide_refill(lexer) <= 0)
      // may still be EOF
      return _ahead(lexer) >= need;
   return _ahead(lexer) >= need;
}

static int _prime(const lexer_t lexer) {
   // reserve ZLANG_LEXER_KEEP_BACK bytes for lexer_unget region at front
   memset(lexer->buf, 0, ZLANG_LEXER_KEEP_BACK);
   size_t got = fread(lexer->buf + ZLANG_LEXER_KEEP_BACK, 1,
                      ZLANG_LEXER_BUFSIZ - ZLANG_LEXER_KEEP_BACK, lexer->fp);
   lexer->bytes_read = ZLANG_LEXER_KEEP_BACK + got;
   lexer->bufptr =
      lexer->buf + ZLANG_LEXER_KEEP_BACK; // current char at first real byte
   return (int)got;
}

static inline void _tbuf_reset(const lexer_t lexer) {
   lexer->tlen = 0;
   if (ZLANG_TOKSIZ > 0)
      lexer->tbuf[0] = '\0';
}

static inline void _tbuf_put(const lexer_t lexer, int ch) {
   if (lexer->tlen + 1 < ZLANG_TOKSIZ) // leave room for NULL
   {
      lexer->tbuf[lexer->tlen++] = (char)ch;
      lexer->tbuf[lexer->tlen] = '\0';
   }
}

static inline void _tbuf_put2(const lexer_t lexer, int a, int b) {
   _tbuf_put(lexer, a);
   _tbuf_put(lexer, b);
}

static inline token_t _make_token(Token type, const char *buf, size_t len) {
   char *lexeme = NULL;
   if (len) {
      lexeme = malloc(len + 1);
      if (lexeme) {
         memcpy(lexeme, buf, len);
         lexeme[len] = '\0';
      }
   }

   token_t token = token_create(type, lexeme);
   return token;
}

static int lexer_current(const lexer_t lexer) {
   if (lexer->bytes_read == 0)
      if (_prime(lexer) == 0)
         return EOF;
   if (_buf_full(lexer))
      return EOF;
   return (unsigned char)*lexer->bufptr;
}

static int lexer_peek(const lexer_t lexer) {
   if (lexer->bytes_read == 0)
      if (_prime(lexer) == 0)
         return EOF;
   if (!_ensure_ahead(lexer, ZLANG_LEXER_LOOKAHEAD))
      return EOF;
   return (unsigned char)*(lexer->bufptr + 1);
}

static int lexer_pre_advance(const lexer_t lexer) {
   if (lexer->bytes_read == 0)
      if (_prime(lexer) == 0)
         return EOF;

   if (!_ensure_ahead(lexer, 1)) {
      // no next char, but need to move past so
      // subsequent calls see EOF
      if (!_buf_full(lexer)) {
         // save pos for possible lexer_unget()
         _push_pos(lexer);

         unsigned char cur = (unsigned char)*lexer->bufptr;
         if (cur == '\n') {
            lexer->line++;
            lexer->col = 1;
         } else {
            lexer->col++;
         }

         // move to one-past-last
         ++lexer->bufptr;
      }
      return EOF;
   }

   // normal path, we have look_ahead
   // save current pos for possible lexer_unget()
   _push_pos(lexer);

   unsigned char next = (unsigned char)*(++lexer->bufptr);
   if (next == '\n') {
      lexer->line++;
      lexer->col = 1;
   } else
      lexer->col++;

   return (int)next;
}

static int lexer_post_advance(const lexer_t lexer) {
   if (lexer->bytes_read == 0)
      if (_prime(lexer) == 0)
         return EOF;

   char current = lexer_current(lexer);

   if (!_ensure_ahead(lexer, 1)) {
      // no next char, but need to move past so
      // subsequent calls see EOF
      if (!_buf_full(lexer)) {
         // save pos for possible lexer_unget()
         _push_pos(lexer);

         unsigned char cur = (unsigned char)*lexer->bufptr;
         if (cur == '\n') {
            lexer->line++;
            lexer->col = 1;
         } else {
            lexer->col++;
         }

         // move to one-past-last
         ++lexer->bufptr;
      }
      return (int)current;
   }

   // normal path, we have look_ahead
   // save current pos for possible lexer_unget()
   _push_pos(lexer);

   unsigned char next = (unsigned char)*(++lexer->bufptr);
   if (next == '\n') {
      lexer->line++;
      lexer->col = 1;
   } else
      lexer->col++;

   return (int)current;
}

static bool lexer_unget(const lexer_t lexer) {
   if (lexer->_hist_len <= 0)
      return false; // nothing to restore
   if (lexer->bufptr <= lexer->buf)
      return false; // cannot move before buffer start

   --lexer->bufptr;

   // restore saved pos
   lexer->line = lexer->_hist_line[lexer->_hist_len - 1];
   lexer->col = lexer->_hist_col[lexer->_hist_len - 1];
   lexer->_hist_len--;

   return true;
}

static void lexer_skip(const lexer_t lexer, int n) {
   if (n <= 0)
      return;
   while (n-- > 0)
      if (lexer_pre_advance(lexer) == EOF)
         return;
}

static void lexer_skip_whitespace(const lexer_t lexer) {
   for (;;) {
      int cur = lexer_current(lexer);
      if (cur == EOF)
         return;
      if (!isspace((unsigned char)cur))
         return;
      if (cur == '\n')
         return; // preserve newline as a token
      lexer_skip(lexer, 1);
   }
}

static const token_t lex_single_line_comment(lexer_t lexer) {
   lexer_skip(lexer, 2); // skip "//"

   while (lexer_current(lexer) != '\n' && lexer_current(lexer) != EOF)
      _tbuf_put(lexer, lexer_pre_advance(lexer));

   return _make_token(TOKEN_COMMENT, lexer->tbuf, lexer->tlen);
}

static const token_t lex_multi_line_comment(lexer_t lexer) {
   lexer_skip(lexer, 2); // skip opening "/."

   for (;;) {
      // end of comment
      if (lexer_current(lexer) == '.' && lexer_peek(lexer) == '/') {
         lexer_skip(lexer, 2); // skip "./"
         break;
      }
      if (lexer_current(lexer) == EOF)
         break;

      _tbuf_put(lexer, lexer_pre_advance(lexer));
   }

   return _make_token(TOKEN_COMMENT, lexer->tbuf, lexer->tlen);
}

static const token_t lex_identifier(lexer_t lexer) {
   for (;;) {
      int c = lexer_current(lexer);

      if (!(isalnum((unsigned char)c) || c == '_'))
         break;

      _tbuf_put(lexer, lexer_current(lexer));
      lexer_skip(lexer, 1);
   }

   for (int i = 0; KW_TAB[i] != NULL; i++) {
      if (strcmp(lexer->tbuf, KW_TAB[i]) == 0) {
         return _make_token(TOKEN_KEYWORD, lexer->tbuf, lexer->tlen);
      }
   }

   return _make_token(TOKEN_IDENTIFIER, lexer->tbuf, lexer->tlen);
}

static const token_t lex_string(lexer_t lexer, char entry_quote) {
   lexer_skip(lexer, 1); // consume opening quote

   for (;;) {
      int ch = lexer_current(lexer);
      if (ch == EOF || ch == '\n')
         break;
      if (ch == entry_quote) {
         lexer_skip(lexer, 1); // consume closing quote
         break;
      }

      if (ch == '\\') {
         char esc = lexer_pre_advance(lexer); // consume '\'
         switch (esc) {
         // TODO make a table similar to KW_TAB that defines supported escape
         // seqs
         case 'n':
            _tbuf_put(lexer, '\n');
            break;
         case 't':
            _tbuf_put(lexer, '\t');
            break;
         case '\\':
            _tbuf_put(lexer, '\\');
            break;
         case '"':
            _tbuf_put(lexer, '"');
            break;
         case '\'':
            _tbuf_put(lexer, '\'');
            break;
         default:
            // handle invalid esc sequence by appending as-is
            _tbuf_put(lexer, '\\');
            _tbuf_put(lexer, esc);
            break;
         }
         lexer_skip(lexer, 1); // move past escaped char
      } else {
         _tbuf_put(lexer, ch);
         lexer_skip(lexer, 1);
      }
   }

   return _make_token(TOKEN_STRING, lexer->tbuf, lexer->tlen);
}
static const token_t lex_number_or_dot(lexer_t lexer, char current) {
   if (current == '.' && !isdigit((unsigned char)lexer_peek(lexer))) {
      _tbuf_put(lexer, '.');
      lexer_skip(lexer, 1);
      return _make_token(TOKEN_DOT, lexer->tbuf, lexer->tlen);
   }

   char prev;
   bool dot_seen = (current == '.');
   while (isdigit((unsigned char)lexer_current(lexer)) ||
          lexer_current(lexer) == '.') {
      int ch = lexer_current(lexer);
      if (ch == '.') {
         if (!dot_seen)
            dot_seen = true;
         else
            break; // end the number if 2 dots found
      }

      _tbuf_put(lexer, ch);
      prev = lexer_post_advance(lexer);
   }

   // if ended with a dot split into number and dot
   if (prev == '.')
      lexer_unget(lexer);

   return _make_token(TOKEN_NUMBER, lexer->tbuf, lexer->tlen);
}

static const token_t lex_arrow(lexer_t lexer, char arrow_symbol) {
   _tbuf_put2(lexer, arrow_symbol, '>');
   lexer_skip(lexer, 2);
   return _make_token(TOKEN_ARROW, lexer->tbuf, lexer->tlen);
}

static const token_t lex_comparison_op(lexer_t lexer, char current) {
   // == or != or <= or >=
   if (lexer_peek(lexer) == '=') {
      _tbuf_put2(lexer, current, '=');
      lexer_skip(lexer, 2);

      Token type;
      switch (current) {
      case '=':
         type = TOKEN_EQ;
         break;
      case '!':
         type = TOKEN_NE;
         break;
      case '<':
         type = TOKEN_LE;
         break;
      case '>':
         type = TOKEN_GE;
         break;
      default:
         type = TOKEN_INVALID;
         break;
      }

      return _make_token(type, lexer->tbuf, lexer->tlen);
   }

   _tbuf_put(lexer, current);
   lexer_skip(lexer, 1);

   Token type;
   switch (current) {
   case '=':
      type = TOKEN_ASSIGN;
      break;
   case '!':
      type = TOKEN_NOT;
      break;
   case '<':
      type = TOKEN_LT;
      break;
   case '>':
      type = TOKEN_GT;
      break;
   default:
      type = TOKEN_INVALID;
      break;
   }

   return _make_token(type, lexer->tbuf, lexer->tlen);
}

static const token_t lex_binary_op(lexer_t lexer, char current) {
   // in-place binary op
   if (lexer_peek(lexer) == '=') {
      _tbuf_put2(lexer, current, '=');
      lexer_skip(lexer, 2);

      Token type;
      switch (current) {
      case '+':
         type = TOKEN_ADD_ASSIGN;
         break;
      case '-':
         type = TOKEN_SUB_ASSIGN;
         break;
      case '*':
         type = TOKEN_MUL_ASSIGN;
         break;
      case '/':
         type = TOKEN_DIV_ASSIGN;
         break;
      case '%':
         type = TOKEN_MOD_ASSIGN;
         break;
      default:
         type = TOKEN_INVALID;
         break;
      }

      return _make_token(type, lexer->tbuf, lexer->tlen);
   }

   // regular binary op
   _tbuf_put(lexer, current);
   lexer_skip(lexer, 1);

   Token type;
   switch (current) {
   case '+':
      type = TOKEN_ADD;
      break;
   case '-':
      type = TOKEN_SUB;
      break;
   case '*':
      type = TOKEN_MUL;
      break;
   case '/':
      type = TOKEN_DIV;
      break;
   case '%':
      type = TOKEN_MOD;
      break;
   default:
      type = TOKEN_INVALID;
      break;
   }

   return _make_token(type, lexer->tbuf, lexer->tlen);
}

static const token_t lex_logic_op(lexer_t lexer, char symbol) {
   _tbuf_put2(lexer, symbol, symbol);
   lexer_skip(lexer, 2);

   if (symbol == '&')
      return _make_token(TOKEN_AND, lexer->tbuf, lexer->tlen);
   else
      return _make_token(TOKEN_OR, lexer->tbuf, lexer->tlen);
}

static const token_t lex_single_symbol(lexer_t lexer, char current) {
   _tbuf_put(lexer, current);
   lexer_skip(lexer, 1);

   Token type;
   switch (current) {
   case '!':
      type = TOKEN_NOT;
      break;
   case ',':
      type = TOKEN_COMMA;
      break;
   case '(':
      type = TOKEN_LT_PAREN;
      break;
   case ')':
      type = TOKEN_RT_PAREN;
      break;
   case '[':
      type = TOKEN_LT_BRACK;
      break;
   case ']':
      type = TOKEN_RT_BRACK;
      break;
   case '{':
      type = TOKEN_LT_BRACE;
      break;
   case '}':
      type = TOKEN_RT_BRACE;
      break;
   case '\n':
      type = TOKEN_NEWLINE;
      break;
   default:
      type = TOKEN_INVALID;
      break;
   }

   return _make_token(type, lexer->tbuf, lexer->tlen);
}

const lexer_t lexer_create(FILE *fp, char *filename) {
   const lexer_t lexer = malloc(sizeof(*lexer));
   if (!lexer)
      return NULL;

   lexer->fp = fp;
   lexer->filename = filename;
   lexer->bufptr = lexer->buf;
   lexer->bytes_read = 0;
   lexer->line = 1;
   lexer->col = 1;
   lexer->tlen = 0;
   if (ZLANG_TOKSIZ > 0)
      lexer->tbuf[0] = '\0';
   lexer->_hist_len = 0;

   return lexer;
}

void lexer_destroy(const lexer_t lexer) {
   fclose(lexer->fp);
   free(lexer->filename);
   free(lexer->bufptr);
   free(lexer->tbuf);
   free(lexer->buf);
   free(lexer->_hist_line);
   free(lexer->_hist_col);
   free(lexer);
}

const char *lexer_get_filename(lexer_t lexer) { return lexer->filename; }

int lexer_get_line(lexer_t lexer) { return lexer->line; }

int lexer_get_column(lexer_t lexer) { return lexer->col; }

token_t lexer_get_next(const lexer_t lexer) {
   _tbuf_reset(lexer);

   lexer_skip_whitespace(lexer);

   int cc = lexer_current(lexer);
   if (cc == EOF) {
      return _make_token(TOKEN_EOF, lexer->tbuf, lexer->tlen);
   }

   char cur = (char)cc;

   if (cur == '/' && lexer_peek(lexer) == '/')
      return lex_single_line_comment(lexer);

   if (cur == '/' && lexer_peek(lexer) == '.')
      return lex_multi_line_comment(lexer);

   if (isalpha((unsigned char)cur) || cur == '_')
      return lex_identifier(lexer);

   // quotes not included in token
   if (cur == '"' || cur == '\'')
      return lex_string(lexer, cur);

   if (isdigit((unsigned char)cur) || cur == '.')
      return lex_number_or_dot(lexer, cur);

   if ((cur == '-' || cur == '=') && lexer_peek(lexer) == '>')
      return lex_arrow(lexer, cur);

   if (cur == '=' || cur == '!' || cur == '<' || cur == '>')
      return lex_comparison_op(lexer, cur);

   if (cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '%')
      return lex_binary_op(lexer, cur);

   if (cur == '&' || cur == '|') {
      if (lexer_peek(lexer) == cur)
         return lex_logic_op(lexer, cur);
      // continue if not && or ||, will be lexed below
   }

   return lex_single_symbol(lexer, cur);
}