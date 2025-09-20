#pragma once
#include "Expr.hpp"
#include "Tokenizer.hpp"

class Parser {
public:
  Parser(const std::string &input) : m_lexer(input) { nextToken(); }
  [[nodiscard]] std::unique_ptr<systemT::Expr> parse();

private:
  void nextToken();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseAtom();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseApp();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseLam();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseTerm();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseSuccArg();
  [[nodiscard]] systemT::Type parseType();
  [[nodiscard]] systemT::Type parseTypeAtom();
  Token consume(const TokenType &expected, const std::string &expected_str);
  Token advance();

  Lexer m_lexer;
  Token m_currentToken;
};
