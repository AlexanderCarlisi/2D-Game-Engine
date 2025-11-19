#include "render_algorithms.h"

void render_algo_test(struct WindowConfig* config, struct Shape* shape, uint32_t color) {
  if (shape == NULL) return;
  
  struct PoseVector* vertices = &shape->vertex_poses;
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
    const struct Pose* p0 = &vertices->data[i];
    if (p0 == NULL) continue;
    
    const struct Pose* p1 = (i == n - 1) // Wrap to 0
      ? &vertices->data[0]
      : &vertices->data[i];
    if (p1 == NULL) continue;
    
    float dy = (float) p1->y_pixels - p0->y_pixels;
    float dx = (float) p1->x_pixels - p0->x_pixels;
    
    struct Pose cursor = {.x_pixels=p0->x_pixels, .y_pixels=p0->y_pixels};
    // printf("curse: (%d, %d)", cursor.x_pixels, cursor.y_pixels);
    // printf("Slope: (%f, %f)", dy, dx);
    // printf("p0: %zu (%d, %d)\n", i, p0->x_pixels, p0->y_pixels);
    // printf("p1: %zu (%d, %d)\n", i, p1->x_pixels, p1->y_pixels);
    while (
      (dx > 0 && cursor.x_pixels < p1->x_pixels)
      || (dx < 0 && cursor.x_pixels > p1->x_pixels)
      || (dy > 0 && cursor.y_pixels < p1->y_pixels)
      || (dy < 0 && cursor.y_pixels > p1->y_pixels)
    ) {
      // printf("cursor: (%d, %d)\n", cursor.x_pixels, cursor.y_pixels);
      // printf("DY,DX: (%f, %f)\n", dy, dx);
      // printf("p0: (%d, %d)\n", p0->x_pixels, p0->y_pixels);
      // printf("p1: (%d, %d)\n", p1->x_pixels, p1->y_pixels);
      render_draw_pixel(config, &cursor, color);

      if (dx > 0)
        cursor.x_pixels++;
      else if (dx < 0)
        cursor.x_pixels--;
      
      if (dy > 0)
        cursor.y_pixels++;
      else if (dy < 0)
        cursor.y_pixels--;
    }
  }
}
