#pragma once

#include "math.hpp"

struct Camera {
  Vec3 pos;
  Vec3 target{0, 0, -1000};
  Vec3 up{0, 1, 0};

  void reset() {
    pos = {};
    target = {0, 0, -1000};
    up = {0, 1, 0};
  }

  Mat4 view() const {
    assert(target != pos);

    Mat4 i{identity()};
    Vec3 f{norm(pos - target)};
    Vec3 r{up.cross(f)};
    Vec3 u{f.cross(r)};

    float tx{-pos.dot(r)};
    float ty{-pos.dot(u)};
    float tz{-pos.dot(f)};

    i(0) = r.x;
    i(4) = r.y;
    i(8) = r.z;
    i(12) = tx;

    i(1) = u.x;
    i(5) = u.y;
    i(9) = u.z;
    i(13) = ty;

    i(2) = f.x;
    i(6) = f.y;
    i(10) = f.z;
    i(14) = tz;

    return i;
  }
};
