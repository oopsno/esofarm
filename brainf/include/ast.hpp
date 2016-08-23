#pragma once
#include <vector>
#include <typeinfo>
#include "operator.hpp"

namespace EsoFarm {
namespace BrainF {

enum class ASTType {
  Arithmetic, Pointer, Read, Print, Loop
};

/**
 * A sort of classes holding AST
 */
class AST {
 public:
  virtual ASTType getType() const = 0;
  virtual std::string dump(size_t indent = 0) const = 0;
};

/**
 * + and -
 */
class Arithmetic: public AST {
 public:
  Arithmetic(const Operator &o);
  ASTType getType() const;
  std::string dump(size_t indent = 0) const;
 private:
  Operator opr;
};

/**
 * < and >
 */
class Pointer: public AST {
 public:
  Pointer(const Operator &o);
  ASTType getType() const;
  std::string dump(size_t indent = 0) const;
 private:
  Operator opr;
};

/**
 * ,
 */
class Read: public AST {
 public:
  Read(const Operator &o);
  ASTType getType() const;
  std::string dump(size_t indent = 0) const;
 private:
  Operator opr;
};

/**
 * .
 */
class Print: public AST {
 public:
  Print(const Operator &o);
  ASTType getType() const;
  std::string dump(size_t indent = 0) const;
 private:
  Operator opr;
};

/**
 * [ and ]
 */
class Loop: public AST {
 public:
  Loop(const Operator &o);
  ASTType getType() const;
  void setLoopEnd(const Operator &o);
  std::vector<AST *> &getBlock();
  std::string dump(size_t indent = 0) const;
 private:
  std::vector<AST *> block;
  Operator opr, end;
};

std::ostream &operator<<(std::ostream &os, AST *ast);
std::ostream &operator<<(std::ostream &os, std::vector<AST *> &asts);

template<typename T>
bool isa(AST *ast) {
  return typeid(*ast).hash_code() == typeid(T).hash_code();
}

}
}


