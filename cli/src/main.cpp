#include <CLI/CLI.hpp>
#include <SystemT/ASTPrint.hpp>
#include <SystemT/Parser.hpp>
#include <SystemT/Substitution.hpp>
#include <optional>

std::optional<std::string>
interpret_file(const std::filesystem::path &filePath) {
  std::ifstream file{filePath};
  std::stringstream content;
  content << file.rdbuf();
  Parser parser(content.str());
  const auto ast = parser.parse();
  systemT::VariableEnv env;
  systemT::SubstitutionVisitor visitor(env);
  for (const auto &statement : ast) {
    visitor.reduce(*statement);
  }
  if (!env.contains("main")) {
    return std::nullopt;
  }
  systemT::ASTPrintVisitor printVisitor;
  return printVisitor.toString(*env.lookup("main"));
}

int main(int argc, char **argv) {
  CLI::App app{"System T interpreter"};
  std::string file;
  app.add_option("input_file", file, "Require an existing file")
      ->check(CLI::ExistingFile);
  CLI11_PARSE(app, argc, argv);
  const auto output = interpret_file(std::filesystem::path{file});
  if (!output) {
    std::cerr << "There is no 'main' variable in the file\n";
    return 1;
  }
  std::cout << output.value() << '\n';
}
