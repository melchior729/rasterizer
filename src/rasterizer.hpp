#pragma once

#include "frame_buffer.hpp"

struct Vec2 {
  int x;
  int y;
};

void draw_point(FrameBuffer &buffer, Vec2 p, uint32_t color);

void draw_line(FrameBuffer &buffer, Vec2 p0, Vec2 p1, uint32_t color);

void draw_triangle(FrameBuffer &buffer, Vec2 p0, Vec2 p1, Vec2 p2,
                   uint32_t color);
