/// @file frame_buffer.hpp
/// @brief declaration of the FrameBuffer struct
/// @author Abhay Manoj
/// @date Mar 24 2026
#pragma once

#include <cstdint>
#include <limits>
#include <vector>

/// @brief contains the data for pixel_buffer on the screen
struct FrameBuffer {
public:
  size_t width;
  size_t height;
  std::vector<uint32_t> pixel_buffer;
  std::vector<float> depth_buffer;

  /// @brief constructs a empty FrameBuffer
  /// @param w the width of the screen
  /// @param h the height of the screen
  FrameBuffer(size_t w, size_t h)
      : width(w), height(h), pixel_buffer(width * height),
        depth_buffer(width * height, std::numeric_limits<float>::infinity()) {}

  /// @brief sets the pixel at x, y with color
  /// @param x the x coordinate
  /// @param y the y coordinate, top left is 0
  /// @param color the color to set the pixel
  void set_pixel(int x, int y, uint32_t color) {
    pixel_buffer[y * width + x] = color;
  }

  /// @brief sets the depth at the pixel, if closer to the screen
  /// @param x the x coordinate
  /// @param y the y coordinate
  /// @param z the z coordinate
  void set_depth(float x, float y, float z) {
    if (is_out_of_bounds(x, y)) {
      return;
    }

    int i = static_cast<int>(y) * width + static_cast<int>(x);
    if (z > depth_buffer[i]) {
      return;
    }

    depth_buffer[i] = z;
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

    return pixel_buffer[y * width + x] == color;
  }

  /// @brief fills the display buffer with the color, default black
  /// @param color the color to fill the buffer with
  void fill(uint32_t color = 0xFF000000) {
    std::fill(pixel_buffer.begin(), pixel_buffer.end(), color);
  }

private:
  bool is_out_of_bounds(int x, int y) {
    int w = static_cast<int>(width);
    int h = static_cast<int>(height);
    return x < 0 || x >= w || y < 0 || y >= h;
  }
};
