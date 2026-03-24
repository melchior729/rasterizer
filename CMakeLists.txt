cmake_minimum_required(VERSION 4.2.3)
project(rasterizer)

set(CMAKE_CXX_VERSION 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main main.cpp)

find_package(SDL3 REQUIRED)
target_link_libraries(main PRIVATE SDL3::SDL3)
target_compile_definitions(main PRIVATE SDL_MAIN_USE_CALLBACKS)

