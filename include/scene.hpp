#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"

int draw_scene(FrameBuffer &buffer, const Camera &camera, const Vec3 light_dir,
               Vec3 rot);
