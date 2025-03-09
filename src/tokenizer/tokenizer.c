/**
 * @file tokenizer.c
 * @author Gavin Borne
 * @brief Tokenizer for the Zen programming language
 *
 * @copyright Copyright (c) 2025 Gavin Borne
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software
 * and associated documentation files (the “Software”),
 * to deal in the Software without restriction,
 * including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "tokenizer.h"

#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TOKEN_LENGTH 100
#define NEWLINE_CHAR '\n'
#define BUFFER_SIZE 1024

const char BINARY_OPERATORS[] = {'+', '-', '*', '/', '%', '='};
const char *IN_PLACE_BINARY_OPERATORS[] = {"+=", "-=", "*=", "/=", "%="};
const char *COMPARISON_OPERATORS[] = {"==", "!=", ">", "<", ">=", "<="};
const char *LOGIC_OPERATORS[] = {"and", "or", "not"};

void skip_whitespace(FILE *fp, char **ptr, char *buffer, size_t *bytesRead, size_t bufferSize);
Token next_token(FILE *fp);
void print_token(Token token);

/**
 * @brief Check if a char is in a char array.
 *
 * @param ch       Char
 * @param arr      Array
 * @param size     Size of arr
 * @return true  - If ch is in arr
 * @return false - If ch is not in arr
 */
bool char_in_arr(char ch, const char *arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == ch) return true;
    }
    return false;
}

/**
 * @brief Check if a string is in a string array.
 *
 * @param str      String
 * @param arr      Array
 * @param size     Size of arr
 * @return true  - If str is in arr
 * @return false - If str is not in arr
 */
bool string_in_arr(const char *str, const char *arr[], int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(str, arr[i]) == 0) return true;
    }
    return false;
}

/**
 * @brief Skip the meaningless whitespace in the
 *        file until a token is found.
 *        Newline characters as well as semicolons
 *        are valid whitespace, so track those.
 *
 * @param fp         File pointer
 * @param ptr        Pointer to the current position in the buffer
 * @param buffer     The buffer containing file data
 * @param bytesRead  Pointer to the count of valid bytes in buffer
 * @param bufferSize The size of the buffer
 */
void skip_whitespace(FILE *fp, char **ptr, char *buffer, size_t *bytesRead, size_t bufferSize) {
    while (true) {
        // Refill buffer if needed
        if (*ptr >= buffer + *bytesRead) {
            *bytesRead = fread(buffer, 1, bufferSize, fp);
            *ptr = buffer;
            if (*bytesRead == 0) { // EOF reached
                break;
            }
        }

        char ch = **ptr;
        if (isspace(ch)) {
            if (ch == NEWLINE_CHAR) {
                // Preserve newline as a token, don't skip
                break;
            }
            // Otherwise skip the whitespace
            (*ptr)++;
        } else {
            break;
        }
    }
}

/**
 * @brief Get the next token in the file.
 *
 * @param fp       File pointer
 * @return Token - The token found
 */
Token next_token(FILE *fp) {
    static char buffer[BUFFER_SIZE];
    static char *ptr = buffer;
    static size_t bytesRead = 0;

    Token token;
    token.value[0] = '\0'; // Reset token value

    // Refill the buffer if needed and check for EOF
    if (ptr >= buffer + bytesRead) {
        bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
        ptr = buffer;
        if (bytesRead == 0) {
            token.type = TOKEN_EOF;
            return token;
        }
    }

    // Skip over whitespace in the buffer
    skip_whitespace(fp, &ptr, buffer, &bytesRead, BUFFER_SIZE);
    // Check if we skipped to the end of the buffer
    if (ptr >= buffer + bytesRead) {
        bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
        ptr = buffer;
        if (bytesRead == 0) {
            token.type = TOKEN_EOF;
            return token;
        }
    }

    // Read next char from the buffer
    char ch = *ptr++;

    // Handle single-line comments
    if (ch == '/' && ptr < buffer + bytesRead && *ptr == '/') {
        token.type = TOKEN_COMMENT;
        int i = 0;
        // Skip the second '/'
        ptr++;

        while (ptr < buffer + bytesRead && *ptr != NEWLINE_CHAR) {
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }

        token.value[i] = '\0';
        return token;
    }

    // Handle multi-line comments (/. ... ./)
    if (ch == '/' && ptr < buffer + bytesRead && *ptr == '.') {
        token.type = TOKEN_COMMENT;
        int i = 0;
        // Skip the '.'
        ptr++;

        while (ptr < buffer + bytesRead) {
            // End of comment
            if (*ptr == '.' && (ptr + 1 < buffer + bytesRead && *(ptr + 1)) == '/') {
                ptr += 2; // Skip the closing "./"
                break;
            }
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle identifiers
    // (First character of an identifier cannot be a digit)
    if (isalpha(ch) || ch == '_') {
        int i = 0;
        token.value[i++] = ch;

        while (ptr < buffer + bytesRead && (isalnum(*ptr) || *ptr == '_')) {
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }
        token.value[i] = '\0';

        // Ok, there's probably a better way of doing this,
        // however it is 4am and i want to die and i can't
        // be bothered to change this so suck it up and cry about it
        if ((      token.value[0] == 'a'
                && token.value[1] == 'n'
                && token.value[2] == 'd'
                && token.value[3] == '\0')
               || (token.value[0] == 'o'
                && token.value[1] == 'r'
                && token.value[2] == '\0')
               || (token.value[0] == 'n'
                && token.value[1] == 'o'
                && token.value[2] == 't'
                && token.value[3] == '\0'))
        {
            token.type = TOKEN_LOGIC_OP;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }

        return token;
    }

    // Handle numbers
    if (isdigit(ch)) {
        token.type = TOKEN_NUMBER;
        int i = 0;
        token.value[i++] = ch;

        while (ptr < buffer + bytesRead && isdigit(*ptr)) {
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle comparison operators
    if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
        char next = *ptr++;
        // '==' or '!=' or '<=' or '>='
        if (next == '=') {
            token.type = TOKEN_COMPARISON_OP;
            token.value[0] = ch;
            token.value[1] = next;
            token.value[2] = '\0';
            return token;
        }

        // If we didn't need the next char,
        // decrement the ptr
        *ptr--;
        // '<' or '>'
        if (ch == '<' || ch == '>') {
            token.type = TOKEN_COMPARISON_OP;
            token.value[0] = ch;
            token.value[1] = '\0';
            return token;
        }
        // Continue on if the cases didn't work
        // (like '=')
    }

    // Handle in-place binary operators
    // (all in-place binary operators are string of length 2)
    if (ch != '=' && char_in_arr(ch, BINARY_OPERATORS, sizeof BINARY_OPERATORS)) {
        token.type = TOKEN_IP_BINARY_OP;
        token.value[0] = ch;
        token.value[1] = *ptr++;
        token.value[2] = '\0';
        return token;
    }

    // Handle binary operators
    if (char_in_arr(ch, BINARY_OPERATORS, sizeof BINARY_OPERATORS)) {
        token.type = TOKEN_BINARY_OP;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }

    // Handle parenthesis, brackets, and curly braces
    if (ch == '(') {
        token.type = TOKEN_LT_PAREN;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == ')') {
        token.type = TOKEN_RT_PAREN;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == '[') {
        token.type = TOKEN_LT_BRACK;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == ']') {
        token.type = TOKEN_RT_BRACK;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == '{') {
        token.type = TOKEN_LT_CURLY;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == '}') {
        token.type = TOKEN_RT_CURLY;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }

    // Handle new code lines
    if (ch == '\n' || ch == ';') {
        token.type = TOKEN_NEWLINE;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }

    // If none of the above, the token is invalid
    token.type = TOKEN_INVALID;
    token.value[0] = ch;
    token.value[1] = '\0';
    return token;
}

/**
 * @brief Print a representation of a token.
 *
 * @param token Token to print
 */
void print_token(Token token) {
    const char *token_types[] = {
        // Make sure these align with the defined token
        // types in TokenType from tokenizer.h

        // Make these have equal length for readability
        "TOKEN_COMMENT       ",
        "TOKEN_IDENTIFIER    ",
        "TOKEN_NUMBER        ",
        "TOKEN_BINARY_OP     ",
        "TOKEN_IP_BINARY_OP  ",
        "TOKEN_COMPARISON_OP ",
        "TOKEN_LOGIC_OP      ",
        "TOKEN_SGL_QTE       ",
        "TOKEN_DBL_QTE       ",
        "TOKEN_LT_PAREN      ",
        "TOKEN_RT_PAREN      ",
        "TOKEN_LT_BRACK      ",
        "TOKEN_RT_BRACK      ",
        "TOKEN_LT_CURLY      ",
        "TOKEN_RT_CURLY      ",
        "TOKEN_DOT           ",
        "TOKEN_ARROW         ",
        "TOKEN_DBL_ARROW     ",
        "TOKEN_NEWLINE       ",
        "TOKEN_EOF           ",
        "TOKEN_INVALID       "
    };

    // Newline representation is "\\n"
    // (to prevent gaps in output)
    if (token.value[0] == '\n') {
        printf("%s: '%s'\n", token_types[token.type], "\\n");
    } else {
        printf("%s: '%s'\n", token_types[token.type], token.value);
    }
}

int main(int argc, char const *argv[]) {;
    if (argc != 2) {
        fprintf(stderr, "Usage: tokenizer <source>\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Unable to open file");
        return 1;
    }

    Token token;
    while ((token = next_token(fp)).type != TOKEN_EOF) {
        print_token(token);
    }

    fclose(fp);
    return 0;
}
