#pragma once

#include "color.hpp"
#include "math.hpp"
#include <vector>

struct Vertex {
  Vec4 pos{};
  Color color{};
};

struct Face {
  std::array<Vertex, 3> vertices{};
};

struct Mesh {
  std::vector<Vertex> vertices{};
  std::vector<Face> faces{};
};
