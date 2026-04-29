#define SDL_MAIN_USE_CALLBACKS 1

#include "rasterizer.hpp"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <cmath>
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
};

// AI Generated Scene
void draw_scene(FrameBuffer &buffer) {

  // --- SKY GRADIENT ---
  Color deep_sky{0xFF, 0x0A, 0x05, 0x2E};
  Color mid_sky{0xFF, 0x8B, 0x1A, 0x4A};
  Color horizon_sky{0xFF, 0xFF, 0x6B, 0x35};
  Color horizon_glow{0xFF, 0xFF, 0xA5, 0x00};

  Vertex sky_tl{{0, 0, -10}, deep_sky};
  Vertex sky_tr{{1920, 0, -10}, deep_sky};
  Vertex sky_ml{{0, 400, -10}, mid_sky};
  Vertex sky_mr{{1920, 400, -10}, mid_sky};
  Vertex sky_hl{{0, 680, -10}, horizon_sky};
  Vertex sky_hr{{1920, 680, -10}, horizon_sky};
  Vertex sky_gl{{0, 780, -10}, horizon_glow};
  Vertex sky_gr{{1920, 780, -10}, horizon_glow};

  draw_triangle(buffer, sky_tl, sky_tr, sky_ml);
  draw_triangle(buffer, sky_tr, sky_mr, sky_ml);
  draw_triangle(buffer, sky_ml, sky_mr, sky_hl);
  draw_triangle(buffer, sky_mr, sky_hr, sky_hl);
  draw_triangle(buffer, sky_hl, sky_hr, sky_gl);
  draw_triangle(buffer, sky_hr, sky_gr, sky_gl);

  // --- SUN ---
  Color sun_core{0xFF, 0xFF, 0xFF, 0xAA};
  Color sun_glow{0xFF, 0xFF, 0xCC, 0x00};
  Color sun_outer{0xFF, 0xFF, 0x80, 0x00};

  Vertex sun_center{{960, 680, -9}, sun_core};
  constexpr int sun_segments = 24;
  constexpr float sun_r = 80.0f;
  constexpr float halo_r = 160.0f;
  constexpr float two_pi = 2.0f * 3.14159f;

  for (int i = 0; i < sun_segments; ++i) {
    float a1 =
        two_pi * static_cast<float>(i) / static_cast<float>(sun_segments);
    float a2 =
        two_pi * static_cast<float>(i + 1) / static_cast<float>(sun_segments);

    int x1 = static_cast<int>(960.0f + sun_r * std::cos(a1));
    int y1 = static_cast<int>(680.0f + sun_r * std::sin(a1));
    int x2 = static_cast<int>(960.0f + sun_r * std::cos(a2));
    int y2 = static_cast<int>(680.0f + sun_r * std::sin(a2));

    Vertex v1{{static_cast<float>(x1), static_cast<float>(y1), -9}, sun_glow};
    Vertex v2{{static_cast<float>(x2), static_cast<float>(y2), -9}, sun_glow};
    draw_triangle(buffer, sun_center, v1, v2);
  }

  for (int i = 0; i < sun_segments; ++i) {
    float a1 =
        two_pi * static_cast<float>(i) / static_cast<float>(sun_segments);
    float a2 =
        two_pi * static_cast<float>(i + 1) / static_cast<float>(sun_segments);

    int ix1 = static_cast<int>(960.0f + sun_r * std::cos(a1));
    int iy1 = static_cast<int>(680.0f + sun_r * std::sin(a1));
    int ix2 = static_cast<int>(960.0f + sun_r * std::cos(a2));
    int iy2 = static_cast<int>(680.0f + sun_r * std::sin(a2));
    int ox1 = static_cast<int>(960.0f + halo_r * std::cos(a1));
    int oy1 = static_cast<int>(680.0f + halo_r * std::sin(a1));
    int ox2 = static_cast<int>(960.0f + halo_r * std::cos(a2));
    int oy2 = static_cast<int>(680.0f + halo_r * std::sin(a2));

    Vertex inner1{{static_cast<float>(ix1), static_cast<float>(iy1), -9},
                  sun_glow};
    Vertex inner2{{static_cast<float>(ix2), static_cast<float>(iy2), -9},
                  sun_glow};
    Vertex outer1{{static_cast<float>(ox1), static_cast<float>(oy1), -9},
                  sun_outer};
    Vertex outer2{{static_cast<float>(ox2), static_cast<float>(oy2), -9},
                  sun_outer};
    draw_triangle(buffer, inner1, inner2, outer1);
    draw_triangle(buffer, inner2, outer2, outer1);
  }

  // --- MOUNTAINS (back layer) ---
  Color mtn_back{0xFF, 0x2D, 0x1B, 0x4E};
  Color mtn_back_base{0xFF, 0x1A, 0x0D, 0x33};

  struct Peak {
    int x, y;
  };
  const Peak back_peaks[] = {{0, 780},    {80, 600},   {200, 520},  {350, 580},
                             {500, 480},  {650, 560},  {750, 430},  {900, 510},
                             {960, 400},  {1020, 510}, {1150, 450}, {1280, 540},
                             {1400, 470}, {1550, 580}, {1700, 500}, {1820, 560},
                             {1920, 620}, {1920, 780}, {0, 780}};
  constexpr int back_count =
      static_cast<int>(sizeof(back_peaks) / sizeof(back_peaks[0]));
  for (int i = 0; i + 2 < back_count; ++i) {
    Vertex base{{960, 780, -8}, mtn_back_base};
    Vertex p1{{static_cast<float>(back_peaks[i].x),
               static_cast<float>(back_peaks[i].y), -8},
              mtn_back};
    Vertex p2{{static_cast<float>(back_peaks[i + 1].x),
               static_cast<float>(back_peaks[i + 1].y), -8},
              mtn_back};
    draw_triangle(buffer, base, p1, p2);
  }

  // --- MOUNTAINS (front layer) ---
  Color mtn_front{0xFF, 0x1C, 0x2E, 0x3A};
  Color mtn_front_lit{0xFF, 0x3A, 0x4A, 0x5C};
  Color mtn_front_base{0xFF, 0x0D, 0x18, 0x20};

  const Peak front_peaks[] = {
      {0, 900},    {100, 820},  {250, 730},  {400, 800},  {520, 690},
      {680, 760},  {800, 670},  {920, 720},  {1000, 640}, {1100, 700},
      {1250, 650}, {1380, 730}, {1500, 660}, {1650, 750}, {1800, 820},
      {1920, 780}, {1920, 900}, {0, 900}};
  constexpr int front_count =
      static_cast<int>(sizeof(front_peaks) / sizeof(front_peaks[0]));
  for (int i = 0; i + 2 < front_count; ++i) {
    Vertex base{{960, 900, -7}, mtn_front_base};
    Vertex p1{{static_cast<float>(front_peaks[i].x),
               static_cast<float>(front_peaks[i].y), -7},
              mtn_front};
    Vertex p2{{static_cast<float>(front_peaks[i + 1].x),
               static_cast<float>(front_peaks[i + 1].y), -7},
              mtn_front_lit};
    draw_triangle(buffer, base, p1, p2);
  }

  // --- WATER ---
  Color water_dark{0xFF, 0x05, 0x10, 0x2A};
  Color water_mid{0xFF, 0x0A, 0x25, 0x45};

  Vertex wl1{{0, 900, -6}, water_dark};
  Vertex wr1{{1920, 900, -6}, water_dark};
  Vertex wl2{{0, 1080, -6}, water_mid};
  Vertex wr2{{1920, 1080, -6}, water_mid};
  draw_triangle(buffer, wl1, wr1, wl2);
  draw_triangle(buffer, wr1, wr2, wl2);

  // Sun reflection
  Color ref_bright{0xFF, 0xFF, 0xCC, 0x44};
  Color ref_dim{0xFF, 0xFF, 0x66, 0x00};
  Vertex ref_tl{{820, 900, -5}, ref_dim};
  Vertex ref_tr{{1100, 900, -5}, ref_dim};
  Vertex ref_bl{{700, 1080, -5}, ref_bright};
  Vertex ref_br{{1220, 1080, -5}, ref_bright};
  draw_triangle(buffer, ref_tl, ref_tr, ref_bl);
  draw_triangle(buffer, ref_tr, ref_br, ref_bl);

  // --- TREES ---
  Color tree_dark{0xFF, 0x05, 0x08, 0x0A};

  auto draw_tree = [&](int x, int base_y, int w, int h) {
    Vertex trunk_l{
        {static_cast<float>(x - w / 6), static_cast<float>(base_y), -4},
        tree_dark};
    Vertex trunk_r{
        {static_cast<float>(x + w / 6), static_cast<float>(base_y), -4},
        tree_dark};
    Vertex trunk_t{
        {static_cast<float>(x), static_cast<float>(base_y - h / 4), -4},
        tree_dark};
    draw_triangle(buffer, trunk_l, trunk_r, trunk_t);

    Vertex b1l{
        {static_cast<float>(x - w / 2), static_cast<float>(base_y - h / 5), -4},
        tree_dark};
    Vertex b1r{
        {static_cast<float>(w / 2 + x), static_cast<float>(base_y - h / 5), -4},
        tree_dark};
    Vertex b1t{{static_cast<float>(x), static_cast<float>(base_y - h / 2), -4},
               tree_dark};
    draw_triangle(buffer, b1l, b1r, b1t);

    Vertex b2l{{static_cast<float>(x - 2 * w / 5),
                static_cast<float>(base_y - h * 2 / 5), -4},
               tree_dark};
    Vertex b2r{{static_cast<float>(x + w * 2 / 5),
                static_cast<float>(base_y - h * 2 / 5), -4},
               tree_dark};
    Vertex b2t{
        {static_cast<float>(x), static_cast<float>(base_y - h * 4 / 5), -4},
        tree_dark};
    draw_triangle(buffer, b2l, b2r, b2t);

    Vertex b3l{{static_cast<float>(x - w / 4),
                static_cast<float>(base_y - h * 3 / 5), -4},
               tree_dark};
    Vertex b3r{{static_cast<float>(x + w / 4),
                static_cast<float>(base_y - h * 3 / 5), -4},
               tree_dark};
    Vertex b3t{{static_cast<float>(x), static_cast<float>(base_y - h), -4},
               tree_dark};
    draw_triangle(buffer, b3l, b3r, b3t);
  };

  draw_tree(60, 1000, 120, 280);
  draw_tree(160, 1020, 100, 240);
  draw_tree(280, 1010, 130, 300);
  draw_tree(420, 1030, 90, 220);
  draw_tree(530, 1000, 110, 260);
  draw_tree(1400, 1010, 110, 270);
  draw_tree(1530, 1000, 130, 310);
  draw_tree(1660, 1020, 100, 240);
  draw_tree(1780, 1005, 120, 280);
  draw_tree(1880, 1030, 90, 200);
}

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
  *appstate = state.release();

  return SDL_APP_CONTINUE;
}

// remove maybe unused when implementing input
SDL_AppResult SDL_AppEvent([[maybe_unused]] void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto *state = static_cast<AppState *>(appstate);

  draw_scene(*state->buffer);

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
