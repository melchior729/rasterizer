#include "rasterizer.hpp"
#include "algorithm"
#include <cmath>

static constexpr float epsilon{-1e-4f};

struct Bounds {
  int min_x;
  int min_y;
  int max_x;
  int max_y;
};

void point(FrameBuffer &buffer, const Vertex &p, const Color color) {
  buffer.set(static_cast<int>(p.pos.x), static_cast<int>(p.pos.y), p.pos.z,
             color);
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

static Bounds calculate_bounds(const Vertex &a, const Vertex &b,
                               const Vertex &c) {
  return {
      std::clamp(
          static_cast<int>(std::floor(std::min({a.pos.x, b.pos.x, c.pos.x}))),
          0, WIDTH),
      std::clamp(
          static_cast<int>(std::floor(std::min({a.pos.y, b.pos.y, c.pos.y}))),
          0, HEIGHT),
      std::clamp(
          static_cast<int>(std::ceil(std::max({a.pos.x, b.pos.x, c.pos.x}))), 0,
          WIDTH),
      std::clamp(
          static_cast<int>(std::ceil(std::max({a.pos.y, b.pos.y, c.pos.y}))), 0,
          HEIGHT)};
}

static float det(const Vec2 &a, const Vec2 &b, const Vec2 &c) {
  return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
}

static bool get_barycentric(const Vec4 &p, const Vec4 &a, const Vec4 &b,
                            const Vec4 &c, const float inv_det, float &l1,
                            float &l2, float &l3) {
  l1 = det({p.x, p.y}, {b.x, b.y}, {c.x, c.y}) * inv_det;
  l2 = det({a.x, a.y}, {p.x, p.y}, {c.x, c.y}) * inv_det;
  l3 = det({a.x, a.y}, {b.x, b.y}, {p.x, p.y}) * inv_det;
  return l1 >= epsilon && l2 >= epsilon && l3 >= epsilon;
}

void triangle_wireframe(FrameBuffer &buffer, const Vertex &a, const Vertex &b,
                        const Vertex &c) {
  line(buffer, a, b);
  line(buffer, a, c);
  line(buffer, b, c);
}

static float interpolate(const float a, const float b, const float c,
                         const float l1, const float l2, const float l3) {
  return a * l1 + b * l2 + c * l3;
}

uint32_t compute_channel(float base, float spec, float brightness) {
  float shaded{AMBIENT + (brightness * base * CHANNEL_VAL_F) + spec};
  return std::clamp(static_cast<uint32_t>(shaded), 0U, 255U);
}

static Color shade_pixel(const Vertex &a, const Vertex &b, const Vertex &c,
                         const Vec3 light_dir, const float l1, const float l2,
                         const float l3, const float bright_a,
                         const float bright_b, const float bright_c,
                         const float w, const Material &material,
                         RenderMode mode) {

  Vec3 normal{norm({
      interpolate(a.normal.x, b.normal.x, c.normal.x, l1, l2, l3) * w,
      interpolate(a.normal.y, b.normal.y, c.normal.y, l1, l2, l3) * w,
      interpolate(a.normal.z, b.normal.z, c.normal.z, l1, l2, l3) * w,
  })};

  auto brightness{interpolate(bright_a, bright_b, bright_c, l1, l2, l3)};
  float spec_r{}, spec_g{}, spec_b{};

  if (mode == RenderMode::Phong) {
    Vec3 view_pos{norm({
        -interpolate(a.view_pos.x, b.view_pos.x, c.view_pos.x, l1, l2, l3),
        -interpolate(a.view_pos.y, b.view_pos.y, c.view_pos.y, l1, l2, l3),
        -interpolate(a.view_pos.z, b.view_pos.z, c.view_pos.z, l1, l2, l3),
    })};

    auto H{norm(light_dir + view_pos)};
    auto spec{std::max(normal.dot(H), 0.0f)};
    for (int i = 0; i < material.shine_log2; i++) {
      spec *= spec;
    }

    spec_r = material.specular.r() * spec;
    spec_g = material.specular.g() * spec;
    spec_b = material.specular.b() * spec;

    brightness = std::clamp(normal.dot(light_dir), AMBIENT, 1.0f);
  }

  float r_ch = material.diffuse.r();
  float g_ch = material.diffuse.g();
  float b_ch = material.diffuse.b();

  if (material.texture && !material.texture->pixels.empty()) {
    auto u{interpolate(a.uv.x, b.uv.x, c.uv.x, l1, l2, l3) * w};
    auto v{interpolate(a.uv.y, b.uv.y, c.uv.y, l1, l2, l3) * w};
    auto texel{material.texture->sample(u, v)};

    r_ch = texel.r();
    g_ch = texel.g();
    b_ch = texel.b();
  }

  auto R{compute_channel(r_ch / CHANNEL_VAL_F, spec_r, brightness)};
  auto G{compute_channel(g_ch / CHANNEL_VAL_F, spec_g, brightness)};
  auto B{compute_channel(b_ch / CHANNEL_VAL_F, spec_b, brightness)};

  return {0xFF, R, G, B};
}

void triangle(FrameBuffer &buffer, const Vertex &a, const Vertex &b,
              const Vertex &c, const Material material, const Vec3 light_dir,
              const float bright_a, const float bright_b, const float bright_c,
              RenderMode mode) {
  float det_val{
      det({a.pos.x, a.pos.y}, {b.pos.x, b.pos.y}, {c.pos.x, c.pos.y})};
  if (std::abs(det_val) < 1e-7) {
    return;
  }

  float inv_det{1.0f / det_val};
  Bounds bounds{calculate_bounds(a, b, c)};

  for (int i{bounds.min_y}; i < bounds.max_y; i++) {
    for (int j{bounds.min_x}; j < bounds.max_x; j++) {
      Vertex p{{static_cast<float>(j), static_cast<float>(i)}, {}, {}, {}};

      float l1, l2, l3;
      if (!get_barycentric(p.pos, a.pos, b.pos, c.pos, inv_det, l1, l2, l3)) {
        continue;
      }

      float z{interpolate(a.pos.z, b.pos.z, c.pos.z, l1, l2, l3)};
      p.pos.z = z;

      float w{1.0f / interpolate(1.0f / a.pos.w, 1.0f / b.pos.w, 1.0f / c.pos.w,
                                 l1, l2, l3)};

      Color color{shade_pixel(a, b, c, light_dir, l1, l2, l3, bright_a,
                              bright_b, bright_c, w, material, mode)};
      point(buffer, p, color);
    }
  }
}
