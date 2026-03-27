/// @file rasterizer.cpp
/// @brief implementation of rasterizer functions
/// @author Abhay Manoj
/// @date Mar 24 2026

#include "rasterizer.hpp"
#include <algorithm>
#include <cmath>

void draw_point(FrameBuffer &buffer, Vertex v) {
  float x = v.position.x, y = v.position.y;
  if (x < 0 || x >= buffer.width || y < 0 || y >= buffer.height) {
    return;
  }

  int i = (int)y * buffer.width + (int)x;
  if (v.position.z > buffer.depth_buffer[i]) {
    return;
  }

  buffer.depth_buffer[i] = v.position.z;
  buffer.set_pixel(x, y, v.color);
}

static float edge_function(Vec3 p0, Vec3 p1, Vec3 p2) {
  return (double)(p2.y * (p1.x - p0.x) - p0.y * (p1.x - p2.x) +
                  p1.y * (p0.x - p2.x));
}

static uint32_t blend_channel(double u, double v, double w, uint32_t c0,
                              uint32_t c1, uint32_t c2, int shift) {
  return std::round((double)((c0 >> shift) & 0xFF) * u +
                    (double)((c1 >> shift) & 0xFF) * v +
                    (double)((c2 >> shift) & 0xFF) * w);
}

void draw_triangle(FrameBuffer &buffer, Vertex v0, Vertex v1, Vertex v2) {
  const float det = edge_function(v0.position, v1.position, v2.position);
  if (std::abs(det) < 1e-7) {
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
          edge_function({(float)x, (float)y, 0}, v1.position, v2.position) *
          inv_det;
      const float v =
          edge_function(v0.position, {(float)x, (float)y, 0}, v2.position) *
          inv_det;
      const float w = 1.0 - u - v;
      if (u < 0 || v < 0 || w < 0) {
        continue;
      }

      const float z = u * v0.position.z + v * v1.position.z + w * v2.position.z;
      const Vec2 uv = {(float)(v0.uv.x * u + v1.uv.x * v + v2.uv.x * w),
                       (float)(v0.uv.y * u + v1.uv.y * v + v2.uv.y * w)};

      const Vec3 normal = {
          (float)(v0.normal.x * u + v1.normal.x * v + v2.normal.x * w),
          (float)(v0.normal.y * u + v1.normal.y * v + v2.normal.y * w),
          (float)(v0.normal.z * u + v1.normal.z * v + v2.normal.z * w)};
      auto red = blend_channel(u, v, w, v0.color, v1.color, v2.color, 16);
      auto green = blend_channel(u, v, w, v0.color, v1.color, v2.color, 8);
      auto blue = blend_channel(u, v, w, v0.color, v1.color, v2.color, 0);
      uint32_t color = (0xFF << 24) | (red << 16) | (green << 8) | (blue);
      Vertex vt = {{(float)x, (float)y, (float)z}, normal, uv, color};
      draw_point(buffer, vt);
    }
  }
}
