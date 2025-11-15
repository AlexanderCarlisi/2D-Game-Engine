#include "world_handler.h"
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
