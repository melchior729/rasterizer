#pragma once

#include "frame_buffer.hpp"
#include "math.hpp"

void draw_point(FrameBuffer &buffer, Vec3 p, Color color);

void draw_triangle(FrameBuffer &buffer, Vec3 a, Vec3 b, Vec3 c, Color color);
