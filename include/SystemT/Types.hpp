#pragma once
#include <memory>
#include <print>
#include <string>
#include <variant>

namespace systemT::experimental {
class Type;
}
template <> struct std::formatter<systemT::experimental::Type>;

namespace systemT {

namespace experimental {

class Any {
public:
  [[nodiscard]] constexpr bool operator==(const Any &) const { return true; }
};

class NaturalType {
public:
  [[nodiscard]] constexpr bool operator==(const NaturalType &) const {
    return true;
  }
};

class Lambda {
public:
  Lambda(std::unique_ptr<Type> domain, std::unique_ptr<Type> codomain) noexcept;
  Lambda(Type domain, Type codomain) noexcept;
  Lambda(const Lambda &other) noexcept;
  Lambda(Lambda &&other) noexcept = default;
  ~Lambda() noexcept = default;
  Lambda &operator=(Lambda other) noexcept;
  Lambda &operator=(Lambda &&other) noexcept = default;

  [[nodiscard]] bool operator==(const Lambda &) const;

  [[nodiscard]] const Type &getDomain() const { return *m_domain; }
  [[nodiscard]] const Type &getCodomain() const { return *m_codomain; }

private:
  std::unique_ptr<Type> m_domain;
  std::unique_ptr<Type> m_codomain;
  void swap(Lambda &lambda) noexcept;
};

class Boolean {
public:
  [[nodiscard]] constexpr bool operator==(const Boolean &) const {
    return true;
  }
};

class Type {
public:
  Type(const Lambda &lambda) : content(lambda) {}
  Type(const NaturalType &nat) : content(nat) {}
  Type(const Boolean &b) : content(b) {}
  Type(const Any &a) : content(a) {}
  [[nodiscard]] bool operator==(const Type &rhs) const {
    return std::holds_alternative<Any>(content) ||
           std::holds_alternative<Any>(rhs.content) || content == rhs.content;
  }
  [[nodiscard]] std::string toString() const {
    return std::visit(PrintVisitor{}, content);
  }
  template <typename T, typename... Args>
  [[nodiscard]] bool check(Args... args) const {
    if (!std::holds_alternative<T>(content))
      return false;
    return std::get<T>(content) == T(std::forward<Args>(args)...);
  }
  template <typename T> [[nodiscard]] T as() { return std::get<T>(content); }

private:
  class PrintVisitor {
  public:
    std::string operator()(const NaturalType &) const { return "Nat"; }
    std::string operator()(const Boolean &) const { return "Bool"; }
    std::string operator()(const Lambda &lambda) const;
    std::string operator()(const Any &) const { return "Any"; }
  };
  std::variant<NaturalType, Lambda, Boolean, Any> content;
};

} // namespace experimental
class Type {
public:
  virtual ~Type() = default;
  [[nodiscard]] virtual bool operator==(const Type &) const = 0;
  virtual std::unique_ptr<Type> clone() const = 0;
  [[nodiscard]] virtual std::string toString() const = 0;
};

class NaturalType : public Type {
public:
  [[nodiscard]] bool operator==(const Type &rhs) const override {
    return typeid(*this) == typeid(rhs);
  }
  std::unique_ptr<Type> clone() const override {
    return std::make_unique<NaturalType>(*this);
  };
  [[nodiscard]] std::string toString() const override { return "Nat"; }
};

class Lambda : public Type {
public:
  std::shared_ptr<Type> m_domain;
  std::shared_ptr<Type> m_codomain;

  Lambda(std::shared_ptr<Type> domain, std::shared_ptr<Type> codomain)
      : m_domain(domain), m_codomain(codomain) {}
  [[nodiscard]] bool operator==(const Type &rhs) const override {
    if (typeid(*this) != typeid(rhs))
      return false;
    auto rhs_as_lambda = dynamic_cast<const Lambda &>(rhs);
    auto ret1 = *m_domain == *rhs_as_lambda.m_domain;
    auto ret2 = *m_codomain == *rhs_as_lambda.m_codomain;
    std::println("ret1 = {}", ret1);
    std::println("ret2 = {}", ret2);
    return *m_domain == *rhs_as_lambda.m_domain &&
           *m_codomain == *rhs_as_lambda.m_codomain;
  }
  std::unique_ptr<Type> clone() const override {
    return std::make_unique<Lambda>(m_domain->clone(), m_codomain->clone());
  };

  [[nodiscard]] std::string toString() const override {
    return std::format("Lam({} -> {})", m_domain->toString(),
                       m_codomain->toString());
  }
};

class Boolean : public Type {
public:
  [[nodiscard]] bool operator==(const Type &rhs) const override {
    return typeid(*this) == typeid(rhs);
  }
  std::unique_ptr<Type> clone() const override {
    return std::make_unique<Boolean>(*this);
  };

  [[nodiscard]] std::string toString() const override { return "Bool"; }
};

} // namespace systemT

template <std::derived_from<systemT::Type> Derived, typename CharT>
struct std::formatter<Derived, CharT> : std::formatter<std::string> {
  template <typename FormatContext>
  auto format(Derived &e, FormatContext &ctx) const {
    return std::formatter<std::string>::format(e.toString(), ctx);
  }
};
