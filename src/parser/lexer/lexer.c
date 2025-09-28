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
static inline bool buf_full(const Lexer *lex)
{
   return lex->ptr >= lex->buf + lex->bytes_read;
}

static inline size_t idx(const Lexer *lex)
{
   return (size_t)(lex->ptr - lex->buf);
}

static inline size_t ahead(const Lexer *lex)
{
   // bytes in the buffer after current char
   return (size_t)(lex->buf + lex->bytes_read - lex->ptr - 1);
}

static void push_pos(Lexer *lex)
{
   // push current (line, col) into history for unget()
   if (lex->_hist_len == LEX_KEEP_BACK)
   {
      memmove(
          &lex->_hist_line[0], &lex->_hist_line[1], sizeof(int) * (LEX_KEEP_BACK - 1));
      memmove(
          &lex->_hist_col[0], &lex->_hist_col[1], sizeof(int) * (LEX_KEEP_BACK - 1));
      lex->_hist_len--;
   }
   lex->_hist_line[lex->_hist_len] = lex->line;
   lex->_hist_col[lex->_hist_len] = lex->col;
   lex->_hist_len++;
}

static int slide_refill(Lexer *lex)
{
   // carry up to LEX_KEEP_BACK bytes ending at current char
   // tail all unread bytes after current char
   size_t i = idx(lex);
   size_t back = MIN_(LEX_KEEP_BACK, i + 1); // bytes to keep
   size_t tail = lex->bytes_read - (i + 1);  // unread after current

   // move back so the current char ends up at index LEX_KEEP_BACK
   if (back)
      memmove(lex->buf + (LEX_KEEP_BACK - back), lex->buf + (i + 1 - back), back);

   // move tail right after current (at LEX_KEEP_BACK + 1)
   if (tail)
      memmove(lex->buf + (LEX_KEEP_BACK + 1), lex->buf + (i + 1), tail);

   // update ptr
   lex->ptr = lex->buf + LEX_KEEP_BACK;

   // fill rest of buffer
   size_t filled = LEX_KEEP_BACK + 1 + tail;
   size_t space = (LEX_BUF_SIZE > filled) ? (LEX_BUF_SIZE - filled) : 0;
   size_t got = (space > 0) ? fread(lex->buf + filled, 1, space, lex->fp) : 0;

   lex->bytes_read = filled + got;
   return (int)got;
}

static bool ensure_ahead(Lexer *lex, size_t need)
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

static int prime(Lexer *lex)
{
   // reserve LEX_KEEP_BACK bytes for unget region at front
   memset(lex->buf, 0, LEX_KEEP_BACK);
   size_t got = fread(
       lex->buf + LEX_KEEP_BACK, 1, LEX_BUF_SIZE - LEX_KEEP_BACK, lex->fp);
   lex->bytes_read = LEX_KEEP_BACK + got;
   lex->ptr = lex->buf + LEX_KEEP_BACK; // current char at first real byte
   return (int)got;
}

static inline void tbuf_reset(Lexer *lex)
{
   lex->tlen = 0;
   if (TOK_SIZE > 0)
      lex->tbuf[0] = '\0';
}

static inline void tbuf_put(Lexer *lex, int ch)
{
   if (lex->tlen + 1 < TOK_SIZE) // leave room for NULL
   {
      lex->tbuf[lex->tlen++] = (char)ch;
      lex->tbuf[lex->tlen] = '\0';
   }
}

static inline void tbuf_put2(Lexer *lex, int a, int b)
{
   tbuf_put(lex, a);
   tbuf_put(lex, b);
}

// ----- public api -----
static const char *kw_tab[] = {
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

Lexer *init_lexer(FILE *fp, const char *filename)
{
   Lexer *lex = (Lexer *)malloc(sizeof(Lexer));
   if (!lex)
      return NULL;

   lex->fp = fp;
   lex->filename = filename;
   lex->ptr = lex->buf;
   lex->bytes_read = 0;
   lex->line = 1;
   lex->col = 1;
   lex->tlen = 0;
   if (TOK_SIZE > 0)
      lex->tbuf[0] = '\0';
   lex->_hist_len = 0;

   return lex;
}

int cur_char(Lexer *lex)
{
   if (lex->bytes_read == 0)
      if (prime(lex) == 0)
         return EOF;
   if (buf_full(lex))
      return EOF;
   return (unsigned char)*lex->ptr;
}

int peek(Lexer *lex)
{
   if (lex->bytes_read == 0)
      if (prime(lex) == 0)
         return EOF;
   if (!ensure_ahead(lex, LEX_LOOKAHEAD))
      return EOF;
   return (unsigned char)*(lex->ptr + 1);
}

int advance(Lexer *lex)
{
   if (lex->bytes_read == 0)
      if (prime(lex) == 0)
         return EOF;
   if (!ensure_ahead(lex, 1))
      return EOF;

   // save current pos for possible unget()
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

bool unget(Lexer *lex)
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

void skip(Lexer *lex, int n)
{
   if (n <= 0)
      return;
   while (n-- > 0)
      if (advance(lex) == EOF)
         return;
}

void skip_wsp(Lexer *lex)
{
   for (;;)
   {
      int cur = cur_char(lex);
      if (cur == EOF)
         return;
      if (!isspace((unsigned char)cur))
         return;
      if (cur == '\n')
         return; // preserve newline as a token
      advance(lex);
   }
}

Token next_tok(Lexer *lex)
{
   Token tok;
   tbuf_reset(lex);

   skip_wsp(lex);
   int cc = cur_char(lex);
   if (cc == EOF)
   {
      tok.type = TOK_EOF;
      tok.val = lex->tbuf;
      tok.len = 0;
      return tok;
   }

   char cur = (char)cc;

   // single-line comments
   if (cur == '/' && peek(lex) == '/')
   {
      tok.type = TOK_COMMENT;
      skip(lex, 2); // skip "//"

      while (cur_char(lex) != '\n' && cur_char(lex) != EOF)
         tbuf_put(lex, advance(lex));

      tok.val = lex->tbuf;
      tok.len = lex->tlen;
      return tok;
   }

   // multi-line comments
   if (cur == '/' && peek(lex) == '.')
   {
      tok.type = TOK_COMMENT;
      skip(lex, 2); // skip opening "/."

      while (true)
      {
         // end of comment
         if (cur_char(lex) == '.' && peek(lex) == '/')
         {
            skip(lex, 2); // skip closing "./"
            break;
         }
         if (cur_char(lex) == EOF)
            break;

         tbuf_put(lex, advance(lex));
      }

      tok.val = lex->tbuf;
      tok.len = lex->tlen;
      return tok;
   }

   // keywords and identifiers
   if (isalpha((unsigned char)cur) || cur == '_')
   {
      tbuf_put(lex, cur);

      while (isalnum((unsigned char)cur_char(lex)) || cur_char(lex) == '_')
      {
         tbuf_put(lex, advance(lex));
      }

      // check for keyword
      for (int i = 0; kw_tab[i] != NULL; i++)
      {
         if (strcmp(lex->tbuf, kw_tab[i]) == 0)
         {
            tok.type = TOK_KW;
            tok.val = lex->tbuf;
            tok.len = lex->tlen;
            return tok;
         }
      }

      tok.type = TOK_ID;
      tok.val = lex->tbuf;
      tok.len = lex->tlen;
      return tok;
   }

   // string literals (quotes not included in token)
   if (cur == '"' || cur == '\'')
   {
      char quote = cur; // store quote type for consistency
      tok.type = TOK_STR;

      while (true)
      {
         if (peek(lex) == '\\')
         {
            skip(lex, 1); // skip the '\'
            char esc = advance(lex);
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
         }
         else if (cur_char(lex) != quote)
         {
            int a = advance(lex);
            if (a == EOF || a == '\n')
               break; // unterminated string, stop
            tbuf_put(lex, a);
         }
         else
         {
            break; // found closing quote
         }
      }

      skip(lex, 1); // skip final quote
      tok.val = lex->tbuf;
      tok.len = lex->tlen;
      return tok;
   }

   // number literals and dot
   if (isdigit((unsigned char)cur) || cur == '.')
   {
      // dot without surrounding digits, dot token
      if (cur == '.' && !isdigit((unsigned char)peek(lex)))
      {
         tok.type = TOK_DOT;
         tbuf_put(lex, '.');
         tok.val = lex->tbuf;
         tok.len = 1;
         return tok;
      }

      tok.type = TOK_NUM;
      tbuf_put(lex, cur);
      bool dot_seen = (cur == '.');

      while (isdigit((unsigned char)peek(lex)) || peek(lex) == '.')
      {
         if (peek(lex) == '.')
         {
            if (!dot_seen)
               dot_seen = true;
            else
               break; // end the number at 2 dots
         }

         tbuf_put(lex, advance(lex));
      }

      tok.val = lex->tbuf;
      tok.len = lex->tlen;
      return tok;
   }

   // arrows
   if (cur == '-')
   {
      char next = advance(lex);
      if (next == '>')
      {
         tok.type = TOK_ARROW;
         tbuf_put2(lex, '-', '>');
         tok.val = lex->tbuf;
         tok.len = 2;
         return tok;
      }

      // didn't need the next char, unget
      unget(lex);
   }
   if (cur == '=')
   {
      char next = advance(lex);
      if (next == '>')
      {
         tok.type = TOK_DBL_ARROW;
         tbuf_put2(lex, '=', '>');
         tok.val = lex->tbuf;
         tok.len = 2;
         return tok;
      }

      // didn't need the next char, unget
      unget(lex);
   }

   // comparison ops
   if (cur == '=' || cur == '!' || cur == '<' || cur == '>')
   {
      char next = advance(lex);
      // == or != or <= or >=
      if (next == '=')
      {
         switch (cur)
         {
         case '=':
            tok.type = TOK_EQ;
            break;
         case '!':
            tok.type = TOK_NE;
            break;
         case '<':
            tok.type = TOK_LE;
            break;
         case '>':
            tok.type = TOK_GE;
            break;
         default:
            tok.type = TOK_INVALID;
            break;
         }

         tbuf_put2(lex, cur, '=');
         tok.val = lex->tbuf;
         tok.len = 2;
         return tok;
      }

      // didn't need the next char, unget
      unget(lex);

      switch (cur)
      {
      case '=':
         tok.type = TOK_ASSIGN;
         break;
      case '!':
         tok.type = TOK_NOT;
         break;
      case '<':
         tok.type = TOK_LT;
         break;
      case '>':
         tok.type = TOK_GT;
         break;
      default:
         tok.type = TOK_INVALID;
         break;
      }

      tbuf_put(lex, cur);
      tok.val = lex->tbuf;
      tok.len = 1;
      return tok;
   }

   // binary ops
   if (cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '%')
   {
      char next = advance(lex);
      // in-place binary op
      if (next == '=')
      {
         switch (cur)
         {
         case '+':
            tok.type = TOK_ADD_ASSIGN;
            break;
         case '-':
            tok.type = TOK_SUB_ASSIGN;
            break;
         case '*':
            tok.type = TOK_MUL_ASSIGN;
            break;
         case '/':
            tok.type = TOK_DIV_ASSIGN;
            break;
         case '%':
            tok.type = TOK_MOD_ASSIGN;
            break;
         default:
            tok.type = TOK_INVALID;
            break;
         }

         tbuf_put2(lex, cur, '=');
         tok.val = lex->tbuf;
         tok.len = 2;
         return tok;
      }

      // didn't need the next char, unget
      unget(lex);

      // regular binary op
      switch (cur)
      {
      case '+':
         tok.type = TOK_ADD;
         break;
      case '-':
         tok.type = TOK_SUB;
         break;
      case '*':
         tok.type = TOK_MUL;
         break;
      case '/':
         tok.type = TOK_DIV;
         break;
      case '%':
         tok.type = TOK_MOD;
         break;
      default:
         tok.type = TOK_INVALID;
         break;
      }

      tbuf_put(lex, cur);
      tok.val = lex->tbuf;
      tok.len = 1;
      return tok;
   }

   // single chars
   switch (cur)
   {
   case ',':
      tok.type = TOK_COMMA;
      break;
   case '(':
      tok.type = TOK_LT_PAREN;
      break;
   case ')':
      tok.type = TOK_RT_PAREN;
      break;
   case '[':
      tok.type = TOK_LT_BRACK;
      break;
   case ']':
      tok.type = TOK_RT_BRACK;
      break;
   case '{':
      tok.type = TOK_LT_BRACE;
      break;
   case '}':
      tok.type = TOK_RT_BRACE;
      break;
   case '\n':
   case ';':
      tok.type = TOK_NEWLINE;
      break;
   default:
      tok.type = TOK_INVALID;
      break;
   }

   tbuf_put(lex, cur);
   tok.val = lex->tbuf;
   tok.len = 1;
   return tok;
}