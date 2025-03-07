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

#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TOKEN_LENGTH 100
#define NEWLINE_CHAR '\n'
#define BUFFER_SIZE 1024

const char BINARY_OPERATORS[] = {'+', '-', '*', '/', '='};

/**
 * @brief All token types.
 *        FILEEND is used instead of EOF to avoid
 *        name conflicts with stdio.h.
 */
typedef enum {
    COMMENT,
    IDENTIFIER,
    CONSTANT,
    BIN_OPERATOR,
    LT_PAREN,   // '('
    RT_PAREN,   // ')'
    LT_BRACKET, // '['
    RT_BRACKET, // ']'
    LT_CURLY,   // '{'
    RT_CURLY,   // '}'
    NEWLINE,    // '\n' or ';'
    FILEEND,    // EOF
    INVALID
} TokenType;

/**
 * @brief A token that contains its type and value.
 */
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

bool char_in_arr(char val, const char *arr, size_t n);
void skip_whitespace(FILE *fp, char **ptr, char *buffer, size_t *bytesRead, size_t bufferSize);
Token next_token(FILE *fp);
void print_token(Token token);

int main(int argc, char const *argv[]) {
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
    while ((token = next_token(fp)).type != FILEEND) {
        print_token(token);
    }

    fclose(fp);
    return 0;
}

/**
 * @brief Check if a char is in an array
 *
 * @param ch       Char
 * @param arr      Array
 * @param n        Size of arr
 * @return true  - If ch is in arr
 * @return false - If ch is not in arr
 */
bool char_in_arr(char ch, const char *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (arr[i] == ch)
            return true;
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
            token.type = FILEEND;
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
            token.type = FILEEND;
            return token;
        }
    }

    // Read next char from the buffer
    char ch = *ptr++;

    // Handle single-line comments
    if (ch == '/' && ptr < buffer + bytesRead && *ptr == '/') {
        token.type = COMMENT;
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
        token.type = COMMENT;
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

    // Handle variable declaration
    // (First character of an identifier cannot be a digit)
    if (isalpha(ch) || ch == '_') {
        token.type = IDENTIFIER;
        int i = 0;
        token.value[i++] = ch;

        while (ptr < buffer + bytesRead && (isalnum(*ptr) || *ptr == '_')) {
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle numbers (constants)
    if (isdigit(ch)) {
        token.type = CONSTANT;
        int i = 0;
        token.value[i++] = ch;

        while (ptr < buffer + bytesRead && isdigit(*ptr)) {
            token.value[i++] = *ptr++;
            if (i >= MAX_TOKEN_LENGTH - 1) break;
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle binary operators
    if (char_in_arr(ch, BINARY_OPERATORS, sizeof BINARY_OPERATORS)) {
        token.type = BIN_OPERATOR;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }

    // Handle parenthesis, brackets, and curly braces
    if (ch == '(') {
        token.type = LT_PAREN;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == ')') {
        token.type = RT_PAREN;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == '[') {
        token.type = LT_BRACKET;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == ']') {
        token.type = RT_BRACKET;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == '{') {
        token.type = LT_CURLY;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }
    if (ch == '}') {
        token.type = RT_CURLY;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }

    // Handle new code lines
    if (ch == '\n' || ch == ';') {
        token.type = NEWLINE;
        token.value[0] = ch;
        token.value[1] = '\0';
        return token;
    }

    // If none of the above, the token is invalid
    token.type = INVALID;
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
        // Make these equal length for readability
        "COMMENT      ",
        "IDENTIFIER   ",
        "CONSTANT     ",
        "BIN_OPERATOR ",
        "LT_PAREN     ",
        "RT_PAREN     ",
        "LT_BRACKET   ",
        "RT_BRACKET   ",
        "LT_CURLY     ",
        "RT_CURLY     ",
        "NEWLINE      ",
        "FILEEND      ",
        "INVALID      ",
    };

    // Newline representation = "\\n"
    if (token.value[0] == '\n') {
        printf("%s: '%s'\n", token_types[token.type], "\\n");
    } else {
        printf("%s: '%s'\n", token_types[token.type], token.value);
    }
}