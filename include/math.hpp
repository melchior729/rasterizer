#pragma once

#include <array>
#include <cassert>
#include <cmath>

struct Vec2 {
  float x{};
  float y{};
};

struct Vec3 {
  float x{};
  float y{};
  float z{};

  Vec3 operator+(const Vec3 &v) const { return {x + v.x, y + v.y, z + v.z}; }

  Vec3 operator-(const Vec3 &v) const { return {x - v.x, y - v.y, z - v.z}; }

  Vec3 cross(const Vec3 &v) const {
    return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
  }

  float dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

  float len() const { return std::sqrt(x * x + y * y + z * z); }
};

inline Vec3 norm(const Vec3 &v) {
  float len = v.len();
  return {v.x / len, v.y / len, v.z / len};
}

struct Vec4 {
  float x{};
  float y{};
  float z{};
  float w{1};

  Vec3 xyz() const { return {x, y, z}; }

  Vec3 sub_xyz(const Vec4 &v) const { return {x - v.x, y - v.y, z - v.z}; }
};

struct Mat4 {
  std::array<float, 16> m{};

  float &operator()(std::size_t i) {
    assert(i < 16);
    return m[i];
  }

  float &operator()(std::size_t i, std::size_t j) {
    assert(i < 4 && j < 4);
    return m[j * 4 + i];
  }

  const float &operator()(std::size_t i, std::size_t j) const {
    assert(i < 4 && j < 4);
    return m[j * 4 + i];
  }

  Mat4 operator*(const Mat4 &other) const;

  Vec4 operator*(const Vec3 &v) const {
    return {m[0] * v.x + m[4] * v.y + m[8] * v.z,
            m[1] * v.x + m[5] * v.y + m[9] * v.z,
            m[2] * v.x + m[6] * v.y + m[10] * v.z, 1};
  }

  Vec4 operator*(const Vec4 &v) const {
    return {m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w,
            m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w,
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
            m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w};
  }
};

constexpr Mat4 identity() {
  return {{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}};
}

Mat4 translate(const Vec3 &t);

Mat4 scale(const Vec3 &s);

Mat4 rot_x(float theta);

Mat4 rot_y(float theta);

Mat4 rot_z(float theta);

Mat4 project();
