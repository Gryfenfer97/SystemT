#include "SystemT/Tokenizer.hpp"
#include <stdexcept>

Token Lexer::getNextToken() {
  while (m_pos < m_input.length() && std::isspace(m_input[m_pos]) != 0) {
    m_pos++;
  }

  if (m_pos >= m_input.length()) {
    return {.type = TokenType::END_OF_FILE, .value = "EOF"};
  }

  char currentChar = m_input[m_pos];

  if (currentChar == '(') {
    m_pos++;
    return {.type = TokenType::LPAREN, .value = "("};
  }
  if (currentChar == ')') {
    m_pos++;
    return {.type = TokenType::RPAREN, .value = ")"};
  }
  if (currentChar == '.') {
    m_pos++;
    return {.type = TokenType::DOT, .value = "."};
  }
  if (currentChar == ':') {
    m_pos++;
    return {.type = TokenType::COLON, .value = ":"};
  }
  if (currentChar == 'N') {
    m_pos++;
    return {.type = TokenType::NAT_TYPE, .value = "N"};
  }
  if (currentChar == '-' && m_input[m_pos + 1] == '>') {
    m_pos += 2;
    return {.type = TokenType::ARROW, .value = "->"};
  }

  // Handle numeric literals.
  if (std::isdigit(currentChar) != 0) {
    std::string num_str;
    while (m_pos < m_input.length() && std::isdigit(m_input[m_pos]) != 0) {
      num_str += m_input[m_pos];
      m_pos++;
    }
    return {.type = TokenType::NAT, .value = num_str};
  }

  // Handle identifiers (variables and keywords).
  if (std::isalpha(currentChar) != 0) {
    std::string ident_str;
    while (m_pos < m_input.length() &&
           (std::isalnum(m_input[m_pos]) != 0 || m_input[m_pos] == '_')) {
      ident_str += m_input[m_pos];
      m_pos++;
    }

    if (ident_str == "lam") {
      return {.type = TokenType::LAMBDA, .value = "lam"};
    }
    if (ident_str == "rec") {
      return {.type = TokenType::REC, .value = "rec"};
    }

    // TODO: if
    // if (ident_str == "if") {
    //   return {IF, "if"};
    // }
    return {.type = TokenType::VAR, .value = ident_str};
  }

  throw std::runtime_error("Lexer error: unexpected character '" +
                           std::string(1, currentChar) + "'");
}
