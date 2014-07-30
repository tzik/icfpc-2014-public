#pragma once

#include <ostream>
#include <deque>
#include <set>
#include <vector>

#include "lisp2gdb.h"

class Parser {
 public:
  Value* Parse(std::deque<std::string> tokens);

  const std::set<std::string>& GetUnusedFunctions();
  const std::set<std::string>& GetUndefinedFunctions();
  
 private:
  Value* ReadFrom(std::deque<std::string> &tokens);
  Value* Atom(std::string token);

  std::set<std::string> called_funcs_;
  std::set<std::string> defined_funcs_;
  std::set<std::string> unknown_symbols_;
};

