#pragma once
#include "Expr.hpp"
#include "Types.hpp"
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace systemT {
using TypeEnv = std::map<std::string, experimental::Type>;

class TypeCheckerVisitor : public ExprVisitor<experimental::Type> {
private:
  TypeEnv current_env;

public:
  TypeCheckerVisitor(const TypeEnv &initial_env = {})
      : current_env(initial_env) {}

  experimental::Type getType(Expr &expr) {
    return std::visit(*this, expr.kind);
  }

  experimental::Type operator()(const VarExpr &expr) override {
    const auto it = current_env.find(expr.m_name);
    if (it == current_env.end()) {
      throw std::runtime_error(
          std::format("Type Error: Unbound variable '{}'", expr.m_name));
    }
    return it->second;
  };

  experimental::Type operator()(const NatConstExpr &) override {
    return experimental::NaturalType{};
  }

  experimental::Type operator()(const SuccExpr &expr) override {
    experimental::Type op_type = getType(*expr.m_operand);
    if (!op_type.check<experimental::NaturalType>()) {
      throw std::runtime_error(std::format(
          "Type Error: 'succ' expects a Nat, but got {}", op_type.toString()));
    }
    return experimental::NaturalType{};
  }

  experimental::Type operator()(const LambdaExpr &expr) override {
    TypeEnv body_env = current_env;
    body_env.emplace(
        std::make_pair(expr.m_parameterName, expr.m_parameterType));
    TypeCheckerVisitor body_checker(body_env);
    const auto bodyType = body_checker.getType(*expr.m_body);
    return experimental::Lambda{expr.m_parameterType, bodyType};
  }

  experimental::Type operator()(const ApplyExpr &expr) override {
    experimental::Type func_type = getType(*expr.m_func);
    experimental::Type arg_type = getType(*expr.m_arg);

    if (!func_type.check<experimental::Lambda>(arg_type, experimental::Any{})) {
      throw std::runtime_error("Type Error: Function expected, but got " +
                               func_type.toString());
    }

    return func_type.as<experimental::Lambda>().getCodomain();
  }

  experimental::Type operator()(const RecursionExpr &expr) override {
    const experimental::Type num_type = getType(*expr.m_num);
    if (!num_type.check<experimental::NaturalType>()) {
      throw std::runtime_error(std::format(
          "Type Error (recursion): Argument mismatch. Expected Nat but got {}",
          num_type.toString()));
    }
    const experimental::Type zeroCase_type = getType(*expr.m_zeroCase);
    const experimental::Type succCase_type = getType(*expr.m_succCase);
    if (!succCase_type.check<experimental::Lambda>(
            experimental::NaturalType{},
            experimental::Lambda(zeroCase_type, zeroCase_type))) {
      throw std::runtime_error(std::format(
          "Type Error (recursion): expected Lam Nat -> Lam {} -> {}, got {}",
          NaturalType{}.toString(), zeroCase_type.toString(),
          zeroCase_type.toString(), succCase_type.toString()));
    }
    return zeroCase_type;
  }

  experimental::Type operator()(const NativeFunctionExpr &expr) override {
    return experimental::Lambda{expr.m_domain, expr.m_codomain};
  }
};
} // namespace systemT
