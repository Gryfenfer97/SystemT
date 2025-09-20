#include <SystemT/Types.hpp>
#include <format>
#include <memory>

template <>
struct std::formatter<systemT::Type> : std::formatter<std::string_view> {
  auto format(const systemT::Type &obj, std::format_context &ctx) const {
    return std::formatter<string_view>::format(obj.toString(), ctx);
  }
};

namespace systemT {

Lambda::Lambda(std::unique_ptr<Type> domain,
               std::unique_ptr<Type> codomain) noexcept
    : m_domain(std::move(domain)), m_codomain(std::move(codomain)) {}

Lambda::Lambda(const Type &domain, const Type &codomain) noexcept
    : m_domain(std::make_unique<Type>(domain)),
      m_codomain(std::make_unique<Type>(codomain)) {}

Lambda::Lambda(const Lambda &other) noexcept
    : m_domain(std::make_unique<Type>(*other.m_domain)),
      m_codomain(std::make_unique<Type>(*other.m_codomain)) {}

Lambda &Lambda::operator=(Lambda other) noexcept {
  other.swap(*this);
  return *this;
}

bool Lambda::operator==(const Lambda &) const {
  return *m_domain == *m_domain && *m_codomain == *m_codomain;
}

void Lambda::swap(Lambda &other) noexcept {
  std::swap(m_domain, other.m_domain);
  std::swap(m_codomain, other.m_codomain);
}

std::string Type::PrintVisitor::operator()(const Lambda &lambda) const {
  return std::format("λ {} -> {}", lambda.getDomain(), lambda.getCodomain());
}

} // namespace systemT
