/**
 * @file state.h
 * @author Gavin Borne
 * @brief Tokenizer state and helper functions header for the Zen programming language
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

// TODO: ORGANIZE ORDER OF FUNCS IN ALL FILES

#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

/**
 * @brief A representation of the tokenizer's state.
 */
typedef struct
{
   FILE *fp;
   char buffer[BUFFER_SIZE];
   char *ptr;
   size_t bytesRead;
   int line;
   int column;
} Tokenizer;

/**
 * @brief A snapshot of a tokenizer, used to restore previous states.
 */
typedef struct
{
   char buffer[BUFFER_SIZE];
   ptrdiff_t ptrOffset;
   size_t bytesRead;
   int line;
   int column;
   fpos_t filePos;
} TokenizerSnapshot;

/**
 * @brief Determine if the current tokenizer's buffer is full.
 *
 * @param tokenizer Tokenizer
 * @return true     If the buffer is full
 * @return false    Otherwise
 */
bool buffer_full(Tokenizer *tokenizer);

/**
 * @brief Refill the buffer, and return the number of bytes read.
 *
 * @param tokenizer Tokenizer
 * @return int      Number of bytes read
 */
int refill_buffer(Tokenizer *tokenizer);

/**
 * @brief Get the current char being pointed at without moving.
 *
 * @param tokenizer Tokenizer
 * @return char     Current char
 */
char current_char(Tokenizer *tokenizer);

/**
 * @brief Peek at the next character in the buffer.
 *
 * @param tokenizer Tokenizer
 * @return char     Next char
 */
char peek(Tokenizer *tokenizer);

/**
 * @brief Advance to the next char and return the current char.
 *
 * @param tokenizer Tokenizer
 * @return char     Char that was advanced past
 */
char advance(Tokenizer *tokenizer);

/**
 * @brief Determine whether the next character is in bounds or not.
 *
 * @param tokenizer Tokenizer state
 * @return true     If advancing would not go out of bounds
 * @return false    Otherwise
 */
bool next_char_in_bounds(Tokenizer *tokenizer);

/**
 * @brief Move the tokenizer's pointer by a certain amount.
 *
 * @param tokenizer Tokenizer
 * @param amount    Amount to move pointer by
 */
void move_pointer(Tokenizer *tokenizer, int amount);

/**
 * @brief Initialize the tokenizer.
 *        This function an be used on an existing
 *        tokenizer to reset it with a new file
 *        or rewind the existing file.
 *
 * @param tokenizer Tokenizer
 * @param fp        File pointer
 */
void initialize_tokenizer(Tokenizer *tokenizer, FILE *fp);

/**
 * @brief Save a tokenizer's state to a snapshot.
 *
 * @param tokenizer Tokenizer to save the state of
 * @param snapshot  Snapshot to save into
 */
void save_tokenizer_state(Tokenizer *tokenizer, TokenizerSnapshot *snapshot);

/**
 * @brief Load a stored snapshot into a tokenizer.
 *
 * @param tokenizer Tokenizer to load state into
 * @param snapshot  State to load
 */
void load_tokenizer_state(Tokenizer *tokenizer, const TokenizerSnapshot *snapshot);

#endif // STATE_H