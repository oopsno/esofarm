#include <sstream>
#include "generator.hpp"

namespace EsoFarm {
namespace BrainF {

static inline std::string make_indent(size_t indent) {
  std::stringstream ss;
  for (auto i = 0; i < indent; ++i) {
    ss << "  ";
  }
  return ss.str();
}

std::string CGenerator::on_arithmetic(Arithmetic *ast) {
  if (ast->opr.type == OperatorType::Increase) {
    return make_indent(indent) + "U += " + std::to_string(ast->opr.repeat) + ";";
  } else {
    return make_indent(indent) + "U -= " + std::to_string(ast->opr.repeat) + ";";
  }
}

std::string CGenerator::on_pointer(Pointer *ast) {
  if (ast->opr.type == OperatorType::MoveLeft) {
    return make_indent(indent) + "P -= " + std::to_string(ast->opr.repeat) + ";";
  } else {
    return make_indent(indent) + "P += " + std::to_string(ast->opr.repeat) + ";";
  }
}

std::string CGenerator::on_read(Read *ast) {
  std::stringstream ss{make_indent(indent)};
  for (auto i = 1; i < ast->opr.repeat; ++i) {
    ss << "getchar(); ";
  }
  ss << "U = getchar();";
  return ss.str();
}

std::string CGenerator::on_print(Print *ast) {
  std::stringstream ss{make_indent(indent)};
  for (auto i = 0; i < ast->opr.repeat; ++i) {
    ss << "putchar(U); ";
  }
  return ss.str();
}

std::string CGenerator::on_loop(Loop *ast) {
  std::stringstream ss{};
  ss << make_indent(indent) + "while (U) {" << std::endl;
  indent += 1;
  for (auto *sub : ast->block) {
    ss << on_ast(sub) << std::endl;
  }
  indent -= 1;
  ss << make_indent(indent) + "}";
  return ss.str();
}

void CGenerator::collect(value_t value) {
  buffer << value << std::endl;
}

CGenerator::code_t CGenerator::codegen(const std::vector<AST *> &asts) {
  buffer.clear();
  buffer << "#include <stdio.h>\n"
      "char units[30000];\n"
      "char *p = units;\n\n"
      "#define U (*p)\n"
      "#define P (p)\n\n"
      "int main() {\n";
  indent = 1;
  for (auto *ast : asts) {
    collect(on_ast(ast));
  }
  buffer << "}\n";
  return buffer.str();
}

}
}