#pragma once

#include <sstream>
#include <vector>

namespace ghc {

struct Instruction;

typedef std::vector<Instruction> Program;
#ifdef _MSC_VER
typedef unsigned char uint8_t;
#endif
typedef uint8_t Word;

struct Argument;
struct Instruction;

// GHC interpreter.
struct GHC {
  GHC();
  ~GHC();

  static Program LoadProgram(std::istream& in);
  void Step(const Program& program);

  void MOV(const Argument& dst, const Argument& src);
  void INC(const Argument& dst);
  void DEC(const Argument& dst);
  void ADD(const Argument& dst, const Argument& src);
  void SUB(const Argument& dst, const Argument& src);
  void MUL(const Argument& dst, const Argument& src);
  void DIV(const Argument& dst, const Argument& src);
  void AND(const Argument& dst, const Argument& src);
  void OR(const Argument& dst, const Argument& src);
  void XOR(const Argument& dst, const Argument& src);
  void JLT(const Argument& targ, const Argument& x, const Argument& y);
  void JEQ(const Argument& targ, const Argument& x, const Argument& y);
  void JGT(const Argument& targ, const Argument& x, const Argument& y);
  void INT(const Argument& i);
  void HLT();

  Word GetVal(const Argument& arg, bool follow_ref = true);
  void StoreVal(const Argument& dst, Word val);

  bool is_running = true;
  Word pc = 0;
  Word regist[8];  // A[0]-H[7]
  Word memory[256];
};

}  // namespace ghc
