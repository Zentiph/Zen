// test_lexer.cpp

#include "../code_handling/lexer.h"

#include <iostream>

void printTestLexer(const std::string& source) {
  Lexer lexer(source);
  Token token = lexer.nextToken();

  while (token.type != TokenType::EndOfFile) {
    std::cout << "TokenType:" << static_cast<int>(token.type) << ", Value: '"
              << token.value << "'" << ", Line: " << token.line
              << ", Column: " << token.column << ")\n";
    token = lexer.nextToken();
  }
}

int main() {
  std::string sourceCode = R"(
fn main() {
    int x = 5
    if (x > 0) {
        print("Positive")
    } else {
        print("0 or Negative")
    }
}
)";

  std::cout << "Testing with sample code:\n" << sourceCode << "\n\n";
  printTestLexer(sourceCode);

  return 0;
}