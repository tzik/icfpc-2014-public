#pragma once

#include <memory>

#include "pos.h"

namespace game {
struct Game;
}

// Base class for Lambda-man and Ghost.
struct Character {
  Character(game::Game* game,
            const Pos& pos,
            int first_movable_ticks);
  virtual ~Character();
  void InitializeCharacter(game::Game* game,
                           const Pos& pos,
                           int first_movable_ticks);

  virtual void Move() = 0;
  const Pos& GetPos() const;

  bool IsMyTurn(int ticks) const;
  int GetNextMovableTicks() const;
  void ResetPosition();

  game::Game* game;
  int next_movable_ticks;
  Pos initial_pos;
  Pos pos;
  int direction;
};
