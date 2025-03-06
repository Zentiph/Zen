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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 100
#define NEWLINE_CHAR '\n'

/**
 * @brief All token types.
 *        FILEEND is used instead of EOF to avoid
 *        name conflicts with stdio.h.
 */
typedef enum {
    COMMENT, VARIABLE, OPERATOR, CONSTANT, NEWLINE, FILEEND, INVALID
} TokenType;

/**
 * @brief A token that contains its type and value.
 */
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

void skip_whitespace(FILE *fp);
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
        fprintf(stderr, "Usage: tokenizer <source>\n");
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
 * @brief Skip the meaningless whitespace in the
 *        file until a token is found.
 *        Newline characters as well as semicolons
 *        are valid whitespace, so track those.
 *
 * @param fp Filepath
 */
void skip_whitespace(FILE *fp) {
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (isspace(ch)) {
            if (ch == NEWLINE_CHAR) {
                return; // Newline character is allowed
            }
        } else {
            // Backtrack if not whitespace and end
            ungetc(ch, fp);
            return;
        }
    }
}

/**
 * @brief Get the next token in the file.
 *
 * @param fp Filepath
 * @return Token - The token found
 */
Token next_token(FILE *fp) {
    Token token;
    token.value[0] = '\0'; // Reset value
    char ch = fgetc(fp);

    if (ch == EOF) {
        token.type = FILEEND;
        return token;
    }

    skip_whitespace(fp);

    // Handle single-line comments
    if (ch == '/' && (ch = fgetc(fp)) == '/') {
        token.type = COMMENT;
        int i = 0;
        while ((ch = fgetc(fp)) != FILEEND && ch != NEWLINE_CHAR) {
            token.value[i++] = ch;
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle multi-line comments (/. ... ./)
    if (ch == '/' && (ch = fgetc(fp)) == '.') {
        token.type = COMMENT;
        int i = 0;
        while ((ch = fgetc(fp)) != FILEEND) {
            // End of comment
            if (ch == '.' && (ch = fgetc(fp)) == '/') {
                break;
            }
            token.value[i++] = ch;
        }
        token.value[i] = '\0';
        return token;
    }

    // Handle variable declaration
    // First character of an identifier cannot be a digit
    if (isalpha(ch) || ch == '_') {
        token.type = VARIABLE;
        int i = 0;
        token.value[i++] = ch;

        while ((ch = fgetc(fp)) != FILEEND && (isalnum(ch) || ch == '_')) {
            token.value[i++] = ch;
        }
        token.value[i] = '\0';

        if (ch != EOF) ungetc(ch, fp); // ungetc if we're not at EOF
        return token;
    }

    // Handle numbers (constants)
    if (isdigit(ch)) {
        token.type = CONSTANT;
        int i = 0;
        token.value[i++] = ch;

        while ((ch = fgetc(fp)) != FILEEND && isdigit(ch)) {
            token.value[i++] = ch;
        }
        token.value[i] = '\0';

        if (ch != EOF) ungetc(ch, fp); // ungetc if we're not at EOF
        return token;
    }

    // Handle operators
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
        token.type = OPERATOR;
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
        "COMMENT",
        "VARIABLE",
        "OPERATOR",
        "CONSTANT",
        "NEWLINE",
        "EOF",
        "INVALID",
    };

    printf("%s: %s\n", token_types[token.type], token.value);
}