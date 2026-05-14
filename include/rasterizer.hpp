#pragma once

#include "frame_buffer.hpp"
#include "model.hpp"

void point(FrameBuffer &buffer, Vertex &p, Color color);

void triangle(FrameBuffer &buffer, Vertex &a, Vertex &b, Vertex &c,
              Material material);
