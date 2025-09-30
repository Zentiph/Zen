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

#define MIN_(a, b) ((a) < (b) ? (a) : (b))

// ----- internals -----
static inline bool buf_full(const lexer_t *lex)
{
   return lex->ptr >= lex->buf + lex->bytes_read;
}

static inline size_t idx(const lexer_t *lex)
{
   return (size_t)(lex->ptr - lex->buf);
}

static inline size_t ahead(const lexer_t *lex)
{
   // bytes in the buffer after current char
   return (size_t)(lex->buf + lex->bytes_read - lex->ptr - 1);
}

static void push_pos(lexer_t *lex)
{
   // push current (line, col) into history for lex_unget()
   if (lex->_hist_len == ZLANG_LEX_KEEP_BK)
   {
      memmove(
          &lex->_hist_line[0], &lex->_hist_line[1], sizeof(int) * (ZLANG_LEX_KEEP_BK - 1));
      memmove(
          &lex->_hist_col[0], &lex->_hist_col[1], sizeof(int) * (ZLANG_LEX_KEEP_BK - 1));
      lex->_hist_len--;
   }
   lex->_hist_line[lex->_hist_len] = lex->line;
   lex->_hist_col[lex->_hist_len] = lex->col;
   lex->_hist_len++;
}

static int slide_refill(lexer_t *lex)
{
   // carry up to ZLANG_LEX_KEEP_BK bytes ending at current char
   // tail all unread bytes after current char
   size_t i = idx(lex);
   size_t back = MIN_(ZLANG_LEX_KEEP_BK, i + 1); // bytes to keep
   size_t tail = lex->bytes_read - (i + 1);      // unread after current

   // move back so the current char ends up at index ZLANG_LEX_KEEP_BK
   if (back)
      memmove(lex->buf + (ZLANG_LEX_KEEP_BK + 1 - back), lex->buf + (i + 1 - back), back);

   // move tail right after current (at ZLANG_LEX_KEEP_BK + 1)
   if (tail)
      memmove(lex->buf + (ZLANG_LEX_KEEP_BK + 1), lex->buf + (i + 1), tail);

   // update ptr
   lex->ptr = lex->buf + ZLANG_LEX_KEEP_BK;

   // fill rest of buffer
   size_t filled = ZLANG_LEX_KEEP_BK + 1 + tail;
   size_t space = (ZLANG_LEX_BUFSIZ > filled) ? (ZLANG_LEX_BUFSIZ - filled) : 0;
   size_t got = (space > 0) ? fread(lex->buf + filled, 1, space, lex->fp) : 0;

   lex->bytes_read = filled + got;
   return (int)got;
}

static bool ensure_ahead(lexer_t *lex, size_t need)
{
   if (ahead(lex) >= need)
      return true;
   if (lex->bytes_read == 0)
      return false; // nothing loaded
   if (slide_refill(lex) <= 0)
      // may still be EOF
      return ahead(lex) >= need;
   return ahead(lex) >= need;
}

static int prime(lexer_t *lex)
{
   // reserve ZLANG_LEX_KEEP_BK bytes for lex_unget region at front
   memset(lex->buf, 0, ZLANG_LEX_KEEP_BK);
   size_t got = fread(
       lex->buf + ZLANG_LEX_KEEP_BK, 1, ZLANG_LEX_BUFSIZ - ZLANG_LEX_KEEP_BK, lex->fp);
   lex->bytes_read = ZLANG_LEX_KEEP_BK + got;
   lex->ptr = lex->buf + ZLANG_LEX_KEEP_BK; // current char at first real byte
   return (int)got;
}

static inline void tbuf_reset(lexer_t *lex)
{
   lex->tlen = 0;
   if (ZLANG_TOK_SIZ > 0)
      lex->tbuf[0] = '\0';
}

static inline void tbuf_put(lexer_t *lex, int ch)
{
   if (lex->tlen + 1 < ZLANG_TOK_SIZ) // leave room for NULL
   {
      lex->tbuf[lex->tlen++] = (char)ch;
      lex->tbuf[lex->tlen] = '\0';
   }
}

static inline void tbuf_put2(lexer_t *lex, int a, int b)
{
   tbuf_put(lex, a);
   tbuf_put(lex, b);
}

static inline token_t make_tok(Token type, const char *buf, size_t len)
{
   token_t tok = {.type = type, .lexeme = NULL, .len = len};
   if (len)
   {
      tok.lexeme = malloc(len + 1);
      if (tok.lexeme)
      {
         memcpy(tok.lexeme, buf, len);
         tok.lexeme[len] = '\0';
      }
   }
   return tok;
}

// ----- public api -----
const char *kw_tab[] = {
    "if",
    "else",
    "while",
    "for",
    "in",
    "fn",
    "class",
    "extends",
    "import",
    "from",
    "export",
    "module",
    "and",
    "or",
    "not",
    NULL}; // end of table marker

lexer_t *lex_init(FILE *fp, const char *filename)
{
   lexer_t *lex = (lexer_t *)malloc(sizeof(lexer_t));
   if (!lex)
      return NULL;

   lex->fp = fp;
   lex->filename = filename;
   lex->ptr = lex->buf;
   lex->bytes_read = 0;
   lex->line = 1;
   lex->col = 1;
   lex->tlen = 0;
   if (ZLANG_TOK_SIZ > 0)
      lex->tbuf[0] = '\0';
   lex->_hist_len = 0;

   return lex;
}

int lex_cur(lexer_t *lex)
{
   if (lex->bytes_read == 0)
      if (prime(lex) == 0)
         return EOF;
   if (buf_full(lex))
      return EOF;
   return (unsigned char)*lex->ptr;
}

int lex_peek(lexer_t *lex)
{
   if (lex->bytes_read == 0)
      if (prime(lex) == 0)
         return EOF;
   if (!ensure_ahead(lex, ZLANG_LEX_LOOKAHEAD))
      return EOF;
   return (unsigned char)*(lex->ptr + 1);
}

int lex_adv(lexer_t *lex)
{
   if (lex->bytes_read == 0)
      if (prime(lex) == 0)
         return EOF;

   if (!ensure_ahead(lex, 1))
   {
      // no next char, but need to move past so
      // subsequent calls see EOF
      if (!buf_full(lex))
      {
         // save pos for possible lex_unget()
         push_pos(lex);

         unsigned char cur = (unsigned char)*lex->ptr;
         if (cur == '\n')
         {
            lex->line++;
            lex->col = 1;
         }
         else
         {
            lex->col++;
         }

         // move to one-past-last
         ++lex->ptr;
      }
      return EOF;
   }

   // normal path, we have lookahead
   // save current pos for possible lex_unget()
   push_pos(lex);

   unsigned char next = (unsigned char)*(++lex->ptr);
   if (next == '\n')
   {
      lex->line++;
      lex->col = 1;
   }
   else
      lex->col++;

   return (int)next;
}

bool lex_unget(lexer_t *lex)
{
   if (lex->_hist_len <= 0)
      return false; // nothing to restore
   if (lex->ptr <= lex->buf)
      return false; // cannot move before buffer start

   --lex->ptr;

   // restore saved pos
   lex->line = lex->_hist_line[lex->_hist_len - 1];
   lex->col = lex->_hist_col[lex->_hist_len - 1];
   lex->_hist_len--;

   return true;
}

void lex_skip(lexer_t *lex, int n)
{
   if (n <= 0)
      return;
   while (n-- > 0)
      if (lex_adv(lex) == EOF)
         return;
}

void lex_skip_wsp(lexer_t *lex)
{
   for (;;)
   {
      int cur = lex_cur(lex);
      if (cur == EOF)
         return;
      if (!isspace((unsigned char)cur))
         return;
      if (cur == '\n')
         return; // preserve newline as a token
      lex_adv(lex);
   }
}

token_t lex_next(lexer_t *lex)
{
   tbuf_reset(lex);

   lex_skip_wsp(lex);

   int cc = lex_cur(lex);
   if (cc == EOF)
   {
      return make_tok(TOK_EOF, lex->tbuf, lex->tlen);
   }

   char cur = (char)cc;

   // single-line comments
   if (cur == '/' && lex_peek(lex) == '/')
   {
      lex_skip(lex, 2); // lex_skip "//"

      while (lex_cur(lex) != '\n' && lex_cur(lex) != EOF)
         tbuf_put(lex, lex_adv(lex));

      return make_tok(TOK_COMMENT, lex->tbuf, lex->tlen);
   }

   // multi-line comments
   if (cur == '/' && lex_peek(lex) == '.')
   {
      lex_skip(lex, 2); // lex_skip opening "/."

      while (true)
      {
         // end of comment
         if (lex_cur(lex) == '.' && lex_peek(lex) == '/')
         {
            lex_skip(lex, 2); // lex_skip closing "./"
            break;
         }
         if (lex_cur(lex) == EOF)
            break;

         tbuf_put(lex, lex_adv(lex));
      }

      return make_tok(TOK_COMMENT, lex->tbuf, lex->tlen);
   }

   // keywords and identifiers
   if (isalpha((unsigned char)cur) || cur == '_')
   {
      for (;;)
      {
         int c = lex_cur(lex);
         if (!(isalnum((unsigned char)c) || c == '_'))
            break;
         tbuf_put(lex, lex_cur(lex));
         lex_adv(lex);
      }

      // check for keyword
      for (int i = 0; kw_tab[i] != NULL; i++)
      {
         if (strcmp(lex->tbuf, kw_tab[i]) == 0)
         {
            return make_tok(TOK_KW, lex->tbuf, lex->tlen);
         }
      }

      return make_tok(TOK_ID, lex->tbuf, lex->tlen);
   }

   // string literals (quotes not included in token)
   if (cur == '"' || cur == '\'')
   {
      char quote = cur; // store quote type for consistency
      lex_adv(lex);     // consume opening quote

      for (;;)
      {
         int ch = lex_cur(lex);
         if (ch == EOF || ch == '\n')
            break; // unterminated
         if (ch == quote)
         {
            lex_adv(lex); // consume closing quote
            break;
         }

         if (ch == '\\')
         {
            lex_adv(lex); // consume '\'
            char esc = lex_adv(lex);
            switch (esc)
            {
            case 'n':
               tbuf_put(lex, '\n');
               break;
            case 't':
               tbuf_put(lex, '\t');
               break;
            case '\\':
               tbuf_put(lex, '\\');
               break;
            case '"':
               tbuf_put(lex, '"');
               break;
            case '\'':
               tbuf_put(lex, '\'');
               break;
            default:
               // handle invalid esc sequence by appending as-is
               tbuf_put(lex, '\\');
               tbuf_put(lex, esc);
               break;
            }
            lex_adv(lex); // move past escaped char
         }
         else
         {
            tbuf_put(lex, ch);
            lex_adv(lex);
         }
      }

      return make_tok(TOK_STR, lex->tbuf, lex->tlen);
   }

   // number literals and dot
   if (isdigit((unsigned char)cur) || cur == '.')
   {
      // dot without surrounding digits, dot token
      if (cur == '.' && !isdigit((unsigned char)lex_peek(lex)))
      {
         tbuf_put(lex, '.');
         lex_adv(lex);
         return make_tok(TOK_DOT, lex->tbuf, lex->tlen);
      }

      bool dot_seen = (cur == '.');
      while (isdigit((unsigned char)lex_cur(lex)) || lex_cur(lex) == '.')
      {
         int ch = lex_cur(lex);
         if (ch == '.')
         {
            if (!dot_seen)
               dot_seen = true;
            else
               break; // end the number at 2 dots
         }

         tbuf_put(lex, ch);
         lex_adv(lex);
      }

      return make_tok(TOK_NUM, lex->tbuf, lex->tlen);
   }

   // arrows
   if (cur == '-')
   {
      int next = lex_peek(lex);
      if (next == '>')
      {
         tbuf_put2(lex, '-', '>');
         lex_adv(lex);
         lex_adv(lex);
         return make_tok(TOK_ARROW, lex->tbuf, lex->tlen);
      }
   }
   if (cur == '=')
   {
      int next = lex_peek(lex);
      if (next == '>')
      {
         tbuf_put2(lex, '=', '>');
         lex_adv(lex);
         lex_adv(lex);
         return make_tok(TOK_DBL_ARROW, lex->tbuf, lex->tlen);
      }
   }

   // comparison ops
   if (cur == '=' || cur == '!' || cur == '<' || cur == '>')
   {
      // == or != or <= or >=
      if (lex_peek(lex) == '=')
      {
         tbuf_put2(lex, cur, '=');
         lex_adv(lex);
         lex_adv(lex);

         Token type;
         switch (cur)
         {
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

         return make_tok(type, lex->tbuf, lex->tlen);
      }

      tbuf_put(lex, cur);
      lex_adv(lex);

      Token type;
      switch (cur)
      {
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

      return make_tok(type, lex->tbuf, lex->tlen);
   }

   // binary ops
   if (cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '%')
   {
      // in-place binary op
      if (lex_peek(lex) == '=')
      {
         tbuf_put2(lex, cur, '=');
         lex_adv(lex);
         lex_adv(lex);

         Token type;
         switch (cur)
         {
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

         return make_tok(type, lex->tbuf, lex->tlen);
      }

      // regular binary op
      tbuf_put(lex, cur);
      lex_adv(lex);

      Token type;
      switch (cur)
      {
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

      return make_tok(type, lex->tbuf, lex->tlen);
   }

   // single chars
   tbuf_put(lex, cur);
   lex_adv(lex);

   Token type;
   switch (cur)
   {
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
   case ';':
      type = TOK_NEWLINE;
      break;
   default:
      type = TOK_INVALID;
      break;
   }

   return make_tok(type, lex->tbuf, lex->tlen);
}