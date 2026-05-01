#pragma once

#include "frame_buffer.hpp"
#include "math.hpp"

struct Vertex {
  Vec4 pos{};
  Color color{};
};

void point(FrameBuffer &buffer, Vertex p);

void triangle(FrameBuffer &buffer, Vertex a, Vertex b, Vertex c);
