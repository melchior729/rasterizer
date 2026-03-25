/// @file frame_buffer.hpp
/// @brief declaration of the FrameBuffer class
/// @author Abhay Manoj
/// @date Mar 24 2026
#pragma once

#include <cstdint>
#include <vector>

/// @brief contains the data for pixels on the screen
struct FrameBuffer {
  size_t width;
  size_t height;
  std::vector<uint32_t> pixels;

  /// @brief constructs a empty FrameBuffer
  /// @param w the width of the screen
  /// @param h the height of the screen
  FrameBuffer(size_t w, size_t h)
      : width(w), height(h), pixels(width * height) {}

  /// @brief sets the pixel at x, y with color
  /// @param x the x coordinate
  /// @param y the y coordinate, top left is 0
  /// @param color the color to set the pixel
  void setPixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= (int)width || y < 0 || y >= (int)height) {
      return;
    }

    pixels[y * width + x] = color;
  }

  /// @brief fills the display buffer with the given color
  /// @param color the color to fill the buffer with
  void fill(uint32_t color = 0xFF000000) {
    std::fill(pixels.begin(), pixels.end(), color);
  }
};
