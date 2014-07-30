#include "map.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "util.h"

Map::Map() {}

void Map::Parse(const std::vector<std::string>& lines) {
  width = lines[0].size();
  height = lines.size();
  for (int y = 0; y < MAX_HEIGHT; ++y) {
    for (int x = 0; x < MAX_WIDHT; ++x) {
      map[y][x] = EMPTY;
    }
  }

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Cell ch = static_cast<Cell>(lines[y][x]);
      switch (ch) {
      case INVALID:
        map[y][x] = INVALID;
        break;
      case EMPTY:
      case PILL:
      case POWER_PILL:
      case WALL:
        map[y][x] = ch;
        break;
      case FRUIT:
        fruits.push_back(Pos(x, y));
        break;
      case LAMBDA_MAN:
        initial_lambda_man = Pos(x, y);
        break;
      case GHOST:
        initial_ghosts.push_back(Pos(x, y));
        break;
      }
    }
  }
}

int Map::GetWidth() const {
  return width;
}

int Map::GetHeight() const {
  return height;
}

bool Map::IsEmpty(const Pos& pos) const {
  return map[pos.y][pos.x] == EMPTY;
}

bool Map::IsWall(const Pos& pos) const {
  return map[pos.y][pos.x] == WALL;
}

bool Map::IsPill(const Pos& pos) const {
  return map[pos.y][pos.x] == PILL;
}

bool Map::IsPowerPill(const Pos& pos) const {
  return map[pos.y][pos.x] == POWER_PILL;
}

bool Map::IsFruit(const Pos& pos) const {
  return map[pos.y][pos.x] == FRUIT;
}

void Map::Clear(const Pos& pos) {
  if (IsPill(pos)) {
  }
  map[pos.y][pos.x] = EMPTY;
}

void Map::AppearFruit(int index) {
  has_fruit = true;
  map[fruits[index].y][fruits[index].x] = FRUIT;
}

void Map::ExpireFruit(int index) {
  has_fruit = false;
  map[fruits[index].y][fruits[index].x] = EMPTY;
}

int Map::GetNumberOfFruits() const {
  return fruits.size();
}

int Map::GetNumberOfGhosts() const {
  return initial_ghosts.size();
}

const Pos& Map::GetInitialLambdaMan() const {
  return initial_lambda_man;
}

const Pos& Map::GetInitialGhost(int index) const {
  return initial_ghosts[index];
}

int Map::GetNumberOfRemainingPills() const {
  int width = GetWidth();
  int height = GetHeight();
  int number_of_remaining_pills = 0;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (IsPill(Pos(x, y))) {
        ++number_of_remaining_pills;
      }
    }
  }
  return number_of_remaining_pills;
}

namespace {

const char kEsc[] = "\x1b[";

std::string Locate(int y, int x) {
  char tmp[30];
  std::sprintf(tmp, "%s%d;%dH", kEsc, y, x);
  return std::string(tmp);
}

std::string CLS() {
  char tmp[30];
  std::sprintf(tmp, "%s2J", kEsc);
  return std::string(tmp);
}

}

std::string Map::GetDebugDump() const {
  std::ostringstream oss;
  oss << CLS();
  oss << Locate(0, 0);
  oss << "Size: " << height << " x " << width << "\n";
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j)
      oss << map[i][j];
    oss << "\n";
  }

  return oss.str();
}
