#include "gcc_vm.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace gcc {

namespace {

template <typename F>
void BinaryOperation(VM* vm, F&& f) {
  ValueRef y = vm->PopFromDataStack();
  ValueRef x = vm->PopFromDataStack();
  vm->PushToDataStack(std::make_shared<IntegerValue>(
      f(x->AsInteger()->value, y->AsInteger()->value)));
  ++vm->program_counter;
}

void ApplyInstruction(VM* vm, void (VM::* method)(),
                      const Instruction&) {
  (vm->*method)();
}

void ApplyInstruction(VM* vm, void (VM::* method)(integer),
                      const Instruction& instruction) {
  (vm->*method)(instruction.lhs);
}

void ApplyInstruction(VM* vm, void (VM::* method)(integer, integer),
                      const Instruction& instruction) {
  (vm->*method)(instruction.lhs, instruction.rhs);
}

size_t OperandSize(void (VM::*)()) { return 0; }
size_t OperandSize(void (VM::*)(integer)) { return 1; }
size_t OperandSize(void (VM::*)(integer, integer)) { return 2; }

OperandSizeMismatch OperandSizeMismatchErrorGenerator(const std::string& line, size_t size) {
  std::stringstream ss;
  ss << "(expected: " << size << "): " << line;
  return OperandSizeMismatch(ss.str());
}

}  // namespace

Frame::Frame() {}
Frame::Frame(integer n, FrameRef parent)
    : values(n),
      parent(std::move(parent)) {}

Value::Value(Tag tag) : tag(tag) {}

IntegerValue* Value::AsInteger() {
  if (tag != Integer)
    throw ValueTypeMismatch();
  return static_cast<IntegerValue*>(this);
}

const IntegerValue* Value::AsInteger() const {
  if (tag != Integer)
    throw ValueTypeMismatch();
  return static_cast<const IntegerValue*>(this);
}

ConsValue* Value::AsCons() {
  if (tag != Cons)
    throw ValueTypeMismatch();
  return static_cast<ConsValue*>(this);
}

const ConsValue* Value::AsCons() const {
  if (tag != Cons)
    throw ValueTypeMismatch();
  return static_cast<const ConsValue*>(this);
}

ClosureValue* Value::AsClosure() {
  if (tag != Closure)
    throw ValueTypeMismatch();
  return static_cast<ClosureValue*>(this);
}

const ClosureValue* Value::AsClosure() const {
  if (tag != Closure)
    throw ValueTypeMismatch();
  return static_cast<const ClosureValue*>(this);
}

IntegerValue::IntegerValue(integer n)
    : Value(Value::Integer),
      value(n) {}

ConsValue::ConsValue(ValueRef car, ValueRef cdr)
    : Value(Value::Cons),
      car(std::move(car)), cdr(std::move(cdr)) {}

ValueRef WrapToValue(integer value) {
  return std::make_shared<IntegerValue>(value);
}

ValueRef WrapToValue(ValueRef value) {
  return value;
}

ValueRef MakeList() {
  return std::make_shared<IntegerValue>(0);
}

ClosureValue::ClosureValue(integer function, FrameRef frame)
    : Value(Value::Closure),
      function(function),
      frame(std::move(frame)) {}

Control::Control(Tag tag) : tag(tag) {}

JoinControl* Control::AsJoinControl() {
  if (tag != Join)
    throw ControlTypeMismatch();
  return static_cast<JoinControl*>(this);
}

const JoinControl* Control::AsJoinControl() const {
  if (tag != Join)
    throw ControlTypeMismatch();
  return static_cast<const JoinControl*>(this);
}

RetControl* Control::AsRetControl() {
  if (tag != Ret)
    throw ControlTypeMismatch();
  return static_cast<RetControl*>(this);
}

const RetControl* Control::AsRetControl() const {
  if (tag != Ret)
    throw ControlTypeMismatch();
  return static_cast<const RetControl*>(this);
}

JoinControl::JoinControl(integer point)
    : Control(Control::Join),
      point(point) {}

RetControl::RetControl(integer point, FrameRef frame)
      : Control(Control::Ret),
        point(point),
        frame(std::move(frame)) {}

Instruction::Instruction(OpCode op_code, integer lhs, integer rhs)
    : op_code(op_code), lhs(lhs), rhs(rhs) {}

VM::VM() {}

VM::~VM() {
  GC();
}

void VM::Apply(const Instruction& instruction) {
#define DISPATCH(op_code)                                \
  case Instruction::op_code:                             \
    ApplyInstruction(this, &VM::op_code, instruction);   \
    return
  switch (instruction.op_code) {
    DISPATCH(LDC);
    DISPATCH(LD);
    DISPATCH(ADD);
    DISPATCH(SUB);
    DISPATCH(MUL);
    DISPATCH(DIV);
    DISPATCH(CEQ);
    DISPATCH(CGT);
    DISPATCH(CGTE);
    DISPATCH(ATOM);
    DISPATCH(CONS);
    DISPATCH(CAR);
    DISPATCH(CDR);
    DISPATCH(SEL);
    DISPATCH(JOIN);
    DISPATCH(LDF);
    DISPATCH(AP);
    DISPATCH(RTN);
    DISPATCH(DUM);
    DISPATCH(RAP);
    DISPATCH(STOP);
    DISPATCH(TSEL);
    DISPATCH(TAP);
    DISPATCH(TRAP);
    DISPATCH(ST);
    DISPATCH(DBUG);
    DISPATCH(BRK);
  }
#undef DISPATCH
  assert(false);
}

void VM::Step(const Program& program) {
  if (program_counter >= static_cast<integer>(program.size()))
    throw ProgramOverrun();
  Apply(program[program_counter]);
}

void VM::Dump(std::ostream& out) const {
  TraverseContext context;
  DumpDataStack(&context, out);
  DumpControlStack(&context, out);
  DumpEnvironment(&context, out);
}

void VM::DumpDataStack(TraverseContext* context,
                       std::ostream& out) const {
  out << "=== Data Stack Dump\n";
  for (auto&& cell : data_stack)
    DumpValue(context, out, *cell);
}

void VM::DumpControlStack(TraverseContext* context, std::ostream& out) const {
  out << "=== Control Stack Dump\n";
  for (auto&& cell : control_stack)
    DumpControl(context, out, *cell);
}

void VM::DumpEnvironment(TraverseContext* context, std::ostream& out) const {
  DumpFrame(context, out, *environment);
}

void VM::DumpFrame(TraverseContext* context, std::ostream& out,
                   const Frame& frame) const {
  out << "=== Frame Dump (0x"
      << std::hex << reinterpret_cast<uintptr_t>(&frame)
      <<  ")\n";
  const Frame* target_frame = &frame;
  while (target_frame) {
    if (!context->visited_frame.insert(target_frame).second) {
      out << "(already visited)\n";
      return;
    }

    for (auto&& cell : target_frame->values) {
      if (cell)
        DumpValue(context, out, *cell);
      else
        out << "null value\n";
    }
    out << "==== Parent Frame (0x"
        << std::hex << reinterpret_cast<uintptr_t>(target_frame)
        << ")\n";
    target_frame = target_frame->parent.get();
  }
}

void VM::DumpValue(TraverseContext* context, std::ostream& out,
                   const Value& value) const {
  out << "Value(0x"
      << std::hex << reinterpret_cast<uintptr_t>(&value)
      << ") ";
  if (!context->visited_value.insert(&value).second) {
    out << "(already visited)";
    return;
  }

  switch (value.tag) {
    case Value::Integer:
      out << "Integer: " << std::dec << value.AsInteger()->value << "\n";
      break;
    case Value::Cons: {
      const ConsValue* cons = value.AsCons();
      out << "Cons Cell: \n"
          << "  CAR: ";
      DumpValue(context, out, *cons->car);
      out << "  CDR: ";
      DumpValue(context, out, *cons->cdr);
      break;
    }
    case Value::Closure: {
      const ClosureValue* closure = value.AsClosure();
      out << "Cons Cell:\n"
          << "  FUNCTION: " << std::dec << closure->function << "\n";
      out << "  FRAME:\n";
      DumpFrame(context, out, *closure->frame);
      break;
    }
  }
}

void VM::DumpControl(TraverseContext* context, std::ostream& out,
                     const Control& control) const {
  switch (control.tag) {
    case Control::Join:
      out << "Join Control: " << control.AsJoinControl()->point << "\n";
      break;
    case Control::Ret:
      out << "Ret Control: " << control.AsRetControl()->point << "\n";
      DumpFrame(context, out, *control.AsRetControl()->frame);
      break;
  }
}

void VM::LDC(integer n) {
  PushToDataStack(std::make_shared<IntegerValue>(n));
  ++program_counter;
}

void VM::LD(integer n, integer i) {
  Frame* target_frame = environment.get();
  if (!target_frame)
    throw InvalidFrame(program_counter);

  for (integer k = 0; k < n; ++k) {
    target_frame = target_frame->parent.get();
    if (!target_frame)
      throw InvalidFrame(program_counter);
  }

  if (i < 0 || i >= static_cast<integer>(target_frame->values.size()))
    throw FrameOutOfRange(program_counter);

  PushToDataStack(target_frame->values[i]);
  ++program_counter;
}

void VM::ADD() {
  BinaryOperation(this, [](integer x, integer y) {return x + y;});
}

void VM::SUB() {
  BinaryOperation(this, [](integer x, integer y) {return x - y;});
}

void VM::MUL() {
  BinaryOperation(this, [](integer x, integer y) {return x * y;});
}

void VM::DIV() {
  BinaryOperation(this, [](integer x, integer y) {
    if (!y)
      throw DivideByZero();
    return x / y;
  });
}

void VM::CEQ() {
  BinaryOperation(this, [](integer x, integer y) {return x == y ? 1 : 0;});
}

void VM::CGT() {
  BinaryOperation(this, [](integer x, integer y) {return x > y ? 1 : 0;});
}

void VM::CGTE() {
  BinaryOperation(this, [](integer x, integer y) {return x >= y ? 1 : 0;});
}

void VM::ATOM() {
  PushToDataStack(std::make_shared<IntegerValue>(
      PopFromDataStack()->tag == Value::Integer ? 1 : 0));
  ++program_counter;
}

void VM::CONS() {
  // Make local variable to ensure pop order.
  ValueRef y = PopFromDataStack();
  ValueRef x = PopFromDataStack();
  PushToDataStack(std::make_shared<ConsValue>(
      std::move(x), std::move(y)));
  ++program_counter;
}

void VM::CAR() {
  PushToDataStack(PopFromDataStack()->AsCons()->car);
  ++program_counter;
}

void VM::CDR() {
  PushToDataStack(PopFromDataStack()->AsCons()->cdr);
  ++program_counter;
}

void VM::SEL(integer t, integer f) {
  PushToControlStack(std::make_shared<JoinControl>(program_counter + 1));
  if (PopFromDataStack()->AsInteger()->value)
    program_counter = t;
  else
    program_counter = f;
}

void VM::JOIN() {
  program_counter = PopFromControlStack()->AsJoinControl()->point;
}

void VM::LDF(integer f) {
  PushToDataStack(std::make_shared<ClosureValue>(
      f, environment));
  ++program_counter;
}

void VM::AP(integer n) {
  ValueRef x = PopFromDataStack();
  ClosureValue* closure = x->AsClosure();
  PushToControlStack(std::make_shared<RetControl>(
      program_counter + 1, std::move(environment)));
  environment = MakeFrame(n, closure->frame);
  FillEnvironmentByDataStack(n);
  program_counter = closure->function;
}

void VM::RTN() {
  ControlRef x = PopFromControlStack();
  if (!x || !x->AsRetControl()->frame) {
    is_running = false;
    return;
  }

  RetControl* ret = x->AsRetControl();
  program_counter = ret->point;
  environment = ret->frame;
}

void VM::DUM(integer n) {
  // TODO(tzik): Make DUMFrame
  environment = MakeFrame(n, environment);
  ++program_counter;
}

void VM::RAP(integer n) {
  ValueRef x = PopFromDataStack();

  // TODO(tzik): DUM check

  ClosureValue* closure = x->AsClosure();
  PushToControlStack(std::make_shared<RetControl>(
      program_counter + 1, std::move(environment)));
  environment = closure->frame;
  FillEnvironmentByDataStack(n);
  program_counter = closure->function;
}

void VM::STOP() {
  is_running = false;
}

void VM::TSEL(integer t, integer f) {
  if (PopFromDataStack()->AsInteger()->value)
    program_counter = t;
  else
    program_counter = f;
}

void VM::TAP(integer n) {
  ValueRef x = PopFromDataStack();
  ClosureValue* closure = x->AsClosure();
  environment = MakeFrame(n, closure->frame);
  FillEnvironmentByDataStack(n);
  program_counter = closure->function;
}

void VM::TRAP(integer n) {
  ValueRef x = PopFromDataStack();
  ClosureValue* closure = x->AsClosure();
  // TODO(tzik): Check for DUM frame.
  environment = closure->frame;
  FillEnvironmentByDataStack(n);
  program_counter = closure->function;
}

void VM::ST(integer n, integer i) {
  Frame* target_frame = environment.get();
  if (!target_frame)
    throw InvalidFrame(program_counter);

  for (integer k = 0; k < n; ++k) {
    target_frame = target_frame->parent.get();
    if (!target_frame)
      throw InvalidFrame(program_counter);
  }
  if (i < 0 || i >= static_cast<integer>(target_frame->values.size()))
    throw FrameOutOfRange(program_counter);

  target_frame->values[i] = PopFromDataStack();
  ++program_counter;
}

void VM::DBUG() {
  ValueRef x = PopFromDataStack();
  // TODO(tzik): Dump |x|.
  ++program_counter;
}

void VM::BRK() {
  ++program_counter;
}

FrameRef VM::MakeFrame(integer n, FrameRef parent) {
  FrameRef new_frame = std::make_shared<Frame>(n, parent);
  frames.push_back(new_frame);
  return new_frame;
}

void VM::PushToDataStack(ValueRef value) {
  data_stack.push_back(std::move(value));
}

ValueRef VM::PopFromDataStack() {
  ValueRef value = std::move(data_stack.back());
  data_stack.pop_back();
  return value;
}

void VM::PushToControlStack(ControlRef control) {
  control_stack.push_back(std::move(control));
}

ControlRef VM::PopFromControlStack() {
  if (control_stack.empty())
    return nullptr;

  ControlRef control = std::move(control_stack.back());
  control_stack.pop_back();
  return control;
}

void VM::FillEnvironmentByDataStack(integer n) {
  if (static_cast<integer>(environment->values.size()) != n ||
      static_cast<integer>(data_stack.size()) < n)
    throw FrameSizeMismatch(program_counter);
  for (auto iter = environment->values.rbegin();
       iter != environment->values.rend(); ++iter)
    *iter = PopFromDataStack();
}

void VM::GC(const std::vector<ValueRef>* extra_root_set) {
  TraverseContext context;
  if (extra_root_set)
    for(auto&& extra_root : *extra_root_set)
      MarkValue(&context, *extra_root);
  MarkDataStack(&context);
  MarkControlStack(&context);
  MarkEnvironment(&context);
  Sweep(context);
}

void VM::MarkDataStack(TraverseContext* context) const {
  for (auto&& cell : data_stack)
    MarkValue(context, *cell);
}

void VM::MarkControlStack(TraverseContext* context) const {
  for (auto&& cell : control_stack)
    MarkControl(context, *cell);
}
void VM::MarkEnvironment(TraverseContext* context) const {
  MarkFrame(context, *environment);
}

void VM::MarkFrame(TraverseContext* context, const Frame& frame) const {
  const Frame* target_frame = &frame;
  while (target_frame) {
    if (!context->visited_frame.insert(target_frame).second)
      return;

    for (auto&& cell : target_frame->values)
      MarkValue(context, *cell);
    target_frame = target_frame->parent.get();
  }
}

void VM::MarkValue(TraverseContext* context, const Value& value) const {
  if (!context->visited_value.insert(&value).second)
    return;

  switch (value.tag) {
    case Value::Integer: break;
    case Value::Cons: {
      const ConsValue* cons = value.AsCons();
      MarkValue(context, *cons->car);
      MarkValue(context, *cons->cdr);
      break;
    }
    case Value::Closure: {
      const ClosureValue* closure = value.AsClosure();
      MarkFrame(context, *closure->frame);
      break;
    }
  }
}

void VM::MarkControl(TraverseContext* context, const Control& control) const {
  switch (control.tag) {
    case Control::Join: break;
    case Control::Ret:
      MarkFrame(context, *control.AsRetControl()->frame);
      break;
  }
}

void VM::Sweep(const TraverseContext& context) {
  std::vector<std::weak_ptr<Frame>> new_frames;
  for (auto&& frame : frames) {
    if (frame.expired())
      continue;

    auto frame_ptr = frame.lock();
    if (context.visited_frame.count(frame_ptr.get())) {
      new_frames.push_back(std::move(frame));
      continue;
    }

    frame_ptr->parent = nullptr;
    frame_ptr->values.clear();
  }
  frames = std::move(new_frames);
}

Program LoadProgram(std::istream& in) {
  ProgramContext context;
  LoadInstructionLines(in, &context);

  Program program;
  ResolveInstructions(context, &program);
  return program;
}

void LoadInstructionLines(std::istream& in,
                          ProgramContext* context) {
  std::string line;
  while(std::getline(in, line)) {
    if (line.empty())
      continue;

    // Removes comment.
    size_t comment_starting_point = line.find(';');
    if (comment_starting_point != std::string::npos)
      line.resize(comment_starting_point);

    std::string first_segment;
    std::istringstream iss(line);
    iss >> first_segment;
    if (first_segment.size() == 0)
      continue;

    if (first_segment == "#INCLUDE") {
      std::string path = line.substr(first_segment.size() + 1);
      if (!context->included_files.insert(path).second)
        continue;

      std::ifstream inc(path);
      if (!inc.is_open())
        throw FileNotFound(path);
      LoadInstructionLines(inc, context);
      continue;
    }

    if (first_segment.back() == ':') {
      first_segment.resize(first_segment.size() - 1);
      context->labels[first_segment] =
          context->instruction_lines.size();
      continue;
    }

    if (!line.empty())
      context->instruction_lines.push_back(line);
  }
}

void ResolveInstructions(const ProgramContext& context, Program* program) {
  std::string line;

  std::unordered_map<std::string, std::pair<Instruction::OpCode, size_t>> parse_op;
#define PARSE_OP(op_code)                                               \
  parse_op[#op_code] = std::make_pair(Instruction::op_code,             \
                                      OperandSize(&VM::op_code))
  PARSE_OP(LDC);
  PARSE_OP(LD);
  PARSE_OP(ADD);
  PARSE_OP(SUB);
  PARSE_OP(MUL);
  PARSE_OP(DIV);
  PARSE_OP(CEQ);
  PARSE_OP(CGT);
  PARSE_OP(CGTE);
  PARSE_OP(ATOM);
  PARSE_OP(CONS);
  PARSE_OP(CAR);
  PARSE_OP(CDR);
  PARSE_OP(SEL);
  PARSE_OP(JOIN);
  PARSE_OP(LDF);
  PARSE_OP(AP);
  PARSE_OP(RTN);
  PARSE_OP(DUM);
  PARSE_OP(RAP);
  PARSE_OP(STOP);
  PARSE_OP(TSEL);
  PARSE_OP(TAP);
  PARSE_OP(TRAP);
  PARSE_OP(ST);
  PARSE_OP(DBUG);
  PARSE_OP(BRK);
#undef PARSE_OP

  auto parse_operand = [&](std::istream& iss, integer* operand) {
    std::string x;
    if (!(iss >> x))
      return false;
    auto iter = context.labels.find(x);
    if (iter != context.labels.end()) {
      *operand = iter->second;
      return true;
    }

    std::istringstream iss2(x);
    int y = 0;
    if (iss2 >> y) {
      *operand = y;
      return true;
    }
    throw LoadFailed(x);
  };

  for (auto&& instruction_line : context.instruction_lines) {
    std::string op_code;
    std::istringstream iss(instruction_line);
    iss >> op_code;
    auto iter = parse_op.find(op_code);
    if (iter == parse_op.end())
      throw UnknownInstruction(op_code);
    Instruction instruction(iter->second.first);
    size_t expected_size = parse_op[op_code].second;
    size_t actual_size = 0;
    if (parse_operand(iss, &instruction.lhs))
      ++actual_size;
    if (parse_operand(iss, &instruction.rhs))
      ++actual_size;
    if (expected_size != actual_size)
      throw OperandSizeMismatchErrorGenerator(instruction_line, expected_size);
    program->push_back(instruction);
  }
}

void SaveProgram(const Program& program, std::ostream& out) {
#define PRINT_OP(op_code)                \
  case Instruction::op_code:             \
    out << #op_code;                     \
    if (OperandSize(&VM::op_code) >= 1)  \
      out << ' ' << instruction.lhs;     \
    if (OperandSize(&VM::op_code) >= 2)  \
      out << ' ' << instruction.rhs;     \
    out << '\n';                         \
    break

  for (auto&& instruction : program) {
    switch(instruction.op_code) {
      PRINT_OP(LDC);
      PRINT_OP(LD);
      PRINT_OP(ADD);
      PRINT_OP(SUB);
      PRINT_OP(MUL);
      PRINT_OP(DIV);
      PRINT_OP(CEQ);
      PRINT_OP(CGT);
      PRINT_OP(CGTE);
      PRINT_OP(ATOM);
      PRINT_OP(CONS);
      PRINT_OP(CAR);
      PRINT_OP(CDR);
      PRINT_OP(SEL);
      PRINT_OP(JOIN);
      PRINT_OP(LDF);
      PRINT_OP(AP);
      PRINT_OP(RTN);
      PRINT_OP(DUM);
      PRINT_OP(RAP);
      PRINT_OP(STOP);
      PRINT_OP(TSEL);
      PRINT_OP(TAP);
      PRINT_OP(TRAP);
      PRINT_OP(ST);
      PRINT_OP(DBUG);
      PRINT_OP(BRK);
    }
  }
#undef PRINT_OP
}

}  // namespace gcc
