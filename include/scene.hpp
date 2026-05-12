#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"

void draw_scene(FrameBuffer &buffer, const Camera &camera,
                const Vec3 &light_dir, float x, float y, float z);
