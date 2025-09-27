#pragma once
#include "Expr.hpp"
#include <map>
#include <string>
#include <variant>

namespace systemT {

using VariableEnv = std::map<std::string, Expr>;

class SubstitutionVisitor : public ExprVisitor<Expr> {
  VariableEnv current_env;

public:
  SubstitutionVisitor();
  SubstitutionVisitor(VariableEnv initial_env)
      : current_env(std::move(initial_env)) {}

  Expr reduce(const Expr &expr) { return std::visit(*this, expr.kind); }

  Expr operator()(const VarExpr &expr) override;

  Expr operator()(const NatConstExpr &expr) override { return expr; }

  Expr operator()(const SuccExpr &expr) override;

  Expr operator()(const LambdaExpr &expr) override;

  Expr operator()(const ApplyExpr &expr) override;

  Expr operator()(const RecursionExpr &expr) override;

  Expr operator()(const NativeFunctionExpr &expr) override { return expr; }

  Expr operator()(const AssignExpr &expr) override;

private:
  Expr applyLambda(const LambdaExpr &func, const Expr &arg) {
    VariableEnv body_env = current_env;
    body_env.emplace(std::pair(func.m_parameterName, arg));
    SubstitutionVisitor reduction_visitor(body_env);
    return reduction_visitor.reduce(*func.m_body);
  }
  Expr applyNativeFunction(const NativeFunctionExpr &func, const Expr &arg) {
    return func.m_function(arg);
  }
};
} // namespace systemT
