#ifdef __EMSCRIPTEN__

#include "web_bridge.hpp"
#include <emscripten.h>

extern "C" {

EMSCRIPTEN_KEEPALIVE void set_model(int id) { web_set_model(id); }

EMSCRIPTEN_KEEPALIVE void set_render_mode(int mode) {
  web_set_render_mode(mode);
}

EMSCRIPTEN_KEEPALIVE void set_light_angle(float angle) {
  web_set_light_angle(angle);
}

EMSCRIPTEN_KEEPALIVE void toggle_overlay() { web_toggle_overlay(); }

EMSCRIPTEN_KEEPALIVE void set_rotate_sens(float v) { web_set_rotate_sens(v); }

EMSCRIPTEN_KEEPALIVE void set_pan_sens(float v) { web_set_pan_sens(v); }

EMSCRIPTEN_KEEPALIVE void set_zoom_sens(float v) { web_set_zoom_sens(v); }

EMSCRIPTEN_KEEPALIVE void set_light_step(float v) { web_set_light_step(v); }

EMSCRIPTEN_KEEPALIVE float get_rotate_sens() { return web_get_rotate_sens(); }

EMSCRIPTEN_KEEPALIVE float get_pan_sens() { return web_get_pan_sens(); }

EMSCRIPTEN_KEEPALIVE float get_zoom_sens() { return web_get_zoom_sens(); }

EMSCRIPTEN_KEEPALIVE float get_light_step() { return web_get_light_step(); }

EMSCRIPTEN_KEEPALIVE int has_user_mesh() { return web_has_user_mesh(); }

EMSCRIPTEN_KEEPALIVE void reload_user_mesh() { web_reload_user_mesh(); }
}

#endif
