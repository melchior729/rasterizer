// @file main.cpp
// @brief Handles the SDL3 Output
// @author Abhay Manoj
// @date Mar 23 2026

#include "frame_buffer.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <memory>
#include <string>

#define SDL_MAIN_USE_CALLBACKS 1
static constexpr std::size_t HEIGHT = 720;
static constexpr std::size_t WIDTH = 1280;
static const std::string TITLE = "Abhay's Rasterizer";

struct AppState {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  std::unique_ptr<FrameBuffer> buffer;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto *state = new AppState();
  state->buffer = std::make_unique<FrameBuffer>(WIDTH, HEIGHT);
  *appstate = state;

  // draw line
  for (std::size_t i = 0; i < HEIGHT; i++) {
    state->buffer->setPixel(i, i, 0xFF, 0xFF, 0xFF);
  }

  if (!SDL_CreateWindowAndRenderer(TITLE.c_str(), WIDTH, HEIGHT, 0,
                                   &state->window, &state->renderer)) {
    delete state;
    *appstate = nullptr;
    return SDL_APP_FAILURE;
  }

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
  const auto &buffer = state->buffer;
  for (std::size_t i = 0; i < WIDTH * HEIGHT; i++) {
    uint32_t color = buffer->pixels.at(i);
    SDL_SetRenderDrawColor(state->renderer, color >> 16, color >> 8, color,
                           color >> 24);
    SDL_RenderPoint(state->renderer, i % WIDTH, i / WIDTH);
  }
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
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  delete state;

  SDL_Quit();
}
