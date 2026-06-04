#pragma once

#ifdef __EMSCRIPTEN__

void web_bind_app_state(void *appstate);

void web_set_model(int id);

void web_set_render_mode(int mode);

void web_set_light_angle(float angle);

void web_sync_light_angle(float angle);

void web_toggle_overlay();

void web_sync_model_dropdown(int id);

void web_sync_mode_dropdown(int mode);

void web_set_rotate_sens(float v);

void web_set_pan_sens(float v);

void web_set_zoom_sens(float v);

void web_set_light_step(float v);

float web_get_rotate_sens();

float web_get_pan_sens();

float web_get_zoom_sens();

float web_get_light_step();

int web_has_user_mesh();

void web_reload_user_mesh();

#endif
