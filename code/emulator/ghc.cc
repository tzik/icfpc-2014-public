#include "ghc.h"

#include <string>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cassert>

namespace ghc {

namespace {
typedef int Register;
}

const Register kRegisterA = 0;
const Register kRegisterB = 1;
const Register kRegisterC = 2;
const Register kRegisterD = 3;
const Register kRegisterE = 4;
const Register kRegisterF = 5;
const Register kRegisterG = 6;
const Register kRegisterH = 7;
const Register kPC = 8;
const Register kNonRegister = -1; 

struct Argument {
  Argument() {};
    
  Argument(std::string str) {
    bool ref1 = str.front() == '[';
    bool ref2 = str.back() == ']';
    assert(ref1 == ref2);

    ref = ref1;
    if (ref) {
      str = str.substr(1, str.size() - 2);
    }

    if (str == "PC") reg = kPC;
    else if (str =="A") reg = kRegisterA;
    else if (str =="B") reg = kRegisterB;
    else if (str =="C") reg = kRegisterC;
    else if (str =="D") reg = kRegisterD;
    else if (str =="E") reg = kRegisterE;
    else if (str =="F") reg = kRegisterF;
    else if (str =="G") reg = kRegisterG;
    else if (str =="H") reg = kRegisterH;
    else if (std::isdigit(str.front()))
      imm = std::strtoll(str.c_str(), NULL, 10);
    else
      assert(false);
  }
  
  bool ref = false;  // refer memory
  uint8_t imm = 0;
  Register reg = kNonRegister;
};

struct Instruction {
  enum OpCode {
    MOV,
    INC, DEC,
    ADD, SUB, MUL, DIV,
    AND, OR , XOR,
    JLT, JEQ, JGT,
    INT,
    HLT,
    INVALID,
  };

  static OpCode ConvertToOpCode(std::string& name) {
#define PARSE_OP(op)                            \
    if (name == #op)                            \
      return op;                                \
    else

    PARSE_OP(MOV);
    PARSE_OP(INC);
    PARSE_OP(DEC);
    PARSE_OP(ADD);
    PARSE_OP(SUB);
    PARSE_OP(MUL);
    PARSE_OP(DIV);
    PARSE_OP(AND);
    PARSE_OP(OR);
    PARSE_OP(XOR);
    PARSE_OP(JLT);
    PARSE_OP(JEQ);
    PARSE_OP(JGT);
    PARSE_OP(INT);
    return INVALID;
#undef PARSE_OP
  }

  OpCode op_code;
  Argument arg[3];
};

GHC::GHC() {
  for (int i = 0; i < 8; ++i)
    regist[i] = 0;
  for (int i = 0; i < 256; ++i)
    memory[i] = 0;
}

GHC::~GHC() {}

void GHC::Step(const Program& program) {
  const Instruction& inst = program[pc];
  switch (inst.op_code) {
  case Instruction::MOV:
    MOV(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::INC:
    INC(inst.arg[0]);
    return;
  case Instruction::DEC:
    DEC(inst.arg[0]);
    return;
  case Instruction::ADD:
    ADD(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::SUB:
    SUB(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::MUL:
    MUL(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::DIV:
    DIV(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::AND:
    AND(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::OR:
    OR(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::XOR:
    XOR(inst.arg[0], inst.arg[1]);
    return;
  case Instruction::JLT:
    JLT(inst.arg[0], inst.arg[1], inst.arg[2]);
    return;
  case Instruction::JEQ:
    JEQ(inst.arg[0], inst.arg[1], inst.arg[2]);
    return;
  case Instruction::JGT:
    JGT(inst.arg[0], inst.arg[1], inst.arg[2]);
    return;
  case Instruction::INT:
    INT(inst.arg[0]);
    return;
  case Instruction::HLT:
    HLT();
    return;
  case Instruction::INVALID:
    return;
  }
  assert(false);
}

void GHC::MOV(const Argument& dst, const Argument& src) {
  StoreVal(dst, GetVal(src));
}

void GHC::INC(const Argument& dst) {
  StoreVal(dst, GetVal(dst) + 1);
}

void GHC::DEC(const Argument& dst) {
  StoreVal(dst, GetVal(dst) - 1);
}

void GHC::ADD(const Argument& dst, const Argument& src) {
  StoreVal(dst, GetVal(dst) + GetVal(src));
}

void GHC::SUB(const Argument& dst, const Argument& src) {
  StoreVal(dst, GetVal(dst) - GetVal(src));
}

void GHC::MUL(const Argument& dst, const Argument& src) {
  StoreVal(dst, GetVal(dst) * GetVal(src));
}

void GHC::DIV(const Argument& dst, const Argument& src) {
  Word s_val = GetVal(src);
  if (s_val == 0) {
    HLT();
    return;
  }
  StoreVal(dst, GetVal(dst) / s_val);
}

void GHC::AND(const Argument& dst, const Argument& src) {
  StoreVal(dst, GetVal(dst) & GetVal(src));
}

void GHC::OR(const Argument& dst, const Argument& src) {
  StoreVal(dst, GetVal(dst) | GetVal(src));
}

void GHC::XOR(const Argument& dst, const Argument& src) {
  StoreVal(dst, GetVal(dst) ^ GetVal(src));
}

void GHC::JLT(const Argument& targ, const Argument& x, const Argument& y) {
  assert(!targ.ref && targ.reg != kNonRegister);
  if (GetVal(x) < GetVal(y))
    pc = targ.imm;
}

void GHC::JEQ(const Argument& targ, const Argument& x, const Argument& y) {
  assert(!targ.ref && targ.reg != kNonRegister);
  if (GetVal(x) == GetVal(y))
    pc = targ.imm;
}

void GHC::JGT(const Argument& targ, const Argument& x, const Argument& y) {
  assert(!targ.ref && targ.reg != kNonRegister);
  if (GetVal(x) > GetVal(y))
    pc = targ.imm;
}

void GHC::INT(const Argument& i) {
}

void GHC::HLT() {
}

Word GHC::GetVal(const Argument& arg, bool follow_ref) {
  if (arg.ref && follow_ref) {
    return memory[GetVal(arg, false)];
  }
  Register reg_id = arg.reg;
  if (reg_id == kPC)
    return pc;
  return (reg_id == kNonRegister) ? arg.imm : regist[reg_id];
}

void GHC::StoreVal(const Argument& dst, Word val) {
  assert(dst.ref || (dst.reg != kNonRegister && dst.reg != kPC));
  if (dst.ref) {
    memory[GetVal(dst, false)] = val;
    return;
  }
  regist[dst.reg] = val;
}

Program GHC::LoadProgram(std::istream& in) {
  Program program;
  for (std::string line; std::getline(in, line);) {
    if (line.empty())
      continue;
    // Remove comments
    size_t semi_colon = line.find(';');
    if (semi_colon != std::string::npos)
      line.resize(semi_colon);
    for (size_t i = 0; i < line.size(); ++i)
      if (line[i] == ',')
        line[i] = ' ';

    std::istringstream iss(line);
    std::string segment;
    iss >> segment;
    
    Instruction inst;
    inst.op_code = Instruction::ConvertToOpCode(segment);
    for (int id = 0; iss >> segment; ++id)  {
      inst.arg[id] = Argument(segment);
    }
  }
    
  return program;
}

}  // namespace ghc
