#include <climits>
#include <vector>

#include "game.h"
#include "lambdaman.h"
#include "util.h"

using namespace std;

namespace {
const int kTicksPerMove = 127;
const int kTicksPerMoveEating = 137;
}  // namespace

LambdaMan::LambdaMan(game::Game* game, const Pos& pos)
  : Character(game, pos, 1),
    score(0), num_lives(3), eating(false) {}

LambdaMan::~LambdaMan() {}

void LambdaMan::Move() {
  int direction = Think();
  pos.Advance(direction);

  next_movable_ticks += eating ? kTicksPerMoveEating : kTicksPerMove;
  eating = false;
}

void LambdaMan::Initialize(game::Game* game, const Pos& pos) {
  InitializeCharacter(game, pos, 1);
}

bool LambdaMan::IsFrightMode() const {
  return game->IsFrightMode();
}

void LambdaMan::AddScore(int add_score) {
  score += add_score;
}

void LambdaMan::LoseALife() {
  --num_lives;
}

void LambdaMan::SetEating() {
  eating = true;
}
