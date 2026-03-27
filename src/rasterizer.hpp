/// @file rasterizer.hpp
/// @brief declaration of rasterizer functions
/// @author Abhay Manoj
/// @date Mar 24 2026
#pragma once

#include "frame_buffer.hpp"

/// @brief contains x, y, z coordinates
struct Vec3 {
  float x;
  float y;
  float z;
};

/// @brief draws the given point with the color, only if z depth is closest
/// @param buffer the buffer to draw in
/// @param p the point to draw
/// @param color the color to paint the point
void draw_point(FrameBuffer &buffer, Vec3 p, uint32_t color);

/// @brief draws the given line with the color
/// @param buffer the buffer to draw in
/// @param p0 the start of the line
/// @param p1 the end of the line
/// @color the color to paint the line
void draw_line(FrameBuffer &buffer, Vec3 p0, Vec3 p1, uint32_t color);

/// @brief draws the outline of the triangle with the color
/// @param buffer the buffer to draw in
/// @param p0 the first corner of the triangle
/// @param p1 the second corner of the triangle
/// @param p2 the third corner of the triangle
/// @color the color to paint the point
void draw_triangle(FrameBuffer &buffer, Vec3 p0, Vec3 p1, Vec3 p2, uint32_t c0,
                   uint32_t c1, uint32_t c2);

/// @brief draws the outline of the triangle with the color
/// @param buffer the buffer to draw in
/// @param p0 the first corner of the triangle
/// @param p1 the second corner of the triangle
/// @param p2 the third corner of the triangle
/// @color the color to paint the point
void draw_triangle_outline(FrameBuffer &buffer, Vec3 p0, Vec3 p1, Vec3 p2,
                           uint32_t color);

/// @brief draws the outline of the triangle with the color
/// @param buffer the buffer to draw in
/// @param p0 the first corner of the triangle
/// @param p1 the second corner of the triangle
/// @param p2 the third corner of the triangle
/// @color the color to paint the point
void draw_flat_triangle(FrameBuffer &buffer, Vec3 p0, Vec3 p1, Vec3 p2,
                        uint32_t color);
