#pragma once

#include <array>
#include <cassert>

struct Vec2 {
  float x{};
  float y{};
};

struct Vec3 {
  float x{};
  float y{};
  float z{};
};

struct Mat4 {
  std::array<float, 16> m{};

  float &operator[](std::size_t i) {
    assert(i <= 16);
    return m[i];
  }
};

constexpr Mat4 identity() {
  return {{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}};
}
