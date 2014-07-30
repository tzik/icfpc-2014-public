#pragma once

#include "ghost.h"

namespace game {
struct Game;
}

// A ghost implementation for debugging.
struct FakeGhost : Ghost {
  FakeGhost(game::Game* game,
            const Pos& pos,
            int index);
  virtual ~FakeGhost();
  virtual int Think() const override;
};

#define FAKE_GHOST(name)                        \
  struct name : Ghost {                         \
    name(game::Game* game,                      \
         const Pos& pos,                        \
         int index);                            \
    virtual ~name();                            \
    virtual int Think() const override;         \
  };

FAKE_GHOST(FakeGhost1)
FAKE_GHOST(FakeGhost2)
FAKE_GHOST(FakeGhost3)
FAKE_GHOST(FakeGhost4)
FAKE_GHOST(FakeGhost5)
FAKE_GHOST(FakeGhost6)
FAKE_GHOST(FakeGhost7)
