#pragma once

#include <string>
#include <vector>
#include "operator.hpp"
#include "ast.hpp"

namespace EsoFarm {
namespace BrainF {

class SyntaxError: std::exception {
 public:
  SyntaxError(const std::string &what, const Position &where) : what(what), where(where) { }
  const std::string what;
  const Position where;
};

std::vector<Operator> tokenize(const std::string &code);
bool syntax_check(const std::vector<Operator> &tokens) throw(SyntaxError);
std::vector<AST *> parser(const std::vector<Operator> &opr);


}
}
