#include <cassert>
#include <functional>
#include <map>
#include <vector>
#include <limits.h>

#include "fake_lambdaman.h"
#include "game.h"
#include "util.h"
#include "ghost.h"
#include "pos.h"

using namespace std;

namespace {

namespace {
int kWorstScore = INT_MIN / 10;

const int WEIGHTS[] = {
//    50 ,
//    50 ,
//    11 ,
//    10 ,
//   137 ,
//    -6 ,
#include "weights.csv"
};
const int kMaxTrials = WEIGHTS[0];
const int kMaxSteps = WEIGHTS[1];
//const int kScorePill = WEIGHTS[2];
const int kScorePill = WEIGHTS[3];
const int kScorePowerPill = WEIGHTS[3];
const int kFrightModeTicksDecremental = WEIGHTS[4];
const int kScoreNoValidDirections = WEIGHTS[5];
}

int Step(const game::Game& game,
         int remaining_fright_mode_ticks,
         Pos pos,
         int last_direction,
         int remain_try,
         int current_score) {
  if (remain_try == 0)
    return current_score;

  int score1 = game.GetMap().IsPill(pos) ? kScorePill :
      game.GetMap().IsPowerPill(pos) ? kScorePowerPill : 0;

  std::vector<int> valid_directions;
  for (int direction = 0; direction < 4; ++direction) {
    if (last_direction == BACKWARD_DIRECTION(direction))
      continue;

    Pos next_pos = pos;
    next_pos.Advance(direction);
    if (game.GetMap().IsWall(next_pos))
      continue;
    if (remaining_fright_mode_ticks < 0 &&
    	game.HasGhost(next_pos) &&
    	!game.GetMap().IsPowerPill(pos))
      continue;

    valid_directions.push_back(direction);
  }

  if (valid_directions.empty()) {
    return current_score + score1 + kScoreNoValidDirections;
  }

  int next_direction =
      valid_directions[util::xor128() % valid_directions.size()];
  pos.Advance(next_direction);

  return Step(game,
              remaining_fright_mode_ticks - kFrightModeTicksDecremental,
              pos,
              next_direction,
              remain_try - 1,
              score1 + current_score);
}

int WalkRandomly(
    const game::Game& game, Pos pos, int last_direction) {
  int remaining_fright_mode_ticks = game.GetRemainingFrightModeTick();
  if (remaining_fright_mode_ticks < 0 &&
      game.NeighbotGhost(pos) &&
      !game.GetMap().IsPowerPill(pos))
    return kWorstScore;

  return Step(game, remaining_fright_mode_ticks, pos,
              last_direction, kMaxSteps, 0);
}

int GetScore(const game::Game& game,
             Pos next_pos,
             int direction,
             int total_score,
             int remain_try) {
  if (remain_try == 0)
    return total_score;

  int score = WalkRandomly(game, next_pos, direction);
  return GetScore(
      game, next_pos, direction, total_score + score, remain_try - 1);
}

std::pair<int, int>  // direction, score
ThinkDirection(const game::Game& game,
               int direction,
               Pos current_pos,
               std::pair<int, int> current_best) {
  if (direction >= 4)
    return current_best;

  Pos next_pos = current_pos;
  next_pos.Advance(direction);
  if (game.GetMap().IsWall(next_pos))
    return ThinkDirection(game, direction + 1, current_pos, current_best);

  int score = GetScore(game, next_pos, direction, 0, kMaxTrials);
  if (score > current_best.second) {
    current_best = std::make_pair(direction, score);
  } else {
    current_best = current_best;  // do nothing
  }

  return ThinkDirection(game, direction + 1, current_pos, current_best);
}

}  // namespace

FakeLambdaMan::FakeLambdaMan(game::Game* game,
                             const Pos& pos)
  : LambdaMan(game, pos) {}

FakeLambdaMan::~FakeLambdaMan() {}

int FakeLambdaMan::Think() {
  std::map<int, int> count_to_direction;
  return ThinkDirection(
     *game, 0, pos, make_pair(1 /* defaut right */, INT_MIN)).first;
}

