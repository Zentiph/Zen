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

#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#define MIN_(a, b) ((a) < (b) ? (a) : (b))

// ----- helpers -----

static inline bool _buf_full(const Lexer *lex)
{
   return lex->ptr >= lex->buf + lex->bytesRead;
}

static inline size_t _idx(const Lexer *lex)
{
   return (size_t)(lex->ptr - lex->buf);
}

static inline size_t _ahead(const Lexer *lex)
{
   // bytes in the buffer after current char
   return (size_t)(lex->buf + lex->bytesRead - lex->ptr - 1);
}

static void _push_pos(Lexer *lex)
{
   // push current (line, col) into history for unget()
   if (lex->_hist_len == LEX_KEEP_BACK)
   {
      memmove(&lex->_hist_line[0], &lex->_hist_line[1], sizeof(int) * (LEX_KEEP_BACK - 1));
      memmove(&lex->_hist_col[0], &lex->_hist_col[1], sizeof(int) * (LEX_KEEP_BACK - 1));
      lex->_hist_len--;
   }
   lex->_hist_line[lex->_hist_len] = lex->line;
   lex->_hist_col[lex->_hist_len] = lex->col;
   lex->_hist_len++;
}

static int _slide_refill(Lexer *lex)
{
   // carry up to LEX_KEEP_BACK bytes ending at current char
   // tail all unread bytes after current char
   size_t i = _idx(lex);
   size_t back = MIN_(LEX_KEEP_BACK, i + 1); // bytes to keep
   size_t tail = lex->bytesRead - (i + 1);   // unread after current

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

   lex->bytesRead = filled + got;
   return (int)got;
}

static bool _ensure_ahead(Lexer *lex, size_t need)
{
   if (_ahead(lex) >= need)
      return true;
   if (lex->bytesRead == 0)
      return false; // nothing loaded
   if (_slide_refill(lex) <= 0)
      // may still be EOF
      return _ahead(lex) >= need;
   return _ahead(lex) >= need;
}

static int _prime(Lexer *lex)
{
   // reserve LEX_KEEP_BACK bytes for unget region at front
   memset(lex->buf, 0, LEX_KEEP_BACK);
   size_t got = fread(lex->buf + LEX_KEEP_BACK, 1, LEX_BUF_SIZE - LEX_KEEP_BACK, lex->fp);
   lex->bytesRead = LEX_KEEP_BACK + got;
   lex->ptr = lex->buf + LEX_KEEP_BACK; // current char at first real byte
   return (int)got;
}

// ----- public api -----

int cur_char(Lexer *lex)
{
   if (lex->bytesRead == 0)
      if (_prime(lex) == 0)
         return EOF;
   if (_buf_full(lex))
      return EOF;
   return (unsigned char)*lex->ptr;
}

int peek(Lexer *lex)
{
   if (lex->bytesRead == 0)
      if (_prime(lex) == 0)
         return EOF;
   if (!_ensure_ahead(lex, LEX_LOOKAHEAD))
      return EOF;
   return (unsigned char)*(lex->ptr + 1);
}

int advance(Lexer *lex)
{
   if (lex->bytesRead == 0)
      if (_prime(lex) == 0)
         return EOF;
   if (!_ensure_ahead(lex, 1))
      return EOF;

   // save current pos for possible unget()
   _push_pos(lex);

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

Lexer *init_lexer(FILE *fp, const char *filename)
{
   Lexer *lex = (Lexer *)malloc(sizeof(Lexer));
   if (!lex)
      return NULL;

   lex->fp = fp;
   lex->filename = filename;
   lex->ptr = lex->buf;
   lex->bytesRead = 0;
   lex->line = 1;
   lex->col = 1;
   lex->_hist_len = 0;

   return lex;
}