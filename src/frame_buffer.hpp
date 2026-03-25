/// @file frame_buffer.hpp
/// @brief declaration of the FrameBuffer class
/// @author Abhay Manoj
/// @date Mar 24 2026
#pragma once

#include <cstdint>
#include <vector>

/// @brief contains the data for pixels on the screen
struct FrameBuffer {
public:
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
  void set_pixel(int x, int y, uint32_t color) {
    if (is_out_of_bounds(x, y)) {
      return;
    }

    pixels[y * width + x] = color;
  }

  /// @brief verifies that the pixel at x, y, is the color
  /// @param x the x coordinate
  /// @param y the y coordinate, top left is 0
  /// @param color the color to set the pixel
  /// @return if it colored the provided color
  bool is_colored(int x, int y, uint32_t color) {
    if (is_out_of_bounds(x, y)) {
      return false;
    }

    return pixels[y * width + x] == color;
  }

  /// @brief fills the display buffer with the color, default black
  /// @param color the color to fill the buffer with
  void fill(uint32_t color = 0xFF000000) {
    std::fill(pixels.begin(), pixels.end(), color);
  }

private:
  bool is_out_of_bounds(int x, int y) {
    return (x < 0 || x >= (int)width || y < 0 || y >= (int)height);
  }
};
