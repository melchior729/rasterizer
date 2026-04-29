#include "rasterizer.hpp"
#include "algorithm"

void draw_point(FrameBuffer &buffer, Vertex p) {
  buffer.set(static_cast<int>(p.pos.x), static_cast<int>(p.pos.y), p.pos.z,
             p.color);
}

static float get_determinant(Vec3 a, Vec3 b, Vec3 c) {
  return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
}

// user must provide A, B, C, in CCW
void draw_triangle(FrameBuffer &buffer, Vertex a, Vertex b, Vertex c) {
  float det{get_determinant({a.pos.x, a.pos.y}, {b.pos.x, b.pos.y},
                            {c.pos.x, c.pos.y})};
  if (det < 1e-7) {
    return;
  }

  float inv_det{1.0f / det};
  int min_x{static_cast<int>(std::min({a.pos.x, b.pos.x, c.pos.x}))};
  int min_y{static_cast<int>(std::min({a.pos.y, b.pos.y, c.pos.y}))};
  int max_x{static_cast<int>(std::max({a.pos.x, b.pos.x, c.pos.x}))};
  int max_y{static_cast<int>(std::max({a.pos.y, b.pos.y, c.pos.y}))};

  for (int i = min_x; i < max_x; i++) {
    for (int j = min_y; j < max_y; j++) {
      Vertex p{{static_cast<float>(i), static_cast<float>(j)}};

      float u{get_determinant(p.pos, b.pos, c.pos) * inv_det};
      float v{get_determinant(a.pos, p.pos, c.pos) * inv_det};
      float w{get_determinant(a.pos, b.pos, p.pos) * inv_det};

      if (u < 0 || v < 0 || w < 0) {
        continue;
      }

      float z{a.pos.z * u + b.pos.z * v + c.pos.z * w};
      p.pos.z = z;

      auto alpha{static_cast<uint32_t>(a.color.a() * u + b.color.a() * v +
                                       c.color.a() * w)};

      auto r{static_cast<uint32_t>(a.color.r() * u + b.color.r() * v +
                                   c.color.r() * w)};

      auto g{static_cast<uint32_t>(a.color.g() * u + b.color.g() * v +
                                   c.color.g() * w)};

      auto blue{static_cast<uint32_t>(a.color.b() * u + b.color.b() * v +
                                      c.color.b() * w)};

      p.color = {alpha, r, g, blue};
      draw_point(buffer, p);
    }
  }
}
