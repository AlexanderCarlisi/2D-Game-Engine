#include "render_algorithms.h"

void render_algo_test(struct WindowConfig* config, struct Shape* shape, uint32_t color) {
  if (shape == NULL) return;
  
  struct VertexVector* vertices = &shape->normalized_vertices;
  if (vertices == NULL) return;
  
  size_t n = vertices->count;
  if (n < 3) return;

  // Simple idea
  // Vertices go from left to right
  // bottom to top
  //
  // p0 -> p1 makes a right triangle on the X axis
  // find the angle, then compute each pixel between p0 and p1
  //
  // or find the slope between p0 and p1, draw that.

  for (size_t i = 0; i < n; i++) {
    const struct Vertex* p0 = &vertices->data[i];
    if (p0 == NULL) continue;
    
    const struct Vertex* p1 = (i == n - 1) // Wrap to 0
      ? &vertices->data[0]
      : &vertices->data[i];
    if (p1 == NULL) continue;
    
    float dy = (float) p1->y - p0->y;
    float dx = (float) p1->x - p0->x;
    struct Vertex cursor = {.x=p0->x, .y=p0->y};
    
    // printf("dy: %f, dx: %f, cursor: %d, %d", dy, dx, cursor.x, cursor.y);

    while (cursor.x < p1->x && cursor.y < p1->y) {
      struct Pose cursorPose = pose_from_pixels(20, cursor.x, cursor.y);
      render_draw_pixel(config, &cursorPose, color);
      cursor.x += dx;
      cursor.y += dy;
    }

    printf("done");
  }
}
