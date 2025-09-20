#pragma once

#include "Types.hpp"
#include <functional>
#include <memory>
#include <string>
#include <variant>

namespace systemT {
class VarExpr;
class NatConstExpr;
class SuccExpr;
class LambdaExpr;
class ApplyExpr;
class RecursionExpr;
class NativeFunctionExpr;

struct Expr;
using ExprHandle = std::unique_ptr<Expr>;

class VarExpr {
public:
  std::string m_name;
  VarExpr(std::string name) : m_name(std::move(name)) {}

  [[nodiscard]] bool operator==(const VarExpr &rhs) const {
    return m_name == rhs.m_name;
  }
};

class NatConstExpr {
public:
  std::size_t m_value;
  NatConstExpr(std::size_t value) : m_value(value) {}
  [[nodiscard]] bool operator==(const NatConstExpr &rhs) const;
};

class SuccExpr {
public:
  ExprHandle m_operand;
  SuccExpr(ExprHandle operand);
  SuccExpr(const SuccExpr &);
  SuccExpr(SuccExpr &&) = default;
  [[nodiscard]] bool operator==(const SuccExpr &rhs) const;
};

class LambdaExpr {
public:
  std::string m_parameterName;
  Type m_parameterType;
  ExprHandle m_body;

  LambdaExpr(std::string parameterName, Type parameterType, ExprHandle body);
  ~LambdaExpr();
  LambdaExpr(const LambdaExpr &);
  LambdaExpr(LambdaExpr &&) = default;
  [[nodiscard]] bool operator==(const LambdaExpr &rhs) const;
};

class ApplyExpr {
public:
  std::unique_ptr<Expr> m_func;
  ExprHandle m_arg;

  ApplyExpr(std::unique_ptr<Expr> function, ExprHandle arg);
  ~ApplyExpr();
  ApplyExpr(const ApplyExpr &);
  ApplyExpr(ApplyExpr &&) = default;

  [[nodiscard]] bool operator==(const ApplyExpr &rhs) const;
};

class RecursionExpr {
public:
  ExprHandle m_zeroCase;
  std::unique_ptr<Expr> m_succCase;
  ExprHandle m_num;

  RecursionExpr(ExprHandle zeroCase, std::unique_ptr<Expr> succCase,
                ExprHandle num);
  ~RecursionExpr();
  RecursionExpr(const RecursionExpr &);
  RecursionExpr(RecursionExpr &&) = default;

  [[nodiscard]] bool operator==(const RecursionExpr &rhs) const;
};

class NativeFunctionExpr {
public:
  using NativeFn = std::function<Expr(const Expr &)>;
  std::string m_name;
  NativeFn m_function;
  Type m_domain;
  Type m_codomain;

  NativeFunctionExpr(std::string name, NativeFn function, Type domain,
                     Type codomain);
  ~NativeFunctionExpr();
  NativeFunctionExpr(const NativeFunctionExpr &);
  NativeFunctionExpr(NativeFunctionExpr &&) = default;

  [[nodiscard]] bool operator==(const NativeFunctionExpr &rhs) const;
};

struct Expr {
  Expr(const VarExpr &expr) : kind(expr) {}
  Expr(const NatConstExpr &expr) : kind(expr) {}
  Expr(const SuccExpr &expr) : kind(expr) {}
  Expr(SuccExpr &&expr) : kind(std::move(expr)) {}
  Expr(const LambdaExpr &expr) : kind(expr) {}
  Expr(LambdaExpr &&expr) : kind(std::move(expr)) {}
  Expr(const ApplyExpr &expr) : kind(expr) {}
  Expr(ApplyExpr &&expr) : kind(std::move(expr)) {}
  Expr(const RecursionExpr &expr) : kind(expr) {}
  Expr(RecursionExpr &&expr) : kind(std::move(expr)) {}
  Expr(const NativeFunctionExpr &expr) : kind(expr) {}
  Expr(NativeFunctionExpr &&expr) : kind(std::move(expr)) {}
  std::variant<VarExpr, NatConstExpr, SuccExpr, LambdaExpr, ApplyExpr,
               RecursionExpr, NativeFunctionExpr>
      kind;
  [[nodiscard]] bool operator==(const Expr &expr) const {
    return kind == expr.kind;
  }
  template <typename T> [[nodiscard]] bool checkType() const {
    return std::holds_alternative<T>(kind);
  }
  template <typename T> [[nodiscard]] T as() const { return std::get<T>(kind); }
};

template <typename R> class ExprVisitor {
  virtual R operator()(const VarExpr &) = 0;
  virtual R operator()(const NatConstExpr &) = 0;
  virtual R operator()(const SuccExpr &) = 0;
  virtual R operator()(const LambdaExpr &) = 0;
  virtual R operator()(const ApplyExpr &) = 0;
  virtual R operator()(const RecursionExpr &) = 0;
  virtual R operator()(const NativeFunctionExpr &) = 0;
};
} // namespace systemT
