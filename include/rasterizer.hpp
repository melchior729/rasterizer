#pragma once

#include "frame_buffer.hpp"
#include "math.hpp"

struct Vertex {
  Vec3 pos{};
  Color color{};
};

void draw_point(FrameBuffer &buffer, Vertex p);

void draw_triangle(FrameBuffer &buffer, Vertex a, Vertex b, Vertex c);
