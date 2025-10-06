#include "SystemT/Substitution.hpp"
#include "SystemT/ASTPrint.hpp"
#include "SystemT/Expr.hpp"
#include "SystemT/builtins.hpp"
#include <memory>

systemT::VariableEnv::VariableEnv() : m_parent(nullptr) {
  assign(systemT::builtins::successor.m_name,
         Expr(systemT::builtins::successor));
}

systemT::Expr
systemT::SubstitutionVisitor::operator()(const systemT::VarExpr &expr) {
  if (!current_env.contains(expr.m_name)) {
    throw std::runtime_error(
        std::format("Variable {} does not exists", expr.m_name));
  }
  return *current_env.lookup(expr.m_name);
}

systemT::Expr
systemT::SubstitutionVisitor::operator()(const systemT::SuccExpr &expr) {
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

systemT::Expr
systemT::SubstitutionVisitor::operator()(const systemT::ApplyExpr &expr) {
  SubstitutionVisitor arg_evaluator(current_env);
  Expr arg_reduced = arg_evaluator.reduce(*expr.m_arg);

  SubstitutionVisitor func_evaluator(current_env);
  Expr func = func_evaluator.reduce(*expr.m_func);

  if (func.checkType<VarExpr>()) {
    const auto *value = this->current_env.lookup(func.as<VarExpr>().m_name);
    if (value == nullptr) {
      throw std::runtime_error(
          std::format("Variable {} not found", func.as<VarExpr>().m_name));
    }
    if (value->checkType<LambdaExpr>()) {
      return applyLambda(value->as<LambdaExpr>(), arg_reduced);
    }
    if (value->checkType<NativeFunctionExpr>()) {
      return applyNativeFunction(value->as<NativeFunctionExpr>(), arg_reduced);
    }
    if (value->checkType<VarExpr>()) {
      // TODO: use the type checker here
      return ApplyExpr(std::make_unique<Expr>(func),
                       std::make_unique<Expr>(arg_reduced));
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

systemT::Expr
systemT::SubstitutionVisitor::operator()(const systemT::LambdaExpr &expr) {
  VariableEnv new_env(current_env);
  new_env.assign(expr.m_parameterName, VarExpr(expr.m_parameterName));
  SubstitutionVisitor visitor(new_env);
  auto reduced_body = visitor.reduce(*expr.m_body);
  return LambdaExpr{expr.m_parameterName, expr.m_parameterType,
                    std::make_unique<Expr>(reduced_body)};
}

systemT::Expr
systemT::SubstitutionVisitor::operator()(const systemT::RecursionExpr &expr) {
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

systemT::Expr
systemT::SubstitutionVisitor::operator()(const systemT::AssignExpr &expr) {
  Expr value = reduce(*expr.m_value);
  current_env.assign(expr.m_varName, value);
  return value;
}
