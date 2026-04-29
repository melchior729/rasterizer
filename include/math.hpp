#pragma once

#include <array>
#include <cassert>

struct Vec2 {
  float x{};
  float y{};
};

struct Vec3 {
  float x{};
  float y{};
  float z{};
};

struct Mat4 {
  std::array<float, 16> m{};

  float &operator()(std::size_t i) {
    assert(i < 16);
    return m[i];
  }

  float &operator()(std::size_t i, std::size_t j) {
    assert(i < 4 && j < 4);
    return m[i * 4 + j];
  }

  Mat4 operator*(Mat4 other) const;

  Vec3 operator*(Vec3 v) const {
    // TODO wrap in 4d vector
    return {
        m[0] * v.x + m[4] * v.y + m[8] * v.z,
        m[1] * v.x + m[5] * v.y + m[9] * v.z,
        m[2] * v.x + m[6] * v.y + m[10] * v.z,
    };
  }
};

constexpr Mat4 identity() {
  return {{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}};
}

Mat4 mat4_multiply(Mat4 a, Mat4 b);

inline Mat4 Mat4::operator*(Mat4 other) const {
  return mat4_multiply(*this, other);
}

Mat4 translate(Vec3 t);

Mat4 scale(Vec3 t);

Mat4 rot_x(Vec3 t);

Mat4 rot_y(Vec3 t);

Mat4 rot_z(Vec3 t);
