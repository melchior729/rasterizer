#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "color.hpp"
#include <numbers>

enum struct RenderMode { Wireframe, Flat, Gouraud, Phong };

inline constexpr float AMBIENT{0.2f};

inline constexpr std::size_t CHANNEL_VAL{255};
inline constexpr float CHANNEL_VAL_F{255.0f};

#ifdef __EMSCRIPTEN__
inline constexpr int WIDTH{1280};
inline constexpr int HEIGHT{720};
#else
inline constexpr int WIDTH{1920};
inline constexpr int HEIGHT{1080};
#endif
inline constexpr float ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr float FOV{std::numbers::pi_v<float> / 2.0f};
inline constexpr float NEAR{1.0f};
inline constexpr float FAR{100.0f};

inline constexpr const Color RED{0xFF, 0xFF, 0, 0};
inline constexpr const Color GREEN{0xFF, 0, 0xFF, 0};
inline constexpr const Color BLUE{0xFF, 0, 0, 0xFF};
inline constexpr const Color BLACK{0xFF, 0, 0, 0};
inline constexpr const Color WHITE{0xFF, 0xFF, 0xFF, 0xFF};
inline constexpr const Color BG{0xFF, 0x1C, 0x1C, 0x1C};
inline constexpr const Color BG_DAY{0xFF, 0xF0, 0xF4, 0xFA};

inline constexpr const char TITLE[]{"Abhay's Rasterizer"};
inline constexpr const char MODEL_PATH[]{"models/"};
inline constexpr const char TEXTURE_PATH[]{"textures/"};
