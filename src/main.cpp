// @file main.cpp
// @brief Handles the SDL3 Output
// @author Abhay Manoj
// @date Mar 23 2026

#include "frame_buffer.hpp"
#include "rasterizer.hpp"
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

void draw_attribute_test(FrameBuffer &buffer) {
  Vertex v0 = {
      .position = {300.0f, 600.0f, 1.0f},
      .normal = {-1.0f, 0.0f, 1.0f},
      .uv = {0.0f, 1.0f},
      .color = 0xFFFF0000 // red
  };
  Vertex v1 = {
      .position = {500.0f, 300.0f, 1.0f},
      .normal = {0.0f, 1.0f, 1.0f},
      .uv = {0.5f, 0.0f},
      .color = 0xFF00FF00 // green
  };
  Vertex v2 = {
      .position = {700.0f, 600.0f, 1.0f},
      .normal = {1.0f, 0.0f, 1.0f},
      .uv = {1.0f, 1.0f},
      .color = 0xFF0000FF // blue
  };
  draw_triangle(buffer, v0, v1, v2);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto *state = new AppState();
  state->buffer = std::make_unique<FrameBuffer>(WIDTH, HEIGHT);
  *appstate = state;

  draw_attribute_test(*state->buffer);

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
