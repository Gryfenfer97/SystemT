#pragma once
#include "Expr.hpp"
#include "Substitution.hpp"

namespace systemT::builtins {
static NativeFunctionExpr successor(
    "S",
    [](const Expr &arg) -> Expr {
      VariableEnv env;
      SubstitutionVisitor visitor(env);
      if (arg.checkType<NatConstExpr>()) {
        return NatConstExpr{arg.as<NatConstExpr>().m_value + 1};
      }
      if (arg.checkType<VarExpr>()) {
        return SuccExpr{std::make_unique<Expr>(arg)};
      }
      throw std::runtime_error("Trying to call Succ on a non-number");
    },
    NaturalType{}, NaturalType{});
} // namespace systemT::builtins
