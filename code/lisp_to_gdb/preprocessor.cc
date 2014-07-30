#include "preprocessor.h"

#include <cassert>
#include <deque>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cmath>

#include "lisp2gdb.h"
#include "parser.h"

std::string PreProcessor::PreProcess(std::istream& is) {
  std::string program, line;
  while(std::getline(is, line)) {
    size_t comment_starting_point = line.find(';');
    if (comment_starting_point != std::string::npos)
      line.resize(comment_starting_point);

    program += line;
  }
  return program;
}
