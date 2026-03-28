/// @file rasterizer.hpp
/// @brief declaration of rasterizer functions
/// @author Abhay Manoj
/// @date Mar 24 2026
#pragma once

#include "frame_buffer.hpp"

/// @brief vector of size 2
struct Vec2 {
  float x;
  float y;
};

/// @brief vector of size 3
struct Vec3 {
  float x;
  float y;
  float z;
};

/// @brief vertex with position data and various fields
struct Vertex {
  Vec3 position;
  Vec3 normal;
  Vec2 uv;
  uint32_t color;
};

/// @brief
struct Texture {
  size_t width;
  size_t height;
  std::vector<uint32_t> pixels;
};

/// @brief draws the given point with the color, only if z depth is closest
/// @param buffer the buffer to draw in
/// @param v the vertex to draw
void draw_point(FrameBuffer &buffer, Vertex v);

/// @brief draws the outline of the triangle with the color
/// @param buffer the buffer to draw in
/// @param v0 the first corner of the triangle
/// @param v1 the second corner of the triangle
/// @param v2 the third corner of the triangle
/// @param texture
void draw_triangle(FrameBuffer &buffer, Vertex v0, Vertex v1, Vertex v2,
                   Texture &texture);
