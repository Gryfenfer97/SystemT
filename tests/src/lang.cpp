#include <SystemT/Expr.hpp>
#include <SystemT/Parser.hpp>
#include <SystemT/Substitution.hpp>
#include <SystemT/TypeChecker.hpp>
#include <SystemT/Types.hpp>
#include <gtest/gtest.h>

namespace st = systemT;

static systemT::VariableEnv emptyEnv = {};

TEST(Lang, succ) {

  const std::string code = "S 5";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor(emptyEnv);
  auto value = visitor.reduce(*ast[0]);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 6);
}

TEST(Lang, lambda) {
  const std::string code = "lam x: N -> N.S x";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor(emptyEnv);
  auto value = visitor.reduce(*ast[0]);
  systemT::TypeCheckerVisitor tc;
  auto type = tc.getType(value);
  ASSERT_TRUE(type.check<st::Lambda>(st::NaturalType{}, st::NaturalType{}));
}

TEST(Lang, application) {
  const std::string code = "(lam x: N.S x) 5";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor(emptyEnv);

  auto value = visitor.reduce(*ast[0]);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 6);
}

TEST(Lang, MultipleArgs) {
  const std::string code = "(lam m: N. lam n: N. (S n)) 0 7";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor(emptyEnv);
  auto value = visitor.reduce(*ast[0]);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 7 + 1);
}

TEST(Lang, Recurence) {
  const std::string code = "rec 42 (lam k: N. k) 0";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor(emptyEnv);
  auto value = visitor.reduce(*ast[0]);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 42);
}

TEST(Lang, AddFunction) {
  const std::string code = "(lam x: N. lam y: N. rec x (lam k: N. (lam r: N. "
                           "(S r))) y) 5 7";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor(emptyEnv);
  auto value = visitor.reduce(*ast[0]);
  ASSERT_TRUE(value.checkType<systemT::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 5 + 7);
}

TEST(Lang, Assignment) {
  const std::string code =
      "add = (lam x: N. lam y: N. rec x (lam k: N. (lam r: N. "
      "(S r))) y)";
  Parser parser(code);
  auto ast = parser.parse();
  systemT::SubstitutionVisitor visitor(emptyEnv);
  auto value = visitor.reduce(*ast[0]);
  ASSERT_TRUE(value.checkType<systemT::LambdaExpr>());
}

TEST(Lang, parseMultipleAssignment) {
  const std::string code = R"(succ = S
  succ 1)";
  Parser parser(code);
  auto ast = parser.parse();
  ASSERT_EQ(ast.size(), 2);
}

TEST(Lang, AddDefAndApply) {
  const std::string code =
      R"(add = (lam x: N. lam y: N. rec x (lam k: N. (lam r: N. (S r))) y)
  main = add 5 7)";
  Parser parser(code);
  auto ast = parser.parse();
  ASSERT_EQ(ast.size(), 2);
  systemT::VariableEnv env;
  systemT::SubstitutionVisitor visitor(env);
  for (const auto &statement : ast) {
    visitor.reduce(*statement);
  }
  ASSERT_TRUE(env.contains("main"));
  ASSERT_TRUE(env.lookup("main")->checkType<systemT::NatConstExpr>());
  ASSERT_EQ(env.lookup("main")->as<systemT::NatConstExpr>().m_value, 5 + 7);
}
