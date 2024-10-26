// lexer.cpp

// TODO:
//    1. MODULARIZE
//    2. ADD EDGE CASE PROTECTION FOR STRING PARSING (ESCAPE SEQUENCES, NO CLOSING QUOTE, ETC)

#include <algorithm>
#include <any>
#include <stdexcept>
#include <string>
#include <vector>

const char EOF_CHAR = '\0';
const std::vector<std::string> KEYWORDS = {
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
const std::vector<char> CHAR_OPERATORS = {
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
const std::vector<std::string> STRING_OPERATORS = {
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
const std::vector<std::string> TYPES = {
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
  "nulltype",
}; // Add more when needed
const std::vector<char> WHITESPACE = {
  ' ',  // U+0020
  '\t', // U+0009
  '\n', // U+000A
  '\r', // U+000D
  '\v', // U+000B
  '\f', // U+000C
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

  Token(TokenType type, const std::string& value, size_t line, size_t column) : type(type), value(value), line(line), column(column) {}
};

class InvalidTokenError : public std::exception {
  private:
    std::string message, source;
    size_t current, line, column;

  public:
    InvalidTokenError(std::string src, size_t pos, size_t lineno, size_t colno)
    : source(src), current(pos), line(lineno), column(colno) {
      int bkBuff = 5, fwdBuff = 50;
      message = "InvalidTokenError thrown at line " + std::to_string(line) + ", column " + std::to_string(column) + " in src:\n...";
      for (size_t i = current - bkBuff; i < source.length() && i < current + fwdBuff; i++) {
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
    if (current == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);

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
        if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
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
        if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
      }
      return Token(TokenType::Number, number, line, initColumn);

    // Handle strings
    if (currentChar == '\'') {
      size_t initColumn = column;
      std::string str;

      currentChar = advance();
      if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
      while (currentChar != '\'') {
        str += advance();
        currentChar = peek();
        if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
      }
      str += advance();
      currentChar = peek();
      if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
      return Token(TokenType::String, str, line, initColumn);
    }
    if (currentChar == '"') {
      size_t initColumn = column;
      std::string str;

      currentChar = advance();
      if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
      while (currentChar != '"') {
        str += advance();
        currentChar = peek();
        if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
      }
      str += advance();
      currentChar = peek();
      if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
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
        if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
        return Token(TokenType::Operator, op, line, initColumn);
      }
      if (currentChar == '/' && next == '/') { // Check if it's a comment
        while (currentChar != '\n') {
          currentChar = advance();
          if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
        }
        currentChar = advance(); // Advance again once hitting newline to go to next line
        if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
        return Token(TokenType::Comment, "", line, initColumn);
      }
      if (currentChar == '/' && next == '.') { // Check if it's a multiline comment
        std::string comment;
        size_t initLine = line;

        while (currentChar != '.' && next != '/') {
          comment += advance();
          currentChar = peek();
          if (currentChar == EOF_CHAR) return Token(TokenType::EndOfFile, "", line, column);
          next = peekNext();
        }
        return Token(TokenType::MultilineComment, comment, initLine, initColumn);
      }
      // else
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
    if (current + 1 >= source.length()) {
      return EOF_CHAR;
    }

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
      } else column++;
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
