#pragma once

#include <stdexcept>
#include <string>
#include <deque>

#include "lisp2gdb.h"
#include "util.h"

std::string Compile(Value* program);

// Error handlers
struct UnknownValueType : std::runtime_error {
  UnknownValueType(Value::Type t)
    : std::runtime_error("UnknownValueType" + util::IntToString(t)) {}
};

struct ValueTypeConflict : std::runtime_error {
  ValueTypeConflict() : std::runtime_error("ValueTypeConflict") {}
};

struct UnknownLabel : std::runtime_error {
  UnknownLabel(const std::string& label)
    : std::runtime_error("UnknownLabel: " + label) {}
};

struct UnknownSymbol : std::runtime_error {
  UnknownSymbol(const std::string& symbol)
    : std::runtime_error("UnknownSymbol: " + symbol) {}
};
