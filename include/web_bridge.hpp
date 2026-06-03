#pragma once

#ifdef __EMSCRIPTEN__

void web_bind_app_state(void *appstate);

void web_set_model(int id);

void web_set_render_mode(int mode);

void web_set_light_angle(float angle);

void web_set_day_mode(int is_day);

int web_has_user_mesh();

void web_reload_user_mesh();

#endif
