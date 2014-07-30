#pragma once

#include <string>
#include <vector>

#include "pos.h"

// Return the current map state in a tick.

struct Map {
  static const int MAX_WIDHT = 256;
  static const int MAX_HEIGHT = 256;
  static const int MAX_NUMBER_OF_FRUITS = 2;
  static const int MAX_NUMBER_LAMBDA_MANS = 2;
  static const int MAX_NUMBER_OF_GHOSTS = 1024;

  enum Cell : char {
    INVALID = '\0',
    EMPTY = ' ',
    WALL = '#',
    PILL = '.',
    POWER_PILL = 'o',
    FRUIT = '%',
    LAMBDA_MAN = '\\',
    GHOST = '=',
  };

  Map();

  void Parse(const std::vector<std::string>& lines);
  int GetWidth() const;
  int GetHeight() const;
  bool IsEmpty(const Pos& pos) const;
  bool IsWall(const Pos& pos) const;
  bool IsPill(const Pos& pos) const;
  bool IsPowerPill(const Pos& pos) const;
  bool IsFruit(const Pos& pos) const;
  void Clear(const Pos& pos);
  void AppearFruit(int index);
  void ExpireFruit(int index);
  int GetNumberOfFruits() const;
  int GetNumberOfGhosts() const;
  const Pos& GetInitialLambdaMan() const;
  const Pos& GetInitialGhost(int index) const;
  int GetNumberOfRemainingPills() const;

  // Returns a string to monitor map status.
  std::string GetDebugDump() const;

  int width;
  int height;
  Cell map[MAX_HEIGHT][MAX_WIDHT];
  bool has_fruit = false;
  std::vector<Pos> fruits;
  Pos initial_lambda_man;
  std::vector<Pos> initial_ghosts;
};
