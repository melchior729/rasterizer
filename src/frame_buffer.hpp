// @file frame_buffer.hpp
// @brief declaration of the FrameBuffer class
// @author Abhay Manoj
// @date Mar 24 2026
#pragma once

#include <cstdint>
#include <vector>

// @brief contains the data for pixels on the screen
struct FrameBuffer {
  std::size_t width;
  std::size_t height;
  std::vector<uint32_t> pixels;

  // @brief constructs a empty FrameBuffer
  // @param w the width of the screen
  // @param h the height of the screen
  FrameBuffer(std::size_t w, std::size_t h)
      : width(w), height(h), pixels(width * height) {}

  void setPixel(std::size_t x, std::size_t y, std::uint8_t r, std::uint8_t g,
                std::uint8_t b) {
    pixels[y * width + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
  }

  void clear(uint32_t color = 0xFF000000) {
    std::fill(pixels.begin(), pixels.end(), color);
  }
};
