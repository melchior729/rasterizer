#!/bin/bash

mkdir -p web

emcc src/main.cpp src/math.cpp src/parser.cpp src/rasterizer.cpp src/scene.cpp src/texture.cpp src/stb_impl.cpp \
  -Iinclude \
  -o web/index.html \
  -sUSE_SDL=3 \
  --embed-file models/ \
  --embed-file textures/ \
  -sALLOW_MEMORY_GROWTH=1 \
  -std=c++20 \
  -O3 \
  -DSDL_MAIN_USE_CALLBACKS=1 \
  -msimd128 \
  -flto \
  -ffast-math
