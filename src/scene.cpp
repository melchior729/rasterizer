#include "scene.hpp"
#include "rasterizer.hpp"
#include <algorithm>

static constexpr float ambient{0.2f};

static Mat4 projection{project()};

static Mesh cube_mesh{Mesh::load("cube.obj")};
static Mesh cow_mesh{Mesh::load("cow.obj")};

static SceneObject cube{cube_mesh, {0, 0, -25}, {1.0f, 1.0f, 1.0f},
                        0.0f,      1.78f,       0.0f};

static SceneObject cow{cow_mesh, {0, -5, -50}, {0.01f, 0.01f, 0.01f},
                       0.0f,     0.0f,         0.0f};

static std::vector<SceneObject> objects{cube, cow};

static void get_visible_faces_and_colors(const Camera &camera,
                                         const Vec3 light_dir,
                                         const std::vector<Face> &faces,
                                         std::vector<Face> &visible_faces,
                                         std::vector<Color> &colors,
                                         std::vector<Vertex> &vertices) {

  Vec3 light_dir_n{norm(light_dir)};
  Vec4 light{light_dir_n.x, light_dir_n.y, light_dir_n.z, 0};
  Vec3 looking{norm((camera.view() * (camera.target - camera.pos)).xyz())};
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

void draw_scene(FrameBuffer &buffer, const Camera &camera,
                const Vec3 &light_dir) {
  for (const auto &o : objects) {
    auto vertices{o.mesh.vertices};
    for (auto &v : vertices) {
      v.pos = camera.view() * o.model * v.pos;
      v.color = WHITE;
    }

    std::vector<Face> visible_faces{};
    std::vector<Color> colors{};
    get_visible_faces_and_colors(camera, light_dir, o.mesh.faces, visible_faces,
                                 colors, vertices);

    perspective_divide_and_screen_space(vertices);
    draw_faces(buffer, visible_faces, colors, vertices);
  }
}
