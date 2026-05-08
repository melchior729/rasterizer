#include "scene.hpp"
#include "rasterizer.hpp"
#include <algorithm>

static constexpr Vec3 translation{0, -5, -10};
static constexpr float theta{0};
static constexpr Vec3 scaling{0.01f, 0.01f, 0.01f};
static constexpr Vec3 light_dir{1, 1, 1};
static constexpr float ambient{-1.2f};

static Mat4 t{translate(translation)};
static Mat4 r{rot_x(theta)};
static Mat4 s{scale(scaling)};
static Mat4 model{t * r * s};
static Mat4 projection{project()};

static Mesh cube{Mesh::load("cow.obj")};

static void get_visible_faces_and_colors(const Camera &camera,
                                         const std::vector<Face> &faces,
                                         std::vector<Face> &visible_faces,
                                         std::vector<Color> &colors,
                                         std::vector<Vertex> &vertices) {

  Vec4 light{light_dir.x, light_dir.y, light_dir.z, 0};
  Vec3 looking{norm(camera.target - camera.pos)};
  Vec3 view_light = norm((camera.view() * light).xyz());

  for (auto &f : faces) {
    auto first = vertices[f[0]].pos.xyz();
    auto second = vertices[f[1]].pos.xyz();
    auto third = vertices[f[2]].pos.xyz();

    auto u{second - first};
    auto v{third - first};
    auto normal{norm(u.cross(v))};

    auto brightness{std::clamp((normal.dot(view_light) + ambient), 0.0f, 1.0f)};

    Color color{0xFF, static_cast<uint32_t>(WHITE.r() * brightness),
                static_cast<uint32_t>(WHITE.g() * brightness),
                static_cast<uint32_t>(WHITE.b() * brightness)};

    float val{normal.dot(looking)};
    if (val < 0) {
      visible_faces.push_back(f);
      colors.push_back(color);
    }
  }
}

static void perspective_divide_and_screen_space(std::vector<Vertex> &vertices) {
  for (auto &v : vertices) {
    v.pos = projection * v.pos;

    v.pos.x /= v.pos.w;
    v.pos.y /= v.pos.w;
    v.pos.z /= v.pos.w;

    v.pos.x = (v.pos.x + 1) * WIDTH / 2;
    v.pos.y = (1 - v.pos.y) * HEIGHT / 2;
  }
}

static void draw_faces(FrameBuffer &buffer, const std::vector<Face> &faces,
                       const std::vector<Color> &colors,
                       const std::vector<Vertex> &vertices) {
  for (std::size_t i = 0; i < faces.size(); i++) {
    auto f = faces[i];
    auto first = vertices[f[0]];
    auto second = vertices[f[1]];
    auto third = vertices[f[2]];

    first.color = colors[i];
    second.color = colors[i];
    third.color = colors[i];

    triangle(buffer, first, third, second);
  }
}

void draw_scene(FrameBuffer &buffer, const Camera &camera) {
  auto vertices{cube.vertices};
  for (auto &v : vertices) {
    v.pos = camera.view() * model * v.pos;
    v.color = WHITE;
  }

  std::vector<Face> visible_faces{};
  std::vector<Color> colors{};
  get_visible_faces_and_colors(camera, cube.faces, visible_faces, colors,
                               vertices);

  perspective_divide_and_screen_space(vertices);
  draw_faces(buffer, visible_faces, colors, vertices);
}
