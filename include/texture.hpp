#include "color.hpp"
#include "stb_image.h"
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

struct Texture {
  std::vector<Color> pixels{};
  int width{};
  int height{};

  static Texture load(const std::string &path) {
    Texture t;
    int channels;
    uint8_t *data = stbi_load(path.c_str(), &t.width, &t.height, &channels, 4);
    assert(data);

    auto n = static_cast<std::size_t>(t.width * t.height);
    t.pixels.resize(n);
    for (std::size_t i = 0; i < n; i++) {
      uint32_t r = data[i * 4 + 0];
      uint32_t g = data[i * 4 + 1];
      uint32_t b = data[i * 4 + 2];
      uint32_t a = data[i * 4 + 3];
      t.pixels[i] = {a, r, g, b};
    }

    stbi_image_free(data);
    return t;
  }
};
