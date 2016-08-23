#pragma once

#include <istream>
#include <type_traits>
#include <vector>

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

enum class OperandType {
  DynamicUnit, PredictedUnit, Pointer
};

struct Position {
  size_t line, column, offset;
};

struct Operand {
  OperandType type;
  uint32_t value = 0;
};

struct Operator {
  OperatorType type;
  Operand operand;
  size_t repeat;
  Position begin, end;
};

std::ostream &operator<<(std::ostream &os, const Operand &o);
std::ostream &operator<<(std::ostream &os, const Position &p);
std::ostream &operator<<(std::ostream &os, const Operator &o);
std::ostream &operator<<(std::ostream &os, const std::vector<Operator> &oprs);

}
}