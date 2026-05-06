#pragma once

#include "color.hpp"
#include "math.hpp"
#include <vector>

struct Vertex {
  Vec4 pos;
  Color color;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<std::array<int, 3>> faces;
};
