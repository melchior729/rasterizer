#pragma once

#include "math.hpp"

struct Camera {
  Vec4 pos;
  Vec4 target;
  Vec3 up{0, 1, 0};

  // Mat4 view() {
  //     Vec4 f{target - pos};
  //
  // }
};
