#include "util.h"

#include <cstdint>

namespace util {

namespace {  // for xor128
uint32_t x = 123456789;
uint32_t y = 362436069;
uint32_t z = 521288629;
uint32_t w = 88675123;
}

void xor128_init() {
  x = 123456789;
  y = 362436069;
  z = 521288629;
  w = 88675123;
}

uint32_t xor128(){
  uint32_t t = (x ^ (x << 11));
  x = y;
  y = z;
  z = w;
  return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

}  // namespace util

