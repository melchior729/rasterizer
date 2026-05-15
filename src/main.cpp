#define SDL_MAIN_USE_CALLBACKS 1

#include "camera.hpp"
#include "scene.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <memory>

// static Vec3 light_dir{1, 1, 1};
static Vec3 light_dir{0, 1, 0};
static constexpr float angle_increment{FOV / 4};

Vec3 rotations{0.0f, 0.0f, 0.0f};

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
  Camera camera;
  uint64_t last_time{};
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

  SDL_SetRenderVSync(state->renderer.get(), 1);
  state->last_time = SDL_GetPerformanceCounter();
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
      state->camera.pos.z -= 1;
      break;
    case SDLK_S:
      state->camera.pos.z += 1;
      break;
    case SDLK_A:
      state->camera.pos.x -= 1;
      break;
    case SDLK_D:
      state->camera.pos.x += 1;
      break;
    case SDLK_R:
      state->camera.pos.y += 1;
      break;
    case SDLK_F:
      state->camera.pos.y -= 1;
      break;
    case SDLK_J:
      state->camera.target.x -= 10;
      break;
    case SDLK_K:
      state->camera.target.x += 10;
      break;
    case SDLK_O:
      state->camera.target.y -= 10;
      break;
    case SDLK_L:
      state->camera.target.y += 10;
      break;
    case SDLK_T:
      rotations.x += 0.1f;
      break;
    case SDLK_Y:
      rotations.y += 0.1f;
      break;
    case SDLK_U:
      rotations.z += 0.1f;
      break;
    case SDLK_N:
      float old_x{light_dir.x};
      light_dir.x = light_dir.x * std::cos(angle_increment) -
                    light_dir.y * std::sin(angle_increment);
      light_dir.y = old_x * std::sin(angle_increment) +
                    light_dir.y * std::cos(angle_increment);
      light_dir = norm(light_dir);
      break;
    }
  }

  return SDL_APP_CONTINUE;
}

static void draw_overlay(AppState *state, int faces) {

  uint64_t now{SDL_GetPerformanceCounter()};
  double frame_ms = (double)(now - state->last_time) * 1000 /
                    static_cast<double>(SDL_GetPerformanceFrequency());
  state->last_time = now;
  char line[64];
  SDL_snprintf(line, sizeof(line), "%.2f ms | %.0f fps | %d faces drawn",
               frame_ms, 1000.0 / frame_ms, faces);
  SDL_SetRenderDrawColor(state->renderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_SetRenderScale(state->renderer.get(), 2.0f, 2.0f);
  SDL_RenderDebugText(state->renderer.get(), 4.0f, 4.0f, line);
  SDL_SetRenderScale(state->renderer.get(), 1.0f, 1.0f);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto *state = static_cast<AppState *>(appstate);
  auto &camera = state->camera;
  state->buffer->clear();

  int faces{draw_scene(*state->buffer, camera, light_dir, rotations)};

  SDL_UpdateTexture(state->texture.get(), nullptr,
                    state->buffer.get()->pixels.data(), WIDTH * sizeof(Color));

  SDL_RenderClear(state->renderer.get());
  SDL_RenderTexture(state->renderer.get(), state->texture.get(), nullptr,
                    nullptr);

  draw_overlay(state, faces);
  SDL_RenderPresent(state->renderer.get());

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result) {
  if (appstate != nullptr) {
    std::unique_ptr<AppState> state{static_cast<AppState *>(appstate)};
  }

  SDL_Quit();
}
