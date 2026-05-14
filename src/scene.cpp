#include "scene.hpp"
#include "rasterizer.hpp"

static Mat4 projection{project()};

static Mesh cube_mesh{Mesh::load("cube.obj")};
static Mesh cow_mesh{Mesh::load("cow.obj")};
static Mesh man_mesh{Mesh::load("man.obj")};
// static Mesh gun_mesh{Mesh::load("pistol.obj")};

std::vector<SceneObject> get_objects(Vec3 r) {
  std::vector<SceneObject> objects;
  SceneObject cube{cube_mesh, {10, 0, -10}, {1.0f, 1.0f, 1.0f}, r};
  SceneObject cow{cow_mesh, {-10, 0, -20}, {0.8f, 0.8f, 0.8f}, r};
  SceneObject man{man_mesh, {0, -10, -10}, {0.4f, 0.4f, 0.4f}, r};
  // SceneObject gun{gun_mesh, {-10, 0, -50}, {1.0f, 1.0f, 1.0f}, r};
  objects.push_back(cube);
  objects.push_back(cow);
  objects.push_back(man);
  // objects.push_back(gun);
  return objects;
}

static void get_visible_faces(const Camera &camera, std::vector<Face> &faces,
                              std::vector<Face> &visible_faces,
                              std::vector<Vertex> &vertices) {

  Vec3 looking{norm((camera.view() * (camera.target - camera.pos)).xyz())};

  for (auto &f : faces) {
    auto first = vertices[f.indices[0]].pos.xyz();
    auto second = vertices[f.indices[1]].pos.xyz();
    auto third = vertices[f.indices[2]].pos.xyz();

    auto u{second - first};
    auto v{third - first};
    auto normal{norm(u.cross(v))};

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

static void draw_faces(FrameBuffer &buffer, const Vec3 light_dir,
                       const std::vector<Face> &faces,
                       const std::vector<Vertex> &vertices) {
  for (std::size_t i = 0; i < faces.size(); i++) {
    auto f = faces[i];
    auto first = vertices[f.indices[0]];
    auto second = vertices[f.indices[1]];
    auto third = vertices[f.indices[2]];

    triangle(buffer, first, second, third, f.material, light_dir);
  }
}

void draw_scene(FrameBuffer &buffer, const Camera &camera, const Vec3 light_dir,
                Vec3 rot) {
  auto objects{get_objects(rot)};
  for (auto &o : objects) {
    auto vertices{o.mesh.vertices};
    for (auto &v : vertices) {
      v.pos = camera.view() * o.model * v.pos;
    }

    std::vector<Face> visible_faces{};
    get_visible_faces(camera, o.mesh.faces, visible_faces, vertices);

    perspective_divide_and_screen_space(vertices);
    draw_faces(buffer, light_dir, visible_faces, vertices);
  }
}
