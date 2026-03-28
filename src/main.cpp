// @file main.cpp
// @brief Handles the SDL3 Output
// @author Abhay Manoj
// @date Mar 23 2026

#define STB_IMAGE_IMPLEMENTATION

#include "frame_buffer.hpp"
#include "rasterizer.hpp"
#include "stb_image.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <memory>
#include <string>

#define SDL_MAIN_USE_CALLBACKS 1
static constexpr int WIDTH = 1280;
static constexpr int HEIGHT = 720;
static const std::string TITLE = "Abhay's Rasterizer";

/// @brief contains the required state of the application
struct AppState {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *texture = nullptr;
  std::unique_ptr<FrameBuffer> buffer;
};

void draw_texture_test(FrameBuffer &buffer) {
  int width, height, channels;
  unsigned char *data = stbi_load("texture.png", &width, &height, &channels, 4);
  if (data == nullptr) {
    return;
  }

  std::vector<uint32_t> pixels(width * height);
  for (int i = 0; i < width * height; i++) {
    unsigned char r = data[i * 4 + 0];
    unsigned char g = data[i * 4 + 1];
    unsigned char b = data[i * 4 + 2];
    pixels[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
  }
  stbi_image_free(data);
  Texture tex = {static_cast<size_t>(width), static_cast<size_t>(height),
                 pixels};
  Vertex v0 = {
      .position = {640.0f, 100.0f, 1.0f}, .normal = {}, .uv = {0.5f, 0.0f}};
  Vertex v1 = {
      .position = {150.0f, 650.0f, 1.0f}, .normal = {}, .uv = {0.0f, 1.0f}};
  Vertex v2 = {
      .position = {1130.0f, 650.0f, 1.0f}, .normal = {}, .uv = {1.0f, 1.0f}};

  draw_triangle(buffer, v0, v1, v2, tex);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto *state = new AppState();
  state->buffer = std::make_unique<FrameBuffer>(WIDTH, HEIGHT);
  *appstate = state;

  draw_texture_test(*state->buffer);
  // draw_attribute_test(*state->buffer);

  if (!SDL_CreateWindowAndRenderer(TITLE.c_str(), WIDTH, HEIGHT, 0,
                                   &state->window, &state->renderer)) {
    delete state;
    *appstate = nullptr;
    return SDL_APP_FAILURE;
  }

  state->texture =
      SDL_CreateTexture(state->renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  SDL_SetRenderVSync(state->renderer, 1);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void draw_buffer(AppState *state) {
  SDL_UpdateTexture(state->texture, nullptr, state->buffer->pixel_buffer.data(),
                    WIDTH * sizeof(uint32_t));
  SDL_RenderTexture(state->renderer, state->texture, nullptr, nullptr);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto *state = static_cast<AppState *>(appstate);
  SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(state->renderer);
  draw_buffer(state);
  SDL_RenderPresent(state->renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate == nullptr) {
    SDL_Quit();
    return;
  }

  const auto *state = static_cast<AppState *>(appstate);
  SDL_DestroyTexture(state->texture);
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  delete state;

  SDL_Quit();
}
