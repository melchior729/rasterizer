#pragma once

#include "config.hpp"
#include <array>

struct FrameBuffer {
  std::array<Color, WIDTH * HEIGHT> pixels{};

  void set(int x, int y, Color color) {
    if (out_of_bounds(x, y)) {
      return;
    }

    pixels[y * WIDTH + x] = color;
  }

  static bool out_of_bounds(int x, int y) {
    return (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT);
  }
};
