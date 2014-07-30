#pragma once

#include <string>
#include <deque>

class List;

class Value {
 public:
  enum Type {
    NUMBER,
    SYMBOL,
    LIST,
  };

  virtual Type type() = 0;
  virtual std::string to_string() { throw; };
  virtual long to_int() { throw; };
  virtual List* as_list() { throw; };
  virtual const std::deque<Value*>* to_list() { throw; };
};

class Number: public Value {
 public:
  Number(long value) : value_(value) {}
  virtual Type type() { return NUMBER; }
  virtual long to_int() { return value_; };
 private:
  long value_;
};

class Symbol: public Value {
 public:
  Symbol(std::string value) : symbol_name_(value) {}
  virtual Type type() { return SYMBOL; }
  virtual std::string to_string() { return symbol_name_;; };
 private:
  std::string symbol_name_;
};

class List: public Value {
 public:
  List(const std::deque<Value*> list) : list_(list) {}
  virtual Type type() { return LIST; }
  Value* at(int index) { return list_[index]; }
  size_t length() { return list_.size(); }
  virtual List* as_list() { return this; };
  virtual const std::deque<Value*>* to_list() { return &list_; };
 private:
  std::deque<Value*> list_;
};

class Environment {
 public:
  Environment()
      : arguments_(new std::deque<std::string>),
        consts_(new std::deque<std::pair<std::string, Value*> >),
        functions_(new std::deque<std::string>),
        local_(new std::deque<std::string>) {
  }
  Environment(const Environment* e)
      : arguments_(new std::deque<std::string>),
        consts_(e->consts_),
        functions_(e->functions_),
        local_(new std::deque<std::string>) {
  }
  std::deque<std::string>& arguments() {
    return *arguments_;
  };
  std::deque<std::pair<std::string, Value*> >& consts() {
    return *consts_;
  };
  std::deque<std::string>& functions() {
    return *functions_;
  };

 private:
  std::deque<std::string>* arguments_;
  std::deque<std::pair<std::string, Value*> >* consts_;
  std::deque<std::string>* functions_;
  std::deque<std::string>* local_;
};
