#include "tokenizer.h"

#include <cassert>
#include <deque>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cmath>

#include "lisp2gdb.h"
#include "parser.h"

namespace {

void replace(std::string &s, std::string from, std::string to) {
  size_t next;
  for (size_t idx = 0; (next = s.find(from, idx)) != std::string::npos;) {
    s.replace(next, from.size(), to);
    idx = next + to.size(); 
  }
}

}  // namespace

std::deque<std::string> Tokenizer::Tokenize(std::string program) {
  replace(program, ")", " ) ");
  replace(program, "(", " ( ");
  std::istringstream iss(program);
  std::deque<std::string> result;
  result.insert(result.end(),
                std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>());
  return result;
}
