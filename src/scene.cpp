#include "../include/scene.hpp"
#include "../include/math.hpp"
#include "rasterizer.hpp"

void draw_scene(FrameBuffer &buffer) {
  Vertex lbn = {{-1, -1, -1}}; // left  bottom near
  Vertex rbn = {{1, -1, -1}};  // right bottom near
  Vertex rtn = {{1, 1, -1}};   // right top    near
  Vertex ltn = {{-1, 1, -1}};  // left  top    near
  Vertex lbf = {{-1, -1, 1}};  // left  bottom far
  Vertex rbf = {{1, -1, 1}};   // right bottom far
  Vertex rtf = {{1, 1, 1}};    // right top    far
  Vertex ltf = {{-1, 1, 1}};   // left  top    far

  Color red = {255, 220, 50, 50};
  Color orange = {255, 230, 140, 40};
  Color yellow = {255, 220, 200, 50};
  Color green = {255, 50, 180, 80};
  Color blue = {255, 50, 100, 220};
  Color violet = {255, 160, 60, 220};

  auto face = [](Vertex v, Color c) -> Vertex {
    v.color = c;
    return v;
  };

  Vertex vertices[]{lbn, rbn, rtn, ltn, lbf, rbf, rtf, ltf};

  Mat4 t = translate({0, 0, -10});

  for (auto &v : vertices) {
    Vec4 x{t * v.pos};
    v.pos = x;

    // TODO: implement projection matrix
  }

  lbn = vertices[0];
  rbn = vertices[1];
  rtn = vertices[2];
  ltn = vertices[3];
  lbf = vertices[4];
  rbf = vertices[5];
  rtf = vertices[6];
  ltf = vertices[7];

  // FRONT  (z = -1, normal points toward -z, i.e. toward viewer)
  triangle(buffer, face(lbn, red), face(rbn, red), face(rtn, red));
  triangle(buffer, face(lbn, red), face(rtn, red), face(ltn, red));

  // BACK   (z = +1, normal points toward +z)
  triangle(buffer, face(rbf, orange), face(lbf, orange), face(ltf, orange));
  triangle(buffer, face(rbf, orange), face(ltf, orange), face(rtf, orange));

  // LEFT   (x = -1, normal points toward -x)
  triangle(buffer, face(lbf, yellow), face(lbn, yellow), face(ltn, yellow));
  triangle(buffer, face(lbf, yellow), face(ltn, yellow), face(ltf, yellow));

  // RIGHT  (x = +1, normal points toward +x)
  triangle(buffer, face(rbn, green), face(rbf, green), face(rtf, green));
  triangle(buffer, face(rbn, green), face(rtf, green), face(rtn, green));

  // BOTTOM (y = -1, normal points toward -y)
  triangle(buffer, face(lbf, blue), face(rbf, blue), face(rbn, blue));
  triangle(buffer, face(lbf, blue), face(rbn, blue), face(lbn, blue));

  // TOP    (y = +1, normal points toward +y)
  triangle(buffer, face(ltn, violet), face(rtn, violet), face(rtf, violet));
  triangle(buffer, face(ltn, violet), face(rtf, violet), face(ltf, violet));
}
