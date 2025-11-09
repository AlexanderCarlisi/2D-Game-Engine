#include "render.h"
#include "string.h"
#include <stdio.h>

void render_clear(struct WindowConfig* window, uint32_t color) {
  if (window == NULL) return;
  size_t size = window->render_aspect.width * window->render_aspect.height;
  uint32_t* framebuffer = window->framebuffer;

  if (framebuffer == NULL) {
    printf("\n>>> FRAMEBUFFER NULL <<<\n");
    return;
  }

  // Fill a small buffer with the color, then copy across
  uint32_t buffer[8];
  for (int i = 0; i < 8; i++) {
    buffer[i] = color;
  }
  for (size_t i = 0; i < size; i += 8) {
    memcpy(&framebuffer[i], buffer, sizeof(buffer));
  }
}

void render_draw_pixel(struct WindowConfig* window, Pose* point, uint32_t color) {
  if (window == NULL) return;
  struct Aspect res = window->render_aspect;
  if (!pose_pixels_in_bounds(point, res.width, res.height)) return;
  window->framebuffer[point->y_pixels * res.width + point->x_pixels] = color;
}

void render_interpolate_pixel(struct WindowConfig* window, struct Pose* point1, struct Pose* point2, uint32_t color, float alpha) {
  if (window == NULL) return;
  int x = (int) ((point2->x_pixels - point1->x_pixels) * alpha);
  int y = (int) ((point2->y_pixels - point1->x_pixels) * alpha);
  Pose p = pose_from_pixels(window->world_handler->active->config.pixels_per_meter, x, y); // defeated the entire purpose lmao
  render_draw_pixel(window, &p, color);
}

void render_draw_shape(struct WindowConfig* window, struct Shape* shape, uint32_t color, int vertices) {
  // TODO:
}

/// @brief Helper function for drawing Game Objects
/// TODO: Shape Rendering
void _draw_object(struct WindowConfig* window, struct GameObject* object, float alpha) {
    for (size_t i = 0; i < object->collider_vector.count; i++) {
      struct CollisionBox cb = object->collider_vector.data[i];

      // TESTING
      render_interpolate_pixel(window, &object->pose, &object->previous_pose, cb.shape.color, alpha);
    }
}

void render_draw(struct WindowConfig* window, float alpha) {
  if (window == NULL || window->world_handler == NULL) {
    printf("\n>>> WINDOW or HANDLER is NULL <<<\n");
    return;
  }
  
  struct World* world = window->world_handler->active;
  struct Interval interval_buffer = world->interval_buffer;
  struct Interval interval_pool = world->interval_pool;

  for (size_t i = interval_buffer.start; i < interval_buffer.end; i++) {
    struct GameObject* object = world_buffer_get_object(world, i);
    _draw_object(window, object, alpha);
  }

  for (size_t i = interval_pool.start; i < interval_pool.end; i++) {
    struct GameObject* object = world_pool_get_object(world, i);
    _draw_object(window, object, alpha);
  }
}
