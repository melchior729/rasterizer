#include <SDL3/SDL_log.h>
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

static constexpr const char GRID_PATH[]{"textures/grid.png"};

static float angle{0.0f};

static bool rot_lock_x{false};
static bool rot_lock_y{false};
static bool rot_lock_z{false};

struct SDL_Deleter {
  void operator()(SDL_Window *w) const { SDL_DestroyWindow(w); }
  void operator()(SDL_Renderer *r) const { SDL_DestroyRenderer(r); }
  void operator()(SDL_Texture *t) const { SDL_DestroyTexture(t); }
};

struct AppState {
  std::unique_ptr<SDL_Window, SDL_Deleter> window{};
  std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer{};
  std::unique_ptr<SDL_Texture, SDL_Deleter> texture{};
  std::unique_ptr<SDL_Texture, SDL_Deleter> grid_texture{};
  std::unique_ptr<FrameBuffer> buffer{};
  Camera camera;
  Texture grid_tex;
  SceneObject *object;
  SceneConfig config{{0.577f, 0.577f, 0.557f}, RenderMode::Phong};
  uint64_t last_time{};
  bool show_overlay{false};
  bool swap_mouse_buttons{false};
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

  state->grid_tex = Texture::load(GRID_PATH);
  SDL_Texture *raw_grid = SDL_CreateTexture(
      raw_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
      state->grid_tex.width, state->grid_tex.height);
  SDL_UpdateTexture(raw_grid, nullptr, state->grid_tex.pixels.data(),
                    static_cast<int>(state->grid_tex.width) *
                        static_cast<int>(sizeof(Color)));
  if (!raw_grid) {
    SDL_Log("Grid texture failed: %s", SDL_GetError());
  }

  state->window.reset(raw_window);
  state->renderer.reset(raw_renderer);
  state->texture.reset(raw_texture);
  state->buffer = std::make_unique<FrameBuffer>();
  state->grid_texture.reset(raw_grid);
  state->object = &venus;

  SDL_SetRenderVSync(state->renderer.get(), 0);
  state->last_time = SDL_GetPerformanceCounter();
  *appstate = state.release();

#ifdef __EMSCRIPTEN__
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

static void apply_mouse_rotation(AppState *state, float xrel, float yrel) {
  const float dx{xrel * input_sens.rotate};
  const float dy{yrel * input_sens.rotate};

  if (rot_lock_x) {
    apply_rotation(state, dy, 0.0f, 0.0f);
  } else if (rot_lock_y) {
    apply_rotation(state, 0.0f, dx, 0.0f);
  } else if (rot_lock_z) {
    apply_rotation(state, 0.0f, 0.0f, dx);
  } else {
    apply_rotation(state, dy, dx, 0.0f);
  }
}

static void apply_pan(Camera &camera, float dx, float dy) {
  camera.pos.x += dx;
  camera.pos.y += dy;
  camera.target.x += dx;
  camera.target.y += dy;
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
    const bool left_down{
        (event->motion.state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0};
    const bool right_down{
        (event->motion.state & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0};

    if (state->swap_mouse_buttons) {
      if (left_down) {
        apply_pan(state->camera, -event->motion.xrel * input_sens.pan,
                  event->motion.yrel * input_sens.pan);
      }
      if (right_down) {
        apply_mouse_rotation(state, event->motion.xrel, event->motion.yrel);
      }
    } else {
      if (left_down) {
        apply_mouse_rotation(state, event->motion.xrel, event->motion.yrel);
      }
      if (right_down) {
        apply_pan(state->camera, -event->motion.xrel * input_sens.pan,
                  event->motion.yrel * input_sens.pan);
      }
    }
  }

  if (event->type == SDL_EVENT_MOUSE_WHEEL) {
    state->camera.pos.z -= event->wheel.y * input_sens.zoom;
  }

  if (event->type == SDL_EVENT_KEY_UP) {
    switch (event->key.key) {
    case SDLK_T:
      rot_lock_x = false;
      break;
    case SDLK_Y:
      rot_lock_y = false;
      break;
    case SDLK_U:
      rot_lock_z = false;
      break;
    default:
      break;
    }
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.key) {
    case SDLK_F1:
      set_render_mode(state, 0);
#ifdef __EMSCRIPTEN__
      web_sync_mode_dropdown(0);
#endif
      break;
    case SDLK_F2:
      set_render_mode(state, 1);
#ifdef __EMSCRIPTEN__
      web_sync_mode_dropdown(1);
#endif
      break;
    case SDLK_F3:
      set_render_mode(state, 2);
#ifdef __EMSCRIPTEN__
      web_sync_mode_dropdown(2);
#endif
      break;
    case SDLK_F4:
      set_render_mode(state, 3);
#ifdef __EMSCRIPTEN__
      web_sync_mode_dropdown(3);
#endif
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
      state->camera.target.y += 10;
      break;
    case SDLK_L:
      state->camera.target.y -= 10;
      break;
    case SDLK_T:
      rot_lock_x = true;
      rot_lock_y = false;
      rot_lock_z = false;
      apply_rotation(state, KEY_ROTATE_STEP, 0.0f, 0.0f);
      break;
    case SDLK_Y:
      rot_lock_x = false;
      rot_lock_y = true;
      rot_lock_z = false;
      apply_rotation(state, 0.0f, KEY_ROTATE_STEP, 0.0f);
      break;
    case SDLK_U:
      rot_lock_x = false;
      rot_lock_y = false;
      rot_lock_z = true;
      apply_rotation(state, 0.0f, 0.0f, KEY_ROTATE_STEP);
      break;
    case SDLK_1:
      draw_venus(state);
#ifdef __EMSCRIPTEN__
      web_sync_model_dropdown(0);
#endif
      break;
    case SDLK_2:
      draw_ship(state);
#ifdef __EMSCRIPTEN__
      web_sync_model_dropdown(1);
#endif
      break;
    case SDLK_3:
      draw_skull(state);
#ifdef __EMSCRIPTEN__
      web_sync_model_dropdown(2);
#endif
      break;
    case SDLK_4:
      draw_turtle(state);
#ifdef __EMSCRIPTEN__
      web_sync_model_dropdown(3);
#endif
      break;
    case SDLK_5:
      draw_plane(state);
#ifdef __EMSCRIPTEN__
      web_sync_model_dropdown(4);
#endif
      break;
    case SDLK_N:
      angle = wrap_light_angle(angle + input_sens.light_step);
      set_light_angle(state, angle);
#ifdef __EMSCRIPTEN__
      web_sync_light_angle(angle);
#endif
      break;
    case SDLK_I:
      state->show_overlay = !state->show_overlay;
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

  if (!state->show_overlay) {
    return;
  }

  char line[64];
#ifdef __EMSCRIPTEN__
  SDL_snprintf(line, sizeof(line), "%.2f ms | %.0f fps | %d faces drawn",
               frame_ms, 1000.0 / frame_ms, faces);
#else
  const char *mode_str = mode_names[static_cast<int>(state->config.mode)];
  SDL_snprintf(line, sizeof(line),
               "%.2f ms | %.0f fps | %d faces drawn | Method: %s", frame_ms,
               1000.0 / frame_ms, faces, mode_str);
#endif
  SDL_SetRenderDrawColor(state->renderer.get(), WHITE.r(), WHITE.g(), WHITE.b(),
                         WHITE.a());
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

  SDL_SetRenderDrawColor(state->renderer.get(), BLACK.r(), BLACK.g(), BLACK.b(),
                         BLACK.a());
  SDL_RenderClear(state->renderer.get());
#ifdef __EMSCRIPTEN__
  SDL_SetTextureScaleMode(state->texture.get(), SDL_SCALEMODE_LINEAR);
#else
  SDL_SetTextureScaleMode(state->texture.get(), SDL_SCALEMODE_NEAREST);
#endif

  if (state->grid_texture.get()) {
    SDL_RenderTexture(state->renderer.get(), state->grid_texture.get(), nullptr,
                      nullptr);
  }

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

#include <emscripten.h>

// clang-format off
void web_sync_model_dropdown(int id) {
  EM_ASM(
      {
        if (typeof window.syncModelSelect === "function") {
          window.syncModelSelect($0);
        } else {
          var el = document.getElementById("model-select");
          if (el) {
            el.value = String($0);
          }
        }
      },
      id);
}

void web_sync_mode_dropdown(int mode) {
  EM_ASM(
      {
        if (typeof window.syncModeSelect === "function") {
          window.syncModeSelect($0);
        } else {
          var el = document.getElementById("mode-select");
          if (el) {
            el.value = String($0);
          }
        }
      },
      mode);
}

void web_sync_light_angle(float light_angle) {
  EM_ASM(
      {
        if (typeof window.syncLightAngle === "function") {
          window.syncLightAngle($0);
        } else {
          var el = document.getElementById("light-slider");
          if (el) {
            el.value = String($0);
          }
        }
      },
      light_angle);
}

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
  angle = wrap_light_angle(light_angle);
  set_light_angle(g_app_state, angle);
}

void web_toggle_overlay() {
  if (!g_app_state) {
    return;
  }
  g_app_state->show_overlay = !g_app_state->show_overlay;
}

void web_toggle_swap_mouse() {
  if (!g_app_state) {
    return;
  }
  g_app_state->swap_mouse_buttons = !g_app_state->swap_mouse_buttons;
}

void web_set_rotate_sens(float v) { set_rotate_sens(v); }

void web_set_pan_sens(float v) { set_pan_sens(v); }

void web_set_zoom_sens(float v) { set_zoom_sens(v); }

void web_set_light_step(float v) { set_light_step(v); }

float web_get_rotate_sens() { return get_rotate_sens(); }

float web_get_pan_sens() { return get_pan_sens(); }

float web_get_zoom_sens() { return get_zoom_sens(); }

float web_get_light_step() { return get_light_step(); }

int web_has_user_mesh() { return user_mesh_loaded ? 1 : 0; }

void web_reload_user_mesh() {
  // when loading user models from memfs
}

// clang-format on

#endif
