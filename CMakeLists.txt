cmake_minimum_required(VERSION 3.20)
project(rasterizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB SRCS "src/*.cpp")
list(FILTER SRCS EXCLUDE REGEX ".*/web_api\\.cpp$")

add_executable(main)
target_sources(main PRIVATE ${SRCS})
target_include_directories(main PUBLIC include)
set_source_files_properties(src/stb_impl.cpp PROPERTIES COMPILE_FLAGS "-w")

find_package(SDL3 REQUIRED)
target_link_libraries(main PRIVATE SDL3::SDL3)
target_compile_definitions(main PRIVATE SDL_MAIN_USE_CALLBACKS)
target_compile_options(main PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wsign-conversion
    -O3
    -g
    -march=native
    -ffast-math
    -flto
)
