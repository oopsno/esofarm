#pragma once

#include <string>
#include <vector>
#include "operator.hpp"

namespace EsoFarm {
namespace BrainF {

std::vector<Operator> tokenize(const std::string &code);

}
}
