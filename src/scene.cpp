#include "scene.hpp"
#include "rasterizer.hpp"
#include <algorithm>

static constexpr float ambient{0.2f};

static Mat4 projection{project()};

static Mesh cube_mesh{Mesh::load("cube.obj")};
static Mesh cow_mesh{Mesh::load("cow.obj")};
// static Mesh gun_mesh{Mesh::load("pistol.obj")};

std::vector<SceneObject> get_objects(float x, float y, float z) {
  std::vector<SceneObject> objects;
  SceneObject cube{cube_mesh, {-10, 0, -10}, {1.0f, 1.0f, 1.0f}, x, y, z};
  SceneObject cow{cow_mesh, {0, 0, -30}, {0.01f, 0.01f, 0.01f}, x, y, z};
  // SceneObject gun{gun_mesh, {-10, 0, -50}, {1.0f, 1.0f, 1.0f}, x, y, z};
  objects.push_back(cube);
  objects.push_back(cow);
  // objects.push_back(gun);
  return objects;
}

static void get_visible_faces(const Camera &camera, const Vec3 light_dir,
                              std::vector<Face> &faces,
                              std::vector<Face> &visible_faces,
                              std::vector<Vertex> &vertices) {

  Vec3 light_dir_n{norm(light_dir)};
  Vec4 light{light_dir_n.x, light_dir_n.y, light_dir_n.z, 0};
  Vec3 looking{norm((camera.view() * (camera.target - camera.pos)).xyz())};
  Vec3 view_light = norm((camera.view() * light).xyz());

  for (auto &f : faces) {
    auto first = vertices[f.indices[0]].pos.xyz();
    auto second = vertices[f.indices[1]].pos.xyz();
    auto third = vertices[f.indices[2]].pos.xyz();

    auto u{second - first};
    auto v{third - first};
    auto normal{norm(u.cross(v))};

    auto brightness{std::clamp((normal.dot(view_light) + ambient), 0.0f, 1.0f)};

    Color color{0xFF,
                static_cast<uint32_t>(f.material.diffuse.r() * brightness),
                static_cast<uint32_t>(f.material.diffuse.g() * brightness),
                static_cast<uint32_t>(f.material.diffuse.b() * brightness)};

    f.material.diffuse.color = color.color;
    float val{normal.dot(looking)};
    if (val < 0) {
      visible_faces.push_back(f);
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
                       const std::vector<Vertex> &vertices) {
  for (std::size_t i = 0; i < faces.size(); i++) {
    auto f = faces[i];
    auto first = vertices[f.indices[0]];
    auto second = vertices[f.indices[1]];
    auto third = vertices[f.indices[2]];

    triangle(buffer, first, third, second, f.material);
  }
}

void draw_scene(FrameBuffer &buffer, const Camera &camera,
                const Vec3 &light_dir, float x, float y, float z) {
  auto objects{get_objects(x, y, z)};
  for (auto &o : objects) {
    auto vertices{o.mesh.vertices};
    for (auto &v : vertices) {
      v.pos = camera.view() * o.model * v.pos;
    }

    std::vector<Face> visible_faces{};
    get_visible_faces(camera, light_dir, o.mesh.faces, visible_faces, vertices);

    perspective_divide_and_screen_space(vertices);
    draw_faces(buffer, visible_faces, vertices);
  }
}
