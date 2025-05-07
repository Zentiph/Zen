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
   /** A buffer of size BUFFER_SIZE to hold chars */
   char buffer[BUFFER_SIZE];
   /** Pointer to the buffer */
   char *ptr;
   /** The number of bytes read from the most recent fread operation */
   size_t bytesRead;
} TokenizerState;

/**
 * @brief Determine if the current state's buffer is full.
 *
 * @param state    Tokenizer state
 * @return true  - If the buffer is full
 * @return false - Otherwise
 */
bool buffer_full(TokenizerState *state);

/**
 * @brief Refill the buffer, and return the number of bytes read.
 *
 * @param fp    File pointer
 * @param state Tokenizer state
 * @return int  - Number of bytes read
 */
int refill_buffer(FILE *fp, TokenizerState *state);

/**
 * @brief Get the current char being pointed at without moving.
 *
 * @param state   Tokenizer state
 * @return char - Current char
 */
char current_char(TokenizerState *state);

/**
 * @brief Peek at the next character in the buffer.
 *
 * @param state   Tokenizer state
 * @return char - Next char
 */
char peek(TokenizerState *state);

/**
 * @brief Advance to the next char and return the current char.
 *
 * @param state   Tokenizer state
 * @return char - Char that was advanced past
 */
char advance(TokenizerState *state);

/**
 * @brief Retreat to the previous char and return the current char.
 *
 * @param state   Tokenizer state
 * @return char - Char that was retreated past
 */
char retreat(TokenizerState *state);

/**
 * @brief Determine whether the next character is in bounds or not.
 *
 * @param state    Tokenizer state
 * @return true  - If advancing would not go out of bounds
 * @return false - Otherwise
 */
bool next_char_in_bounds(TokenizerState *state);

/**
 * @brief Move the tokenizer's pointer by a certain amount.
 *
 * @param state  Tokenizer state
 * @param amount Amount to move pointer by
 */
void move_pointer(TokenizerState *state, int amount);

/**
 * @brief Initialize the tokenizer state if its pointer is NULL.
 *
 * @param state    Tokenizer state
 * @return true  - If the state needed to be initialized and was therefore changed
 * @return false - Otherwise
 */
void initialize_tokenizer_state(TokenizerState *state);

#endif // STATE_H