#pragma once
#include "Expr.hpp"
#include "Tokenizer.hpp"
#include <memory>
#include <vector>

class Parser {
public:
  Parser(const std::string &input) : m_lexer(input) { m_lexer.tokenize(); }
  [[nodiscard]] std::vector<std::unique_ptr<systemT::Expr>> parse();

private:
  [[nodiscard]] Token nextToken();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseAssignment();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseAtom();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseApp();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseLam();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseTerm();
  [[nodiscard]] std::unique_ptr<systemT::Expr> parseSuccArg();
  [[nodiscard]] systemT::Type parseType();
  [[nodiscard]] systemT::Type parseTypeAtom();
  Token consume(const TokenType &expected, const std::string &expected_str);
  Token advance();
  [[nodiscard]] Token currentToken() { return m_lexer.get(m_currentTokenPos); }

  Lexer m_lexer;
  std::size_t m_currentTokenPos = 0;
};
