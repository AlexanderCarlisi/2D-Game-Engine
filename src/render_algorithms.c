// https://stackoverflow.com/questions/1341399/rasterizing-a-2d-polygon
// https://en.wikipedia.org/wiki/Even%E2%80%93odd_rule

#include "render_algorithms.h"
#include <iso646.h>

void render_algo_test(struct WindowConfig* config, struct Shape* shape, uint32_t color) {
  if (shape == NULL) return;
  
  struct PoseVector* vertices = &shape->vertex_poses;
  if (vertices == NULL) return;
  
  size_t n = vertices->count;
  if (n < 3) {
    printf("\n\t Not enough Verticies \n");
    return;
  }

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
      : &vertices->data[i + 1];
    if (p1 == NULL) continue;
    
    float dy = (float) p1->y_pixels - (float) p0->y_pixels;
    float dx = (float) p1->x_pixels - (float) p0->x_pixels;
    struct Pose cursor = {.x_pixels=p0->x_pixels, .y_pixels=p0->y_pixels};
    
    // printf("curse: (%d, %d)", cursor.x_pixels, cursor.y_pixels);
    // printf("Slope: %zu (%f, %f)\n", i, dx, dy);
    // printf("p0: %zu (%d, %d)\n", i, p0->x_pixels, p0->y_pixels);
    // printf("p1: %zu (%d, %d)\n", i, p1->x_pixels, p1->y_pixels);

    float slope = 0;
    if (dx != 0) {
      slope = dy/dx;
    }
    
    // while (dy != 0 || dx != 0) {
    while (slope != 0) {
      dy = (float) p1->y_pixels - cursor.y_pixels;
      dx = (float) p1->x_pixels - cursor.x_pixels;
      if (dx == 0) break;
      slope = dy / dx;
      
      // printf("cursor: (%d, %d)\n", cursor.x_pixels, cursor.y_pixels);
      // printf("Slope: (%f, %f)\n", dx, dy);
      printf("Slope: %f\n", slope);
      // printf("p0: (%d, %d)\n", p0->x_pixels, p0->y_pixels);
      // printf("p1: (%d, %d)\n", p1->x_pixels, p1->y_pixels);
      
      render_draw_pixel(config, &cursor, color);

      if (dx > 0)
        cursor.x_pixels += slope;
      else if (dx < 0)
        cursor.x_pixels -= slope;
      
      if (dy > 0)
        cursor.y_pixels += slope;
      else if (dy < 0)
        cursor.y_pixels -= slope;
    }
  }
}

/// helper function, is point XY to the left of edge AB
bool _is_left(int x, int y, struct Pose a, struct Pose b) {
  return (( b.x_pixels - a.x_pixels) * (y - a.y_pixels) - (x - a.x_pixels) * (b.y_pixels - a.y_pixels)) > 0;
}

/// helper function
/// NoneZero Algorithm: https://en.wikipedia.org/wiki/Nonzero-rule
/// @return true if inside polygon; false otherwise
bool _nonzero_rule(int x, int y, struct Pose* vertices, int n) {
  int windingNumber = 0;

  for (int i = 0; i < n; i++) {
    struct Pose a = vertices[i];
    struct Pose b = vertices[(i + 1) % n];

    if (a.y_pixels < y && b.y_pixels >= y) {
      // is point left of edge AB
      if (_is_left(x, y, a, b)) windingNumber++;
      
    } else if (b.y_pixels < y) {
      if (!_is_left(x, y, a, b)) windingNumber--;
    }
  }

  return windingNumber != 0;
}


void render_algo_scanline(struct WindowConfig* config, struct Shape* shape, uint32_t color) {
  // only check pixels around Shape bounds

  // Find shape bounds
  struct Pose max = shape->vertex_poses.data[0];
  struct Pose min = shape->vertex_poses.data[0];
  for (size_t i = 1; i < shape->vertex_poses.count; i++) {
    struct Pose p = shape->vertex_poses.data[i];

    if (max.x_pixels < p.x_pixels) {
      max.x_pixels = p.x_pixels;
    }

    if (max.y_pixels < p.y_pixels) {
      max.y_pixels = p.y_pixels;
    }

    if (min.x_pixels > p.x_pixels) {
      min.x_pixels = p.x_pixels;
    }

    if (min.y_pixels > p.y_pixels) {
      min.y_pixels = p.y_pixels;
    }
  }

  // Render
  struct Pose cursor = {
    .x_pixels = min.x_pixels,
    .y_pixels = min.y_pixels
  };
  
  while (cursor.y_pixels <= max.y_pixels) {
    if (_nonzero_rule(cursor.x_pixels, cursor.y_pixels, shape->vertex_poses.data, shape->vertex_poses.count)) {
      render_draw_pixel(config, &cursor, color);
    }

    cursor.x_pixels++;
    if (cursor.x_pixels > max.x_pixels) {
      cursor.x_pixels = min.x_pixels;
      cursor.y_pixels++;
    }
  }
}


// void render_algo_scanline(struct WindowConfig* config, struct Shape* shape, uint32_t color) {
//   // float maxWidth = 0;
//   // float maxHeight = 0;
//   // for (size_t i = 0; i < shape->normalized_vertices.count; i++) {
//   //   if (shape->normalized_vertices.data[i].x > maxWidth)
//   //     maxWidth = shape->normalized_vertices.data[i].x;
//   //
//   //   if (shape->normalized_vertices.data[i].y > maxHeight)
//   //     maxHeight = shape->normalized_vertices.data[i].y;
//   // }
//
//   struct Pose* leftMost = &shape->vertex_poses.data[0];
//   struct Pose* rightMost = &shape->vertex_poses.data[0];
//   for (size_t i = 1; i < shape->vertex_poses.count; i++) {
//     struct Pose* pose = &shape->vertex_poses.data[i];
//
//     if (leftMost->x_pixels < pose->x_pixels) {
//       leftMost->x_pixels = pose->x_pixels;
//     }
//     if (leftMost->y_pixels < pose->y_pixels) {
//       leftMost->y_pixels = pose->y_pixels;
//     }
//
//     if (rightMost->x_pixels < pose->x_pixels) {
//       rightMost->x_pixels = pose->x_pixels;
//     }
//     if (rightMost->y_pixels < pose->y_pixels) {
//       rightMost->y_pixels = pose->y_pixels;
//     }
//
//     printf("%zu : (%d, %d)", i, pose->x_pixels, pose->y_pixels);
//   }
//
//   uint16_t x = leftMost->x_pixels;
//   uint16_t y = leftMost->y_pixels;
//   printf("%d, %d", x, y);
//
//   while (x != rightMost->x_pixels || y != rightMost->y_pixels) {
//     printf("here");
//     if (_nonzero_rule(x, y, shape->vertex_poses.data, shape->vertex_poses.count)) {
//       struct Pose p = (struct Pose) {.x_pixels=x, .y_pixels=y};
//       render_draw_pixel(config, &p, color);
//     }
//     if ((x + 1) % rightMost->x_pixels == 0) {
//       y++;
//       x = leftMost->x_pixels;
//       continue;
//     }
//     x++;
//   }
//
//   // struct Interval bounds = (Interval) {
//   //   .start = origin->y_pixels * config->render_aspect.width + origin->x_pixels,
//   //   .end = (origin->y_pixels * config->render_aspect.width + origin->x_pixels) 
//   //     + ((shape->size.y_pixels * maxHeight) * config->render_aspect.width + (shape->size.x_pixels * maxWidth))
//   // };
//   //
//   // for (size_t i = bounds.start; i < bounds.end; i++) {
//   //   // rows 5, cols 4
//   //   // 5 * 4 = 20 pixels
//   //   // @17
//   //   // 17 / 5 = 3.truncated = y
//   //   // 17 % 5 = 2 = x
//   //   struct Pose pose = (struct Pose) {
//   //     .x_pixels = i % config->render_aspect.width,
//   //     .y_pixels = i / config->render_aspect.width
//   //   };
//   //
//   //   if (_nonzero_rule(pose.x_pixels, pose.y_pixels, shape->vertex_poses.data, shape->vertex_poses.count)) {
//   //     render_draw_pixel(config, &pose, color);
//   //   }
//   // }
// }





