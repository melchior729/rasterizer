#include "rasterizer.hpp"

static size_t get_index_from_x_y(int x, int y, size_t width) {
  return y * width + x;
}

void drawPoint(FrameBuffer &buffer, Vec2 p, uint32_t color) {
  if (p.x < 0 || p.y < 0) {
    return;
  }

  buffer.setPixel(p.x, p.y, color);
}
