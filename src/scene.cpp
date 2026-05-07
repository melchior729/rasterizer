#include "scene.hpp"
#include "rasterizer.hpp"

// TODO could make camera just the view matrix itself?
void draw_scene(FrameBuffer &buffer, [[maybe_unused]] Camera &camera) {
  // TODO this should not be loaded each time, shoudl be outside
  Mesh cube{Mesh::load("cube.obj")};

  Mat4 t = translate({0, 0, -20});
  Mat4 r = rot_y(0.78f);
  Mat4 s = scale({2, 2, 2});
  Mat4 model = t * r * s;

  Mat4 view = camera.view();
  Mat4 projection = project();

  for (auto &v : cube.vertices) {
    v.pos = projection * view * model * v.pos;

    v.pos.x /= v.pos.w;
    v.pos.y /= v.pos.w;
    v.pos.z /= v.pos.w;

    v.pos.x = (v.pos.x + 1) * WIDTH / 2;
    v.pos.y = (1 - v.pos.y) * HEIGHT / 2;
  }

  std::vector<Color> colors = {
      {255, 255, 0, 0},   {255, 0, 255, 0},   {255, 0, 0, 255},
      {255, 255, 255, 0}, {255, 255, 0, 255}, {255, 0, 255, 255},
  };

  for (std::size_t i = 0; i < cube.faces.size(); i++) {
    auto &f = cube.faces[i];
    auto first = cube.vertices[f[0]];
    auto second = cube.vertices[f[1]];
    auto third = cube.vertices[f[2]];

    Color c = colors[i / 2];
    first.color = c;
    second.color = c;
    third.color = c;

    triangle(buffer, first, third, second);
  }
}
