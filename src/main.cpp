#define SDL_MAIN_USE_CALLBACKS 1

#include "model.hpp"
#include "scene.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <memory>

struct SDL_Deleter {
  void operator()(SDL_Window *w) const { SDL_DestroyWindow(w); }
  void operator()(SDL_Renderer *r) const { SDL_DestroyRenderer(r); }
  void operator()(SDL_Texture *t) const { SDL_DestroyTexture(t); }
};

struct AppState {
  std::unique_ptr<SDL_Window, SDL_Deleter> window{};
  std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer{};
  std::unique_ptr<SDL_Texture, SDL_Deleter> texture{};
  std::unique_ptr<FrameBuffer> buffer{};
  std::unique_ptr<Camera> camera{};
};

SDL_AppResult SDL_AppInit(void **appstate, [[maybe_unused]] int argc,
                          [[maybe_unused]] char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto state = std::make_unique<AppState>();

  SDL_Window *raw_window{};
  SDL_Renderer *raw_renderer{};

  if (!SDL_CreateWindowAndRenderer(TITLE, WIDTH, HEIGHT, 0, &raw_window,
                                   &raw_renderer)) {
    return SDL_APP_FAILURE;
  }

  SDL_Texture *raw_texture =
      SDL_CreateTexture(raw_renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  state->window.reset(raw_window);
  state->renderer.reset(raw_renderer);
  state->texture.reset(raw_texture);
  state->buffer = std::make_unique<FrameBuffer>();
  state->camera = std::make_unique<Camera>();

  SDL_SetRenderVSync(state->renderer.get(), 1);
  *appstate = state.release();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  auto *state = static_cast<AppState *>(appstate);
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.key) {
    case SDLK_W:
      state->camera->pos.z -= 1;
      break;
    case SDLK_S:
      state->camera->pos.z += 1;
      break;
    case SDLK_A:
      state->camera->pos.x -= 1;
      break;
    case SDLK_D:
      state->camera->pos.x += 1;
      break;
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto *state = static_cast<AppState *>(appstate);
  auto &camera = *state->camera;
  state->buffer->clear();

  draw_scene(*state->buffer, camera);

  SDL_UpdateTexture(state->texture.get(), nullptr,
                    state->buffer.get()->pixels.data(), WIDTH * sizeof(Color));

  SDL_RenderClear(state->renderer.get());
  SDL_RenderTexture(state->renderer.get(), state->texture.get(), nullptr,
                    nullptr);
  SDL_RenderPresent(state->renderer.get());

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result) {
  if (appstate != nullptr) {
    std::unique_ptr<AppState> state{static_cast<AppState *>(appstate)};
  }

  SDL_Quit();
}
