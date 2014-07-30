#pragma once

enum DIRECTION {
  DIRECTION_UP = 0,
  DIRECTION_RIGHT = 1,
  DIRECTION_DOWN = 2,
  DIRECTION_LEFT = 3
};

#define BACKWARD_DIRECTION(d) ((d+2)%4)

struct Pos {
  int x;
  int y;

  Pos(int x = 0, int y = 0) : x(x), y(y) { }

  bool operator == (const Pos& rh) const {
    return x == rh.x && y == rh.y;
  }

  bool operator != (const Pos& rh) const {
    return !(*this == rh);
  }

  bool operator < (const Pos& rh) const {
    if (x != rh.x) return x < rh.x;
    return y < rh.y;
  }

  void Advance(int direction);
  int ManhattanDistance(const Pos& rh) const;
};
