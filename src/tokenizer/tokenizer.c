/**
 * @file tokenizer.c
 * @author Gavin Borne
 * @brief Tokenizer for the Zen programming language
 *
 * @copyright Copyright (c) 2025 Gavin Borne
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software
 * and associated documentation files (the "Software"),
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
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

const char BINARY_OPERATORS[] = {'+', '-', '*', '/', '%', '='};

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
void skip_whitespace(FILE *fp, char **ptr, char *buffer, size_t *bytesRead, size_t bufferSize);

/**
 * @brief Get the next token in the file.
 *
 * @param fp       File pointer
 * @return Token - The token found
 */
Token next_token(FILE *fp);

/**
 * @brief Print a representation of a token.
 *
 * @param token Token to print
 */
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
            while (isspace(ch) && ch != NEWLINE_CHAR) {
                (*ptr)++;
                if (*ptr >= buffer + *bytesRead) {
                    *bytesRead = fread(buffer, 1, bufferSize, fp);
                    *ptr = buffer;
                    if (*bytesRead == 0) { // EOF reached
                        break;
                    }
                }
                ch = **ptr;
            }
        } else {
            break;
        }
    }
}

// TODO
// Ensure buffer is handled correctly (if end is reached, it is filled without breaking and causing an invalid token)

// TODO
// If possible make helper funcs to prevent repetition (mainly for checking if buffer needs refill and refilling it)
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

        while (true) {
            // Refill the buffer as needed
            if (ptr >= buffer + bytesRead) {
                bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
                ptr = buffer;
                if (bytesRead == 0) break; // EOF reached
            }
            if (*ptr == NEWLINE_CHAR) break;
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

        while (true) {
            // Refill the buffer as needed
            if (ptr >= buffer + bytesRead) {
                bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
                ptr = buffer;
                if (bytesRead == 0) break; // EOF reached
            }
            // End of comment
            if (*ptr == '.' && (ptr + 1 < buffer + bytesRead && *(ptr + 1) == '/')) {
                ptr += 2; // Skip the closing "./"
                break;
            }
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle strings (outer quotes not included)
    if (ch == '"' || ch == '\'') {
        char quote = ch; // Keep quote type consistent
        token.type = TOKEN_STRING;
        int i = 0;

        while (true) {
            // Refill the buffer if needed
            if (ptr >= buffer + bytesRead) {
                bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
                ptr = buffer;
                if (bytesRead == 0) break; // EOF reached
            }

            // Check for escape coded quotes
            if (*ptr == '\\') {
                // Increment twice to skip a "\'" or "\""
                token.value[i++] = *ptr++;
                if (i >= MAX_TOKEN_LENGTH - 1) break;
                token.value[i++] = *ptr++;
                if (i >= MAX_TOKEN_LENGTH - 1) break;

            } else if (*ptr != quote) {
                token.value[i++] = *ptr++;
                if (i >= MAX_TOKEN_LENGTH - 1) break;

            } else break; // Break if we hit the end of the string
        }
        ptr++; // Move past final quote
        token.value[i] = '\0';
        return token;
    }

    // Handle identifiers
    // (First character of an identifier cannot be a digit)
    if (isalpha(ch) || ch == '_') {
        int i = 0;
        token.value[i++] = ch;

        while (isalnum(*ptr) || *ptr == '_') {
            // Refill the buffer as needed
            if (ptr >= buffer + bytesRead) {
                bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
                ptr = buffer;
                if (bytesRead == 0) break; // EOF reached
            }

            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }
        token.value[i] = '\0';

        // Check for logical operators
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

        while (isdigit(*ptr)) {
            // Refill the buffer as needed
            if (ptr >= buffer + bytesRead) {
                bytesRead = fread(buffer, 1, BUFFER_SIZE, fp);
                ptr = buffer;
                if (bytesRead == 0) break; // EOF reached
            }

            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }

        token.value[i] = '\0';
        return token;
    }

    // Handle arrows
    if (ch == '-') {
        char next = *ptr++;
        if (next == '>') {
            token.type = TOKEN_ARROW;
            token.value[0] = ch;
            token.value[1] = next;
            token.value[2] = '\0';
            return token;
        }
        // If we didn't need the next char, decrement ptr
        ptr--;
    }
    if (ch == '=') {
        char next = *ptr++;
        if (next == '>') {
            token.type = TOKEN_DBL_ARROW;
            token.value[0] = ch;
            token.value[1] = next;
            token.value[2] = '\0';
            return token;
        }
        // If we didn't need the next char, decrement ptr
        *ptr--;
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

    // Handle in-place and regular binary operators
    // (all in-place binary operators are string of length 2)
    if (ch != '=' && char_in_arr(ch, BINARY_OPERATORS, sizeof BINARY_OPERATORS)) {
        char next = *ptr++;
        // In-place binary op
        if (next == '=') {
            token.type = TOKEN_IP_BINARY_OP;
            token.value[0] = ch;
            token.value[1] = next;
            token.value[2] = '\0';
            return token;
        }

        // If we didn't need the next char, decrement ptr
        *ptr--;

        // Regular binary op
        token.type = TOKEN_BINARY_OP;
        token.value[0] = ch;
        token.value[2] = '\0';
        return token;
    }

    // Handle single characters
    if (ch == '.') {
        token.type = TOKEN_DOT;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }

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
        "TOKEN_STRING        ",
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

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: tokenizer <source>\n");
        return 1;
    }

    // Check if the file is a .zen file
    size_t fileLen = strlen(argv[1]);
    int suffixLen = 4; // ".zen"
    if (strcmp(argv[1] + fileLen - suffixLen, ".zen") != 0) {
        fprintf(stderr, "tokenizer source input must be a .zen file");
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
    print_token(token); // Print EOF

    fclose(fp);
    return 0;
}
