#include "util.h"

#include <string>
#include <sstream>

namespace util {

std::string IntToString(int n) {
  std::ostringstream oss;
  oss << n;
  return oss.str();
}

}  // namespace
