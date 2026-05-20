#pragma once

#include "camera.hpp"
#include "config.hpp"
#include "frame_buffer.hpp"
#include "model.hpp"

struct SceneConfig {
  Vec3 light_dir;
  RenderMode mode;
};

int draw_scene(FrameBuffer &buffer, SceneObject &o, const Camera &camera,
               const SceneConfig &config);
