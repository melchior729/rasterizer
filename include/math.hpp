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

  Vec3 operator+(Vec3 v) {
    return {this->x + v.x, this->y + v.y, this->z + v.z};
  }

  Vec3 operator-(Vec3 v) {
    return {this->x - v.x, this->y - v.y, this->z - v.z};
  }
};

struct Vec4 {
  float x{};
  float y{};
  float z{};
  float w{1};
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

  Vec4 operator*(Vec3 v) const {
    return {m[0] * v.x + m[4] * v.y + m[8] * v.z,
            m[1] * v.x + m[5] * v.y + m[9] * v.z,
            m[2] * v.x + m[6] * v.y + m[10] * v.z, 1};
  }

  Vec4 operator*(Vec4 v) const {
    return {m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w,
            m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w,
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
            m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w};
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

Mat4 scale(Vec3 s);

Mat4 rot_x(float theta);

Mat4 rot_y(float theta);

Mat4 rot_z(float theta);

Mat4 project();
