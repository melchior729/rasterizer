cmake_minimum_required(VERSION 3.20)
project(rasterizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRCS "src/*.cpp")

add_executable(main)
target_sources(main PRIVATE ${SRCS})
target_include_directories(main PUBLIC include)

find_package(SDL3 REQUIRED)
target_link_libraries(main PRIVATE SDL3::SDL3)
target_compile_definitions(main PRIVATE SDL_MAIN_USE_CALLBACKS)
