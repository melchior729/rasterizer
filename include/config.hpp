#pragma once

#include "SDL3/SDL_log.h"
#include "color.hpp"

inline constexpr int WIDTH{1920};
inline constexpr int HEIGHT{1080};

inline constexpr const Color RED{0xFF, 0xFF, 0, 0};
inline constexpr const Color GREEN{0xFF, 0, 0xFF, 0};
inline constexpr const Color BLUE{0xFF, 0, 0, 0xFF};

inline constexpr const char TITLE[]{"Abhay's Rasterizer"};
