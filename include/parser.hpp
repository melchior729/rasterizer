#pragma once

#include "model.hpp"
#include <unordered_map>

std::unordered_map<std::string, Material> parse_mtl(const std::string &path);

Mesh parse_obj(const std::string &path);
