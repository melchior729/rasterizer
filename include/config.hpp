#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "color.hpp"
#include <cmath>
#include <numbers>

enum struct RenderMode { Wireframe, Flat, Gouraud, Phong };

inline constexpr float AMBIENT{0.2f};

inline constexpr std::size_t CHANNEL_VAL{255};
inline constexpr float CHANNEL_VAL_F{255.0f};

inline constexpr int WIDTH{1920};
inline constexpr int HEIGHT{1080};
inline constexpr float ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr float FOV{std::numbers::pi_v<float> / 2.0f};
inline constexpr float NEAR{1.0f};
inline constexpr float FAR{100.0f};

inline constexpr float LIGHT_ANGLE_MAX{2.0f * std::numbers::pi_v<float>};

inline float wrap_light_angle(float a) {
  if (a < 0.0f) {
    a = std::fmod(a, LIGHT_ANGLE_MAX);
    if (a < 0.0f) {
      a += LIGHT_ANGLE_MAX;
    }
    return a;
  }
  if (a > LIGHT_ANGLE_MAX) {
    a = std::fmod(a, LIGHT_ANGLE_MAX);
    if (a < 0.0f) {
      a += LIGHT_ANGLE_MAX;
    }
  }
  return a;
}

inline constexpr const Color RED{0xFF, 0xB8, 0x78, 0x7E};
inline constexpr const Color GREEN{0xFF, 0x70, 0xA0, 0x88};
inline constexpr const Color BLUE{0xFF, 0x72, 0x90, 0xA8};
inline constexpr const Color BLACK{0xFF, 0x13, 0x15, 0x1A};
inline constexpr const Color WHITE{0xFF, 0xC8, 0xCC, 0xD4};
inline constexpr const char TITLE[]{"Abhay's Rasterizer"};
inline constexpr const char MODEL_PATH[]{"models/"};
inline constexpr const char TEXTURE_PATH[]{"textures/"};

struct InputSens {
  float rotate{0.005f};
  float pan{0.05f};
  float zoom{0.5f};
  float light_step{0.1f};
};

inline constexpr float KEY_ROTATE_STEP{0.05f};

inline InputSens input_sens{};

inline void set_rotate_sens(float v) { input_sens.rotate = v; }

inline void set_pan_sens(float v) { input_sens.pan = v; }

inline void set_zoom_sens(float v) { input_sens.zoom = v; }

inline void set_light_step(float v) { input_sens.light_step = v; }

inline float get_rotate_sens() { return input_sens.rotate; }

inline float get_pan_sens() { return input_sens.pan; }

inline float get_zoom_sens() { return input_sens.zoom; }

inline float get_light_step() { return input_sens.light_step; }
