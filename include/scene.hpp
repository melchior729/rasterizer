#pragma once

#include "camera.hpp"
#include "config.hpp"
#include "frame_buffer.hpp"

struct SceneConfig {
  Vec3 light_dir;
  Vec3 rot;
  RenderMode mode;
};

int draw_scene(FrameBuffer &buffer, const Camera &camera,
               const SceneConfig &config);
