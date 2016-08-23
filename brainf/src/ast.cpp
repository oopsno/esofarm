#include <algorithm>
#include <sstream>
#include "ast.hpp"

namespace EsoFarm {
namespace BrainF {

#define IMPL_GET_TYPE(t) ASTType t::getType() const { return ASTType::t; }
IMPL_GET_TYPE(Arithmetic);
IMPL_GET_TYPE(Pointer);
IMPL_GET_TYPE(Print);
IMPL_GET_TYPE(Read);
IMPL_GET_TYPE(Loop);
#undef IMPL_GET_TYPE

#define IMPL_CTOR(c) c::c(const Operator &o) : opr{o} { }
IMPL_CTOR(Arithmetic);
IMPL_CTOR(Pointer);
IMPL_CTOR(Print);
IMPL_CTOR(Read);
#undef IMPL_CTOR

Loop::Loop(const Operator &o) : opr{o}, block{} { }

std::vector<AST *> &Loop::getBlock() {
  return block;
}

void Loop::setLoopEnd(const Operator &o) {
  end = o;
}

static inline std::string make_indent(size_t indent) {
  std::stringstream ss;
  for (auto i = 0; i < indent; ++i) {
    ss << "  ";
  }
  return ss.str();
}

#define IMPL_DUMP(t) \
std::string t::dump(size_t indent) const { \
  std::stringstream ss; \
  ss << make_indent(indent) << opr; \
  return ss.str(); \
}
IMPL_DUMP(Arithmetic);
IMPL_DUMP(Pointer);
IMPL_DUMP(Read);
IMPL_DUMP(Print);
#undef IMPL_DUMP

std::string Loop::dump(size_t indent) const {
  std::stringstream ss;
  ss << make_indent(indent) << opr << std::endl;
  for (const auto *ast : block) {
    ss << ast->dump(indent + 1) << std::endl;
  }
  ss << make_indent(indent) << end;
  return ss.str();
}

std::ostream &operator<<(std::ostream &os, AST *ast) {
  return os << ast->dump();
}

std::ostream &operator<<(std::ostream &os, std::vector<AST *> &asts) {
  std::copy(asts.cbegin(), asts.cend(), std::ostream_iterator<AST *>(os, "\n"));
  return os;
}

}
}

