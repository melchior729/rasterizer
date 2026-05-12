#include "parser.hpp"
#include "config.hpp"
#include <charconv>
#include <fstream>
#include <iterator>
#include <sstream>

static void push_vertex(std::vector<std::string> &tokens,
                        std::vector<Vertex> &vertices) {
  float x{std::stof(tokens[1])};
  float y{std::stof(tokens[2])};
  float z{std::stof(tokens[3])};
  float w{1};
  if (tokens.size() > 4) {
    w = std::stof(tokens[4]);
  }

  vertices.push_back({{x, y, z, w}, {}, Vec2::none(), WHITE});
}

static void push_uv(std::vector<std::string> &tokens, std::vector<Vec2> &uvs) {
  float x{std::stof(tokens[1])};
  float y{0};
  if (tokens.size() > 2) {
    y = std::stof(tokens[2]);
  }

  uvs.push_back({x, y});
}

static void push_normal(std::vector<std::string> &tokens,
                        std::vector<Vec3> &normals) {
  float x{std::stof(tokens[1])};
  float y{std::stof(tokens[2])};
  float z{std::stof(tokens[3])};
  normals.push_back(norm({x, y, z}));
}

static std::vector<std::string_view> get_entries(const std::string &str) {
  std::string_view sv{str};
  std::size_t start{};
  std::size_t slash{sv.find("/")};
  std::vector<std::string_view> entries;

  while (slash != std::string::npos) {
    entries.push_back(sv.substr(start, slash - start));
    start = slash + 1;
    slash = sv.find("/", start);
  }

  entries.push_back(sv.substr(start));
  return entries;
}

static void push_face(std::vector<std::string> &tokens,
                      std::vector<Face> &faces, std::vector<Vertex> &vertices,
                      std::vector<Vec2> &uvs, std::vector<Vec3> &normals) {
  Face face;
  std::size_t faces_index{};

  for (std::size_t i{0}; i < tokens.size(); i++) {
    auto entries{get_entries(tokens[i])};
    std::size_t vi;
    std::string_view vi_str{entries[0]};
    auto [_,
          e]{std::from_chars(vi_str.data(), vi_str.data() + vi_str.size(), vi)};
    assert(e == std::errc());
    face[faces_index++] = --vi;

    auto n{entries.size()};
    if (n == 3) {
      std::size_t norm_i;
      std::string_view norm_i_str{entries[2]};
      auto [_, e]{std::from_chars(
          norm_i_str.data(), norm_i_str.data() + norm_i_str.size(), norm_i)};
      assert(e == std::errc());
      vertices[face[faces_index - 1]].normal = normals[--norm_i];
    }

    if (n >= 2 && !entries[1].empty()) {
      std::size_t tex_i;
      std::string_view tex_i_str{entries[1]};
      auto [_, e]{std::from_chars(tex_i_str.data(),
                                  tex_i_str.data() + tex_i_str.size(), tex_i)};
      assert(e == std::errc());
      vertices[face[faces_index - 1]].uv = uvs[--tex_i];
    }
  }

  faces.push_back(face);
}

Mesh parse_obj(const std::string &path) {
  std::ifstream file{MODEL_PATH + path};
  assert(file.is_open());

  Mesh mesh;
  std::vector<Vec2> uvs;
  std::vector<Vec3> normals;

  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::vector<std::string> tokens{std::istream_iterator<std::string>(ss),
                                    std::istream_iterator<std::string>()};
    if (tokens.empty()) {
      continue;
    }

    auto type{tokens[0]};
    if (type == "v") {
      push_vertex(tokens, mesh.vertices);
    }

    else if (type == "vt") {
      push_uv(tokens, uvs);
    }

    else if (type == "vn") {
      push_normal(tokens, normals);
    }

    else if (type == "f") {
      std::vector<std::string> first{tokens[1], tokens[2], tokens[3]};
      push_face(first, mesh.faces, mesh.vertices, uvs, normals);

      if (tokens.size() == 5) {
        std::vector<std::string> second{tokens[1], tokens[3], tokens[4]};
        push_face(second, mesh.faces, mesh.vertices, uvs, normals);
      }
    }

    else if (type == "mtllib") {
      // look at the file
    }

    else if (type == "usemtl") {
      // apply the material on the next faces, you must track the material
    }

    else {
      continue;
    }
  }

  return mesh;
}

Mesh Mesh::load(const std::string &path) { return parse_obj(path); }
