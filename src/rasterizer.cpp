#include "../include/rasterizer.hpp"

void draw_point(FrameBuffer *buffer, float x, float y, Color color) {
  buffer->pixels[y * WIDTH + x] = color;
}
