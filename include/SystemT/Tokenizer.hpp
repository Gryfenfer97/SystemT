#pragma once
#include <string>

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
  Lexer(const std::string &input) : m_input(input), m_pos(0) {}

  Token getNextToken();

private:
  std::string m_input;
  std::size_t m_pos;
};
