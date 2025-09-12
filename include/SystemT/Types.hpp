#pragma once
#include <memory>
#include <string>
#include <variant>

namespace systemT::experimental {
class Type;
}
template <> struct std::formatter<systemT::experimental::Type>;

namespace systemT::experimental {

class Any {
public:
  [[nodiscard]] constexpr bool operator==(const Any & /*rhs*/) const {
    return true;
  }
};

class NaturalType {
public:
  [[nodiscard]] constexpr bool operator==(const NaturalType & /*rhs*/) const {
    return true;
  }
};

class Lambda {
public:
  Lambda(std::unique_ptr<Type> domain, std::unique_ptr<Type> codomain) noexcept;
  Lambda(const Type &domain, const Type &codomain) noexcept;
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
  void swap(Lambda &other) noexcept;
};

class Boolean {
public:
  [[nodiscard]] constexpr bool operator==(const Boolean & /*rhs*/) const {
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
    if (!std::holds_alternative<T>(content)) {
      return false;
    }
    return std::get<T>(content) == T(std::forward<Args>(args)...);
  }
  template <typename T> [[nodiscard]] T as() { return std::get<T>(content); }

private:
  class PrintVisitor {
  public:
    std::string operator()(const NaturalType & /*type*/) const { return "Nat"; }
    std::string operator()(const Boolean & /*type*/) const { return "Bool"; }
    std::string operator()(const Lambda &lambda) const;
    std::string operator()(const Any & /*type*/) const { return "Any"; }
  };
  std::variant<NaturalType, Lambda, Boolean, Any> content;
};

} // namespace systemT::experimental
