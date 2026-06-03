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

EMSCRIPTEN_KEEPALIVE int has_user_mesh() { return web_has_user_mesh(); }

EMSCRIPTEN_KEEPALIVE void reload_user_mesh() { web_reload_user_mesh(); }
}

#endif
