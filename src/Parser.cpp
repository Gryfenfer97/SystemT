#include "SystemT/Parser.hpp"
#include "SystemT/Expr.hpp"
#include "SystemT/Tokenizer.hpp"
#include "SystemT/Types.hpp"
#include <charconv>
#include <memory>

void Parser::nextToken() { m_currentToken = m_lexer.getNextToken(); }

std::unique_ptr<systemT::Expr> Parser::parse() { return parseTerm(); }

Token Parser::consume(const TokenType &expected,
                      const std::string &expected_str) {
  const Token previousToken = m_currentToken;
  if (previousToken.type != expected) {
    throw std::runtime_error(std::format("Expected token '{}' but got '{}'",
                                         expected_str, previousToken.value));
  }
  m_currentToken = m_lexer.getNextToken();
  return previousToken;
}

Token Parser::advance() {
  const Token previousToken = m_currentToken;
  m_currentToken = m_lexer.getNextToken();
  return previousToken;
}

std::unique_ptr<systemT::Expr> Parser::parseTerm() { return parseLam(); }

std::unique_ptr<systemT::Expr> Parser::parseLam() {
  if (m_currentToken.type == TokenType::LAMBDA) {
    advance();
    Token parameterName = consume(TokenType::VAR, "var");
    consume(TokenType::COLON, ":");
    systemT::experimental::Type type = parseType();

    if (!type.check<systemT::experimental::Lambda>(
            systemT::experimental::Any{}, systemT::experimental::Any{})) {
      throw std::runtime_error("Parser error: Type is not a lambda");
    }

    consume(TokenType::DOT, ".");
    auto body = parseLam();
    auto lambda = std::make_unique<systemT::LambdaExpr>(
        parameterName.value,
        type.as<systemT::experimental::Lambda>().getDomain(), std::move(body));
    return std::make_unique<systemT::Expr>(std::move(*lambda));
  }
  return parseApp();
}

std::unique_ptr<systemT::Expr> Parser::parseApp() {
  auto term = parseAtom();
  while (m_currentToken.type != TokenType::END_OF_FILE &&
         m_currentToken.type != TokenType::RPAREN) {
    term = std::make_unique<systemT::Expr>(
        systemT::ApplyExpr{std::move(term), parseAtom()});
  }
  return term;
}

std::unique_ptr<systemT::Expr> Parser::parseAtom() {
  switch (m_currentToken.type) {
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
    std::println("after: LPARENIUHIUH");
    auto expr = parseTerm();
    std::println("before: LPARENIUHIUH");
    consume(TokenType::RPAREN, ")");
    return expr;
  }
  // case TokenType::SUCC: {
  //   advance();
  //   return std::make_unique<systemT::Expr>(systemT::SuccExpr(parseAtom()));
  // }
  case TokenType::REC: {
    consume(TokenType::REC, "rec");
    auto zeroCase = parseAtom();
    std::println("zeroCase");
    auto succCase = parseAtom();
    std::println("succCase");
    auto num = parseAtom();
    std::println("num");
    return std::make_unique<systemT::Expr>(systemT::RecursionExpr(
        std::move(zeroCase), std::move(succCase), std::move(num)));
  }
  default:
    throw std::runtime_error(
        std::format("Parsing error: unkown token {}", m_currentToken.value));
  }
}

systemT::experimental::Type Parser::parseType() {
  auto left = parseTypeAtom();
  if (m_currentToken.type == TokenType::ARROW) {
    advance();
    auto right = parseType();
    return systemT::experimental::Lambda{left, right};
  }
  return systemT::experimental::Lambda{left, systemT::experimental::Any{}};
}

systemT::experimental::Type Parser::parseTypeAtom() {
  if (m_currentToken.type == TokenType::NAT_TYPE) {
    advance();
    return systemT::experimental::NaturalType{};
  }
  if (m_currentToken.type == TokenType::LPAREN) {
    advance();
    auto inside = parseType();
    consume(TokenType::RPAREN, ")");
    return inside;
  }
  throw std::runtime_error("Unexpected token");
}
