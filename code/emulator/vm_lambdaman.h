#pragma once

#include "lambdaman.h"
#include "gcc_vm.h"

#include <iosfwd>

namespace game {

struct TimeLimitExceeded {};

struct VMLambdaMan : public LambdaMan {
  VMLambdaMan(std::istream& src);
  virtual ~VMLambdaMan();
  virtual void Initialize(game::Game* game, const Pos& pos) override;
  virtual int Think() override;

  void ResetVM();

  gcc::ValueRef EncodeWorld() const;
  gcc::ValueRef EncodeMap() const;
  gcc::ValueRef EncodeLambdaManState() const;
  gcc::ValueRef EncodeGhostState() const;
  gcc::ValueRef EncodeFruitState() const;

  gcc::ValueRef ai_state;
  gcc::ValueRef step;

  gcc::VM vm;
  gcc::Program program;
};

}  // namespace game
