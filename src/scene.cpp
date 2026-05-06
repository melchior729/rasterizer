#include "scene.hpp"
#include "rasterizer.hpp"

// TODO could make camera just the view matrix itself?
void draw_scene(FrameBuffer &buffer, Camera &camera) {
  Color red = {255, 255, 0, 0};
  Color green = {255, 0, 255, 0};
  Color blue = {255, 0, 0, 255};
  Color yellow = {255, 255, 255, 0};
  Color pink = {255, 255, 0, 255};
  Color cyan = {255, 0, 255, 255};
  Color white = {255, 255, 255, 255};
  Color orange = {255, 255, 165, 0};

  Vertex vertices[]{
      {{-1, -1, 1}, red},   // lbn
      {{1, -1, 1}, green},  // rbn
      {{1, 1, 1}, blue},    // rtn
      {{-1, 1, 1}, yellow}, // ltn
      {{-1, -1, -1}, pink}, // lbf
      {{1, -1, -1}, cyan},  // rbf
      {{1, 1, -1}, white},  // rtf
      {{-1, 1, -1}, orange} // ltf
  };

  Mat4 t = translate({0, 0, -30});
  Mat4 r = rot_y(0.78f);
  Mat4 s = scale({2, 2, 2});
  Mat4 model = s * r * t;

  Mat4 view = camera.view();
  Mat4 projection = project();

  for (auto &v : vertices) {
    v.pos = model * v.pos;
    v.pos = view * v.pos;
    v.pos = projection * v.pos;

    v.pos.x /= v.pos.w;
    v.pos.y /= v.pos.w;
    v.pos.z /= v.pos.w;

    v.pos.x = (v.pos.x + 1) * WIDTH / 2;
    v.pos.y = (1 - v.pos.y) * HEIGHT / 2;
  }

  auto lbn = vertices[0];
  auto rbn = vertices[1];
  auto rtn = vertices[2];
  auto ltn = vertices[3];
  auto lbf = vertices[4];
  auto rbf = vertices[5];
  auto rtf = vertices[6];
  auto ltf = vertices[7];

  // FRONT
  triangle(buffer, lbn, rbn, rtn);
  triangle(buffer, lbn, rtn, ltn);

  // BACK
  triangle(buffer, ltf, lbf, rbf);
  triangle(buffer, ltf, rtf, rbf);

  // LEFT
  triangle(buffer, lbf, lbn, ltn);
  triangle(buffer, lbf, ltn, ltf);

  // RIGHT
  triangle(buffer, rbn, rbf, rtf);
  triangle(buffer, rbn, rtf, rtn);

  // BOTTOM
  triangle(buffer, lbf, rbf, rbn);
  triangle(buffer, lbf, rbn, lbn);

  // TOP
  triangle(buffer, ltn, rtn, rtf);
  triangle(buffer, ltn, rtf, ltf);
}
