#include "render.h"
#include "string.h"
#include <stdio.h>

void render_clear(struct WindowConfig* config, uint32_t color) {
  for (int y = 0; y < config->render_aspect.height; y++) {
    for (int x = 0; x < config->render_aspect.width; x++) {
      config->framebuffer[y * config->render_aspect.width + x] = color;
    }
  }
}

void render_draw_pixel(struct WindowConfig* config, Pose* point, uint32_t color) {
  if (config == NULL) return;
  struct Aspect res = config->render_aspect;
  if (!pose_pixels_in_bounds(point, res.width, res.height)) return;
  config->framebuffer[point->y_pixels * res.width + point->x_pixels] = color;
}

void render_interpolate_pixel(struct WindowConfig* config, struct Pose* point1, struct Pose* point2, uint32_t color, float alpha) {
  if (config == NULL) return;
  int x = (int) ((point2->x_pixels - point1->x_pixels) * alpha);
  int y = (int) ((point2->y_pixels - point1->x_pixels) * alpha);
  Pose p = pose_from_pixels(config->world_handler->active->config.pixels_per_meter, x, y); // defeated the entire purpose lmao
  render_draw_pixel(config, &p, color);
}

void render_draw_shape(struct WindowConfig* config, struct Shape* shape, uint32_t color, int vertices) {
  // TODO:
}

/// @brief Helper function for drawing Game Objects
/// TODO: Shape Rendering
void _draw_object(struct WindowConfig* config, struct GameObject* object, float alpha) {
    for (size_t i = 0; i < object->collider_vector.count; i++) {
      struct CollisionBox cb = object->collider_vector.data[i];

      // TESTING
      render_interpolate_pixel(config, &object->pose, &object->previous_pose, cb.shape.color, alpha);
    }
}

void render_draw(struct WindowConfig* config, float alpha) {
  if (config == NULL || config->world_handler == NULL) {
    printf("\n>>> config or HANDLER is NULL <<<\n");
    return;
  }
  
  struct World* world = config->world_handler->active;
  struct Interval interval_buffer = world->interval_buffer;
  struct Interval interval_pool = world->interval_pool;

  for (size_t i = interval_buffer.start; i < interval_buffer.end; i++) {
    struct GameObject* object = world_buffer_get_object(world, i);
    _draw_object(config, object, alpha);
  }

  for (size_t i = interval_pool.start; i < interval_pool.end; i++) {
    struct GameObject* object = world_pool_get_object(world, i);
    _draw_object(config, object, alpha);
  }
}
