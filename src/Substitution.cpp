#include "SystemT/Substitution.hpp"
#include "SystemT/builtins.hpp"

systemT::SubstitutionVisitor::SubstitutionVisitor() : current_env({}) {
  current_env.insert(std::pair(systemT::builtins::successor.m_name,
                               Expr(systemT::builtins::successor)));
}
