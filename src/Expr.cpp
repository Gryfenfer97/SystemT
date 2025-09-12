#include <SystemT/Expr.hpp>

namespace systemT {

bool NatConstExpr::operator==(const NatConstExpr &rhs) const {
  return m_value == rhs.m_value;
}

SuccExpr::SuccExpr(ExprHandle operand) : m_operand(std::move(operand)) {}

SuccExpr::SuccExpr(const SuccExpr &expr)
    : m_operand(std::make_unique<Expr>(*expr.m_operand)) {}
bool SuccExpr::operator==(const SuccExpr &rhs) const {
  return m_operand == rhs.m_operand;
}

LambdaExpr::LambdaExpr(std::string parameterName,
                       experimental::Type parameterType, ExprHandle body)
    : m_parameterName(std::move(parameterName)),
      m_parameterType(std::move(parameterType)), m_body(std::move(body)) {}

LambdaExpr::LambdaExpr(const LambdaExpr &expr)
    : m_parameterName(expr.m_parameterName),
      m_parameterType(expr.m_parameterType),
      m_body(std::make_unique<Expr>(*expr.m_body)) {}

LambdaExpr::~LambdaExpr() = default;

bool LambdaExpr::operator==(const LambdaExpr &rhs) const {
  return m_parameterName == rhs.m_parameterName &&
         m_parameterType == rhs.m_parameterType && *m_body == *rhs.m_body;
}
ApplyExpr::ApplyExpr(std::unique_ptr<Expr> function, ExprHandle arg)
    : m_func(std::move(function)), m_arg(std::move(arg)) {}

ApplyExpr::ApplyExpr(const ApplyExpr &expr)
    : m_func(std::make_unique<Expr>(*expr.m_func)),
      m_arg(std::make_unique<Expr>(*expr.m_arg)) {}

ApplyExpr::~ApplyExpr() = default;

bool ApplyExpr::operator==(const ApplyExpr &rhs) const {
  return *m_func == *rhs.m_func && *m_arg == *rhs.m_arg;
}

RecursionExpr::RecursionExpr(ExprHandle zeroCase,
                             std::unique_ptr<Expr> succCase, ExprHandle num)
    : m_zeroCase(std::move(zeroCase)), m_succCase(std::move(succCase)),
      m_num(std::move(num)) {}

RecursionExpr::RecursionExpr(const RecursionExpr &expr)
    : m_zeroCase(std::make_unique<Expr>(*expr.m_zeroCase)),
      m_succCase(std::make_unique<Expr>(*expr.m_succCase)),
      m_num(std::make_unique<Expr>(*expr.m_num)) {}

RecursionExpr::~RecursionExpr() = default;

bool RecursionExpr::operator==(const RecursionExpr &rhs) const {
  return *m_zeroCase == *rhs.m_zeroCase && *m_succCase == *rhs.m_succCase &&
         *m_num == *rhs.m_num;
}

NativeFunctionExpr::NativeFunctionExpr(std::string name, NativeFn function,
                                       experimental::Type domain,
                                       experimental::Type codomain)
    : m_name(std::move(name)), m_function(std::move(function)),
      m_domain(std::move(domain)), m_codomain(std::move(codomain)) {}
NativeFunctionExpr::~NativeFunctionExpr() = default;
NativeFunctionExpr::NativeFunctionExpr(const NativeFunctionExpr &) = default;

[[nodiscard]] bool
NativeFunctionExpr::operator==(const NativeFunctionExpr &rhs) const {
  return m_name == rhs.m_name;
}

} // namespace systemT
