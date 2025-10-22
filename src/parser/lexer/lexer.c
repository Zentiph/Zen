/// @file lexer.c
/// @author Gavin Borne
/// @brief Lexer types and functions for the Zen programming language.
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

const lexer_t lexer_create(FILE *fp, char *filename) {
   const lexer_t lexer = (lexer_t)malloc(sizeof(*lexer));
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
}

int lexer_current(const lexer_t lexer) {
   if (lexer->bytes_read == 0)
      if (_prime(lexer) == 0)
         return EOF;
   if (_buf_full(lexer))
      return EOF;
   return (unsigned char)*lexer->bufptr;
}

int lexer_peek(const lexer_t lexer) {
   if (lexer->bytes_read == 0)
      if (_prime(lexer) == 0)
         return EOF;
   if (!_ensure_ahead(lexer, ZLANG_LEXER_LOOKAHEAD))
      return EOF;
   return (unsigned char)*(lexer->bufptr + 1);
}

int lexer_pre_advance(const lexer_t lexer) {
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

int lexer_post_advance(const lexer_t lexer) {
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

bool lexer_unget(const lexer_t lexer) {
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

void lexer_skip(const lexer_t lexer, int n) {
   if (n <= 0)
      return;
   while (n-- > 0)
      if (lexer_pre_advance(lexer) == EOF)
         return;
}

void lexer_skip_whitespace(const lexer_t lexer) {
   for (;;) {
      int cur = lexer_current(lexer);
      if (cur == EOF)
         return;
      if (!isspace((unsigned char)cur))
         return;
      if (cur == '\n')
         return; // preserve newline as a token
      lexer_pre_advance(lexer);
   }
}

token_t lex_next(const lexer_t lexer) {
   _tbuf_reset(lexer);

   lexer_skip_whitespace(lexer);

   int cc = lexer_current(lexer);
   if (cc == EOF) {
      return _make_token(TOK_EOF, lexer->tbuf, lexer->tlen);
   }

   char cur = (char)cc;

   // single-line comments
   if (cur == '/' && lexer_peek(lexer) == '/') {
      lexer_skip(lexer, 2); // lexer_skip "//"

      while (lexer_current(lexer) != '\n' && lexer_current(lexer) != EOF)
         _tbuf_put(lexer, lexer_pre_advance(lexer));

      return _make_token(TOK_COMMENT, lexer->tbuf, lexer->tlen);
   }

   // multi-line comments
   if (cur == '/' && lexer_peek(lexer) == '.') {
      lexer_skip(lexer, 2); // lexer_skip opening "/."

      while (true) {
         // end of comment
         if (lexer_current(lexer) == '.' && lexer_peek(lexer) == '/') {
            lexer_skip(lexer, 2); // lexer_skip closing "./"
            break;
         }
         if (lexer_current(lexer) == EOF)
            break;

         _tbuf_put(lexer, lexer_pre_advance(lexer));
      }

      return _make_token(TOK_COMMENT, lexer->tbuf, lexer->tlen);
   }

   // keywords and identifiers
   if (isalpha((unsigned char)cur) || cur == '_') {
      for (;;) {
         int c = lexer_current(lexer);
         if (!(isalnum((unsigned char)c) || c == '_'))
            break;
         _tbuf_put(lexer, lexer_current(lexer));
         lexer_pre_advance(lexer);
      }

      // check for keyword
      for (int i = 0; KW_TAB[i] != NULL; i++) {
         if (strcmp(lexer->tbuf, KW_TAB[i]) == 0) {
            return _make_token(TOK_KW, lexer->tbuf, lexer->tlen);
         }
      }

      return _make_token(TOK_ID, lexer->tbuf, lexer->tlen);
   }

   // string literals (quotes not included in token)
   if (cur == '"' || cur == '\'') {
      char quote = cur;         // store quote type for consistency
      lexer_pre_advance(lexer); // consume opening quote

      for (;;) {
         int ch = lexer_current(lexer);
         if (ch == EOF || ch == '\n')
            break; // unterminated
         if (ch == quote) {
            lexer_pre_advance(lexer); // consume closing quote
            break;
         }

         if (ch == '\\') {
            char esc = lexer_pre_advance(lexer); // consume '\'
            switch (esc) {
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
            lexer_pre_advance(lexer); // move past escaped char
         } else {
            _tbuf_put(lexer, ch);
            lexer_pre_advance(lexer);
         }
      }

      return _make_token(TOK_STR, lexer->tbuf, lexer->tlen);
   }

   // number literals and dot
   if (isdigit((unsigned char)cur) || cur == '.') {
      // dot without surrounding digits, dot token
      if (cur == '.' && !isdigit((unsigned char)lexer_peek(lexer))) {
         _tbuf_put(lexer, '.');
         lexer_pre_advance(lexer);
         return _make_token(TOK_DOT, lexer->tbuf, lexer->tlen);
      }

      char prev;
      bool dot_seen = (cur == '.');
      while (isdigit((unsigned char)lexer_current(lexer)) ||
             lexer_current(lexer) == '.') {
         int ch = lexer_current(lexer);
         if (ch == '.') {
            if (!dot_seen)
               dot_seen = true;
            else
               break; // end the number at 2 dots
         }

         _tbuf_put(lexer, ch);
         prev = lexer_current(lexer);
         lexer_pre_advance(lexer);
      }

      // if ended with a dot split into a number and a dot
      if (prev == '.') {
         lexer_unget(lexer);
      }

      return _make_token(TOK_NUM, lexer->tbuf, lexer->tlen);
   }

   // arrows
   if (cur == '-') {
      int next = lexer_peek(lexer);
      if (next == '>') {
         _tbuf_put2(lexer, '-', '>');
         lexer_pre_advance(lexer);
         lexer_pre_advance(lexer);
         return _make_token(TOK_ARROW, lexer->tbuf, lexer->tlen);
      }
   }
   if (cur == '=') {
      int next = lexer_peek(lexer);
      if (next == '>') {
         _tbuf_put2(lexer, '=', '>');
         lexer_pre_advance(lexer);
         lexer_pre_advance(lexer);
         return _make_token(TOK_DBL_ARROW, lexer->tbuf, lexer->tlen);
      }
   }

   // comparison ops
   if (cur == '=' || cur == '!' || cur == '<' || cur == '>') {
      // == or != or <= or >=
      if (lexer_peek(lexer) == '=') {
         _tbuf_put2(lexer, cur, '=');
         lexer_pre_advance(lexer);
         lexer_pre_advance(lexer);

         Token type;
         switch (cur) {
         case '=':
            type = TOK_EQ;
            break;
         case '!':
            type = TOK_NE;
            break;
         case '<':
            type = TOK_LE;
            break;
         case '>':
            type = TOK_GE;
            break;
         default:
            type = TOK_INVALID;
            break;
         }

         return _make_token(type, lexer->tbuf, lexer->tlen);
      }

      _tbuf_put(lexer, cur);
      lexer_pre_advance(lexer);

      Token type;
      switch (cur) {
      case '=':
         type = TOK_ASSIGN;
         break;
      case '!':
         type = TOK_NOT;
         break;
      case '<':
         type = TOK_LT;
         break;
      case '>':
         type = TOK_GT;
         break;
      default:
         type = TOK_INVALID;
         break;
      }

      return _make_token(type, lexer->tbuf, lexer->tlen);
   }

   // binary ops
   if (cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '%') {
      // in-place binary op
      if (lexer_peek(lexer) == '=') {
         _tbuf_put2(lexer, cur, '=');
         lexer_pre_advance(lexer);
         lexer_pre_advance(lexer);

         Token type;
         switch (cur) {
         case '+':
            type = TOK_ADD_ASSIGN;
            break;
         case '-':
            type = TOK_SUB_ASSIGN;
            break;
         case '*':
            type = TOK_MUL_ASSIGN;
            break;
         case '/':
            type = TOK_DIV_ASSIGN;
            break;
         case '%':
            type = TOK_MOD_ASSIGN;
            break;
         default:
            type = TOK_INVALID;
            break;
         }

         return _make_token(type, lexer->tbuf, lexer->tlen);
      }

      // regular binary op
      _tbuf_put(lexer, cur);
      lexer_pre_advance(lexer);

      Token type;
      switch (cur) {
      case '+':
         type = TOK_ADD;
         break;
      case '-':
         type = TOK_SUB;
         break;
      case '*':
         type = TOK_MUL;
         break;
      case '/':
         type = TOK_DIV;
         break;
      case '%':
         type = TOK_MOD;
         break;
      default:
         type = TOK_INVALID;
         break;
      }

      return _make_token(type, lexer->tbuf, lexer->tlen);
   }

   // two-char logic ops
   if (cur == '&' || cur == '|') {
      char peeked = lexer_peek(lexer);
      if (peeked == cur) {
         _tbuf_put2(lexer, cur, peeked);
         lexer_pre_advance(lexer);
         lexer_pre_advance(lexer);

         return _make_token(TOK_AND, lexer->tbuf, lexer->tlen);
      }
      // continue if not && or ||, will be lexed below
   }

   // single chars
   _tbuf_put(lexer, cur);
   lexer_pre_advance(lexer);

   Token type;
   switch (cur) {
   case '!':
      type = TOK_NOT;
      break;
   case ',':
      type = TOK_COMMA;
      break;
   case '(':
      type = TOK_LT_PAREN;
      break;
   case ')':
      type = TOK_RT_PAREN;
      break;
   case '[':
      type = TOK_LT_BRACK;
      break;
   case ']':
      type = TOK_RT_BRACK;
      break;
   case '{':
      type = TOK_LT_BRACE;
      break;
   case '}':
      type = TOK_RT_BRACE;
      break;
   case '\n':
      type = TOK_NEWLINE;
      break;
   default:
      type = TOK_INVALID;
      break;
   }

   return _make_token(type, lexer->tbuf, lexer->tlen);
}