/// @file rasterizer.cpp
/// @brief implementation of rasterizer functions
/// @author Abhay Manoj
/// @date Mar 24 2026

#include "rasterizer.hpp"
#include <cstdlib>

void draw_point(FrameBuffer &buffer, Vec2 p, uint32_t color) {
  if (p.x < 0 || p.y < 0) {
    return;
  }

  buffer.setPixel(p.x, p.y, color);
}

void draw_line(FrameBuffer &buffer, Vec2 p0, Vec2 p1, uint32_t color) {
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

  int x = p0.x, y = p0.y;
  for (int i = 0; i <= fast; i++) {
    draw_point(buffer, {x, y}, color);
    if (error > 0) {
      steep ? x += sx : y += sy;
      error -= 2 * fast;
    }
    error += 2 * slow;
    steep ? y += sy : x += sx;
  }
}

void draw_triangle(FrameBuffer &buffer, Vec2 p0, Vec2 p1, Vec2 p2,
                   uint32_t color) {
  draw_line(buffer, p0, p1, color);
  draw_line(buffer, p0, p2, color);
  draw_line(buffer, p1, p2, color);
}

void draw_filled_triangle(FrameBuffer *buffer, Vec2 p0, Vec2 p1, Vec2 p2,
                          uint32_t color) {}
