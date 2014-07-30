#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace gcc {

struct Value;
struct IntegerValue;
struct ConsValue;
struct ClosureValue;

struct Frame;

struct Control;
struct JoinControl;
struct RetControl;

struct Instruction;

struct TraverseContext {
  std::set<const Value*> visited_value;
  std::set<const Frame*> visited_frame;
};

using integer = int64_t;
using ValueRef = std::shared_ptr<Value>;
using FrameRef = std::shared_ptr<Frame>;
using ControlRef = std::shared_ptr<Control>;
using Program = std::vector<Instruction>;

struct ValueTypeMismatch : std::runtime_error {
  ValueTypeMismatch() : std::runtime_error("ValueTypeMismatch") {}
};

struct InvalidFrame : std::runtime_error {
  InvalidFrame(integer program_counter)
      : std::runtime_error(
          "InvalidFrame: " + std::to_string(program_counter)) {
  }
};

struct FrameOutOfRange : std::runtime_error {
  FrameOutOfRange(integer program_counter)
      : std::runtime_error(
          "FrameOutOfRange: " + std::to_string(program_counter)) {
  }
};

struct FrameSizeMismatch : std::runtime_error {
  FrameSizeMismatch(integer program_counter)
      : std::runtime_error(
          "FrameSizeMismatch: " + std::to_string(program_counter)) {}
};

struct DivideByZero : std::runtime_error {
  DivideByZero() : std::runtime_error("DivideByZero") {}
};

struct ControlTypeMismatch : std::runtime_error {
  ControlTypeMismatch() : std::runtime_error("ControlTypeMismatch") {}
};

struct LoadFailed : std::runtime_error {
  LoadFailed() : std::runtime_error("LoadFailed") {}
  LoadFailed(const std::string& message) :
      std::runtime_error(std::string("LoadFailed: ") + message) {}
};

struct ProgramOverrun : std::runtime_error {
  ProgramOverrun() : std::runtime_error("ProgramOverrun") {}
};

struct OperandSizeMismatch : std::runtime_error {
  OperandSizeMismatch() : std::runtime_error("OperandSizeMismatch") {}
  OperandSizeMismatch(const std::string& message) :
      std::runtime_error(std::string("OperandSizeMismatch: ") + message) {}
};

struct UnknownInstruction : std::runtime_error {
  UnknownInstruction() : std::runtime_error("UnknownInstruction") {}
  UnknownInstruction(const std::string& message) :
      std::runtime_error(std::string("UnknownInstruction: ") + message) {}
};

struct FileNotFound : std::runtime_error {
  FileNotFound(const std::string& path) : std::runtime_error("FileNotFound: " + path) {}
};

struct Frame {
  Frame();
  Frame(integer n, FrameRef parent);

  std::vector<ValueRef> values;
  FrameRef parent;
};

struct Value {
  enum Tag {
    Integer,
    Cons,
    Closure,
  };

  explicit Value(Tag tag);

  IntegerValue* AsInteger();
  ConsValue* AsCons();
  ClosureValue* AsClosure();

  const IntegerValue* AsInteger() const;
  const ConsValue* AsCons() const;
  const ClosureValue* AsClosure() const;

  Tag tag;
};

struct IntegerValue : Value {
  explicit IntegerValue(integer n);
  integer value;
};

struct ConsValue : Value {
  ConsValue(ValueRef car, ValueRef cdr);
  ValueRef car;
  ValueRef cdr;
};

struct ClosureValue : Value {
  ClosureValue(integer function, FrameRef frame);
  integer function;
  FrameRef frame;
};

ValueRef WrapToValue(integer value);
ValueRef WrapToValue(ValueRef value);

ValueRef MakeList();

template <typename Left, typename Right>
ValueRef MakePair(Left&& left, Right&& right) {
  return std::make_shared<ConsValue>(
      WrapToValue(std::forward<Left>(left)),
      WrapToValue(std::forward<Right>(right)));
}

template <typename Left, typename Right>
ValueRef MakeTuple(Left&& left, Right&& right) {
  return MakePair(std::forward<Left>(left), std::forward<Right>(right));
}

template <typename Head, typename... Tail>
ValueRef MakeTuple(Head&& head, Tail&&... tail) {
  return MakePair(std::forward<Head>(head),
                  MakeTuple(std::forward<Tail>(tail)...));
}

template <typename Head, typename... Tail>
ValueRef MakeList(Head&& head, Tail&&... tail) {
  return MakePair(std::forward<Head>(head),
                  MakeList(std::forward<Tail>(tail)...));
}

template <typename Container>
ValueRef MakeListFromContainer(Container&& values) {
  ValueRef result = MakeList();
  for (auto&& value : values)
    result = std::make_shared<ConsValue>(
        std::forward(value), std::move(result));
  return result;
}

struct Control {
  enum Tag {
    Join,
    Ret,
  };

  explicit Control(Tag tag);

  JoinControl* AsJoinControl();
  RetControl* AsRetControl();
  const JoinControl* AsJoinControl() const;
  const RetControl* AsRetControl() const;

  Tag tag;
};

struct JoinControl : Control {
  explicit JoinControl(integer point);
  integer point;
};

struct RetControl : Control {
  RetControl(integer point, FrameRef frame);
  integer point;
  FrameRef frame;
};

struct Instruction {
  enum OpCode {
    LDC, LD,
    ADD, SUB, MUL, DIV,
    CEQ, CGT, CGTE,
    ATOM,
    CONS, CAR, CDR,
    SEL, JOIN,
    LDF, AP, RTN,
    DUM, RAP,
    STOP,
    TSEL,
    TAP,
    TRAP,
    ST,
    DBUG, BRK,
  };

  explicit Instruction(OpCode op_code, integer lhs = 0, integer rhs = 0);
  OpCode op_code;
  integer lhs;
  integer rhs;
};

struct VM {
  VM();
  ~VM();

  void Apply(const Instruction& instruction);
  void Step(const Program& program);

  void Dump(std::ostream& out) const;
  void DumpDataStack(TraverseContext* context, std::ostream& out) const;
  void DumpControlStack(TraverseContext* context, std::ostream& out) const;
  void DumpEnvironment(TraverseContext* context, std::ostream& out) const;
  void DumpFrame(TraverseContext* context, std::ostream& out,
                 const Frame& frame) const;
  void DumpValue(TraverseContext* context, std::ostream& out,
                 const Value& value) const;
  void DumpControl(TraverseContext* context, std::ostream& out,
                   const Control& value) const;

  void LDC(integer n);
  void LD(integer n, integer i);
  void ADD();
  void SUB();
  void MUL();
  void DIV();
  void CEQ();
  void CGT();
  void CGTE();
  void ATOM();
  void CONS();
  void CAR();
  void CDR();
  void SEL(integer t, integer f);
  void JOIN();
  void LDF(integer f);
  void AP(integer n);
  void RTN();
  void DUM(integer n);
  void RAP(integer n);
  void STOP();
  void TSEL(integer t, integer f);
  void TAP(integer n);
  void TRAP(integer n);
  void ST(integer n, integer i);
  void DBUG();
  void BRK();

  FrameRef MakeFrame(integer n, FrameRef parent);

  void PushToDataStack(ValueRef value);
  ValueRef PopFromDataStack();

  void PushToControlStack(ControlRef control);
  ControlRef PopFromControlStack();
  void FillEnvironmentByDataStack(integer n);

  // TODO(tzik): Consolidate the traversal part to Dump().
  void GC(const std::vector<ValueRef>* extra_root_set = nullptr);
  void MarkDataStack(TraverseContext* context) const;
  void MarkControlStack(TraverseContext* context) const;
  void MarkEnvironment(TraverseContext* context) const;
  void MarkFrame(TraverseContext* context, const Frame& frame) const;
  void MarkValue(TraverseContext* context, const Value& value) const;
  void MarkControl(TraverseContext* context, const Control& control) const;
  void Sweep(const TraverseContext& context);

  bool is_running = true;
  std::vector<ValueRef> data_stack;
  std::vector<ControlRef> control_stack;
  FrameRef environment;
  integer program_counter = 0;

  std::vector<std::weak_ptr<Frame>> frames;

  VM(const VM&) = delete;
  VM& operator=(const VM&) = delete;
};

struct ProgramContext {
  std::unordered_set<std::string> included_files;
  std::unordered_map<std::string, integer> labels;
  std::vector<std::string> instruction_lines;
};

Program LoadProgram(std::istream& in);
void LoadInstructionLines(std::istream& in, ProgramContext* context);
void ResolveInstructions(const ProgramContext& context, Program* program);
void SaveProgram(const Program& program, std::ostream& out);

}  // namespace gcc

