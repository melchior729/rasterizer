/// @file rasterizer.cpp
/// @brief implementation of rasterizer functions
/// @author Abhay Manoj
/// @date Mar 24 2026

#include "rasterizer.hpp"
#include <algorithm>
#include <cmath>

void draw_point(FrameBuffer &buffer, Vec3 p, uint32_t color) {
  if (p.x < 0 || p.x >= buffer.width || p.y < 0 || p.y >= buffer.height) {
    return;
  }

  int i = (int)p.y * buffer.width + p.x;
  if (p.z > buffer.depth_buffer[i]) {
    return;
  }

  buffer.depth_buffer[i] = p.z;
  buffer.set_pixel(p.x, p.y, color);
}

void draw_line(FrameBuffer &buffer, Vec3 p0, Vec3 p1, uint32_t color) {
  int dx = p1.x - p0.x;
  int dy = p1.y - p0.y;

  const int sx = dx > 0 ? 1 : -1;
  const int sy = dy > 0 ? 1 : -1;
  dx = std::abs(dx);
  dy = std::abs(dy);

  const bool steep = dy > dx;
  const int fast = steep ? dy : dx;
  const int slow = steep ? dx : dy;
  int error = 2 * slow - fast;

  float x = p0.x, y = p0.y;
  for (int i = 0; i <= fast; i++) {
    draw_point(buffer, {x, y, 0}, color);
    if (error > 0) {
      steep ? x += sx : y += sy;
      error -= 2 * fast;
    }
    error += 2 * slow;
    steep ? y += sy : x += sx;
  }
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

void draw_triangle(FrameBuffer &buffer, Vec3 p0, Vec3 p1, Vec3 p2, uint32_t c0,
                   uint32_t c1, uint32_t c2) {
  const float det = edge_function(p0, p1, p2);
  if (std::abs(det) < 1e-7) {
    return;
  }

  const float inv_det = 1.0 / det;
  const int min_x = static_cast<int>(std::min({p0.x, p1.x, p2.x}));
  const int min_y = static_cast<int>(std::min({p0.y, p1.y, p2.y}));
  const int max_x = static_cast<int>(std::max({p0.x, p1.x, p2.x}));
  const int max_y = static_cast<int>(std::max({p0.y, p1.y, p2.y}));

  for (int y = min_y; y <= max_y; y++) {
    for (int x = min_x; x <= max_x; x++) {
      const float u = edge_function({(float)(x), (float)(y)}, p1, p2) * inv_det;
      const float v = edge_function(p0, {(float)(x), (float)(y)}, p2) * inv_det;
      const float w = 1.0 - u - v;
      if (u < 0 || v < 0 || w < 0) {
        continue;
      }

      const float z = u * p0.z + v * p1.z + w * p2.z;
      auto red = blend_channel(u, v, w, c0, c1, c2, 16);
      auto green = blend_channel(u, v, w, c0, c1, c2, 8);
      auto blue = blend_channel(u, v, w, c0, c1, c2, 0);
      uint32_t color = (0xFF << 24) | (red << 16) | (green << 8) | (blue);
      draw_point(buffer, {(float)(x), (float)(y), z}, color);
    }
  }
}

void draw_flat_triangle(FrameBuffer &buffer, Vec3 p0, Vec3 p1, Vec3 p2,
                        uint32_t color) {
  draw_triangle(buffer, p0, p1, p2, color, color, color);
}

void draw_triangle_outline(FrameBuffer &buffer, Vec3 p0, Vec3 p1, Vec3 p2,
                           uint32_t color) {
  draw_line(buffer, p0, p1, color);
  draw_line(buffer, p0, p2, color);
  draw_line(buffer, p1, p2, color);
}
