#pragma once

#include <type_traits>
#include <istream>

namespace EsoFarm {
namespace BrainF {

enum class OperatorType: char {
  Increase  = '+',
  Decrease  = '-',
  MoveLeft  = '<',
  MoveRight = '>',
  Read      = ',',
  Print     = '.',
  LoopBegin = '[',
  LoopEnd   = ']'
};

struct Position {
  size_t offset, line, column;
};

struct Operator {
  OperatorType type;
  size_t repeat;
  Position begin, end;
};

std::ostream &operator<<(std::ostream &os, const Operator &o);
std::ostream &operator<<(std::ostream &os, const Position &p);

}
}