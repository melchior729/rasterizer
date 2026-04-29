#pragma once

#include "config.hpp"
#include <array>
#include <limits>

struct FrameBuffer {
  std::array<Color, WIDTH * HEIGHT> pixels{};
  std::array<float, WIDTH * HEIGHT> depth{};

  FrameBuffer() { depth.fill(-std::numeric_limits<float>::infinity()); }

  void set(int x, int y, float z, Color color) {
    if (out_of_bounds(x, y)) {
      return;
    }

    auto i = static_cast<std::size_t>(y * WIDTH + x);
    if (z <= depth[i] || z >= 0) {
      return;
    }

    depth[i] = z;
    pixels[i] = color;
  }

  bool out_of_bounds(int x, int y) const {
    return (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT);
  }
};
