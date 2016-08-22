#include "operator.hpp"

namespace EsoFarm {
namespace BrainF {

std::ostream &operator<<(std::ostream &os, const Position &p) {
  return os << p.line << ":" << p.column << " (+" << p.offset << ")";
}

std::ostream &operator<<(std::ostream &os, const Operator &o) {
  return os << o.repeat << " " << (char) (o.type) << " [From " << o.begin << " To " << o.end << "]";
}

}
}
