#pragma once

#include <cstdint>

namespace util {

// xorshift による乱数生成
uint32_t xor128();
void xor128_init();

}  // namespace util
