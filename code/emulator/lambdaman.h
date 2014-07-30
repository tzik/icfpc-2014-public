#pragma once

#include "character.h"

namespace game {
struct Game;
}

// Describe the status of a Lambda man, and a reference to GCC VM.
struct LambdaMan : public Character {
  LambdaMan(game::Game* game = nullptr, const Pos& pos = Pos());
  virtual ~LambdaMan();
  virtual void Move() override;
  virtual int Think() = 0;

  virtual void Initialize(game::Game* game, const Pos& pos);
  bool IsFrightMode() const;
  void AddScore(int add_score);
  void LoseALife();

  void SetEating();

  int score = 0;
  int num_lives = 3;

  bool eating = false;
};
