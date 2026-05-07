#pragma once

// TODO remove this after need for logging is gone

#include "color.hpp"
#include "config.hpp"
#include "math.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

constexpr std::string PATH_PREFIX{"models/"};
using Face = std::array<std::size_t, 3>;

struct Vertex {
  Vec4 pos;
  Color color;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<Face> faces;

  static Mesh load(const std::string &name) {
    Mesh mesh{};
    std::ifstream file{PATH_PREFIX + name};

    if (!file.is_open()) {
      // SDL_Log("Failure reading mesh!");
      return mesh;
    }

    std::string line;
    while (std::getline(file, line)) {
      std::size_t first_space{line.find(" ")};
      std::string first{line.substr(0, first_space)};

      if (first == "v") {
        std::istringstream data(line);
        data >> first;

        float x;
        float y;
        float z;

        data >> x;
        data >> y;
        data >> z;

        mesh.vertices.push_back({{x, y, z}, BLACK});

      } else if (first == "f") {

        std::size_t second_space{line.find(" ", first_space + 1)};
        std::size_t third_space{line.find(" ", second_space + 1)};

        std::string x_str{
            line.substr(first_space + 1, second_space - first_space - 1)};

        std::string y_str{
            line.substr(second_space + 1, third_space - second_space - 1)};

        std::string z_str{
            line.substr(third_space + 1, line.length() - third_space - 1)};

        std::array<std::string, 3> tokens{x_str, y_str, z_str};
        std::array<std::size_t, 3> vals;

        for (std::size_t i = 0; i < 3; i++) {
          auto &t{tokens[i]};
          std::size_t slash{t.find("/")};

          vals[i] = (slash == std::string::npos) ? stoull(t)
                                                 : stoull(t.substr(0, slash));
          vals[i]--;
        }
        mesh.faces.push_back({vals[0], vals[1], vals[2]});
      } else {
        continue;
      }
    }

    file.close();
    return mesh;
  }
};
