#pragma once

#include <string>
#include <deque>

#include "lisp2gdb.h"

struct PreProcessor {
  // Remove comments, and concat all lines.
  static std::string PreProcess(std::istream& is);
};
  
