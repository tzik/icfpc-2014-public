
#include "vm_lambdaman.h"

#include <iostream>
#include <cassert>
#include <map>

#include "game.h"
#include "ghost.h"

namespace game {

namespace {

const int kInstructionPerSecond = 3072000;

}  // namespace

VMLambdaMan::VMLambdaMan(std::istream& src) {
  program = gcc::LoadProgram(src);
}

VMLambdaMan::~VMLambdaMan() {
}

void VMLambdaMan::Initialize(game::Game* game, const Pos& pos) {
  LambdaMan::Initialize(game, pos);

  int time_limit = 60 * kInstructionPerSecond;

  vm.environment = std::make_shared<gcc::Frame>(2, nullptr);
  vm.environment->values[0] = EncodeWorld();
  vm.environment->values[1] = gcc::WrapToValue(0);

  try {
    for (int i = 0; i < time_limit && vm.is_running; ++i)
      vm.Step(program);
  } catch (...) {
    vm.Dump(std::cerr);
    std::cerr << std::dec << vm.program_counter << "\n";
    throw;
  }

  if (vm.is_running)
    throw TimeLimitExceeded();

  gcc::ValueRef res = vm.PopFromDataStack();
  gcc::ConsValue* cons = res->AsCons();
  ai_state = std::move(cons->car);
  step = std::move(cons->cdr);
}

int VMLambdaMan::Think() {
  vm.PushToDataStack(std::move(ai_state));
  vm.PushToDataStack(EncodeWorld());
  vm.PushToDataStack(step);
  vm.is_running = true;

  vm.AP(2);

  int time_limit = 1 * kInstructionPerSecond;
  std::cerr << "HERE\n";
  try {
    for (int i = 0; i < time_limit && vm.is_running; ++i)
      vm.Step(program);
  } catch (...) {
    vm.Dump(std::cerr);
    std::cerr << std::dec << vm.program_counter << "\n";
    throw;
  }

  gcc::ValueRef res = vm.PopFromDataStack();
  gcc::ConsValue* cons = res->AsCons();
  ai_state = std::move(cons->car);
  int decision = cons->cdr->AsInteger()->value;

  ResetVM();

  return decision;
}

void VMLambdaMan::ResetVM() {
  std::vector<gcc::ValueRef> extra_root_set;
  extra_root_set.push_back(ai_state);
  extra_root_set.push_back(step);

  vm.data_stack.clear();
  vm.control_stack.clear();
  vm.environment = nullptr;
  vm.program_counter = 0;
  vm.GC(&extra_root_set);
}

gcc::ValueRef VMLambdaMan::EncodeWorld() const {
  return gcc::MakeTuple(EncodeMap(),
                        EncodeLambdaManState(),
                        EncodeGhostState(),
                        EncodeFruitState());
}

gcc::ValueRef VMLambdaMan::EncodeMap() const {
  gcc::ValueRef result = gcc::MakeList();
  std::map<Pos, int> pt_override;
  for (auto&& fruits : game->map->fruits)
    pt_override[fruits] = 4;
  pt_override[game->map->initial_lambda_man] = 5;
  for (auto&& initial_ghost : game->map->initial_ghosts)
    pt_override[initial_ghost] = 6;

  for (int y = game->map->GetHeight() - 1; y >= 0; --y) {
    gcc::ValueRef row = gcc::MakeList();
    for (int x = game->map->GetWidth() - 1; x >= 0; --x) {
      gcc::integer cell_value = 1;  // empty.
      switch (game->map->map[y][x]) {
        case Map::INVALID:
        case Map::LAMBDA_MAN:
        case Map::GHOST:
          // Lambda-Man and Ghost should not be on the map.
          // Handle them separately.
          assert(false);
          // fall through
        case Map::EMPTY:
          cell_value = 1;
          break;
        case Map::WALL:
          cell_value = 0;
          break;
        case Map::PILL:
          cell_value = 2;
          break;
        case Map::POWER_PILL:
          cell_value = 3;
          break;
        case Map::FRUIT:
          cell_value = 4;
          break;
      }
      auto found = pt_override.find(Pos(x, y));
      if (found != pt_override.end())
        cell_value = found->second;

      row = gcc::MakePair(cell_value, std::move(row));
    }
    result = gcc::MakePair(std::move(row), std::move(result));
  }
  return result;
}

gcc::ValueRef VMLambdaMan::EncodeLambdaManState() const {
  gcc::integer vitality = 0;
  if (IsFrightMode())
    vitality = game->GetRemainingFrightModeTick();
  return gcc::MakeTuple(vitality,
                        gcc::MakePair(pos.x, pos.y),
                        direction,
                        num_lives,
                        score);
}

gcc::ValueRef VMLambdaMan::EncodeGhostState() const {
  gcc::ValueRef result = gcc::MakeList();
  for (auto iter = game->ghosts.rbegin();
       iter != game->ghosts.rend(); ++iter) {
    auto&& ghost = *iter;
    gcc::integer vitality = 0;  // standard
    if (ghost->IsInvisibleMode())
      vitality = 2;
    else if (game->IsFrightMode())
      vitality = 1;
    result = gcc::MakePair(
        gcc::MakeTuple(vitality,
                       gcc::MakePair(ghost->pos.x, ghost->pos.y),
                       ghost->direction),
        std::move(result));
  }
  return result;
}

gcc::ValueRef VMLambdaMan::EncodeFruitState() const {
  gcc::integer result_value = 0;
  if (game->map->has_fruit) {
    if (game->utc <= 127 * 280)
      result_value = 127 * 280 - game->utc;
    else if (game->utc <= 127 * 480)
      result_value = 127 * 280 - game->utc;
  }
  return gcc::WrapToValue(result_value);
}

}  // namespace game
