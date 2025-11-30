# System T

An implementation of **Gödel’s System T**, a simply-typed lambda calculus with natural numbers and primitive recursion.  

---

## 📖 Examples

### Addition

```
add = (lam x: N. lam y: N. rec x (lam k: N. (lam r: N. (S r))) y)
```

## 💻 CLI

```bash
st-cli ./input.st
```

## 🚀 Getting Started

### Prerequisites
- A C++23-compliant compiler
- [CMake](https://cmake.org/) (≥ 3.24)  
- (Optional, for tests) [Conan 2](https://conan.io/)

## 🔧 Build Instructions

### Build the Interpreter
```bash
# Clone the repository
git clone https://github.com/Gryfenfer97/SystemT.git
cd SystemT

# 1. Install test dependencies
conan install . --build=missing

# 2. Configure with the "conan-<config>" preset
cmake --preset conan-release

# 3. Build
cmake --build --preset conan-release
```

### 🧪 Running Tests

```bash
ctest --preset conan-release
```

