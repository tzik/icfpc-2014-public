#include "pos.h"

#include <cstdlib>

static const int DX[] = {0, 1, 0, -1};
static const int DY[] = {-1, 0, 1, 0};

void Pos::Advance(int direction) {
  x += DX[direction];
  y += DY[direction];
}

int Pos::ManhattanDistance(const Pos& rh) const {
  return std::abs(x - rh.x) + std::abs(y - rh.y);
}
