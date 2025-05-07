/**
 * @file state.c
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
#include "state.h"

/**
 * @brief Determine if the current state's buffer is full.
 *
 * @param state    Tokenizer state
 * @return true  - If the buffer is full
 * @return false - Otherwise
 */
bool buffer_full(TokenizerState *state)
{
   return state->ptr >= state->buffer + state->bytesRead;
}

/**
 * @brief Refill the buffer, and return the number of bytes read.
 *
 * @param fp    File pointer
 * @param state Tokenizer state
 * @return int  - Number of bytes read
 */
int refill_buffer(FILE *fp, TokenizerState *state)
{
   state->bytesRead = fread(state->buffer, 1, BUFFER_SIZE, fp);
   state->ptr = state->buffer;
   return state->bytesRead;
}

/**
 * @brief Get the current char being pointed at without moving.
 *
 * @param state   Tokenizer state
 * @return char - Current char
 */
char current_char(TokenizerState *state)
{
   return *state->ptr;
}

/**
 * @brief Peek at the next character in the buffer.
 *
 * @param state   Tokenizer state
 * @return char - Next char
 */
char peek(TokenizerState *state)
{
   return *(state->ptr + 1);
}

/**
 * @brief Advance to the next char and return the current char.
 *
 * @param state   Tokenizer state
 * @return char - Char that was advanced past
 */
char advance(TokenizerState *state)
{
   return *state->ptr++;
}

/**
 * @brief Retreat to the previous char and return the current char.
 *
 * @param state   Tokenizer state
 * @return char - Char that was retreated past
 */
char retreat(TokenizerState *state)
{
   return *state->ptr--;
}

/**
 * @brief Determine whether the next character is in bounds or not.
 *
 * @param state    Tokenizer state
 * @return true  - If advancing would not go out of bounds
 * @return false - Otherwise
 */
bool next_char_in_bounds(TokenizerState *state)
{
   return state->ptr + 1 < state->buffer + state->bytesRead;
}

/**
 * @brief Move the tokenizer's pointer by a certain amount.
 *
 * @param state  Tokenizer state
 * @param amount Amount to move pointer by
 */
void move_pointer(TokenizerState *state, int amount)
{
   state->ptr += amount;
}

/**
 * @brief Initialize the tokenizer state.
 *        Can be used on an existing tokenizer state
 *        to reset it to its initial state.
 *
 * @param state Tokenizer state
 */
void initialize_tokenizer_state(TokenizerState *state)
{
   state->ptr = state->buffer;
   state->bytesRead = 0;
   // Clear buffer
   for (size_t i = 0; i < BUFFER_SIZE; i++)
   {
      state->buffer[i] = '\0';
   }
}