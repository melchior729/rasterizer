#include "scene.hpp"
#include "rasterizer.hpp"

static constexpr Vec3 translation{0, 0, -20};
static constexpr float theta{0.78f};
static constexpr Vec3 scaling{2, 2, 2};
// static constexpr Vec3 light_dir{0, 1, 0};

void draw_scene(FrameBuffer &buffer, const Mat4 &view) {
  // TODO this should not be loaded each time, should be outside
  Mesh cube{Mesh::load("cube.obj")};

  Mat4 t = translate(translation);
  Mat4 r = rot_y(theta);
  Mat4 s = scale(scaling);
  Mat4 model = t * r * s;

  for (auto &v : cube.vertices) {
    v.pos = view * model * v.pos;
    v.color = WHITE;
  }

  std::vector<Face> visible_faces{};
  for (auto &f : cube.faces) {
    auto first = cube.vertices[f[0]];
    auto second = cube.vertices[f[1]];
    auto third = cube.vertices[f[2]];

    auto u{second.pos.sub_xyz(first.pos)};
    auto v{third.pos.sub_xyz(first.pos)};
    auto normal{u.cross(v)};

    // auto g{normal.dot(light_dir)};
    // if (g < 0) {
    //   g = 0;
    // }

    //
    // alter the brighness of the color of the vertex depending on
    // g value [0, 1];
    // so multiply r() * g, b() * g,
    //
    // Color color{0xFF, static_cast<uint32_t>(WHITE.r() * g),
    //             static_cast<uint32_t>(WHITE.g() * g),
    //             static_cast<uint32_t>(WHITE.b() * g)};
    //
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
  }

  for (auto &f : cube.faces) {
    auto first = cube.vertices[f[0]];
    auto second = cube.vertices[f[1]];
    auto third = cube.vertices[f[2]];

    triangle(buffer, first, third, second);
  }
}
