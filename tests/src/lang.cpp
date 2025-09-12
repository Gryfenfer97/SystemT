#include "SystemT/ASTPrint.hpp"
#include <SystemT/Expr.hpp>
#include <SystemT/Parser.hpp>
#include <SystemT/Substitution.hpp>
#include <SystemT/TypeChecker.hpp>
#include <SystemT/Types.hpp>
#include <gtest/gtest.h>

namespace st = systemT;

TEST(Lang, succ) {
  const std::string code = "S 5";
  Parser parser(code);
  auto ast = parser.parse();
  st::ASTPrintVisitor astPrinter;
  std::println("{}", astPrinter.toString(*ast));
  systemT::SubstitutionVisitor visitor;
  auto value = visitor.reduce(*ast);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 6);
}

TEST(Lang, lambda) {
  const std::string code = "lam x: N -> N.S x";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor;
  auto value = visitor.reduce(*ast);
  systemT::TypeCheckerVisitor tc;
  auto type = tc.getType(value);
  ASSERT_TRUE(type.check<st::experimental::Lambda>(
      st::experimental::NaturalType{}, st::experimental::NaturalType{}));
}

TEST(Lang, application) {
  const std::string code = "(lam x: N.S x) 5";
  Parser parser(code);
  auto ast = parser.parse();
  st::ASTPrintVisitor astPrinter;
  std::println("{}", astPrinter.toString(*ast));
  systemT::SubstitutionVisitor visitor;
  auto value = visitor.reduce(*ast);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 6);
}

TEST(Lang, MultipleArgs) {
  const std::string code = "(lam m: N. lam n: N. (S n)) 0 7";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor;
  auto value = visitor.reduce(*ast);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 7 + 1);
}

TEST(Lang, Recurence) {
  const std::string code = "rec 42 (lam k: N. k) 0";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor;
  auto value = visitor.reduce(*ast);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 42);
}

TEST(Lang, AddFunction) {
  // const std::string code =
  //     "lam m: N. lam n: N. (rec n (lam k: N. lam r: N. (S r) m)) 5 7";
  const std::string code = "(lam x: N. (lam y: N. (rec x (lam k: N. (lam r: N. "
                           "(S r))) y))) 5 7";
  Parser parser(code);
  auto ast = parser.parse();
  st::ASTPrintVisitor astPrinter;
  std::println("{}", astPrinter.toString(*ast));
  systemT::SubstitutionVisitor visitor;
  auto value = visitor.reduce(*ast);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 5 + 7);
}
