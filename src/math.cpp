#include "math.hpp"
#include "config.hpp"
#include <cmath>

Mat4 Mat4::operator*(const Mat4 &other) const {
  constexpr size_t WIDTH{4};
  Mat4 m{identity()};

  for (std::size_t i{0}; i < WIDTH; i++) {
    for (std::size_t j{0}; j < WIDTH; j++) {
      float sum{};
      for (std::size_t k{0}; k < WIDTH; k++) {
        sum += (*this)(i, k) * other(k, j);
      }
      m(i, j) = sum;
    }
  }

  return m;
}

Mat4 translate(const Vec3 &t) {
  Mat4 i{identity()};
  i(12) = t.x;
  i(13) = t.y;
  i(14) = t.z;
  return i;
}

Mat4 scale(const Vec3 &s) {
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

Mat4 rot_y(float theta) { return rotate(theta, 0, 2, 8, 10); }

Mat4 rot_z(float theta) { return rotate(theta, 0, 4, 1, 5); }

Mat4 project() {
  Mat4 i{identity()};
  i(0) = 1.0f / (ASPECT * std::tan(FOV / 2.0f));
  i(5) = 1.0f / (std::tan(FOV / 2.0f));
  i(10) = -(FAR + NEAR) / (FAR - NEAR);
  i(11) = -1;
  i(14) = -(2 * NEAR * FAR) / (FAR - NEAR);
  i(15) = 0;
  return i;
}
