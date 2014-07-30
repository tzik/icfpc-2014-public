#include <cassert>
#include <vector>

#include "fake_ghost.h"
#include "game.h"
#include "util.h"
#include "lambdaman.h"

using namespace std;

static const int FRIGHT_MODE_DISTANCE_THRESHOLD = 5;

FakeGhost::FakeGhost(game::Game* game,
                     const Pos& pos,
                     int index)
  : Ghost(game, pos, index) {}

FakeGhost::~FakeGhost() {}

int FakeGhost::Think() const {
  //vector<int> valid_directions;
  //for (int direction = 0; direction < 4; ++direction) {
  //  if (direction == BACKWARD_DIRECTION(this->direction)) {
  //    continue;
  //  }
  //  Pos next_pos = pos;
  //  next_pos.Advance(direction);
  //  if (game->GetMap().IsWall(next_pos)) {
  //    continue;
  //  }
  //  valid_directions.push_back(direction);
  //}

  //if (valid_directions.empty()) {
  //  return BACKWARD_DIRECTION(this->direction);
  //}

  //return valid_directions[util::xor128() % valid_directions.size()];

  const Pos& lambdaMan = game->lambda_man->GetPos();
  Pos up = pos;
  up.Advance(DIRECTION_UP);
  Pos right = pos;
  right.Advance(DIRECTION_RIGHT);
  Pos down = pos;
  down.Advance(DIRECTION_DOWN);
  Pos left = pos;
  left.Advance(DIRECTION_LEFT);
  const Map& map = game->GetMap();
  
  switch (index) {
  case 0:
    // left right
    if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
      lambdaMan.x > pos.x) {
      return DIRECTION_LEFT;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
      pos.x > lambdaMan.x) {
      return DIRECTION_RIGHT;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
      lambdaMan.y > pos.y) {
      return DIRECTION_UP;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
      pos.y > lambdaMan.y) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
      lambdaMan.x < pos.x) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
      pos.x < lambdaMan.x) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
      lambdaMan.y < pos.y) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
      pos.y < lambdaMan.y) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(left)) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right)) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(up)) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down)) {
      return DIRECTION_DOWN;
    }

    return DIRECTION_DOWN;

  case 1:
    // up down
    if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
      lambdaMan.y > pos.y) {
      return DIRECTION_UP;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
      pos.y > lambdaMan.y) {
      return DIRECTION_DOWN;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
      lambdaMan.x > pos.x) {
      return DIRECTION_LEFT;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
      pos.x > lambdaMan.x) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
      lambdaMan.y < pos.y) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
      pos.y < lambdaMan.y) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
      lambdaMan.x < pos.x) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
      pos.x < lambdaMan.x) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(up)) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down)) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(left)) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right)) {
      return DIRECTION_RIGHT;
    }

    return DIRECTION_DOWN;

  case 2:
    // left up down right
    if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
      lambdaMan.x > pos.x) {
      return DIRECTION_LEFT;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
      pos.x > lambdaMan.x) {
      return DIRECTION_RIGHT;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
      lambdaMan.y > pos.y) {
      return DIRECTION_UP;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
      pos.y > lambdaMan.y) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(left)) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(up)) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(down)) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(right)) {
      return DIRECTION_RIGHT;
    }

    return DIRECTION_DOWN;

  case 3:
    // up left right down
    if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
      lambdaMan.y > pos.y) {
      return DIRECTION_UP;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
      pos.y > lambdaMan.y) {
      return DIRECTION_DOWN;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
      lambdaMan.x > pos.x) {
      return DIRECTION_LEFT;
    }
    else if (game->IsFrightMode() &&
      pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
      !map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
      pos.x > lambdaMan.x) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(up) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(left) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(down) &&
      BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
      return DIRECTION_DOWN;
    }
    else if (!map.IsWall(up)) {
      return DIRECTION_UP;
    }
    else if (!map.IsWall(left)) {
      return DIRECTION_LEFT;
    }
    else if (!map.IsWall(right)) {
      return DIRECTION_RIGHT;
    }
    else if (!map.IsWall(down)) {
      return DIRECTION_DOWN;
    }

    return DIRECTION_DOWN;
  }

  return DIRECTION_DOWN;
}


#define FAKE_GHOST_BODY(name)                   \
  name::name(game::Game* game,                  \
             const Pos& pos,                    \
             int index) :                       \
    Ghost(game, pos, index) {}                  \
  name::~name() {}

FAKE_GHOST_BODY(FakeGhost1)
FAKE_GHOST_BODY(FakeGhost2)
FAKE_GHOST_BODY(FakeGhost3)
FAKE_GHOST_BODY(FakeGhost4)
FAKE_GHOST_BODY(FakeGhost5)
FAKE_GHOST_BODY(FakeGhost6)
FAKE_GHOST_BODY(FakeGhost7)

int FakeGhost1::Think() const {
  vector<int> valid_directions;
  for (int direction = 0; direction < 4; ++direction) {
    if (direction == BACKWARD_DIRECTION(this->direction)) {
      continue;
    }
    Pos next_pos = pos;
    next_pos.Advance(direction);
    if (game->GetMap().IsWall(next_pos)) {
      continue;
    }
    valid_directions.push_back(direction);
  }

  if (valid_directions.empty()) {
    return BACKWARD_DIRECTION(this->direction);
  }

  return valid_directions[util::xor128() % valid_directions.size()];
}

int FakeGhost2::Think() const {
  const Pos& lambdaMan = game->lambda_man->GetPos();
  Pos up = pos;
  up.Advance(DIRECTION_UP);
  Pos right = pos;
  right.Advance(DIRECTION_RIGHT);
  Pos down = pos;
  down.Advance(DIRECTION_DOWN);
  Pos left = pos;
  left.Advance(DIRECTION_LEFT);
  const Map& map = game->GetMap();

  // left right
  if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x > pos.x) {
    return DIRECTION_LEFT;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x > lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y > pos.y) {
    return DIRECTION_UP;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y > lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x < pos.x) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x < lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y < pos.y) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y < lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left)) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right)) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up)) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down)) {
    return DIRECTION_DOWN;
  }

  return DIRECTION_DOWN;
}

int FakeGhost3::Think() const {
  const Pos& lambdaMan = game->lambda_man->GetPos();
  Pos up = pos;
  up.Advance(DIRECTION_UP);
  Pos right = pos;
  right.Advance(DIRECTION_RIGHT);
  Pos down = pos;
  down.Advance(DIRECTION_DOWN);
  Pos left = pos;
  left.Advance(DIRECTION_LEFT);
  const Map& map = game->GetMap();

  // left right0
  if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x < pos.x) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x < lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y < pos.y) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y < lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left)) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right)) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up)) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down)) {
    return DIRECTION_DOWN;
  }

  return DIRECTION_DOWN;
}

int FakeGhost4::Think() const {
  const Pos& lambdaMan = game->lambda_man->GetPos();
  Pos up = pos;
  up.Advance(DIRECTION_UP);
  Pos right = pos;
  right.Advance(DIRECTION_RIGHT);
  Pos down = pos;
  down.Advance(DIRECTION_DOWN);
  Pos left = pos;
  left.Advance(DIRECTION_LEFT);
  const Map& map = game->GetMap();

  // up down
  if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y > pos.y) {
    return DIRECTION_UP;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y > lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x > pos.x) {
    return DIRECTION_LEFT;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x > lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y < pos.y) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y < lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x < pos.x) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x < lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up)) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down)) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left)) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right)) {
    return DIRECTION_RIGHT;
  }

  return DIRECTION_DOWN;
}

int FakeGhost5::Think() const {
  const Pos& lambdaMan = game->lambda_man->GetPos();
  Pos up = pos;
  up.Advance(DIRECTION_UP);
  Pos right = pos;
  right.Advance(DIRECTION_RIGHT);
  Pos down = pos;
  down.Advance(DIRECTION_DOWN);
  Pos left = pos;
  left.Advance(DIRECTION_LEFT);
  const Map& map = game->GetMap();

  // up down
  if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y < pos.y) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y < lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x < pos.x) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x < lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up)) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down)) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left)) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right)) {
    return DIRECTION_RIGHT;
  }

  return DIRECTION_DOWN;
}

int FakeGhost6::Think() const {
  const Pos& lambdaMan = game->lambda_man->GetPos();
  Pos up = pos;
  up.Advance(DIRECTION_UP);
  Pos right = pos;
  right.Advance(DIRECTION_RIGHT);
  Pos down = pos;
  down.Advance(DIRECTION_DOWN);
  Pos left = pos;
  left.Advance(DIRECTION_LEFT);
  const Map& map = game->GetMap();

  // left up down right
  if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x > pos.x) {
    return DIRECTION_LEFT;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x > lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y > pos.y) {
    return DIRECTION_UP;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y > lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(left)) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(up)) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(down)) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(right)) {
    return DIRECTION_RIGHT;
  }

  return DIRECTION_DOWN;
}

int FakeGhost7::Think() const {
  const Pos& lambdaMan = game->lambda_man->GetPos();
  Pos up = pos;
  up.Advance(DIRECTION_UP);
  Pos right = pos;
  right.Advance(DIRECTION_RIGHT);
  Pos down = pos;
  down.Advance(DIRECTION_DOWN);
  Pos left = pos;
  left.Advance(DIRECTION_LEFT);
  const Map& map = game->GetMap();

  // up left right down
  if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP &&
    lambdaMan.y > pos.y) {
    return DIRECTION_UP;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN &&
    pos.y > lambdaMan.y) {
    return DIRECTION_DOWN;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT &&
    lambdaMan.x > pos.x) {
    return DIRECTION_LEFT;
  }
  else if (game->IsFrightMode() &&
    pos.ManhattanDistance(lambdaMan) < FRIGHT_MODE_DISTANCE_THRESHOLD &&
    !map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT &&
    pos.x > lambdaMan.x) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(up) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_UP) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(left) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_LEFT) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_RIGHT) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(down) &&
    BACKWARD_DIRECTION(direction) != DIRECTION_DOWN) {
    return DIRECTION_DOWN;
  }
  else if (!map.IsWall(up)) {
    return DIRECTION_UP;
  }
  else if (!map.IsWall(left)) {
    return DIRECTION_LEFT;
  }
  else if (!map.IsWall(right)) {
    return DIRECTION_RIGHT;
  }
  else if (!map.IsWall(down)) {
    return DIRECTION_DOWN;
  }

  return DIRECTION_DOWN;
}
