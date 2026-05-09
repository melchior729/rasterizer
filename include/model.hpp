#pragma once

#include "color.hpp"
#include "math.hpp"
#include <string>
#include <vector>

using Face = std::array<std::size_t, 3>;

struct Vertex {
  Vec4 pos;
  Vec3 normal;
  Vec2 uv;
  Color color;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<Face> faces;

  static Mesh load(const std::string &name);
};
