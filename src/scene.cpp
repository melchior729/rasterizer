#include "scene.hpp"
#include "rasterizer.hpp"

static constexpr Vec3 translation{0, 0, -20};
static constexpr float theta{0.78f};
static constexpr Vec3 scaling{2, 2, 2};

void draw_scene(FrameBuffer &buffer, const Mat4 &view) {
  // TODO this should not be loaded each time, should be outside
  Mesh cube{Mesh::load("cube.obj")};

  Mat4 t = translate(translation);
  Mat4 r = rot_y(theta);
  Mat4 s = scale(scaling);
  Mat4 model = t * r * s;

  for (auto &v : cube.vertices) {
    v.pos = view * model * v.pos;
  }

  std::vector<Face> visible_faces{};
  for (auto &f : cube.faces) {
    auto first = cube.vertices[f[0]].pos;
    auto second = cube.vertices[f[1]].pos;
    auto third = cube.vertices[f[2]].pos;

    auto u{second.sub_xyz(first)};
    auto v{third.sub_xyz(first)};
    auto normal{u.cross(v)};

    float val{normal.dot({0, 0, -1})};
    if (val < 0) {
      visible_faces.push_back(f);
    }
  }

  cube.faces = visible_faces;
  Mat4 projection = project();

  for (auto &v : cube.vertices) {
    v.pos = projection * v.pos;

    v.pos.x /= v.pos.w;
    v.pos.y /= v.pos.w;
    v.pos.z /= v.pos.w;

    v.pos.x = (v.pos.x + 1) * WIDTH / 2;
    v.pos.y = (1 - v.pos.y) * HEIGHT / 2;
    v.color = WHITE;
  }

  for (auto &f : cube.faces) {
    auto first = cube.vertices[f[0]];
    auto second = cube.vertices[f[1]];
    auto third = cube.vertices[f[2]];

    triangle(buffer, first, third, second);
  }
}
