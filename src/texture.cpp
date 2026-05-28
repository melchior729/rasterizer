#include "texture.hpp"
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION

Color Texture::sample(float u, float v) {
  float fx = u * static_cast<float>(width - 1);
  float fy = v * static_cast<float>(height - 1);

  auto x = std::clamp(static_cast<std::size_t>(fx), std::size_t{0},
                      static_cast<std::size_t>(width - 1));
  auto y = std::clamp(static_cast<std::size_t>(fy), std::size_t{0},
                      static_cast<std::size_t>(height - 1));

  return pixels[y * static_cast<size_t>(width) + x];
}

Texture Texture::load(const std::string &path) {
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
