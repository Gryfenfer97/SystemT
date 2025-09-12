project(SystemTTests)

include(CTest)
include(GoogleTest)

set(SOURCES src/main.cpp src/TypeCheckerTests.cpp src/ReductionTests.cpp
            src/lang.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(GTest REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC SystemT GTest::GTest)

gtest_discover_tests(${PROJECT_NAME})
