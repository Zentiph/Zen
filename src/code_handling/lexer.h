// lexer.h

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

extern const char EOF_CHAR;
extern const std::vector<std::string> KEYWORDS;
extern const std::vector<char> CHAR_OPERATORS;
extern const std::vector<std::string> STRING_OPERATORS;
extern const std::vector<std::string> TYPES;
extern const std::vector<char> WHITESPACE;

enum class TokenType {
  Identifier,
  Keyword,
  Number,
  String,
  Operator,
  Comment,
  MultilineComment,
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

  Token(TokenType type, const std::string& value, size_t line, size_t column);
};

class Lexer {
public:
  Lexer(const std::string& source);
  Token Lexer::nextToken();

private:
  std::string source;
  size_t current;
  size_t line;
  size_t column;

  char peek();
  char peekNext();
  char advance();
  void skipWhitespace();
};

#endif // LEXER_H