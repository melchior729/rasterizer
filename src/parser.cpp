#include "parser.hpp"
#include "config.hpp"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iterator>
#include <limits>
#include <sstream>

static constexpr float TARGET_SIZE = 20.0f;

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
    y = 1.0f - std::stof(tokens[2]);
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

static void generate_normals(Mesh &mesh) {
  for (auto &f : mesh.faces) {
    auto first = &mesh.vertices[f.indices[0]];
    auto second = &mesh.vertices[f.indices[1]];
    auto third = &mesh.vertices[f.indices[2]];

    auto U{second->pos.sub_xyz(first->pos)};
    auto V{third->pos.sub_xyz(first->pos)};
    auto normal{norm(U.cross(V))};

    f.normal = normal;
    first->normal = first->normal + f.normal;
    second->normal = second->normal + f.normal;
    third->normal = third->normal + f.normal;
  }

  for (auto &v : mesh.vertices) {
    v.normal = norm(v.normal);
  }
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

  if (normals.empty()) {
    generate_normals(mesh);
  }

  return mesh;
}

std::unordered_map<std::string, Material> parse_mtl(const std::string &str) {
  std::string path{MODEL_PATH + str};
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
      auto r{static_cast<uint32_t>(std::stof(tokens[1]) * CHANNEL_VAL)};
      auto g{static_cast<uint32_t>(std::stof(tokens[2]) * CHANNEL_VAL)};
      auto b{static_cast<uint32_t>(std::stof(tokens[3]) * CHANNEL_VAL)};

      Color color{0xFF, r, g, b};
      materials[newest_material].diffuse = color;
    }

    else if (type == "Ks") {
      auto r{static_cast<uint32_t>(std::stof(tokens[1]) * CHANNEL_VAL)};
      auto g{static_cast<uint32_t>(std::stof(tokens[2]) * CHANNEL_VAL)};
      auto b{static_cast<uint32_t>(std::stof(tokens[3]) * CHANNEL_VAL)};

      Color color{0xFF, r, g, b};
      materials[newest_material].specular = color;
    }

    else if (type == "Ns") {
      float parsed{std::stof(tokens[1])};
      materials[newest_material].shine = parsed;
      materials[newest_material].shine_log2 =
          static_cast<int>(std::log2(parsed));
    }

    else if (type == "map_Kd") {
      materials[newest_material].texture =
          std::make_shared<Texture>(Texture::load(TEXTURE_PATH + tokens[1]));
    }
  }

  return materials;
}

Mesh Mesh::load(const std::string &path) {
  Mesh mesh{parse_obj(path)};
  if (mesh.vertices.empty()) {
    return mesh;
  }

  float min{std::numeric_limits<float>::min()};
  float max{std::numeric_limits<float>::max()};
  Vec3 min_v{max, max, max};
  Vec3 max_v{min, min, min};

  for (auto &v : mesh.vertices) {
    min_v.x = std::min(min_v.x, v.pos.x);
    min_v.y = std::min(min_v.y, v.pos.y);
    min_v.z = std::min(min_v.z, v.pos.z);

    max_v.x = std::max(max_v.x, v.pos.x);
    max_v.y = std::max(max_v.y, v.pos.y);
    max_v.z = std::max(max_v.z, v.pos.z);
  }

  Vec3 len{max_v.x - min_v.x, max_v.y - min_v.y, max_v.z - min_v.z};
  auto max_len{std::max({len.x, len.y, len.z})};
  auto scale_factor{(max_len == 0.0f) ? 1.0f : TARGET_SIZE / max_len};

  Vec3 center{(min_v.x + max_v.x) * 0.5f, (min_v.y + max_v.y) * 0.5f,
              (min_v.z + max_v.z) * 0.5f};

  for (auto &v : mesh.vertices) {
    v.pos.x = (v.pos.x - center.x) * scale_factor;
    v.pos.y = (v.pos.y - center.y) * scale_factor;
    v.pos.z = (v.pos.z - center.z) * scale_factor;
  }

  return mesh;
}
