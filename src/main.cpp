#define SDL_MAIN_USE_CALLBACKS 1

#include "camera.hpp"
#include "model.hpp"
#include "scene.hpp"
#include "web_bridge.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <memory>

static Mesh venus_mesh{Mesh::load("venus.obj")};
static Mesh ship_mesh{Mesh::load("spaceship.obj")};
static Mesh skull_mesh{Mesh::load("skull.obj")};
static Mesh turtle_mesh{Mesh::load("terrorpin.obj")};
static Mesh plane_mesh{Mesh::load("plane.obj")};

static SceneObject venus{
    venus_mesh, {0.0f, 0.0f, -20.0f}, {-1.57f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
static SceneObject ship{ship_mesh};
static SceneObject skull{
    skull_mesh, {0.0f, 0.0f, -20.0f}, {-1.57f, 0.0f, 0.0f}, {0.8f, 0.8f, 0.8f}};
static SceneObject turtle{turtle_mesh};
static SceneObject plane{plane_mesh};

#ifdef __EMSCRIPTEN__
static Mesh user_mesh{};
static SceneObject user_obj{user_mesh};
static bool user_mesh_loaded{false};
#endif

static constexpr const char *mode_names[] = {"Wireframe", "Flat", "Gouraud",
                                             "Phong"};
static float angle{0.0f};

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
  SceneObject *object;
  SceneConfig config{{0.577f, 0.577f, 0.557f}, RenderMode::Phong};
  uint64_t last_time{};
};

#ifdef __EMSCRIPTEN__
static AppState *g_app_state{nullptr};
#endif

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
  state->object = &venus;

  SDL_SetRenderVSync(state->renderer.get(), 0);
  state->last_time = SDL_GetPerformanceCounter();
  *appstate = state.release();

#ifdef __emscripten__
  web_bind_app_state(*appstate);
#endif

  return SDL_APP_CONTINUE;
}

void set_render_mode(AppState *state, int mode) {
  state->config.mode = static_cast<RenderMode>(mode);
}

void apply_rotation(AppState *state, float x, float y, float z) {
  state->object->r.x += x;
  state->object->r.y += y;
  state->object->r.z += z;
  state->object->update_matrix();
}

void set_light_angle(AppState *state, float angle) {
  auto &ld{state->config.light_dir};
  ld.x = std::sin(angle);
  ld.y = 1.0f;
  ld.z = std::cos(angle);
  ld = norm(ld);
}

void draw_venus(AppState *state) { state->object = &venus; }

void draw_plane(AppState *state) { state->object = &plane; }

void draw_skull(AppState *state) { state->object = &skull; }

void draw_ship(AppState *state) { state->object = &ship; }

void draw_turtle(AppState *state) { state->object = &turtle; }

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  auto *state = static_cast<AppState *>(appstate);
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    if (event->motion.state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
      auto sens{0.005f};
      apply_rotation(state, event->motion.yrel * sens,
                     event->motion.xrel * sens, 0.0f);
    }
  }

  if (event->type == SDL_EVENT_MOUSE_WHEEL) {
    state->camera.pos.z -= event->wheel.y * 0.5f;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.key) {
    case SDLK_F1:
      set_render_mode(state, 0);
      break;
    case SDLK_F2:
      set_render_mode(state, 1);
      break;
    case SDLK_F3:
      set_render_mode(state, 2);
      break;
    case SDLK_F4:
      set_render_mode(state, 3);
      break;
    case SDLK_W:
      state->camera.pos.z -= 0.2;
      break;
    case SDLK_S:
      state->camera.pos.z += 0.2;
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
    case SDLK_1:
      draw_venus(state);
      break;
    case SDLK_2:
      draw_ship(state);
      break;
    case SDLK_3:
      draw_skull(state);
      break;
    case SDLK_4:
      draw_turtle(state);
      break;
    case SDLK_5:
      draw_plane(state);
      break;
    case SDLK_N:
      angle += 0.1f;
      set_light_angle(state, angle);
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
  const char *mode_str = mode_names[static_cast<int>(state->config.mode)];

  SDL_snprintf(line, sizeof(line),
               "%.2f ms | %.0f fps | %d faces drawn | Method: %s", frame_ms,
               1000.0 / frame_ms, faces, mode_str);
  SDL_SetRenderDrawColor(state->renderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
#ifdef __EMSCRIPTEN__
  constexpr float overlay_scale{1.0f};
#else
  constexpr float overlay_scale{2.0f};
#endif
  SDL_SetRenderScale(state->renderer.get(), overlay_scale, overlay_scale);
  SDL_RenderDebugText(state->renderer.get(), 4.0f, 4.0f, line);
  SDL_SetRenderScale(state->renderer.get(), 1.0f, 1.0f);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto *state = static_cast<AppState *>(appstate);
  auto &camera = state->camera;
  state->buffer->clear();

  int faces{draw_scene(*state->buffer, *state->object, camera, state->config)};

  SDL_UpdateTexture(state->texture.get(), nullptr,
                    state->buffer.get()->pixels.data(), WIDTH * sizeof(Color));

  SDL_RenderClear(state->renderer.get());
#ifdef __EMSCRIPTEN__
  SDL_SetTextureScaleMode(state->texture.get(), SDL_SCALEMODE_LINEAR);
#else
  SDL_SetTextureScaleMode(state->texture.get(), SDL_SCALEMODE_NEAREST);
#endif

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

#ifdef __EMSCRIPTEN__

void web_bind_app_state(void *appstate) {
  g_app_state = static_cast<AppState *>(appstate);
}

static void select_builtin_model(AppState *state, int id) {
  switch (id) {
  case 0:
    state->object = &venus;
    break;
  case 1:
    state->object = &ship;
    break;
  case 2:
    state->object = &skull;
    break;
  case 3:
    state->object = &turtle;
    break;
  case 4:
    state->object = &plane;
    break;
  default:
    state->object = &venus;
    break;
  }
}

void web_set_model(int id) {
  if (!g_app_state) {
    return;
  }

  constexpr int user_model_id{5};
  if (id == user_model_id) {
    g_app_state->object = user_mesh_loaded ? &user_obj : &venus;
    return;
  }

  select_builtin_model(g_app_state, id);
}

void web_set_render_mode(int mode) {
  if (!g_app_state) {
    return;
  }
  set_render_mode(g_app_state, mode);
}

void web_set_light_angle(float light_angle) {
  if (!g_app_state) {
    return;
  }
  angle = light_angle;
  set_light_angle(g_app_state, light_angle);
}

void web_set_day_mode(int is_day) {
  if (!g_app_state) {
    return;
  }
  g_app_state->buffer->set_clear_color(is_day ? BG_DAY : BG);
  g_app_state->buffer->clear();
}

int web_has_user_mesh() { return user_mesh_loaded ? 1 : 0; }

void web_reload_user_mesh() {
  // when loading user models from memfs
}

#endif
