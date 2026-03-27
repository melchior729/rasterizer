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

void render_test_pattern(FrameBuffer &buffer) {
  // Initialize buffer with a clear color and depth buffer with 1.0 (far plane)
  // Ensure your FrameBuffer initialization handles this.

  // Triangle 1: Blue (Further away)
  Vertex vBlue0 = {{100.0f, 100.0f, 0.9f}, {0}, {0}, 0xFF0000FF};
  Vertex vBlue1 = {{400.0f, 100.0f, 0.9f}, {0}, {0}, 0xFF0000FF};
  Vertex vBlue2 = {{250.0f, 400.0f, 0.9f}, {0}, {0}, 0xFF0000FF};

  // Triangle 2: Red (Closer)
  Vertex vRed0 = {{150.0f, 150.0f, 0.1f}, {0}, {0}, 0xFFFF0000};
  Vertex vRed1 = {{450.0f, 150.0f, 0.1f}, {0}, {0}, 0xFFFF0000};
  Vertex vRed2 = {{300.0f, 450.0f, 0.1f}, {0}, {0}, 0xFFFF0000};

  // Draw further triangle first
  draw_triangle(buffer, vBlue0, vBlue1, vBlue2);

  // Draw closer triangle second
  draw_triangle(buffer, vRed0, vRed1, vRed2);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto *state = new AppState();
  state->buffer = std::make_unique<FrameBuffer>(WIDTH, HEIGHT);
  *appstate = state;

  render_test_pattern(*state->buffer);

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
