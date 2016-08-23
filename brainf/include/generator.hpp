#pragma once
#include <sstream>
#include "ast.hpp"

namespace EsoFarm {
namespace BrainF {

/**
 * Forward Declaration
 */

class Arithmetic;

/**
 * Common Code Generator Interfaces
 */
template<typename C, typename V>
class CodeGenerator {
 public:
  using code_t = C;
  using value_t = V;
  virtual value_t on_arithmetic(Arithmetic *ast) = 0;
  virtual value_t on_pointer(Pointer *pointer) = 0;
  virtual value_t on_read(Read *read) = 0;
  virtual value_t on_print(Print *print) = 0;
  virtual value_t on_loop(Loop *loop) = 0;
  virtual value_t on_ast(const AST * ast) {
      value_t value;
      #define TRY_ON_TYPE(t, fn) if (isa<t>(ast)) { value = fn((t *) ast); }
      TRY_ON_TYPE(Arithmetic, on_arithmetic)
      else TRY_ON_TYPE(Pointer, on_pointer)
      else TRY_ON_TYPE(Read, on_read)
      else TRY_ON_TYPE(Print, on_print)
      else TRY_ON_TYPE(Loop, on_loop)
      #undef TRY_ON_TYPE
      return value;
  }
  virtual code_t codegen(const std::vector<AST *> &asts) = 0;
  virtual void collect(value_t value) = 0;
};

class CGenerator: public CodeGenerator<std::string, std::string> {
 public:
  CGenerator() : buffer{}, indent{0} { }
  value_t on_arithmetic(Arithmetic *ast);
  value_t on_pointer(Pointer *pointer);
  value_t on_read(Read *read);
  value_t on_print(Print *print);
  value_t on_loop(Loop *loop);
  code_t codegen(const std::vector<AST *> &asts);
  void collect(value_t value);
 private:
  std::stringstream buffer;
  size_t indent;
};

}
}