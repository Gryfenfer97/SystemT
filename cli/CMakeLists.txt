add_executable(SystemT-CLI)

target_sources(SystemT-CLI PRIVATE src/main.cpp)

find_package(CLI11 REQUIRED)

target_link_libraries(SystemT-CLI PRIVATE CLI11::CLI11 SystemT)

set_target_properties(SystemT-CLI PROPERTIES OUTPUT_NAME st-cli)
