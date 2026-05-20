#include "rasterizer.hpp"
#include "algorithm"

static constexpr float epsilon{-1e-4f};

void point(FrameBuffer &buffer, const Vertex &p, const Color color) {
  buffer.set(static_cast<int>(p.pos.x), static_cast<int>(p.pos.y), p.pos.z,
             color);
}

static float det(const Vec4 &a, const Vec4 &b, const Vec4 &c) {
  return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
}

void line(FrameBuffer &buffer, const Vertex &a, const Vertex &b) {
  int x0{static_cast<int>(a.pos.x)};
  int x1{static_cast<int>(b.pos.x)};
  int y0{static_cast<int>(a.pos.y)};
  int y1{static_cast<int>(b.pos.y)};

  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;

  int x = x0;
  int y = y0;

  while (true) {
    float t = dx != 0
                  ? static_cast<float>(abs(x - x0)) / static_cast<float>(dx)
                  : static_cast<float>(abs(y - y0)) / static_cast<float>(dy);
    float z{a.pos.z + t * (b.pos.z - a.pos.z)};
    buffer.set(x, y, z, WHITE);
    if (x == x1 && y == y1)
      break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }
}

void triangle_wireframe(FrameBuffer &buffer, const Vertex &a, const Vertex &b,
                        const Vertex &c) {
  line(buffer, a, b);
  line(buffer, a, c);
  line(buffer, b, c);
}

void triangle(FrameBuffer &buffer, const Vertex &a, const Vertex &b,
              const Vertex &c, const Material material,
              [[maybe_unused]] const Vec3 light_dir, const float bright_a,
              const float bright_b, const float bright_c, RenderMode mode) {
  float det_val{
      det({a.pos.x, a.pos.y}, {b.pos.x, b.pos.y}, {c.pos.x, c.pos.y})};
  if (std::abs(det_val) < 1e-7) {
    return;
  }

  float inv_det{1.0f / det_val};
  int min_x{std::clamp(
      static_cast<int>(std::floor(std::min({a.pos.x, b.pos.x, c.pos.x}))), 0,
      WIDTH)};
  int min_y{std::clamp(
      static_cast<int>(std::floor(std::min({a.pos.y, b.pos.y, c.pos.y}))), 0,
      HEIGHT)};
  int max_x{std::clamp(
      static_cast<int>(std::ceil(std::max({a.pos.x, b.pos.x, c.pos.x}))), 0,
      WIDTH)};
  int max_y{std::clamp(
      static_cast<int>(std::ceil(std::max({a.pos.y, b.pos.y, c.pos.y}))), 0,
      HEIGHT)};

  for (int i{min_x}; i < max_x; i++) {
    for (int j{min_y}; j < max_y; j++) {
      // TODO empty UV, Normals for now
      Vertex p{{static_cast<float>(i), static_cast<float>(j)}, {}, {}};

      float u{det(p.pos, b.pos, c.pos) * inv_det};
      float v{det(a.pos, p.pos, c.pos) * inv_det};
      float w{det(a.pos, b.pos, p.pos) * inv_det};

      if (u < epsilon || v < epsilon || w < epsilon) {
        continue;
      }

      float normal_x{a.normal.x * u + b.normal.x * v + c.normal.x * w};
      float normal_y{a.normal.y * u + b.normal.y * v + c.normal.y * w};
      float normal_z{a.normal.z * u + b.normal.z * v + c.normal.z * w};
      Vec3 normal{normal_x, normal_y, normal_z};

      float brightness = (mode == RenderMode::Phong)
                             ? std::clamp(normal.dot(light_dir), ambient, 1.0f)
                             : bright_a * u + bright_b * v + bright_c * w;

      float z{a.pos.z * u + b.pos.z * v + c.pos.z * w};
      p.pos.z = z;

      auto r{static_cast<uint32_t>(brightness * (material.diffuse.r() * u +
                                                 material.diffuse.r() * v +
                                                 material.diffuse.r() * w))};

      auto g{static_cast<uint32_t>(brightness * (material.diffuse.g() * u +
                                                 material.diffuse.g() * v +
                                                 material.diffuse.g() * w))};

      auto blue{static_cast<uint32_t>(brightness * (material.diffuse.b() * u +
                                                    material.diffuse.b() * v +
                                                    material.diffuse.b() * w))};
      Color color{0xFF, r, g, blue};
      point(buffer, p, color);
    }
  }
}
