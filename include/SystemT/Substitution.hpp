#pragma once
#include "ASTPrint.hpp"
#include "Expr.hpp"
#include "Types.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace systemT {

struct Closure;
struct Primitive;

struct Value {
  std::variant<Expr, std::unique_ptr<Closure>, std::shared_ptr<Primitive>> data;
  template <typename T> [[nodiscard]] bool check() {
    return std::holds_alternative<T>(data);
  }
  template <typename T> [[nodiscard]] T as() { return std::get<T>(data); }
};

struct Primitive {
  std::function<Value(Value)> func;
};

using VariableEnv = std::map<std::string, Expr>;

struct Closure {
  std::string param;
  std::shared_ptr<Expr> body;
  std::shared_ptr<VariableEnv> env;
};

class SubstitutionVisitor : public ExprVisitor<Expr> {
  VariableEnv current_env;

public:
  SubstitutionVisitor();
  SubstitutionVisitor(VariableEnv initial_env)
      : current_env(std::move(initial_env)) {}

  Expr reduce(const Expr &expr) { return std::visit(*this, expr.kind); }

  Expr operator()(const VarExpr &expr) override {
    if (!current_env.contains(expr.m_name)) {
      throw std::runtime_error(
          std::format("Variable {} does not exists", expr.m_name));
    }
    return current_env.at(expr.m_name);
  };

  Expr operator()(const NatConstExpr &expr) override { return expr; }

  Expr operator()(const SuccExpr &expr) override {
    SubstitutionVisitor visitor(current_env);
    auto reduced_operand = visitor.reduce(*expr.m_operand);
    if (reduced_operand.checkType<NatConstExpr>()) {
      return NatConstExpr{reduced_operand.as<NatConstExpr>().m_value + 1};
    }
    if (reduced_operand.checkType<VarExpr>()) {
      return SuccExpr{std::make_unique<Expr>(reduced_operand)};
    }
    throw std::runtime_error("Trying to call Succ on a non-number");
  }

  Expr operator()(const LambdaExpr &expr) override {
    VariableEnv new_env = current_env;
    new_env.emplace(
        std::pair(expr.m_parameterName, VarExpr(expr.m_parameterName)));
    SubstitutionVisitor visitor(new_env);
    auto reduced_body = visitor.reduce(*expr.m_body);
    return LambdaExpr{expr.m_parameterName, expr.m_parameterType,
                      std::make_unique<Expr>(reduced_body)};
  }

  Expr operator()(const ApplyExpr &expr) override {
    SubstitutionVisitor arg_evaluator(current_env);
    Expr arg_reduced = arg_evaluator.reduce(*expr.m_arg);

    SubstitutionVisitor func_evaluator(current_env);
    Expr func = func_evaluator.reduce(*expr.m_func);

    if (func.checkType<VarExpr>()) {
      auto value = this->current_env.find(func.as<VarExpr>().m_name);
      if (value == this->current_env.end()) {
        throw std::runtime_error(
            std::format("Variable {} not found", func.as<VarExpr>().m_name));
      }
      if (value->second.checkType<LambdaExpr>()) {
        return applyLambda(value->second.as<LambdaExpr>(), arg_reduced);
      }
      if (value->second.checkType<NativeFunctionExpr>()) {
        return applyNativeFunction(value->second.as<NativeFunctionExpr>(),
                                   arg_reduced);
      }
      throw std::runtime_error(std::format("Variable {} is not a function",
                                           func.as<VarExpr>().m_name));
    }

    if (func.checkType<LambdaExpr>()) {
      return applyLambda(func.as<LambdaExpr>(), arg_reduced);
    }
    if (func.checkType<NativeFunctionExpr>()) {
      return applyNativeFunction(func.as<NativeFunctionExpr>(), arg_reduced);
    }
    ASTPrintVisitor printVisitor;
    throw std::runtime_error(std::format("Expression ({}) is not a function",
                                         printVisitor.toString(func)));
  }

  Expr operator()(const RecursionExpr &expr) override {
    SubstitutionVisitor num_evaluator(current_env);
    Expr num_value = num_evaluator.reduce(*expr.m_num);

    SubstitutionVisitor zero_evaluator(current_env);
    auto reduced_zero = zero_evaluator.reduce(*expr.m_zeroCase);

    if (!num_value.checkType<NatConstExpr>()) {
      return RecursionExpr(std::make_unique<Expr>(reduced_zero),
                           std::make_unique<Expr>(*expr.m_succCase),
                           std::make_unique<Expr>(num_value));
    }

    const std::size_t index = num_value.as<NatConstExpr>().m_value;
    if (index == 0) {
      return reduced_zero;
    }
    SubstitutionVisitor succ_evaluator(current_env);
    const auto l1 = ApplyExpr(std::make_unique<Expr>(*expr.m_succCase),
                              std::make_unique<Expr>(NatConstExpr(index - 1)));
    auto l1_reduced = succ_evaluator.reduce(Expr(l1));
    if (!l1_reduced.checkType<LambdaExpr>()) {
      throw std::runtime_error("Recursion method does not return a lambda");
    }
    auto l2 = RecursionExpr(std::make_unique<Expr>(*expr.m_zeroCase),
                            std::make_unique<Expr>(*expr.m_succCase),
                            std::make_unique<Expr>(NatConstExpr{index - 1}));
    auto app = ApplyExpr(std::make_unique<Expr>(l1_reduced),
                         std::make_unique<Expr>(std::move(l2)));
    return succ_evaluator.reduce(app);
  }

  Expr operator()(const NativeFunctionExpr &expr) override {
    return expr;
    // SubstitutionVisitor visitor(current_env);
    // auto reduced_operand = visitor.reduce(*value.m_operand);
    // if (reduced_operand.checkType<NatConstExpr>()) {
    //   return NatConstExpr{reduced_operand.as<NatConstExpr>().m_value + 1};
    // }
    // if (reduced_operand.checkType<VarExpr>()) {
    //   return SuccExpr{std::make_unique<Expr>(reduced_operand)};
    // }
    // throw std::runtime_error("Trying to call Succ on a non-number");
  }

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
