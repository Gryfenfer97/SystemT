#pragma once
#include "Expr.hpp"
#include "Types.hpp"
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace systemT {
using TypeEnv = std::map<std::string, Type>;

class TypeCheckerVisitor : public ExprVisitor<Type> {
private:
  TypeEnv current_env;

public:
  TypeCheckerVisitor(const TypeEnv &initial_env = {})
      : current_env(initial_env) {}

  Type getType(Expr &expr) { return std::visit(*this, expr.kind); }

  Type operator()(const VarExpr &expr) override {
    const auto variable_it = current_env.find(expr.m_name);
    if (variable_it == current_env.end()) {
      throw std::runtime_error(
          std::format("Type Error: Unbound variable '{}'", expr.m_name));
    }
    return variable_it->second;
  };

  Type operator()(const NatConstExpr & /*expr*/) override {
    return NaturalType{};
  }

  Type operator()(const SuccExpr &expr) override {
    Type op_type = getType(*expr.m_operand);
    if (!op_type.check<NaturalType>()) {
      throw std::runtime_error(std::format(
          "Type Error: 'succ' expects a Nat, but got {}", op_type.toString()));
    }
    return NaturalType{};
  }

  Type operator()(const LambdaExpr &expr) override {
    TypeEnv body_env = current_env;
    body_env.emplace(
        std::make_pair(expr.m_parameterName, expr.m_parameterType));
    TypeCheckerVisitor body_checker(body_env);
    const auto bodyType = body_checker.getType(*expr.m_body);
    return Lambda{expr.m_parameterType, bodyType};
  }

  Type operator()(const ApplyExpr &expr) override {
    Type func_type = getType(*expr.m_func);
    Type arg_type = getType(*expr.m_arg);

    if (!func_type.check<Lambda>(arg_type, Any{})) {
      throw std::runtime_error("Type Error: Function expected, but got " +
                               func_type.toString());
    }

    return func_type.as<Lambda>().getCodomain();
  }

  Type operator()(const RecursionExpr &expr) override {
    const Type num_type = getType(*expr.m_num);
    if (!num_type.check<NaturalType>()) {
      throw std::runtime_error(std::format(
          "Type Error (recursion): Argument mismatch. Expected Nat but got {}",
          num_type.toString()));
    }
    const Type zeroCase_type = getType(*expr.m_zeroCase);
    const Type succCase_type = getType(*expr.m_succCase);
    if (!succCase_type.check<Lambda>(NaturalType{},
                                     Lambda(zeroCase_type, zeroCase_type))) {
      throw std::runtime_error(std::format(
          "Type Error (recursion): expected Lam Nat -> Lam Nat -> {}, got {}",
          zeroCase_type.toString(), zeroCase_type.toString(),
          succCase_type.toString()));
    }
    return zeroCase_type;
  }

  Type operator()(const NativeFunctionExpr &expr) override {
    return Lambda{expr.m_domain, expr.m_codomain};
  }
};
} // namespace systemT
