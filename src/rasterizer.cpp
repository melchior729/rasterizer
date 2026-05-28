#include "rasterizer.hpp"
#include "algorithm"
#include <cmath>

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
      Vertex p{{static_cast<float>(i), static_cast<float>(j)}, {}, {}, {}};

      float l1{det(p.pos, b.pos, c.pos) * inv_det};
      float l2{det(a.pos, p.pos, c.pos) * inv_det};
      float l3{det(a.pos, b.pos, p.pos) * inv_det};

      if (l1 < epsilon || l2 < epsilon || l3 < epsilon) {
        continue;
      }

      float normal_x{a.normal.x * l1 + b.normal.x * l2 + c.normal.x * l3};
      float normal_y{a.normal.y * l1 + b.normal.y * l2 + c.normal.y * l3};
      float normal_z{a.normal.z * l1 + b.normal.z * l2 + c.normal.z * l3};
      Vec3 normal{normal_x, normal_y, normal_z};

      float brightness{bright_a * l1 + bright_b * l2 + bright_c * l3};
      float spec_r{}, spec_g{}, spec_b{};

      if (mode == RenderMode::Phong) {
        brightness = std::clamp(normal.dot(light_dir), ambient, 1.0f);
        float view_x{a.view_pos.x * l1 + b.view_pos.x * l2 + c.view_pos.x * l3};
        float view_y{a.view_pos.y * l1 + b.view_pos.y * l2 + c.view_pos.y * l3};
        float view_z{a.view_pos.z * l1 + b.view_pos.z * l2 + c.view_pos.z * l3};
        Vec3 view_pos{norm({-view_x, -view_y, -view_z})};

        auto H{norm(light_dir + view_pos)};
        auto spec{std::pow(std::max(normal.dot(H), 0.0f), material.shine)};

        spec_r = material.specular.r() * spec;
        spec_g = material.specular.g() * spec;
        spec_b = material.specular.b() * spec;
      }

      float r_ch{}, g_ch{}, b_ch{};
      if (!material.texture || material.texture->pixels.empty()) {
        r_ch = material.diffuse.r() / 255.0f;
        g_ch = material.diffuse.g() / 255.0f;
        b_ch = material.diffuse.b() / 255.0f;
      } else {
        auto u{a.uv.x * l1 + b.uv.x * l2 + c.uv.x * l3};
        auto v{a.uv.y * l1 + b.uv.y * l2 + c.uv.y * l3};
        auto texel{material.texture->sample(u, v)};

        r_ch = texel.r() / 255.0f;
        g_ch = texel.g() / 255.0f;
        b_ch = texel.b() / 255.0f;
      }

      auto R{std::clamp(
          static_cast<uint32_t>(ambient + brightness * r_ch * 255.0f + spec_r),
          0U, 0xFFU)};

      auto G{std::clamp(
          static_cast<uint32_t>(ambient + brightness * g_ch * 255.0f + spec_g),
          0U, 0xFFU)};
      auto B{std::clamp(
          static_cast<uint32_t>(ambient + brightness * b_ch * 255.0f + spec_b),
          0U, 0xFFU)};

      Color color{0xFF, R, G, B};
      point(buffer, p, color);
    }
  }
}
