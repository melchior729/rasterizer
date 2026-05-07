#include "scene.hpp"
#include "rasterizer.hpp"

static constexpr Vec3 translation{0, 0, -20};
static constexpr float theta{0.78f};
static constexpr Vec3 scaling{2, 2, 2};
static constexpr Vec3 light_dir{1, 1, 1};

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
  std::vector<Color> colors{};

  Vec4 light{light_dir.x, light_dir.y, light_dir.z, 0};
  Vec3 view_light = norm((view * light).xyz());

  for (auto &f : cube.faces) {
    auto first = cube.vertices[f[0]];
    auto second = cube.vertices[f[1]];
    auto third = cube.vertices[f[2]];

    auto u{second.pos.sub_xyz(first.pos)};
    auto v{third.pos.sub_xyz(first.pos)};
    auto normal{norm(u.cross(v))};

    auto g{normal.dot(view_light)};
    if (g < 0) {
      g = 0;
    }

    Color color{0xFF, static_cast<uint32_t>(WHITE.r() * g),
                static_cast<uint32_t>(WHITE.g() * g),
                static_cast<uint32_t>(WHITE.b() * g)};

    float val{normal.dot({0, 0, -1})};
    if (val < 0) {
      visible_faces.push_back(f);
      colors.push_back(color);
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

  for (std::size_t i = 0; i < cube.faces.size(); i++) {
    auto f = cube.faces[i];
    auto first = cube.vertices[f[0]];
    auto second = cube.vertices[f[1]];
    auto third = cube.vertices[f[2]];

    first.color = colors[i];
    second.color = colors[i];
    third.color = colors[i];

    triangle(buffer, first, third, second);
  }
}
