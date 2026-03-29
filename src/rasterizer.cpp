/// @file rasterizer.cpp
/// @brief implementation of rasterizer functions
/// @author Abhay Manoj
/// @date Mar 24 2026

#include "rasterizer.hpp"
#include <algorithm>
#include <cmath>

void draw_point(FrameBuffer &buffer, Vertex v) {
  int x = static_cast<int>(v.position.x);
  int y = static_cast<int>(v.position.y);
  if (x < 0 || x >= buffer.width || y < 0 || y >= buffer.height) {
    return;
  }

  int i = y * buffer.width + x;
  if (v.position.z > buffer.depth_buffer[i]) {
    return;
  }

  buffer.depth_buffer[i] = v.position.z;
  buffer.set_pixel(x, y, v.color);
}

static float edge_function(Vec3 p0, Vec3 p1, Vec3 p2) {
  return p2.y * (p1.x - p0.x) - p0.y * (p1.x - p2.x) + p1.y * (p0.x - p2.x);
}

void draw_triangle(FrameBuffer &buffer, Vertex v0, Vertex v1, Vertex v2,
                   const Texture &texture) {
  const float det = edge_function(v0.position, v1.position, v2.position);
  if (std::abs(det) < 1e-7f) {
    return;
  }

  const float inv_det = 1.0 / det;
  const int min_x =
      static_cast<int>(std::min({v0.position.x, v1.position.x, v2.position.x}));
  const int min_y =
      static_cast<int>(std::min({v0.position.y, v1.position.y, v2.position.y}));
  const int max_x =
      static_cast<int>(std::max({v0.position.x, v1.position.x, v2.position.x}));
  const int max_y =
      static_cast<int>(std::max({v0.position.y, v1.position.y, v2.position.y}));

  for (int y = min_y; y <= max_y; y++) {
    for (int x = min_x; x <= max_x; x++) {
      const float u =
          edge_function({static_cast<float>(x), static_cast<float>(y), 0.0f},
                        v1.position, v2.position) *
          inv_det;
      const float v =
          edge_function(v0.position,
                        {static_cast<float>(x), static_cast<float>(y), 0.0f},
                        v2.position) *
          inv_det;
      const float w = 1.0 - u - v;
      if (u < 0 || v < 0 || w < 0) {
        continue;
      }

      const float z = u * v0.position.z + v * v1.position.z + w * v2.position.z;
      const Vec2 uv = {v0.uv.x * u + v1.uv.x * v + v2.uv.x * w,
                       v0.uv.y * u + v1.uv.y * v + v2.uv.y * w};

      const Vec3 normal = {v0.normal.x * u + v1.normal.x * v + v2.normal.x * w,
                           v0.normal.y * u + v1.normal.y * v + v2.normal.y * w,
                           v0.normal.z * u + v1.normal.z * v + v2.normal.z * w};
      int tx = static_cast<int>(uv.x * (texture.width - 1));
      int ty = static_cast<int>(uv.y * (texture.width - 1));
      uint32_t color = texture.pixels[ty * texture.width + tx];
      Vertex vt = {static_cast<float>(x),
                   static_cast<float>(y),
                   static_cast<float>(z),
                   normal,
                   uv,
                   color};
      draw_point(buffer, vt);
    }
  }
}

static Mat4 make_identity() {
  return {{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}};
}

static Mat4 make_translation(float x, float y, float z) {
  Mat4 i = make_identity();
  i.m[12] = x;
  i.m[13] = y;
  i.m[14] = z;
  return i;
}

static Mat4 make_scale(float x, float y, float z) {
  Mat4 i = make_identity();
  i.m[0] = x;
  i.m[5] = y;
  i.m[10] = z;
  return i;
}

static Mat4 make_rotate_y(float t) {
  Mat4 i = make_identity();
  i.m[0] = std::cosf(t);
  i.m[2] = -std::sinf(t);
  i.m[8] = std::sinf(t);
  i.m[10] = std::cosf(t);
  return i;
}

Mat4 matrix_multiply(Mat4 a, Mat4 b) {
  static constexpr size_t width = 4;
  Mat4 c;
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < width; j++) {
      float sum = 0;
      for (size_t k = 0; k < width; k++) {
        sum += a.m[k * width + i] * b.m[j * width + k];
      }
      c.m[j * width + i] = sum;
    }
  }
  return c;
}
