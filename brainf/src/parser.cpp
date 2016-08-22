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
      #define PUSH(t) os.push_back({OperatorType::t, 1, {lineno, column, offset}, {lineno, column, offset}})
      #define APPEND(t) \
        const auto begin = os.back().begin; \
        const auto repeat = os.back().repeat; \
        os.pop_back(); \
        os.push_back({OperatorType::t, repeat + 1, begin, {lineno, column, offset}})
      #define MATCH(t) case static_cast<char>(OperatorType::t): if (SAME_WITH_LATEST(t)) { APPEND(t); } else { PUSH(t); } break
      MATCH(Increase);
      MATCH(Decrease);
      MATCH(MoveLeft);
      MATCH(MoveRight);
      MATCH(Read);
      MATCH(Print);
      MATCH(LoopBegin);
      MATCH(LoopEnd);
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