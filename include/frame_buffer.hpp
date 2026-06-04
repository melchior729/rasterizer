#pragma once

#include "config.hpp"
#include <array>
#include <limits>

struct FrameBuffer {
  static constexpr Color BG{0xC0, 0x13, 0x15, 0x1A};

  std::array<Color, WIDTH * HEIGHT> pixels{};
  std::array<float, WIDTH * HEIGHT> depth{};
  Color clear_color{BG};

  FrameBuffer() { clear(); }

  void set(int x, int y, float z, Color color) {
    if (out_of_bounds(x, y)) {
      return;
    }

    auto i = static_cast<std::size_t>(y * WIDTH + x);
    if (z >= depth[i]) {
      return;
    }

    depth[i] = z;
    pixels[i] = color;
  }

  void clear() {
    depth.fill(std::numeric_limits<float>::max());
    pixels.fill(clear_color);
  }

  bool out_of_bounds(int x, int y) const {
    return (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT);
  }
};
