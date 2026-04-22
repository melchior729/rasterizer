#pragma once

#include <cstdint>

struct Color {
  uint32_t color;

  constexpr Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
      : color{static_cast<uint32_t>((a << 24) | (r << 16) | (g << 8) | b)} {}

  constexpr Color() : color{0xFF000000} {}

  uint8_t a() const { return static_cast<uint8_t>((color >> 24) & 0xFF); }

  uint8_t r() const { return (color >> 16) & 0xFF; }

  uint8_t g() const { return (color >> 8) & 0xFF; }

  uint8_t b() const { return color & 0xFF; }
};
