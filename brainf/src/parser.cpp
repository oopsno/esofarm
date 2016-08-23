#include "parser.hpp"
#include <stack>

namespace EsoFarm {
namespace BrainF {

std::vector<Operator> tokenize(const std::string &code) {
  std::vector<Operator> os{};
  size_t lineno = 1;
  size_t column = 1;
  for (size_t offset = 0; offset < code.length(); ++offset) {
    switch (code[offset]) {
      #define SAME_WITH_LATEST(t) (not os.empty() and os.back().type == OperatorType::t)
      #define PUSH(t, opr) os.push_back({OperatorType::t, {OperandType::opr}, 1, {lineno, column, offset}, {lineno, column, offset}})
      #define APPEND(t, opr) \
        const auto begin = os.back().begin; \
        const auto repeat = os.back().repeat; \
        os.pop_back(); \
        os.push_back({OperatorType::t, {OperandType::opr}, repeat + 1, begin, {lineno, column, offset}})
      #define COUNT(t, opr) case static_cast<char>(OperatorType::t): if (SAME_WITH_LATEST(t)) { APPEND(t, opr); } else { PUSH(t, opr); } break
      #define MATCH(t, opr) case static_cast<char>(OperatorType::t): PUSH(t, opr); break
      COUNT(Increase, DynamicUnit);
      COUNT(Decrease, DynamicUnit);
      COUNT(MoveLeft, Pointer);
      COUNT(MoveRight, Pointer);
      COUNT(Read, DynamicUnit);
      COUNT(Print, DynamicUnit);
      MATCH(LoopBegin, DynamicUnit);
      MATCH(LoopEnd, DynamicUnit);
      case '\n':
        lineno += 1;
        column = 1;
        break;
      default:
        continue;
      #undef MATCH
      #undef COUNT
      #undef APPEND
      #undef PUSH
      #undef SAME_WITH_LATEST
    }
  }
  return os;
}

bool syntax_check(const std::vector<Operator> &tokens) throw(SyntaxError) {
  std::stack<const Operator *> s;
  for (const auto &t : tokens) {
    switch (t.type) {
      case OperatorType::LoopBegin:
        s.push(&t);
        break;
      case OperatorType::LoopEnd:
        if (s.empty()) {
          throw SyntaxError{"Unexpected `['", t.begin};
        } else {
          s.pop();
        }
        break;
      default:
        continue;
    }
  }
  if (not s.empty()) {
    throw SyntaxError{"Unexpected EOF: `[' not matched", s.top()->begin};
  };
  return true;
}

std::vector<AST *> parser(const std::vector<Operator> &opr) {
  std::vector<AST *> asts{};
  std::stack<std::vector<AST *> *> stack{};
  std::vector<AST *> *curr_block = &asts;
  for (auto i = 0; i < opr.size(); ++i) {
    auto const &o = opr[i];
    switch (o.type) {
      case OperatorType::Decrease:
      case OperatorType::Increase:
        curr_block->push_back(new Arithmetic{o});
        break;
      case OperatorType::MoveLeft:
      case OperatorType::MoveRight:
        curr_block->push_back(new Pointer{o});
        break;
      case OperatorType::Read:
        curr_block->push_back(new Read{o});
        break;
      case OperatorType::Print:
        curr_block->push_back(new Print{o});
        break;
      case OperatorType::LoopBegin:
        curr_block->push_back(new Loop{o});
        stack.push(curr_block);
        curr_block = &((Loop *) (curr_block->back()))->getBlock();
        break;
      case OperatorType::LoopEnd:
        curr_block = stack.top();
        stack.pop();
        ((Loop *)(curr_block->back()))->setLoopEnd(o);
        break;
    }
  }
  return asts;
}

}
}