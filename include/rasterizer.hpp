#pragma once

#include "frame_buffer.hpp"
#include "model.hpp"

void point(FrameBuffer &buffer, const Vertex &p, const Color color);

void line(FrameBuffer &buffer, const Vertex &a, const Vertex &b);

void triangle_wireframe(FrameBuffer &buffer, const Vertex &a, const Vertex &b,
                        const Vertex &c);

void triangle(FrameBuffer &buffer, const Vertex &a, const Vertex &b,
              const Vertex &c, const Material material, const Vec3 light_dir);
