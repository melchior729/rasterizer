#include "model.hpp"
#include "config.hpp"
#include <charconv>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <system_error>

static constexpr std::string PATH_PREFIX{"models/"};

Mesh Mesh::load(const std::string &name) {
  Mesh mesh;
  std::ifstream file{PATH_PREFIX + name};

  if (!file.is_open()) {
    return mesh;
  }

  std::vector<Vec2> uvs;
  std::vector<Vec3> normals;
  std::string line;

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::vector<std::string> tokens{std::istream_iterator<std::string>(ss),
                                    std::istream_iterator<std::string>()};

    auto type{tokens[0]};

    if (type == "mtllib") {
      // parse the next value as a mtl lib
    }

    else if (type == "v") {
      float x{std::stof(tokens[1])};
      float y{std::stof(tokens[2])};
      float z{std::stof(tokens[3])};
      float w{1};
      if (tokens.size() > 4) {
        w = std::stof(tokens[4]);
      }

      mesh.vertices.push_back({{x, y, z, w}, WHITE});
    }

    else if (type == "vt") {
      float x{std::stof(tokens[1])};
      float y{0};
      if (tokens.size() > 2) {
        y = std::stof(tokens[2]);
      }

      uvs.push_back({x, y});
    }

    else if (type == "vn") {
      float x{std::stof(tokens[1])};
      float y{std::stof(tokens[2])};
      float z{std::stof(tokens[3])};

      normals.push_back({x, y, z});
    }

    else if (type == "usemtl") {
      // apply the material on the next faces, you must track the material used
    }

    else if (type == "f") {
      // get the 3 tokens and then put it into the vector, this is a unit
      // take each unit and then loop through it
      // when a / is found, take a substring from theprevious / + 1 to this
      // index. add themm to a vector. depending on the size eof the vector, you
      // will assign the things to the correpsonding faces

      std::string_view first{tokens[1]};
      std::string_view second{tokens[2]};
      std::string_view third{tokens[3]};
      std::vector<std::string_view> strs{first, second, third};

      for (const auto &str : strs) {
        std::vector<std::size_t> entries;
        std::size_t slash{std::string::npos};

        for (std::size_t i = 0; i < 2; i++) {

          std::string_view sv;
          std::size_t attempt{str.find("/", slash)};

          // just determine value for string view
          if (attempt == std::string::npos) {
            sv = str.substr(slash + 1, );
          } else {
            slash = attempt;
          }

          std::size_t val{0};
          auto [ptr,
                ec]{std::from_chars(sv.data(), sv.data() + sv.size(), val)};
          assert(ec == std::errc{});
        }
      }
    }

    else {
      continue;
    }
  }

  return mesh;
}
