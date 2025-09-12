cmake_minimum_required(VERSION 3.24)
project(SystemT)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_program(CLANG_TIDY NAMES "clang-tidy" REQUIRED)
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})

set(SOURCES src/Substitution.cpp src/Tokenizer.cpp src/Parser.cpp src/Types.cpp
            src/Expr.cpp)
set(HEADERS include/SystemT/Expr.hpp include/SystemT/Types.hpp
            include/SystemT/TypeChecker.hpp)

add_library(${PROJECT_NAME} SHARED)
target_sources(
  ${PROJECT_NAME}
  PRIVATE ${SOURCES}
  PUBLIC FILE_SET HEADERS BASE_DIRS include/ FILES ${HEADERS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow
                                              -Wconversion)
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--no-undefined)

add_subdirectory(tests)
