#include "parser.hpp"

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
      #define MATCH(t, opr) case static_cast<char>(OperatorType::t): if (SAME_WITH_LATEST(t)) { APPEND(t, opr); } else { PUSH(t, opr); } break
      MATCH(Increase, DynamicUnit);
      MATCH(Decrease, DynamicUnit);
      MATCH(MoveLeft, Pointer);
      MATCH(MoveRight, Pointer);
      MATCH(Read, DynamicUnit);
      MATCH(Print, DynamicUnit);
      MATCH(LoopBegin, DynamicUnit);
      MATCH(LoopEnd, DynamicUnit);
      case '\r':
      case '\n':
        lineno += 1;
        column = 1;
        break;
      default:
        continue;
    }
  }
  return os;
}

}
}