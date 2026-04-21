#include "rasterizer.hpp"
#include "algorithm"
#include <SDL3/SDL_log.h>

void draw_point(FrameBuffer &buffer, Vec2 p, Color color) {
  buffer.set(static_cast<int>(p.x), static_cast<int>(p.y), color);
}

static float get_determinant(Vec2 a, Vec2 b, Vec2 c) {
  return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (b.y - a.y);
}

void draw_triangle(FrameBuffer &buffer, Vec2 a, Vec2 b, Vec2 c, Color color) {
  float det{get_determinant(a, b, c)};
  if (det < 1e-7) {
    return;
  }

  float inv_det{1.0f / det};

  float min_x{std::min({a.x, b.x, c.x})};
  float min_y{std::min({a.y, b.y, c.y})};

  float max_x{std::max({a.x, b.x, c.x})};
  float max_y{std::max({a.y, b.y, c.y})};

  for (int i = min_x; i < max_x; i++) {
    for (int j = min_y; j < max_y; j++) {
      Vec2 p{static_cast<float>(i), static_cast<float>(j)};

      float u{get_determinant(p, b, c) * inv_det};
      float v{get_determinant(a, p, c) * inv_det};
      float w{1.0f - u - v};

      if (u < 0 || v < 0 || w < 0) {
        continue;
      }

      draw_point(buffer, p, color);
    }
  }
}
