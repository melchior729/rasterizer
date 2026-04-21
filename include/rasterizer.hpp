#pragma once

#include "frame_buffer.hpp"
#include "math.hpp"

void draw_point(FrameBuffer &buffer, Vec2 p, Color color);

void draw_triangle(FrameBuffer &buffer, Vec2 a, Vec2 b, Vec2 c, Color color);
