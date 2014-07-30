#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "fake_ghost.h"
#include "fake_lambdaman.h"
#include "game.h"
#include "gcc_vm.h"
#include "ghc.h"
#include "map.h"
#include "vm_lambdaman.h"

#define LOG std::cerr << "[" << __FILE__ << "(" << __LINE__ << ")] "

namespace {
const int kNoSeed = -1;

std::string map_filename;
std::string gcc_filename;
int seed = kNoSeed;
bool clear_monitor = false;

bool PrintUsage(char* argv0, std::string message = "") {
  if (!message.empty())
    std::cerr << message << "\n\n";
  std::cerr
      << "Usage:\n"
      << argv0 << " <map file> [options]\n"
      << "\n"
      << "Options:\n"
      << "  --gcc <gcc file>  Filename of GCC file to run on Lambda-Man.\n"
      << "                    If it is not specified, Fake Lambda-Man runs.\n"
      << "  --clear        Clear terminal and display map in the same place.\n"
      << "  --seed <seed>  Specify an integer as a random seed.\n";

  return false;
}

std::unique_ptr<Map> GetMapFile() {
  std::unique_ptr<Map> map;  // Not allocated yet.

  if (map_filename.empty())
    return map;

  std::ifstream ifs(map_filename);
  if (!ifs.is_open()) {
    std::cerr << "Error: " << map_filename << " is not found." << std::endl;
    return map;
  }
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line);)
    lines.push_back(line);
  map.reset(new Map);
  map->Parse(lines);

  return std::move(map);
}

bool ParseOption(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if (i + 1 < argc && arg == "--gcc") {
      gcc_filename = argv[++i];
    } else if (i + 1 < argc && arg == "--seed") {
      seed = std::atoi(argv[++i]);
    } else if (arg == "--clear") {
      clear_monitor = true;
    } else if (arg[0] == '-') {
      std::string message = "Unknown flag : ";
      message += argv[i];
      return PrintUsage(argv[0], message);
    } else if (map_filename.empty()) {
      map_filename = arg;
    } else {
      return PrintUsage(argv[0], "Too many arguments.");
    }
  }
  if (map_filename.empty())
    return PrintUsage(argv[0], "No map file is specified.");

  return true;
}

}  // namespace

void SetFakeGhosts(game::Game* game);

int main(int argc, char* argv[]) {
  if (!ParseOption(argc, argv))
    return 0;

  std::unique_ptr<LambdaMan> lambda_man(new FakeLambdaMan(NULL, Pos()));
  if (!gcc_filename.empty()) {
    std::ifstream vm_ifs(gcc_filename);
    if (vm_ifs.is_open()) {
      lambda_man.reset(new game::VMLambdaMan(vm_ifs));
      LOG << "VMLambdaman\n";
    }
  }
  std::unique_ptr<Map> map = GetMapFile();

  game::Game game(std::move(map), std::move(lambda_man));
  SetFakeGhosts(&game);
  
  if (clear_monitor)
    std::cout << "\x1b[2J";
  int score = game.Run(clear_monitor);

  std::cout << "score " << score << "  "
            << "live " << game.lambda_man->num_lives << "\n";
}

void SetFakeGhosts(game::Game* game) {
#define CREATE_FAKE_GHOST(name)                         \
   new name(game, game->GetMap().GetInitialGhost(i), i)

  int num_ghost = game->map->GetNumberOfGhosts();
  std::vector<std::unique_ptr<Ghost> > ghosts;

  if (seed == kNoSeed) {
    for (int i = 0; i < num_ghost; ++i) {
      switch (i % 4) {
      case 0:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost2));
        break;
      case 1:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost3));
        break;
      case 2:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost4));
        break;
      case 3:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost5));
        break;
      }
    }
  } else {
    srand(seed);
    for (int i = 0; i < num_ghost; ++i) {
      switch (rand() % 7) {
      case 0:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost1));
        break;
      case 1:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost2));
        break;
      case 2:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost3));
        break;
      case 3:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost4));
        break;
      case 4:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost5));
        break;
      case 5:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost6));
        break;
      case 6:
        ghosts.emplace_back(CREATE_FAKE_GHOST(FakeGhost7));
        break;
      }
    }
  }
#undef CREATE_FAKE_GHOST

  game->SetGhosts(std::move(ghosts));
}
