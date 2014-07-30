#include "converter.h"

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

long g_seq_count = 700000;
long seq() {
  return g_seq_count++;
}

std::string Eval(Value* x, Environment* e);

std::string GenerateCallFunctionStatement(
    std::string function_name,
    const std::deque<Value*>& argument_list,
    bool tail_recursion,
    Environment* e) {
  std::string result;
  for (auto argument: argument_list) {
    result += Eval(argument, e);
  }
  std::string call_function_op = tail_recursion ? "TAP" : "AP";
  result +=
      "LDF " + function_name + "\n" +
      call_function_op + " " + std::to_string(argument_list.size()) + "\n";
  return result;
}

std::string GenerateCallClosureStatement(
    Value* closure,
    const std::deque<Value*>& argument_list,
    Environment* e) {
  std::string result;
  for (auto argument: argument_list) {
    result += Eval(argument, e);
  }
  result += Eval(closure, e);
  result += "AP " + std::to_string(argument_list.size()) + "\n";
  return result;
}

std::string EvalList(List* l, Environment* e) {
  std::string result;
  std::string label = l->at(0)->to_string();
  if (label == "defun" || label == "inline") {
    std::string name = l->at(1)->to_string();
    const std::deque<Value*>* args = l->at(2)->to_list();
    Value* code = l->at(3);
    Environment new_env(e);
    for (Value* arg: *args) {
      if (arg->type() != Value::SYMBOL)
        throw ValueTypeConflict();
      std::string arg_name = arg->to_string();
      new_env.arguments().push_back(arg_name);
    }
    e->functions().push_back(name);
    result = name + ":\n" + Eval(code, &new_env) + "RTN\n";
  } else if (label == "const") {
    std::string name = l->at(1)->to_string();
    Value* value = l->at(2);
    if (value->type() != Value::NUMBER)
      throw ValueTypeConflict();
    e->consts().push_back(std::make_pair(name, value));
  } else if (label == "load") {
    std::string name = l->at(1)->to_string();
    result = "LDF " + name + "\n";
  } else if (label == "progn") {
    for (size_t i = 1; i < l->length(); i++) {
      result += Eval(l->at(i), e);
    }
  } else if (label == "call-closure") {
    std::deque<Value*> argument_list;
    if (l->length() >= 3)
      argument_list = *l->at(2)->to_list();
    result += GenerateCallClosureStatement(l->at(1), argument_list, e);
  } else if (label == "call" || label == "ecall") {  // ecall: external call
    std::string name = l->at(1)->to_string();
    const std::deque<Value*>* args = l->at(2)->to_list();
    result = GenerateCallFunctionStatement(name, *args, false, e);
  } else if (label == "tcall") {
    std::string name = l->at(1)->to_string();
    const std::deque<Value*>* args = l->at(2)->to_list();
    result = GenerateCallFunctionStatement(name, *args, true, e);
  } else if (label == "cons") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "CONS\n";;
  } else if (label == "+") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "ADD\n";;
  } else if (label == "-") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "SUB\n";;
  } else if (label == "*") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "MUL\n";;
  } else if (label == "/") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "DIV\n";;
  } else if (label == "car") {
    Value* lv = l->at(1);
    result = Eval(lv, e) + "CAR\n";;
  } else if (label == "cdr") {
    Value* lv = l->at(1);
    result = Eval(lv, e) + "CDR\n";;
  } else if (label == "=") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "CEQ\n";;
  } else if (label == ">") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "CGT\n";;
  } else if (label == ">=") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + Eval(rv, e) + "CGTE\n";;
  } else if (label == "atom?") {
    Value* lv = l->at(1);
    result = Eval(lv, e) + "ATOM\n";;
  } else if (label == "debug") {
    Value* lv = l->at(1);
    Value* rv = l->at(2);
    result = Eval(lv, e) + "DBUG\n" + Eval(rv, e);
  } else if (label == "if") {
    std::string name = "IF_" + std::to_string(seq());
    std::string name_true = name + "_TRUE";
    std::string name_false = name + "_FALSE";
    std::string name_end = name + "_END";
    Value* cond = l->at(1);
    Value* tc = l->at(2);
    Value* fc = l->at(3);
    result = Eval(cond, e) +
      "TSEL " + name_true + " " + name_false + "\n" +
      name_true + ":\n" +
      Eval(tc, e) +
      "LDC 1\n" +
      "TSEL " + name_end + " " + name_end + "\n" +
      name_false + ":\n" +
      Eval(fc, e) +
      name_end + ":\n";
  } else {
    for (std::string name: e->functions()) {
      if (name == label) {
        auto arguments = *l->to_list();  // Copy
        arguments.pop_front();
        return GenerateCallFunctionStatement(label, arguments, false, e);
      }
    }

    throw UnknownLabel(label);
  }

  return result;
}

std::string EvalSymbol(Value* x, Environment* e) {
  const std::string& label = x->to_string();
  int cnt = 0;
  for (auto arg : e->arguments()) {
    if (arg == label)
      return "LD 0 " + std::to_string(cnt) + "\n";
    cnt++;
  }
  for (auto c : e->consts()) {
    if (c.first == label)
      return "LDC " + std::to_string(c.second->to_int()) + "\n";
  }
  throw UnknownSymbol(label);
}

std::string Eval(Value* x, Environment* e) {
  switch (x->type()) {
  case Value::LIST:
    return EvalList(static_cast<List*>(x), e);
  case Value::NUMBER:
    return "LDC " + std::to_string(x->to_int()) + "\n";
  case Value::SYMBOL:
    return EvalSymbol(x, e);
  }

  throw UnknownValueType(x->type());
}

}  // namespace

std::string Compile(Value* program) {
  std::string result;
  if (program->type() == Value::LIST) {
    Environment e;
    result = "LDC 1\nTSEL main main\n" + Eval(program, &e);
  } else {
    std::cerr << "main method was not found.  "
              << "This code may be a library." << std::endl;
  }
  return result;
}
