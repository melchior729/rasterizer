#pragma once

#include "color.hpp"
#include "math.hpp"
#include <string>
#include <vector>

struct Vertex {
  Vec4 pos;
  Vec3 normal;
  Vec2 uv;
};

struct Material {
  Color diffuse{0xFFFFFFFF};
};

struct Face {
  std::array<std::size_t, 3> indices;
  Material material;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<Face> faces;

  static Mesh load(const std::string &path);
};

struct SceneObject {
  Mesh mesh;
  Mat4 model;

  SceneObject(Mesh &mesh, Mat4 &model) : mesh(mesh), model(model) {}

  SceneObject(Mesh &m, Vec3 t, Vec3 s, Vec3 r)
      : mesh(m),
        model(translate(t) * rot_x(r.x) * rot_y(r.y) * rot_z(r.z) * scale(s)) {}
};
