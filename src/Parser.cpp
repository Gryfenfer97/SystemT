#include "SystemT/Parser.hpp"
#include "SystemT/Expr.hpp"
#include "SystemT/Tokenizer.hpp"
#include "SystemT/Types.hpp"
#include <charconv>
#include <memory>

Token Parser::nextToken() { return m_lexer.get(m_currentTokenPos + 1); }

std::vector<std::unique_ptr<systemT::Expr>> Parser::parse() {
  std::vector<std::unique_ptr<systemT::Expr>> assignments;
  assignments.push_back(parseAssignment());
  while (currentToken().type == TokenType::NEWLINE) {
    advance();
    assignments.push_back(parseAssignment());
  }
  return assignments;
}

Token Parser::consume(const TokenType &expected,
                      const std::string &expected_str) {
  const Token previousToken = currentToken();
  if (previousToken.type != expected) {
    throw std::runtime_error(std::format("Expected token '{}' but got '{}'",
                                         expected_str, previousToken.value));
  }
  ++m_currentTokenPos;
  return previousToken;
}

Token Parser::advance() {
  const Token previousToken = currentToken();
  ++m_currentTokenPos;
  return previousToken;
}

std::unique_ptr<systemT::Expr> Parser::parseAssignment() {
  if (currentToken().type == TokenType::VAR &&
      nextToken().type == TokenType::EQUAL) {

    Token varName = consume(TokenType::VAR, "var");
    advance();
    return std::make_unique<systemT::Expr>(
        systemT::AssignExpr(varName.value, parseTerm()));
  }
  return parseTerm();
}

std::unique_ptr<systemT::Expr> Parser::parseTerm() { return parseLam(); }

std::unique_ptr<systemT::Expr> Parser::parseLam() {
  if (currentToken().type == TokenType::LAMBDA) {
    advance();
    Token parameterName = consume(TokenType::VAR, "var");
    consume(TokenType::COLON, ":");
    systemT::Type type = parseType();

    if (!type.check<systemT::Lambda>(systemT::Any{}, systemT::Any{})) {
      throw std::runtime_error("Parser error: Type is not a lambda");
    }

    consume(TokenType::DOT, ".");
    auto body = parseLam();
    auto lambda = std::make_unique<systemT::LambdaExpr>(
        parameterName.value, type.as<systemT::Lambda>().getDomain(),
        std::move(body));
    return std::make_unique<systemT::Expr>(std::move(*lambda));
  }
  return parseApp();
}

std::unique_ptr<systemT::Expr> Parser::parseApp() {
  auto term = parseAtom();
  while (currentToken().type != TokenType::END_OF_FILE &&
         currentToken().type != TokenType::RPAREN &&
         currentToken().type != TokenType::NEWLINE) {
    term = std::make_unique<systemT::Expr>(
        systemT::ApplyExpr{std::move(term), parseAtom()});
  }
  return term;
}

std::unique_ptr<systemT::Expr> Parser::parseAtom() {
  switch (currentToken().type) {
  case TokenType::VAR: {
    return std::make_unique<systemT::Expr>(systemT::VarExpr(advance().value));
  }
  case TokenType::NAT: {
    const std::string &value_str = advance().value;
    std::size_t value;
    std::from_chars(value_str.data(), value_str.data() + value_str.size(),
                    value);
    return std::make_unique<systemT::Expr>(systemT::NatConstExpr(value));
  }
  case TokenType::LPAREN: {
    advance();
    auto expr = parseTerm();
    consume(TokenType::RPAREN, ")");
    return expr;
  }
  case TokenType::REC: {
    consume(TokenType::REC, "rec");
    auto zeroCase = parseAtom();
    auto succCase = parseAtom();
    auto num = parseAtom();
    return std::make_unique<systemT::Expr>(systemT::RecursionExpr(
        std::move(zeroCase), std::move(succCase), std::move(num)));
  }
  default:
    throw std::runtime_error(
        std::format("Parsing error: unkown token {}", currentToken().value));
  }
}

systemT::Type Parser::parseType() {
  auto left = parseTypeAtom();
  if (currentToken().type == TokenType::ARROW) {
    advance();
    auto right = parseType();
    return systemT::Lambda{left, right};
  }
  return systemT::Lambda{left, systemT::Any{}};
}

systemT::Type Parser::parseTypeAtom() {
  if (currentToken().type == TokenType::NAT_TYPE) {
    advance();
    return systemT::NaturalType{};
  }
  if (currentToken().type == TokenType::LPAREN) {
    advance();
    auto inside = parseType();
    consume(TokenType::RPAREN, ")");
    return inside;
  }
  throw std::runtime_error("Unexpected token");
}
