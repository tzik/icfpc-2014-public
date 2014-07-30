#include <cassert>

#include "game.h"
#include "ghost.h"
#include "map.h"

namespace {
const int kTicksPerMove[] = {130, 132, 134, 136};
const int kTicksPerMoveFright[] = {195, 198, 201, 204};
}  // namespace

// Tick starts from 1.
Ghost::Ghost(game::Game* game,
             const Pos& pos,
             int index)
  : Character(game, pos, kTicksPerMove[index % 4]),
    index(index),
    ticks_per_move(kTicksPerMove[index % 4]),
    ticks_per_move_fright(kTicksPerMoveFright[index % 4]),
    invisible_mode(false) {}

Ghost::~Ghost() {}

void Ghost::Move() {
  direction = Think();
  pos.Advance(direction);
  assert(!game->GetMap().IsWall(pos));

  next_movable_ticks +=
    game->IsFrightMode() ? ticks_per_move_fright : ticks_per_move;
}

int Ghost::GetIndex() {
  return index;
}

void Ghost::SetInvisibleMode(bool invisible_mode) {
  this->invisible_mode = invisible_mode;
}

bool Ghost::IsInvisibleMode() const {
  return invisible_mode;
}

void Ghost::ReverseDirection() {
  direction = BACKWARD_DIRECTION(direction);
}
