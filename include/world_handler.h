#ifndef WORLD_HANDLER_H
#define WORLD_HANDLER_H

#include "vector.h"
#include "world.h"

DECLARE_VECTOR(WorldVector, vector_world, struct World);

/// The handler for all instantiated Worlds.
/// stores all the worlds into a WorldVector.
///
/// Only 1 World can be declared as active at a time.
/// Worlds can be pre-loaded, which will simply call the init function.
/// @see world.h for info on structuring your world.
/// 
/// Worlds exist within the @var worlds.
/// You will access them via pointers.
///
/// You are responsible for worlds during runtime.
/// Once engine closes all will be handled.
typedef struct WorldHandler {
  struct WorldVector worlds;
  struct World* active;
} WorldHandler;

/// @brief Allocate a new WorldHandler
struct WorldHandler* world_handler_new();

/// @brief Sets a new world as active
/// Calls active->close()
/// replaces the active pointer in WorldHandler.
/// Then calls active->start()
/// @note you should call world_handler_load first!
/// @note This will not deallocate the currently active world.
/// @return false if you passed an invalid argument in.
bool world_handler_set_active(struct WorldHandler* handler, struct World* world);

/// @brief Allocates and adds a new world to the worlds vector.
bool world_handler_new_world(struct WorldHandler* handler, struct WorldConfig config);

/// @brief Deallocates all worlds stored in the handler's worlds vector.
bool world_handler_free(struct WorldHandler* handler);

#endif // WORLD_HANDLER_H
