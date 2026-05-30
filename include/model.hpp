#pragma once

#include "color.hpp"
#include "math.hpp"
#include "texture.hpp"
#include <memory>
#include <string>
#include <vector>

inline constexpr Vec3 default_t{0.0f, 0.0f, -20.0f};
inline constexpr Vec3 default_r{0.0f, 0.0f, 0.0f};
inline constexpr Vec3 default_s{1.0f, 1.0f, 1.0f};

struct Vertex {
  Vec4 pos;
  Vec3 view_pos;
  Vec3 normal;
  Vec2 uv;
};

struct Material {
  Color diffuse{0xFFFFFFFF};
  Color specular{0xFFFFFFFF};
  float shine{32.0f};
  int shine_log2{5};
  std::shared_ptr<Texture> texture;
};

struct Face {
  std::array<std::size_t, 3> indices;
  Vec3 normal;
  Material material;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<Face> faces;

  static Mesh load(const std::string &path);
};

struct SceneObject {
  Mesh mesh;
  Vec3 t;
  Vec3 r;
  Vec3 s;
  Mat4 model;

  SceneObject() : t(default_t), r(default_r), s(default_s) { update_matrix(); }

  SceneObject(Mesh &mesh)
      : mesh(mesh), t(default_t), r(default_r), s(default_s) {
    update_matrix();
  }

  SceneObject(Mesh &m, Vec3 t, Vec3 r, Vec3 s) : mesh(m), t(t), r(r), s(s) {
    update_matrix();
  }

  void update_matrix() {
    model = translate(t) * rot_x(r.x) * rot_y(r.y) * rot_z(r.z) * scale(s);
  }
};
