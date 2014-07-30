#pragma once

#include "character.h"

namespace game {
struct Game;
}

// Describe the status of a ghost, and a reference to GHC VM.
struct Ghost : Character {
  Ghost(game::Game* game,
        const Pos& pos,
        int index);
  virtual ~Ghost();
  void Move();
  virtual int Think() const = 0;
  // TODO(peria): Add Initialize to be injectable.
  int GetIndex();
  void SetInvisibleMode(bool invisible_mode);
  bool IsInvisibleMode() const;
  void ReverseDirection();

  int index;
  int ticks_per_move;
  int ticks_per_move_fright;
  bool invisible_mode;
};
