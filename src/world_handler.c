#include "world_handler.h"
#include <math.h>
#include <stdio.h>

DEFINE_VECTOR(WorldVector, vector_world, struct World);

struct WorldHandler* world_handler_new() {
  WorldHandler* wh = (WorldHandler*) calloc(1, sizeof(WorldHandler));
  if (wh == NULL) {
    printf("\n>>> world_handler_new: Calloc Fail <<<\n");
    return NULL;
  }
  wh->active = NULL;
  wh->worlds.realloc_ratio = 2;
  vector_world_init(&wh->worlds, 1);
  return wh;
}

bool world_handler_set_active(struct WorldHandler* handler, struct World* world) {
  if (handler == NULL || world == NULL) return false;
  if (handler->active != NULL) {
    handler->active->close();
  }
  handler->active = world;
  handler->active->start();
  return true;
}

bool world_handler_new_world(struct WorldHandler* handler, struct WorldConfig config) {
  if (handler == NULL) return false;
  
  vector_world_add(&handler->worlds, (struct World) {0});
  return world_new(&handler->worlds.data[handler->worlds.count-1], config);
}

bool world_handler_free(struct WorldHandler* handler) {
  if (handler != NULL) {
    for (size_t i = 0; i < handler->worlds.count; i++) {
      struct World* wptr = &handler->worlds.data[i];

      if (wptr != NULL) {
        if (wptr->dealloc != NULL)
          wptr->dealloc();
        printf("\n>>> world_handler_free : dealloc %zu world <<<\n", i);
      }
    }
    
    vector_world_free(&handler->worlds);
    printf("\n>>> world_handler_free : dealloc worlds vec <<<\n");
    
    free(handler);
    printf("\n>>> world_handler_free : dealloc handler <<<\n");
  }
  return true;
}

void _update_vertex_poses(struct World* world, struct GameObject* object) {
  if (world == NULL || object == NULL) return;
  CollisionBoxVector* cbv = &object->collider_vector;
  struct Pose* origin = &object->pose;
  
  for (size_t c = 0; c < cbv->count; c++) {
    struct CollisionBox* cb = &cbv->data[c];
    for (size_t i = 0; i < cb->shape.vertex_poses.count; i++) {
      struct PoseVector* pv = &cb->shape.vertex_poses;
      struct VertexVector* vv = &cb->shape.normalized_vertices;
      
      if (cb == NULL || vv == NULL || pv == NULL) return;

      // int xPixels = origin->x_pixels + cb->shape.size.x_pixels * vv->data[i].x * cos(object->rotation_degrees * SHAPE_PI / 180);
      // int yPixels = origin->y_pixels + cb->shape.size.y_pixels * vv->data[i].y * sin(object->rotation_degrees * SHAPE_PI / 180);
      int xPixels = origin->x_pixels + cb->shape.size.x_pixels * vv->data[i].x;
      int yPixels = origin->y_pixels + cb->shape.size.y_pixels * vv->data[i].y;

      xPixels += (cb->shape.size.x_pixels * vv->data[i].x * cos(object->rotation_degrees * SHAPE_PI / 180));
      yPixels += cb->shape.size.y_pixels * vv->data[i].y * sin(object->rotation_degrees * SHAPE_PI / 180);

      // printf("%zu vv(%f, %f) pv(%d, %d)\n", i, vv->data[i].x, vv->data[i].y, xPixels, yPixels);
      
      pv->data[i] = pose_from_pixels(
        world->config.pixels_per_meter,
        xPixels,
        yPixels
      );
    }
  }
}

bool world_handler_update_active(struct WorldHandler* handler) {
  if (handler == NULL || handler->active == NULL) return false;
  
  // For each game object
  //  for each collision box
  //    update their Shape's vertex poses
  struct World* w = handler->active;
  for (size_t i = w->interval_buffer.start; i < w->interval_buffer.end; i++) {
    _update_vertex_poses(w, world_buffer_get_object(w, i));
  }
  for (size_t i = w->interval_pool.start; i < w->interval_pool.end; i++) {
    _update_vertex_poses(w, world_pool_get_object(w, i));
  }

  handler->active->loop();

  return true;
}
