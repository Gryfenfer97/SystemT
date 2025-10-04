#pragma once
#include <string>
#include <vector>

enum class TokenType {
  NAT,
  VAR,
  LAMBDA,
  DOT,
  REC,
  IF,
  LPAREN,
  RPAREN,
  ARROW,
  COLON,
  NAT_TYPE,
  SUCC,
  EQUAL,
  NEWLINE,
  END_OF_FILE
};

struct Token {
  TokenType type;
  std::string value;
  bool operator==(const Token &rhs) const {
    return type == rhs.type && value == rhs.value;
  }
  bool operator!=(const Token &rhs) const { return !(*this == rhs); }
};

class Lexer {
public:
  Lexer(std::string input) : m_input(std::move(input)) {}
  void tokenize();
  Token get(std::size_t pos) { return m_tokens.at(pos); }

private:
  Token lexToken();

  std::vector<Token> m_tokens;
  std::string m_input;
  std::size_t m_pos = 0;
};
