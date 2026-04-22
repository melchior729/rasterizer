#include "rasterizer.hpp"
#include "algorithm"
#include <SDL3/SDL_log.h>

void draw_point(FrameBuffer &buffer, Vec3 p, Color color) {
  buffer.set(static_cast<int>(p.x), static_cast<int>(p.y), p.z, color);
}

static float get_determinant(Vec3 a, Vec3 b, Vec3 c) {
  return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
}

void draw_triangle(FrameBuffer &buffer, Vec3 a, Vec3 b, Vec3 c, Color color) {
  float det{get_determinant(a, b, c)};
  if (std::abs(det) < 1e-7) {
    return;
  }

  float inv_det{1.0f / det};
  int min_x{static_cast<int>(std::min({a.x, b.x, c.x}))};
  int min_y{static_cast<int>(std::min({a.x, b.x, c.x}))};
  int max_x{static_cast<int>(std::max({a.y, b.y, c.y}))};
  int max_y{static_cast<int>(std::max({a.y, b.y, c.y}))};

  for (int i = min_x; i < max_x; i++) {
    for (int j = min_y; j < max_y; j++) {
      Vec3 p{static_cast<float>(i), static_cast<float>(j)};

      float u{get_determinant(p, b, c) * inv_det};
      float v{get_determinant(p, c, a) * inv_det};
      float w{get_determinant(p, a, b) * inv_det};

      if (det < 0) {
        u = -u;
        v = -v;
        w = -w;
      }

      if (u < 0 || v < 0 || w < 0) {
        continue;
      }

      float z{a.z * u + b.z * v + c.z * w};
      p.z = z;

      draw_point(buffer, p, color);
    }
  }
}
