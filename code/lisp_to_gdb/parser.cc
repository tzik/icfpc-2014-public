#include "parser.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <sstream>

#include "lisp2gdb.h"

namespace {

const char* kKnownSymbols[] = {
  "defun", "const", "load", "progn", "call-closure", "call", "ecall",
  "tcall", "cons", "+", "-", "*", "/", "car", "cdr", "=", ">", ">=",
  "atom?", "debug", "if", "inline"
};

#define ARRAYSIZE(A) (sizeof(A) / sizeof(A[0]))

bool IsKnownSymbol(const std::string& symbol) {
  return std::find(kKnownSymbols,
                   kKnownSymbols + ARRAYSIZE(kKnownSymbols), symbol)
    != kKnownSymbols + ARRAYSIZE(kKnownSymbols);
}

}  // namespace

Value* Parser::Parse(std::deque<std::string> tokens) {
  Value* program = ReadFrom(tokens);

  std::set<std::string> called(called_funcs_);
  std::set<std::string> defined(defined_funcs_);
  std::set<std::string> unknown(unknown_symbols_);
  // Remove defined program from called methods.
  for (const auto& func : defined) {
    called_funcs_.erase(func);
    unknown_symbols_.erase(func);
  }

  // Remove called methods from defined methods.
  for (const auto& func : called)
    defined_funcs_.erase(func);
  for (const auto& func : unknown)
    defined_funcs_.erase(func);
  // main function must be called.
  defined_funcs_.erase("main");
  
  return program;
}

Value* Parser::Atom(std::string token) {
  std::istringstream oss(token);
  long l;
  if (oss >> l) {
    return new Number(l);
  } else {
    return new Symbol(token);
  }
}

Value* Parser::ReadFrom(std::deque<std::string> &tokens) {
  assert(tokens.size() > 0);

  std::string token = tokens.front();
  tokens.pop_front();
  if (token == "(") {
    std::deque<Value*> list;
    while (tokens.front() != ")") {
      list.push_back(ReadFrom(tokens));
    }
    tokens.pop_front();  // ')'

    if (!list.empty() && list[0]->type() == Value::SYMBOL) {
      std::string label = list[0]->to_string();
      if (label == "defun" || label == "inline") {
        defined_funcs_.insert(list[1]->to_string());
      } else if (label == "call" || label == "tcall" || label == "load" ||
                 label == "ecall") {
        called_funcs_.insert(list[1]->to_string());
      } else if (!IsKnownSymbol(label)) {
        unknown_symbols_.insert(label);
      }
    }
    
    return new List(list);
  } else if (token == ")") { /* error */
    throw "Invalid token before: " +
      std::accumulate(tokens.begin(), tokens.end(), std::string(""));
  } else {
    return Atom(token);
  }
}

const std::set<std::string>& Parser::GetUnusedFunctions() {
  return defined_funcs_;
}

const std::set<std::string>& Parser::GetUndefinedFunctions() {
  return called_funcs_;
}
  
