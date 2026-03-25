/// @file rasterizer.hpp
/// @brief declaration of rasterizer functions
/// @author Abhay Manoj
/// @date Mar 24 2026
#pragma once

#include "frame_buffer.hpp"

/// @brief contains a x coordinate and y coordinate
struct Vec2 {
  int x;
  int y;
};

/// @brief draws the given point with the color
/// @param buffer the buffer to draw in
/// @param p the point containg x, y
/// @color the color to paint the point
void draw_point(FrameBuffer &buffer, Vec2 p, uint32_t color);

/// @brief draws the given line with the color
/// @param buffer the buffer to draw in
/// @param p0 the start of the line
/// @param p1 the end of the line
/// @color the color to paint the line
void draw_line(FrameBuffer &buffer, Vec2 p0, Vec2 p1, uint32_t color);

/// @brief draws the outline of the triangle with the color
/// @param buffer the buffer to draw in
/// @param p0 the first corner of the triangle
/// @param p1 the second corner of the triangle
/// @param p2 the third corner of the triangle
/// @color the color to paint the point
void draw_triangle(FrameBuffer &buffer, Vec2 p0, Vec2 p1, Vec2 p2,
                   uint32_t color);

/// @brief draws the outline of the triangle with the color
/// @param buffer the buffer to draw in
/// @param p0 the first corner of the triangle
/// @param p1 the second corner of the triangle
/// @param p2 the third corner of the triangle
/// @color the color to paint the point
void draw_filled_triangle(FrameBuffer *buffer, Vec2 p0, Vec2 p1, Vec2 p2,
                          uint32_t color);
