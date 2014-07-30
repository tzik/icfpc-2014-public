#pragma once

#include <string>
#include <deque>

#include "lisp2gdb.h"

struct Tokenizer {
  static std::deque<std::string> Tokenize(std::string program);
};
