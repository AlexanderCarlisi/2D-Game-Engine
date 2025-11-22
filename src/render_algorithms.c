// https://stackoverflow.com/questions/1341399/rasterizing-a-2d-polygon
// https://en.wikipedia.org/wiki/Even%E2%80%93odd_rule

#include "render_algorithms.h"

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
