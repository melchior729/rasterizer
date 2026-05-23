#include "parser.hpp"
#include "config.hpp"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iterator>
#include <sstream>

constexpr std::size_t BASE{255};

static void push_pos(std::vector<std::string> &tokens,
                     std::vector<Vec4> &pos_cache) {
  float x{std::stof(tokens[1])};
  float y{std::stof(tokens[2])};
  float z{std::stof(tokens[3])};
  float w{1};
  if (tokens.size() > 4) {
    w = std::stof(tokens[4]);
  }

  pos_cache.push_back({x, y, z, w});
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
                      const std::vector<Vec4> &pos_cache,
                      const std::vector<Vec2> &uvs,
                      const std::vector<Vec3> &normals, Material material,
                      Mesh &mesh) {
  Face face;
  std::size_t faces_index{};

  for (std::size_t i{0}; i < tokens.size(); i++) {
    auto entries{get_entries(tokens[i])};
    std::size_t vi;
    std::string_view vi_str{entries[0]};
    auto [ptr1, e1]{
        std::from_chars(vi_str.data(), vi_str.data() + vi_str.size(), vi)};
    assert(e1 == std::errc());

    Vertex vertex;
    vertex.pos = pos_cache[vi - 1];

    if (entries.size() == 3) {
      std::size_t norm_i;
      std::string_view norm_i_str{entries[2]};
      auto [ptr2, e2]{std::from_chars(
          norm_i_str.data(), norm_i_str.data() + norm_i_str.size(), norm_i)};
      assert(e2 == std::errc());
      vertex.normal = normals[norm_i - 1];
    }

    if (entries.size() >= 2 && !entries[1].empty()) {
      std::size_t tex_i;
      std::string_view tex_i_str{entries[1]};
      auto [ptr3, e3]{std::from_chars(
          tex_i_str.data(), tex_i_str.data() + tex_i_str.size(), tex_i)};
      assert(e3 == std::errc());
      vertex.uv = uvs[tex_i - 1];
    }

    mesh.vertices.push_back(vertex);
    face.indices[faces_index++] = mesh.vertices.size() - 1;
  }

  face.material = material;
  mesh.faces.push_back(face);
}

Mesh parse_obj(const std::string &path) {
  std::ifstream file{MODEL_PATH + path};
  assert(file.is_open());

  Mesh mesh;
  Material current_material;
  std::vector<Vec4> pos_cache;
  std::vector<Vec2> uvs;
  std::vector<Vec3> normals;
  std::unordered_map<std::string, Material> materials;

  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::vector<std::string> tokens{std::istream_iterator<std::string>(ss),
                                    std::istream_iterator<std::string>()};
    if (tokens.empty()) {
      continue;
    }

    auto it{std::find(tokens.begin(), tokens.end(), "#")};
    if (it != tokens.end() && tokens[0] != "#") {
      tokens.erase(it, tokens.end());
    }

    auto type{tokens[0]};
    if (type == "v") {
      push_pos(tokens, pos_cache);
    }

    else if (type == "vt") {
      push_uv(tokens, uvs);
    }

    else if (type == "vn") {
      push_normal(tokens, normals);
    }

    else if (type == "f") {
      std::vector<std::string> first{tokens[1], tokens[2], tokens[3]};
      push_face(first, pos_cache, uvs, normals, current_material, mesh);

      if (tokens.size() == 5) {
        std::vector<std::string> second{tokens[1], tokens[3], tokens[4]};
        push_face(second, pos_cache, uvs, normals, current_material, mesh);
      }
    }

    else if (type == "mtllib") {
      auto new_materials{parse_mtl(tokens[1])};
      materials.merge(new_materials);
    }

    else if (type == "usemtl") {
      current_material = materials[tokens[1]];
    }

    else {
      continue;
    }
  }

  return mesh;
}

std::unordered_map<std::string, Material> parse_mtl(const std::string &str) {
  std::string path{MODEL_PATH + str + ".mtl"};
  std::ifstream file{path};
  std::unordered_map<std::string, Material> materials;
  if (!file.is_open()) {
    return materials;
  }

  std::string newest_material;
  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::vector<std::string> tokens{std::istream_iterator<std::string>(ss),
                                    std::istream_iterator<std::string>()};
    if (tokens.empty()) {
      continue;
    }

    auto type{tokens[0]};

    if (type == "newmtl") {
      Material material;
      materials[tokens[1]] = material;
      newest_material = tokens[1];
    }

    else if (type == "Kd") {
      auto r{static_cast<uint32_t>(std::stof(tokens[1]) * BASE)};
      auto g{static_cast<uint32_t>(std::stof(tokens[2]) * BASE)};
      auto b{static_cast<uint32_t>(std::stof(tokens[3]) * BASE)};

      Color color{0xFF, r, g, b};
      materials[newest_material].diffuse = color;
    }

    else if (type == "Ks") {
      auto r{static_cast<uint32_t>(std::stof(tokens[1]) * BASE)};
      auto g{static_cast<uint32_t>(std::stof(tokens[2]) * BASE)};
      auto b{static_cast<uint32_t>(std::stof(tokens[3]) * BASE)};

      Color color{0xFF, r, g, b};
      materials[newest_material].specular = color;
    }

    else if (type == "Ns") {
      materials[newest_material].shine = std::stof(tokens[1]);
    }
  }

  return materials;
}

Mesh Mesh::load(const std::string &path) { return parse_obj(path); }
