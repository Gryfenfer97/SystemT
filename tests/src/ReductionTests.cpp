#include "SystemT/Expr.hpp"
#include "SystemT/TypeChecker.hpp"
#include "SystemT/builtins.hpp"
#include <SystemT/Substitution.hpp>
#include <gtest/gtest.h>
#include <variant>

namespace st = systemT;
TEST(Reduction, Succ) {
  st::Expr two =
      st::ApplyExpr(std::make_unique<st::Expr>(st::builtins::successor),
                    std::make_unique<st::Expr>(st::NatConstExpr(1)));

  st::SubstitutionVisitor evaluator;
  const auto value = evaluator.reduce(two);
  ASSERT_TRUE(value.checkType<st::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>(), st::NatConstExpr{2});
}

TEST(Reduction, LambdaApplication) {
  st::LambdaExpr plus_one =
      st::LambdaExpr("x", st::NaturalType{},
                     std::make_unique<st::Expr>(st::ApplyExpr(
                         std::make_unique<st::Expr>(st::builtins::successor),
                         std::make_unique<st::Expr>(st::VarExpr("x")))));

  st::Expr five =
      st::ApplyExpr(std::make_unique<st::Expr>(std::move(plus_one)),
                    std::make_unique<st::Expr>(st::NatConstExpr{4}));
  st::SubstitutionVisitor evaluator;
  const auto value = evaluator.reduce(five);
  ASSERT_TRUE(value.checkType<st::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>(), st::NatConstExpr{5});
}

/// Add num1 + num2 (5 + 7)
TEST(Reduction, AddFunction) {
  st::Expr add = st::LambdaExpr(
      "x", st::NaturalType{},
      std::make_unique<st::Expr>(st::LambdaExpr(
          "y", st::NaturalType{},
          std::make_unique<st::Expr>(st::RecursionExpr(
              std::make_unique<st::Expr>(st::VarExpr("x")),
              std::make_unique<st::Expr>(st::LambdaExpr(
                  "_", st::NaturalType{},
                  std::make_unique<st::Expr>(st::LambdaExpr(
                      "r", st::NaturalType{},
                      std::make_unique<st::Expr>(st::ApplyExpr(
                          std::make_unique<st::Expr>(st::builtins::successor),
                          std::make_unique<st::Expr>(st::VarExpr("r")))))))),
              std::make_unique<st::Expr>(st::VarExpr("y")))))));

  constexpr std::size_t num1 = 5;
  constexpr std::size_t num2 = 7;

  st::ApplyExpr app =
      st::ApplyExpr(std::make_unique<st::Expr>(st::ApplyExpr(
                        std::make_unique<st::Expr>(std::move(add)),
                        std::make_unique<st::Expr>(st::NatConstExpr(num1)))),
                    std::make_unique<st::Expr>(st::NatConstExpr(num2)));
  st::SubstitutionVisitor evaluator;
  // const auto intermediateValue = evaluator.reduce(app1);
  // ASSERT_TRUE(intermediateValue.checkType<st::LambdaExpr>());
  // auto value1_reduced_as_lambda = intermediateValue.as<st::LambdaExpr>();
  //
  // st::ApplyExpr app2 =
  //     st::ApplyExpr(std::make_unique<st::LambdaExpr>(value1_reduced_as_lambda),
  //                   std::make_unique<st::Expr>(st::NatConstExpr(num2)));
  const auto value = evaluator.reduce(app);

  ASSERT_TRUE(value.checkType<st::NatConstExpr>());
  ASSERT_EQ(value.as<st::NatConstExpr>().m_value, 7 + 5);
}
