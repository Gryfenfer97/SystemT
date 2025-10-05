#pragma once
#include "Expr.hpp"
#include <string>
#include <unordered_map>
#include <variant>

namespace systemT {

struct VariableEnv {
  std::unordered_map<std::string, Expr> m_locals;
  VariableEnv *m_parent; // because C++ does not support std::optional<T&> yet

  VariableEnv();

  VariableEnv(VariableEnv *parent) : m_parent(parent) {}

  [[nodiscard]] Expr *lookup(const std::string &name) {
    const auto localValue = m_locals.find(name);
    if (localValue != m_locals.end()) {
      return &localValue->second;
    }
    if (m_parent != nullptr) {
      return m_parent->lookup(name);
    }
    return nullptr;
  }

  [[nodiscard]] bool contains(const std::string &name) {
    return lookup(name) != nullptr;
  }

  void assign(const std::string &name, const Expr &value) {
    m_locals.emplace(std::pair(name, value));
  }
};

class SubstitutionVisitor : public ExprVisitor<Expr> {
  VariableEnv &current_env;

public:
  SubstitutionVisitor(VariableEnv &initial_env) : current_env(initial_env) {}

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
    VariableEnv body_env(current_env);
    body_env.assign(func.m_parameterName, arg);
    SubstitutionVisitor reduction_visitor(body_env);
    return reduction_visitor.reduce(*func.m_body);
  }
  Expr applyNativeFunction(const NativeFunctionExpr &func, const Expr &arg) {
    return func.m_function(arg);
  }
};
} // namespace systemT
