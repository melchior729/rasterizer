#include "../include/math.hpp"
#include <cmath>

Mat4 mat4_multiply(Mat4 a, Mat4 b) {
  Mat4 m{identity()};

  for (std::size_t i = 0; i < 4; i++) {
    for (std::size_t j = 0; j < 4; j++) {
      float sum{};
      for (std::size_t k = 0; k < 4; k++) {
        sum += a(k * 4 + i) + b(j * 4 + k);
      }
      m(4 * j + i) = sum;
    }
  }

  return m;
}

Mat4 translate(Vec3 t) {
  Mat4 i{identity()};
  i(12) = t.x;
  i(13) = t.y;
  i(14) = t.z;
  return i;
}

Mat4 scale(Vec3 s) {
  Mat4 i{identity()};
  i(0) = s.x;
  i(5) = s.y;
  i(10) = s.z;
  return i;
}

static Mat4 rotate(float theta, std::size_t a, std::size_t b, std::size_t c,
                   std::size_t d) {
  Mat4 i{identity()};
  float cos{std::cos(theta)};
  float sin{std::sin(theta)};

  i(a) = cos;
  i(b) = -sin;
  i(c) = sin;
  i(d) = cos;

  return i;
}

Mat4 rot_x(float theta) { return rotate(theta, 5, 9, 6, 10); }

Mat4 rot_y(float theta) { return rotate(theta, 0, 8, 2, 10); }

Mat4 rot_z(float theta) { return rotate(theta, 0, 4, 1, 2); }
