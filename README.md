# System T

An implementation of **Gödel’s System T**, a simply-typed lambda calculus with natural numbers and primitive recursion.  

---

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

# Configure and build with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSYSTEMT_BUILD_TESTS=OFF
cmake --build build
```

### 🧪 Running Tests

The interpreter itself has no external dependencies, but the test suite requires **Conan 2**.  
```bash
# 1. Install test dependencies
conan install . --output-folder=build/tests --build=missing

# 2. Configure with the "tests" preset
cmake --preset tests

# 3. Build
cmake --build --preset tests

# 4. Run the tests
ctest --preset tests
```
