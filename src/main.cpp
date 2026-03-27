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

void draw_depth_test(FrameBuffer &buffer) {
  static constexpr uint32_t RED = 0xFFFF0000;
  static constexpr uint32_t GREEN = 0xFF00FF00;
  static constexpr uint32_t BLUE = 0xFF0000FF;
  static constexpr uint32_t YELLOW = 0xFFFFFF00;
  static constexpr uint32_t CYAN = 0xFF00FFFF;
  static constexpr uint32_t MAGENTA = 0xFFFF00FF;
  static constexpr uint32_t WHITE = 0xFFFFFFFF;
  static constexpr uint32_t ORANGE = 0xFFFF8800;

  // back wall — large, far away
  draw_triangle(buffer, {0.0f, 0.0f, 20.0f}, {1280.0f, 0.0f, 20.0f},
                {640.0f, 720.0f, 20.0f}, BLUE, CYAN, BLUE);

  // floor — mid distance
  draw_triangle(buffer, {0.0f, 720.0f, 15.0f}, {1280.0f, 720.0f, 15.0f},
                {640.0f, 400.0f, 15.0f}, MAGENTA, MAGENTA, WHITE);

  // left pillar — closer
  draw_triangle(buffer, {100.0f, 150.0f, 10.0f}, {250.0f, 150.0f, 10.0f},
                {175.0f, 600.0f, 10.0f}, ORANGE, YELLOW, ORANGE);

  // right pillar — same depth as left
  draw_triangle(buffer, {1030.0f, 150.0f, 10.0f}, {1180.0f, 150.0f, 10.0f},
                {1105.0f, 600.0f, 10.0f}, ORANGE, YELLOW, ORANGE);

  // center diamond — closer than pillars, drawn last to stress test order
  draw_triangle(buffer, {640.0f, 150.0f, 5.0f}, {850.0f, 400.0f, 5.0f},
                {640.0f, 650.0f, 5.0f}, RED, MAGENTA, YELLOW);

  draw_triangle(buffer, {640.0f, 150.0f, 5.0f}, {430.0f, 400.0f, 5.0f},
                {640.0f, 650.0f, 5.0f}, RED, CYAN, YELLOW);

  // foreground shards — closest, scattered, drawn first intentionally
  draw_triangle(buffer, {50.0f, 500.0f, 2.0f}, {300.0f, 600.0f, 2.0f},
                {150.0f, 720.0f, 2.0f}, WHITE, CYAN, GREEN);

  draw_triangle(buffer, {900.0f, 480.0f, 2.0f}, {1150.0f, 560.0f, 2.0f},
                {1050.0f, 720.0f, 2.0f}, WHITE, GREEN, CYAN);

  draw_triangle(buffer, {500.0f, 550.0f, 1.0f}, {640.0f, 480.0f, 1.0f},
                {780.0f, 550.0f, 1.0f}, WHITE, YELLOW, WHITE);
}

void draw_colored_triangle(FrameBuffer &buffer) {
  static constexpr uint32_t RED = 0xFFFF0000;
  static constexpr uint32_t GREEN = 0xFF00FF00;
  static constexpr uint32_t BLUE = 0xFF0000FF;

  Vec3 p0 = {300, 600};
  Vec3 p1 = {500, 300};
  Vec3 p2 = {600, 600};

  draw_triangle(buffer, p0, p1, p2, RED, GREEN, BLUE);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto *state = new AppState();
  state->buffer = std::make_unique<FrameBuffer>(WIDTH, HEIGHT);
  *appstate = state;

  draw_depth_test(*state->buffer);

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
