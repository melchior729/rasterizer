#include "color.hpp"
#include "stb_image.h"
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
