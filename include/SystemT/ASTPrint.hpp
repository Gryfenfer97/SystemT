#pragma once
#include "Expr.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <variant>

namespace systemT {

class ASTPrintVisitor : public ExprVisitor<std::string> {
public:
  ASTPrintVisitor() = default;

  std::string toString(Expr &expr) { return std::visit(*this, expr.kind); }

  std::string operator()(const VarExpr &expr) override {
    return std::format("'{}'", expr.m_name);
  };

  std::string operator()(const NatConstExpr &expr) override {
    return std::to_string(expr.m_value);
  }

  std::string operator()(const SuccExpr &expr) override {
    return std::format("S ({})", toString(*expr.m_operand));
  }

  std::string operator()(const LambdaExpr &expr) override {
    return std::format("lam {}: ({}). ({})", expr.m_parameterName,
                       expr.m_parameterType.toString(), toString(*expr.m_body));
  }

  std::string operator()(const ApplyExpr &expr) override {
    return std::format("App ({}) ({})", toString(*expr.m_func),
                       toString(*expr.m_arg));
  }

  std::string operator()(const RecursionExpr &expr) override {
    return std::format("rec ({}) ({}) ({})", toString(*expr.m_zeroCase),
                       toString(*expr.m_succCase), toString(*expr.m_num));
  }
  std::string operator()(const NativeFunctionExpr &expr) override {
    return expr.m_name;
  }
};
} // namespace systemT
