#pragma once

#include "lambdaman.h"

// A lambdaman implementation for debugging.
struct FakeLambdaMan : public LambdaMan {
  FakeLambdaMan(game::Game* game, const Pos& pos);
  virtual ~FakeLambdaMan();
  virtual int Think() override;
};
