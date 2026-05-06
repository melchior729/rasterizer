// TODO this class should be eventually removed
#pragma once

#include <cstdint>

struct Color {
  uint32_t color{};

  constexpr Color() : color{0xFF000000} {}

  constexpr Color(uint32_t a, uint32_t r, uint32_t g, uint32_t b)
      : color{(a << 24) | (r << 16) | (g << 8) | b} {}

  constexpr uint8_t a() const {
    return static_cast<uint8_t>((color >> 24) & 0xFF);
  }

  constexpr uint8_t r() const { return (color >> 16) & 0xFF; }

  constexpr uint8_t g() const { return (color >> 8) & 0xFF; }

  constexpr uint8_t b() const { return color & 0xFF; }
};
