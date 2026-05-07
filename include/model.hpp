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
        // read the string
        //
        // find the indices of first space, second, third
        // x is between first and second,
        // y is between scond and htird
        // z is between third and end
        //
        // get the substrings between those indices - that the is actual token
        // make a list of these tokens
        //
        // make a list of actual values, size_t
        // operate on each token
        // if there is not a / in the token, convert to size t and then put it
        // in the size t list if there is, get the substring to the left of the
        // /, and then convert to size_t and put it in its list
        //
        // push back onto mesh's face of the values of size t list.
      } else {
        continue;
      }
    }

    file.close();
    return mesh;
  }
};
