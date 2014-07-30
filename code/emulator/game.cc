#include "game.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "gcc_vm.h"
#include "ghc.h"
#include "ghost.h"
#include "lambdaman.h"
#include "map.h"
#include "util.h"
#include "fake_ghost.h"

using namespace std;

namespace game {

namespace {
const int FRIGHT_MODE_DURATION = 127 * 20;
const int FRUIT_APPEARS[] = { 127 * 200, 127 * 400 };
const int FRUIT_EXPIRES[] = { 127 * 280, 127 * 480 };
const int SCORE_PILL = 10;
const int SCORE_POWER_PILL = 50;
const int SCORE_FRUIT[] =
  { 0, 100, 300, 500, 500, 700, 700, 1000, 1000, 2000, 2000, 3000, 3000 };
const int SCORE_GHOST[] =
  { 0, 200, 400, 800, 1600, 1600 };

const bool STEP_EXECUTION_MODE = false;
}  // namespace

Game::Game(std::unique_ptr<Map> map_,
           std::unique_ptr<LambdaMan> lambda_man_)
    : utc(1), fright_mode_end_utc(-1) {
  lambda_man = std::move(lambda_man_);
  map = std::move(map_);

  int size = map->width * map->height;
  level = (size + 99) / 100;
  game_end_utc = 127 * size * 16;

  lambda_man->Initialize(this, map->GetInitialLambdaMan());
}

int Game::Run(bool clear_monitor) {
  AdvanceUTC();
  bool fruit_appeared[2] = { false };
  bool fruit_expired[2] = { false };
  int num_eaten_ghost = 0;

  bool win = false;
  bool lose = false;
  while (utc < game_end_utc) {
    // 1. All Lambda - Man and ghost moves scheduled for this tick take place.
    // The next move is also scheduled at this point.
    // (Note that Lambda - Man and the ghosts do not move every tick,
    // only every few ticks; see the ticks section below.)
    if (lambda_man->IsMyTurn(utc))
        lambda_man->Move();
    for (const auto& ghost : ghosts) {
      if (ghost->IsMyTurn(utc))
        ghost->Move();
    }

    // 2. Next, any actions(fright mode deactivating,
    // fruit appearing / disappearing) take place.

    // 2-1. fright mode deactivating
    int remaining_fright_mode_ticks = GetRemainingFrightModeTick();
    if (remaining_fright_mode_ticks <= 0) {
      for (const auto& ghost : ghosts) {
        ghost->SetInvisibleMode(false);
      }
    }

    // 2-2. fruit appearing
    for (int i = 0; i < map->GetNumberOfFruits(); ++i) {
      if (utc <= FRUIT_APPEARS[i] && !fruit_appeared[i]) {
        fruit_appeared[i] = true;
        map->AppearFruit(i);
      }
    }

    // 2-3. fruit disappearing
    for (int i = 0; i < map->GetNumberOfFruits(); ++i) {
      if (utc <= FRUIT_EXPIRES[i] && !fruit_expired[i]) {
        fruit_expired[i] = true;
        map->ExpireFruit(i);
      }
    }

    // 3. Next, we check if Lambda - Man is occupying the same square as pills,
    // power pills, or fruit :

    // 3-1. If Lambda - Man occupies a square with a pill,
    // the pill is eaten by Lambda - Man and removed from the game.
    if (map->IsPill(lambda_man->GetPos())) {
      lambda_man->AddScore(SCORE_PILL);
      lambda_man->SetEating();
      map->Clear(lambda_man->GetPos());
    }

    // 3-2. If Lambda - Man occupies a square with a power pill,
    // the power pill is eaten by Lambda - Man,
    // removed from the game,
    // and fright mode is immediately activated,
    // allowing Lambda - Man to eat ghosts.
    if (map->IsPowerPill(lambda_man->GetPos())) {
      lambda_man->AddScore(SCORE_POWER_PILL);
      lambda_man->SetEating();
      map->Clear(lambda_man->GetPos());
      fright_mode_end_utc = utc + FRIGHT_MODE_DURATION;
      num_eaten_ghost = 0;
      for (const auto& ghost : ghosts) {
        ghost->ReverseDirection();
      }
    }

    // 3-3. If Lambda - Man occupies a square with a fruit,
    // the fruit is eaten by Lambda - Man,
    // and removed from the game.
    if (map->IsFruit(lambda_man->GetPos())) {
      lambda_man->AddScore(GetFruitScore());
      lambda_man->SetEating();
      map->Clear(lambda_man->GetPos());
    }

    // 4. Next, if one or more visible ghosts are on the same square as Lambda - Man,
    // then depending on whether or not fright mode is active,
    // Lambda - Man either loses a life or eats the ghost(s). See below for details.
    // TODO(nodchip): Check the case that multiple lambda mans and ghosts are on the same square.
    // Especially, the case one is in fright mode and the other is not.
    bool died = false;
    for (const auto& ghost : ghosts) {
      if (ghost->IsInvisibleMode())
        continue;
      if (lambda_man->GetPos() != ghost->GetPos())
        continue;

      if (lambda_man->IsFrightMode()) {
        ghost->ResetPosition();
        ++num_eaten_ghost;
        lambda_man->AddScore(GetGhostScore(num_eaten_ghost));
        lambda_man->SetEating();
      } else {
        died = true;
        lambda_man->LoseALife();
        break;
      }
    }

    // 5. Next, if all the ordinary pills(ie not power pills) have been eaten,
    // then Lambda - Man wins and the game is over.
    if (map->GetNumberOfRemainingPills() == 0) {
      win = true;
      break;
    }

    // 6. Next, if the number of Lambda - Man lives is 0,
    // then Lambda - Man loses and the game is over.
    lose = true;
    if (lambda_man->num_lives > 0) {
      lose = false;
    }
    if (lose) {
      break;
    }

    // Losing a life
    if (died) {
      lambda_man->ResetPosition();
      for (const auto& ghost : ghosts)
        ghost->ResetPosition();
    }

    if (STEP_EXECUTION_MODE) {
      cout << "step:";
      string line;
      getline(cin, line);
    }
    if (clear_monitor)
      std::cout << "\x1b[0;0H";  // Locate(0, 0)
    cout << GetDebugDump() << endl;
    //static int counter = 0;
    //if (++counter % 10 == 0) {
    //  fprintf(stderr,
    //    "counter=%d pills=%d lives=%d score=%d\n"
    //    "utc=%d end_of_lives=%d progress=%.1f\n\n",
    //    counter, map->GetNumberOfRemainingPills(), lambda_man->num_lives, lambda_man->score,
    //    utc, game_end_utc, utc / double(game_end_utc) * 100.0);
    //  cout << GetDebugDump() << endl;
    //}

    // 7. Finally, the tick counter is incremented.
    AdvanceUTC();
  }

  if (win) {
    int score = lambda_man->score;
    lambda_man->AddScore(score * lambda_man->num_lives);
  }

  int score = 0;
  score = std::max(score, lambda_man->score);
  return score;
}

int Game::GetUTC() const {
  return utc;
}

int Game::GetRemainingFrightModeTick() const {
  return fright_mode_end_utc - utc;
}

const Map& Game::GetMap() const {
  return *map;
}

void Game::AdvanceUTC() {
  utc = 0x7fffffff;
  utc = std::min(utc, lambda_man->GetNextMovableTicks());
  for (const auto& ghost : ghosts)
    utc = std::min(utc, ghost->GetNextMovableTicks());
}

int Game::GetFruitScore() const {
  if (level <= 12) {
    return SCORE_FRUIT[level];
  }
  else {
    return 5000;
  }
}

int Game::GetGhostScore(int number_of_eaten_ghost) const {
  if (number_of_eaten_ghost < 5) {
    return SCORE_GHOST[number_of_eaten_ghost];
  } else {
    return 1600;
  }
}

bool Game::IsFrightMode() const {
  return utc <= fright_mode_end_utc;
}

std::string Game::GetDebugDump() const {
  vector<string> lines;
  for (int y = 0; y < map->GetHeight(); ++y) {
    string line;
    for (int x = 0; x < map->GetWidth(); ++x) {
      line += map->map[y][x];
    }
    lines.push_back(line);
  }

  int x = lambda_man->GetPos().x;
  int y = lambda_man->GetPos().y;
  lines[y][x] = '\\';

  for (const auto& ghost : ghosts) {
    int x = ghost->GetPos().x;
    int y = ghost->GetPos().y;
    if (ghost->IsInvisibleMode()) {
      lines[y][x] = 'a' + ghost->GetIndex();
    }
    else {
      lines[y][x] = 'A' + ghost->GetIndex();
    }
  }

  string concat;
  for (const auto& line : lines) {
    concat += line;
    concat += '\n';
  }
  return concat;
}

bool Game::HasGhost(const Pos& pos) const {
  for (const auto& ghost : ghosts) {
    if (ghost->GetPos() == pos) {
      return true;
    }
  }

  return false;
}

bool Game::NeighbotGhost(const Pos& pos) const {
  for (const auto& ghost : ghosts) {
    if (ghost->GetPos().ManhattanDistance(pos) <= 1) {
      return true;
    }
  }

  return false;
}

void Game::SetGhosts(std::vector<std::unique_ptr<Ghost>> ghosts) {
  this->ghosts = std::move(ghosts);
}


}  // namespace game

