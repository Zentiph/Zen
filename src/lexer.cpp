// lexer.cpp

// TODO:
//    1. MODULARIZE
//    2. ADD EDGE CASE PROTECTION FOR STRING PARSING (ESCAPE SEQUENCES, NO CLOSING QUOTE, ETC)
//    3. ADD CHECKS TO advance() IN CASE OF OUT OF BOUNDS ISSUES

#include <algorithm>
#include <any>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::string> KEYWORDS = {
    "if",
    "elseif",
    "else",
    "for",
    "in",
    "while",
    "fn",
    "class",
    "private",
};

std::vector<char> CHAR_OPERATORS = {
    '=',
    '+',
    '-',
    '*',
    '/',
    '>',
    '<',
    '@',
    '&',
    '|',
    '^',
    '!',
    '~',
};
std::vector<std::string> STRING_OPERATORS = {
    ">=",
    "<=",
    "==",
    "!=",
    "->",
    "=>",
    "++",
    "--",
    "+=",
    "-=",
    "*=",
    "/=",
    "<<",
    ">>",
};

std::vector<std::string> TYPES = {
    "int",
    "float",
    "complex",
    "string",
    "array",
    "lockedarray",
    "map",
    "lockedmap",
    "set",
    "lockedset",
    "bool",
    "nulltype"
}; // Add more when needed

std::vector<char> WHITESPACE = {
    ' ',
    '\n',
    '\t',
    // TODO: ADD MORE
};

enum class TokenType {
    Identifier,
    Keyword,
    Number,
    String,
    Operator,
    Comment,
    MultilineComment, // We track multiline comments separately since they're used for docstrings
    OpenParen,
    CloseParen,
    OpenBracket,
    CloseBracket,
    OpenBrace,
    CloseBrace,
    Comma,
    EndOfFile,
    Null,
    Type,
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;

    Token(TokenType type, const std::string& value, size_t line, size_t column)
        : type(type), value(value), line(line), column(column) {}
};

class InvalidTokenError : public std::exception {
    private:
        std::string message, source;
        size_t current, line, column;

    public:
        InvalidTokenError(std::string src, size_t pos, size_t lineno, size_t colno)
        : source(src), current(pos), line(lineno), column(colno) {
            message = "InvalidTokenError thrown at line " + std::to_string(line) + ", column " + std::to_string(column) + " in src:\n...";
            for (size_t i = current - 5; i < source.length() && i < current + 50; i++) {
                message += source[i];
            }
        }

        const char* what() const noexcept override {
            return message.c_str();
        }
};

class Lexer {
public:
    Lexer(const std::string& source)
        : source(source), current(0), line(1), column(0) {}

    Token Lexer::nextToken() {
        if (current != 0) advance(); // Skip to next char after returning previous token

        skipWhitespace();

        // Handle end of the file
        if (current >= source.length()) {
            return Token(TokenType::EndOfFile, "", line, column);
        }

        char currentChar = peek();

        // Handle identifiers / keywords
        if (isIdentifier(currentChar, true)) {
            std::string identifier;
            size_t initColumn = column;

            while (isIdentifier(currentChar)) {
                identifier += advance();
                currentChar = peek();
            }
            // Handle null
            if (identifier == "null") return Token(TokenType::Null, identifier, line, initColumn);
            // Check if the identifier is a keyword
            if (valueIn(identifier, KEYWORDS)) return Token(TokenType::Keyword, identifier, line, initColumn);
            // Check if the identifier is a type
            if (valueIn(identifier, TYPES)) return Token(TokenType::Type, identifier, line, initColumn);
            // Otherwise, it's a user defined identifier
            return Token(TokenType::Identifier, identifier, line, initColumn);
        }

        // Handle numbers
        if (isdigit(currentChar)) {
            std::string number;
            size_t initColumn = column;

            while (isdigit(currentChar)) {
                number += advance();
                currentChar = peek();
            }
            return Token(TokenType::Number, number, line, initColumn);

        // Handle strings
        if (currentChar == '\'') {
            size_t initColumn = column;
            std::string str;

            currentChar = advance();
            while (currentChar != '\'') {
                str += advance();
                currentChar = peek();
            }
            str += advance();
            return Token(TokenType::String, str, line, initColumn);
        }
        if (currentChar == '"') {
            size_t initColumn = column;
            std::string str;

            currentChar = advance();
            while (currentChar != '"') {
                str += advance();
                currentChar = peek();
            }
            str += advance();
            return Token(TokenType::String, str, line, initColumn);
        }

        // Handle operators / comments
        if (valueIn(currentChar, CHAR_OPERATORS)) {
            std::string op;
            size_t initColumn = column;

            char next = peekNext();
            if (valueIn(std::to_string(currentChar + next), STRING_OPERATORS)) { // Check if it's a multi-char operator
                op += currentChar + next;
                currentChar += advance();
                return Token(TokenType::Operator, op, line, initColumn);
            }
            if (currentChar == '/' && next == '/') { // Check if it's a comment
                while (currentChar != '\n') advance();
                advance(); // Advance again once hitting newline to go to next line
                currentChar = peek();
                return Token(TokenType::Comment, "", line, initColumn);
            }
            if (currentChar == '/' && next == '.') { // Check if it's a multiline comment
                std::string comment;
                size_t initLine = line;

                while (currentChar != '.' && next != '/') {
                    comment += advance();
                    currentChar = peek();
                    next = peekNext();
                }
                return Token(TokenType::MultilineComment, comment, initLine, initColumn);
            }
            op += currentChar;
            return Token(TokenType::Operator, op, line, initColumn);
        }

        // Handle other chars
        if (currentChar == '(') return Token(TokenType::OpenParen, "(", line, column);
        if (currentChar == ')') return Token(TokenType::OpenParen, ")", line, column);
        if (currentChar == '[') return Token(TokenType::OpenParen, "[", line, column);
        if (currentChar == ']') return Token(TokenType::OpenParen, "]", line, column);
        if (currentChar == '{') return Token(TokenType::OpenParen, "{", line, column);
        if (currentChar == '}') return Token(TokenType::OpenParen, "}", line, column);
        if (currentChar == ',') return Token(TokenType::Comma, ",", line, column);

        // If no valid token is found, throw an exception
        throw InvalidTokenError(source, current, line, column);
    }

}

private:
    std::string source;
    size_t current; // Current position in the source
    size_t line;    // Current line number
    size_t column;  // Current column number

    char peek() {return source[current];}
    char peekNext() {return source[current + 1];}
    char advance() {
        if (current == '\n') {
            line++;
            column = 0;
        } else column++;
        current++;
        return source[current];
    }
    void skipWhitespace() {
        while (valueIn(peek(), WHITESPACE)) {
            if (current == '\n') {
                line++;
                column = 0;
            }
            else column++;
            current++;
        }
    }
};

template <typename T>
bool valueIn(const T& value, const std::vector<T>& vec) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
};

bool isIdentifier(char ch, bool first=false) {
    if (first) {
        if (isdigit(ch)) return false;
        if (!isalpha(ch) && ch != '_') return false;
    }

    if (!isalpha(ch) && !isdigit(ch) && ch != '_') return false;

    return true;
}