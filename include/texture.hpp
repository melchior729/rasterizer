#include "color.hpp"
#include <cassert>
#include <string>
#include <vector>

struct Texture {
  std::vector<Color> pixels{};
  int width{};
  int height{};

  Color sample(float u, float v);

  static Texture load(const std::string &path);
};
