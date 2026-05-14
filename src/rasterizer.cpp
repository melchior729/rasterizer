#include "rasterizer.hpp"
#include "algorithm"

void point(FrameBuffer &buffer, Vertex &p, Color color) {
  buffer.set(static_cast<int>(p.pos.x), static_cast<int>(p.pos.y), p.pos.z,
             color);
}

static float det(const Vec4 &a, const Vec4 &b, const Vec4 &c) {
  return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
}

void triangle(FrameBuffer &buffer, Vertex &a, Vertex &b, Vertex &c,
              Material material) {
  float det_val{
      det({a.pos.x, a.pos.y}, {b.pos.x, b.pos.y}, {c.pos.x, c.pos.y})};
  if (std::abs(det_val) < 1e-7) {
    return;
  }

  float inv_det{1.0f / det_val};
  int min_x{std::clamp(static_cast<int>(std::min({a.pos.x, b.pos.x, c.pos.x})),
                       0, WIDTH)};
  int min_y{std::clamp(static_cast<int>(std::min({a.pos.y, b.pos.y, c.pos.y})),
                       0, HEIGHT)};
  int max_x{std::clamp(static_cast<int>(std::max({a.pos.x, b.pos.x, c.pos.x})),
                       0, WIDTH)};
  int max_y{std::clamp(static_cast<int>(std::max({a.pos.y, b.pos.y, c.pos.y})),
                       0, HEIGHT)};

  for (int i{min_x}; i < max_x; i++) {
    for (int j{min_y}; j < max_y; j++) {
      // TODO empty UV, Normals for now
      Vertex p{{static_cast<float>(i), static_cast<float>(j)}, {}, {}};

      float u{det(p.pos, b.pos, c.pos) * inv_det};
      float v{det(a.pos, p.pos, c.pos) * inv_det};
      float w{det(a.pos, b.pos, p.pos) * inv_det};

      if (u < 0 || v < 0 || w < 0) {
        continue;
      }

      float z{a.pos.z * u + b.pos.z * v + c.pos.z * w};
      p.pos.z = z;

      auto r{static_cast<uint32_t>(material.diffuse.r() * u +
                                   material.diffuse.r() * v +
                                   material.diffuse.r() * w)};

      auto g{static_cast<uint32_t>(material.diffuse.g() * u +
                                   material.diffuse.g() * v +
                                   material.diffuse.g() * w)};

      auto blue{static_cast<uint32_t>(material.diffuse.b() * u +
                                      material.diffuse.b() * v +
                                      material.diffuse.b() * w)};

      point(buffer, p, {0xFF, r, g, blue});
    }
  }
}
