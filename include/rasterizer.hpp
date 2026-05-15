#pragma once

#include "frame_buffer.hpp"
#include "model.hpp"

void point(FrameBuffer &buffer, const Vertex &p, const Color color);

void triangle(FrameBuffer &buffer, const Vertex &a, const Vertex &b,
              const Vertex &c, const Material material, const Vec3 light_dir);
