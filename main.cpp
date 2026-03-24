#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <string>

#define SDL_MAIN_USE_CALLBACKS 1
static constexpr size_t HEIGHT = 1440;
static constexpr size_t WIDTH = 2560;
static const std::string TITLE = "Abhay's Rasterizer";

struct AppState {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto *state = new AppState();
  *appstate = state;

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

SDL_AppResult SDL_AppIterate(void *appstate) {

  auto *state = static_cast<AppState *>(appstate);
  SDL_RenderClear(state->renderer);
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
