#include "character.h"
#include "pos.h"

Character::Character(game::Game* game,
                     const Pos& pos,
                     int first_movable_ticks) {
  InitializeCharacter(game, pos, first_movable_ticks);
}

Character::~Character() {}

void Character::InitializeCharacter(game::Game* game,
                                    const Pos& pos,
                                    int first_movable_ticks) {
  this->game = game;
  this->next_movable_ticks = first_movable_ticks;
  this->initial_pos = pos;
  this->pos = pos;
  this->direction = DIRECTION_DOWN;
}

const Pos& Character::GetPos() const {
  return pos;
}

bool Character::IsMyTurn(int ticks) const {
  return ticks == next_movable_ticks;
}

int Character::GetNextMovableTicks() const {
  return next_movable_ticks;
}

void Character::ResetPosition() {
  pos = initial_pos;
  direction = DIRECTION_DOWN;
}
