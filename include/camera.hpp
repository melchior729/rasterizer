#pragma once

#include "math.hpp"

struct Camera {
  Vec3 pos;
  Vec3 target;
  Vec3 up{0, 1, 0};

  // Mat4 view() {
  //     Vec4 f{target - pos};
  //
  // }
};
