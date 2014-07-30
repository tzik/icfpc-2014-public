#pragma once

#include <memory>
#include <string>
#include <vector>

#include "map.h"

struct Ghost;
struct LambdaMan;

namespace game {

struct Game {
  using Ghosts = std::vector<std::unique_ptr<Ghost>>;
  // public:
  Game(std::unique_ptr<Map> map,
       std::unique_ptr<LambdaMan> lambda);
  // Run VMs and returns score
  int Run(bool clear);

  const Map& GetMap() const;

  // Returns current UTC for debug.
  int GetUTC() const;
  int GetRemainingFrightModeTick() const;

  // Advance UTC to next actionable tick.
  void AdvanceUTC();

  int GetFruitScore() const;
  int GetGhostScore(int number_of_eaten_ghost) const;
  bool IsFrightMode() const;
  bool HasGhost(const Pos& pos) const;
  bool NeighbotGhost(const Pos& pos) const;

  std::string GetDebugDump() const;

  void SetGhosts(std::vector<std::unique_ptr<Ghost>> ghosts);

  int level;
  int utc;
  int fright_mode_end_utc;
  int game_end_utc;

  Ghosts ghosts;  // Up to 256 ghosts.

  std::unique_ptr<Map> map;
  std::unique_ptr<LambdaMan> lambda_man;
};

}  // namespace game
