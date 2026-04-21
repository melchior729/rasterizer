#include "config.hpp"
#include <array>
#include <cstdint>

struct FrameBuffer {
  std::array<uint32_t, WIDTH * HEIGHT> pixels;
};
