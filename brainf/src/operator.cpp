#include <algorithm>
#include "operator.hpp"

namespace EsoFarm {
namespace BrainF {

std::ostream &operator<<(std::ostream &os, const Operand &o) {
  switch (o.type) {
    case OperandType::DynamicUnit:
      os << "U";
      break;
    case OperandType::PredictedUnit:
      os << "#" << o.value;
      break;
    default:
      os << "P";
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const Position &p) {
  return os << p.line << ":" << p.column << " (+" << p.offset << ")";
}

std::ostream &operator<<(std::ostream &os, const Operator &o) {
  return os << o.repeat << " " << (char) (o.type) << " " << o.operand << " [From " << o.begin << " To " << o.end << "]";
}

std::ostream &operator<<(std::ostream &os, const std::vector<Operator> &oprs) {
  std::copy(oprs.cbegin(), oprs.cend(), std::ostream_iterator<Operator>(os, "\n"));
  return os;
}

}
}
