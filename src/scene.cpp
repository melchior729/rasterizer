#include "scene.hpp"
#include "rasterizer.hpp"
#include <algorithm>

static Mat4 projection{project()};

static int get_visible_faces(const Camera &camera, std::vector<Face> &faces,
                             std::vector<Face> &visible_faces,
                             std::vector<Vertex> &vertices) {

  int num_faces{};
  Vec3 looking{norm((camera.view() * (camera.target - camera.pos)).xyz())};

  for (auto &f : faces) {

    auto first = vertices[f.indices[0]];
    auto second = vertices[f.indices[1]];
    auto third = vertices[f.indices[2]];

    if (first.pos.z > -NEAR || second.pos.z > -NEAR || third.pos.z > -NEAR) {
      continue;
    }

    auto u{second.pos.sub_xyz(first.pos)};
    auto v{third.pos.sub_xyz(first.pos)};
    auto normal{norm(u.cross(v))};

    float val{normal.dot(looking)};
    if (val < 0) {
      f.normal = normal;
      visible_faces.push_back(f);
      num_faces++;
    }
  }

  return num_faces;
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
                       std::vector<Face> &faces, std::vector<Vertex> &vertices,
                       const RenderMode mode) {
  for (std::size_t i = 0; i < faces.size(); i++) {
    auto f = faces[i];
    auto first = vertices[f.indices[0]];
    auto second = vertices[f.indices[1]];
    auto third = vertices[f.indices[2]];

    float bright_a{
        std::clamp(f.normal.dot(light_dir) + AMBIENT, AMBIENT, 1.0f)};
    float bright_b{bright_a};
    float bright_c{bright_a};

    if (mode == RenderMode::Gouraud) {
      bright_a =
          std::clamp(first.normal.dot(light_dir) + AMBIENT, AMBIENT, 1.0f);
      bright_b =
          std::clamp(second.normal.dot(light_dir) + AMBIENT, AMBIENT, 1.0f);
      bright_c =
          std::clamp(third.normal.dot(light_dir) + AMBIENT, AMBIENT, 1.0f);
    }

    if (mode == RenderMode::Wireframe) {
      triangle_wireframe(buffer, first, second, third);
    } else {
      triangle(buffer, first, second, third, f.material, norm(light_dir),
               bright_a, bright_b, bright_c, mode);
    }
  }
}

int draw_scene(FrameBuffer &buffer, SceneObject &o, const Camera &camera,
               const SceneConfig &config) {
  int num_faces{};
  auto vertices{o.mesh.vertices};
  for (auto &v : vertices) {
    v.pos = camera.view() * o.model * v.pos;
    v.view_pos = v.pos.xyz();
    v.normal = norm((camera.view() * o.model * v.normal).xyz());
  }

  std::vector<Face> visible_faces{};
  num_faces += get_visible_faces(camera, o.mesh.faces, visible_faces, vertices);

  perspective_divide_and_screen_space(vertices);
  draw_faces(buffer, camera.view().multiply(config.light_dir), visible_faces,
             vertices, config.mode);

  return num_faces;
}
