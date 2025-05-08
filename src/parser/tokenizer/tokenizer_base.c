/**
 * @file parser/tokenizer/tokenizer_base.c
 * @author Gavin Borne
 * @brief Tokenizer state and helper functions for the Zen programming language
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer_base.h"

/**
 * @brief Determine if the current tokenizer's buffer is full.
 *
 * @param tokenizer Tokenizer
 * @return true     If the buffer is full
 * @return false    Otherwise
 */
bool buffer_full(Tokenizer *tokenizer)
{
   return tokenizer->ptr >= tokenizer->buffer + tokenizer->bytesRead;
}

/**
 * @brief Refill the buffer, and return the number of bytes read.
 *
 * @param tokenizer Tokenizer
 * @return int      Number of bytes read
 */
int refill_buffer(Tokenizer *tokenizer)
{
   tokenizer->bytesRead = fread(tokenizer->buffer, 1, BUFFER_SIZE, tokenizer->fp);
   tokenizer->ptr = tokenizer->buffer;
   return tokenizer->bytesRead;
}

/**
 * @brief Get the current char being pointed at without moving.
 *
 * @param tokenizer Tokenizer
 * @return char     Current char
 */
char current_char(Tokenizer *tokenizer)
{
   return *tokenizer->ptr;
}

/**
 * @brief Peek at the next character in the buffer.
 *
 * @param tokenizer Tokenizer
 * @return char     Next char
 */
char peek(Tokenizer *tokenizer)
{
   return *(tokenizer->ptr + 1);
}

/**
 * @brief Advance to the next char return it.
 *
 * @param tokenizer Tokenizer
 * @return char     New char
 */
char advance(Tokenizer *tokenizer)
{
   char next = *tokenizer->ptr++;

   if (next == '\n')
   {
      tokenizer->line++;
      tokenizer->column = 1;
   }
   else
   {
      tokenizer->column++;
   }

   return next;
}

/**
 * @brief Determine whether the next character is in bounds or not.
 *
 * @param tokenizer Tokenizer state
 * @return true     If advancing would not go out of bounds
 * @return false    Otherwise
 */
bool next_char_in_bounds(Tokenizer *tokenizer)
{
   return tokenizer->ptr + 1 < tokenizer->buffer + tokenizer->bytesRead;
}

/**
 * @brief Move the tokenizer's pointer by a certain amount.
 *
 * @param tokenizer Tokenizer
 * @param amount    Amount to move pointer by
 */
void move_pointer(Tokenizer *tokenizer, int amount)
{
   tokenizer->ptr += amount;
}

/**
 * @brief Create a tokenizer object.
 *
 * @param fp            File pointer
 * @return Tokenizer* - Tokenizer object
 */
Tokenizer *create_tokenizer(FILE *fp)
{
   Tokenizer *tokenizer = malloc(sizeof(Tokenizer));
   tokenizer->fp = fp;
   tokenizer->ptr = tokenizer->buffer;
   tokenizer->bytesRead = 0;
   tokenizer->line = 1;
   tokenizer->column = 1;
   return tokenizer;
}

/**
 * @brief Reset the tokenizer with a new file,
 *        or rewind the existing file.
 *
 * @param tokenizer Tokenizer
 * @param fp        File pointer
 */
void reset_tokenizer(Tokenizer *tokenizer, FILE *fp)
{
   rewind(fp);
   tokenizer->fp = fp;
   tokenizer->ptr = tokenizer->buffer;
   tokenizer->bytesRead = 0;
   tokenizer->line = 1;
   tokenizer->column = 1;
   // Clear buffer
   for (size_t i = 0; i < BUFFER_SIZE; i++)
   {
      tokenizer->buffer[i] = '\0';
   }
}

/**
 * @brief Save a tokenizer's state to a snapshot.
 *
 * @param tokenizer Tokenizer to save the state of
 * @param snapshot  Snapshot to save into
 */
void save_tokenizer_state(Tokenizer *tokenizer, TokenizerSnapshot *snapshot)
{
   memcpy(snapshot->buffer, tokenizer->buffer, BUFFER_SIZE);
   snapshot->ptrOffset = tokenizer->ptr - tokenizer->buffer;
   snapshot->bytesRead = tokenizer->bytesRead;
   snapshot->line = tokenizer->line;
   snapshot->column = tokenizer->column;
   fgetpos(tokenizer->fp, &snapshot->filePos);
}

/**
 * @brief Load a stored snapshot into a tokenizer.
 *
 * @param tokenizer Tokenizer to load state into
 * @param snapshot  State to load
 */
void load_tokenizer_state(Tokenizer *tokenizer, const TokenizerSnapshot *snapshot)
{
   memcpy(tokenizer->buffer, snapshot->buffer, BUFFER_SIZE);
   tokenizer->ptr = tokenizer->buffer + snapshot->ptrOffset;
   tokenizer->bytesRead = snapshot->bytesRead;
   tokenizer->line = snapshot->line;
   tokenizer->column = snapshot->column;
   fsetpos(tokenizer->fp, &snapshot->filePos);
}