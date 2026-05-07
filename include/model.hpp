#pragma once

// TODO remove this after need for logging is gone
// #include "config.hpp"

#include "color.hpp"
#include "config.hpp"
#include "math.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

constexpr std::string PATH_PREFIX{"models/"};

struct Vertex {
  Vec4 pos;
  Color color;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<std::array<std::size_t, 3>> faces;

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
        std::istringstream data(line);
        data >> first;

        std::size_t x;
        std::size_t y;
        std::size_t z;

        data >> x;
        data >> y;
        data >> z;

        mesh.faces.push_back({x - 1, y - 1, z - 1});
      } else {
        continue;
      }
    }

    file.close();
    return mesh;
  }
};
