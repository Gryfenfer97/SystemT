#include <SystemT/TypeChecker.hpp>
#include <SystemT/Types.hpp>
#include <SystemT/builtins.hpp>
#include <gtest/gtest.h>
#include <memory>

namespace st = systemT;

TEST(TypeChecker, succ) {
  st::Expr two =
      st::ApplyExpr(std::make_unique<st::Expr>(st::builtins::successor),
                    std::make_unique<st::Expr>(st::NatConstExpr(0)));
  st::TypeCheckerVisitor tc;
  ASSERT_TRUE(tc.getType(two).check<st::NaturalType>());
}

TEST(TypeChecker, Lambda) {
  st::Expr id = st::LambdaExpr("x", st::NaturalType{},
                               std::make_unique<st::Expr>(st::VarExpr("x")));
  st::TypeCheckerVisitor tc;
  auto type = tc.getType(id);
  ASSERT_TRUE(
      tc.getType(id).check<st::Lambda>(st::NaturalType{}, st::NaturalType{}));
}

TEST(TypeChecker, Recursion) {
  st::Expr five = st::RecursionExpr(
      std::make_unique<st::Expr>(st::NatConstExpr(0)),
      std::make_unique<st::Expr>(st::LambdaExpr(
          "x", st::NaturalType{},
          std::make_unique<st::Expr>(st::LambdaExpr(
              "r", st::NaturalType{},
              std::make_unique<st::Expr>(st::ApplyExpr(
                  std::make_unique<st::Expr>(st::builtins::successor),
                  std::make_unique<st::Expr>(st::VarExpr("r")))))))),
      std::make_unique<st::Expr>(st::NatConstExpr(5)));
  st::TypeCheckerVisitor tc;
  ASSERT_TRUE(tc.getType(five).check<st::NaturalType>());
}

TEST(TypeChecker, AddFunction) {
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
  st::TypeCheckerVisitor tc;
  ASSERT_TRUE(tc.getType(add).check<st::Lambda>(
      st::NaturalType{}, st::Lambda(st::NaturalType{}, st::NaturalType{})));
}
