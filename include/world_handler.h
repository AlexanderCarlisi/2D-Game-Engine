#ifndef WORLD_HANDLER_H
#define WORLD_HANDLER_H

#include "vector.h"
#include "world.h"

DECLARE_VECTOR(WorldVector, vector_world, struct World);

/// @struct WorldHandler
/// <p>
/// The handler for all instantiated Worlds.
/// stores all the worlds into a WorldVector.
/// <p>
/// Only 1 World can be declared as active at a time.
/// Worlds can be pre-loaded, which will simply call the init function.
/// @see world.h for info on structuring your world.
/// <p>
/// Worlds exist within the @var worlds.
/// You will access them via pointers.
/// <p>
/// You are responsible for calling world->init, to pre-load them.
/// <p>
/// When a world is set as active, if there is a currently active world
///   that world will have its close function called on it. Then start will
///   be called on the new world.
///   Then loop will be called by the engine.
/// <p>
/// If you wish to remove a world from the World Handler, you are responsible for
///   deallocating it's resources.
/// <p>
/// @note DEALLOCATED on clean close
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
/// @param handler
/// @param world
/// @note you are responsible for calling world->init for any pre-loading
/// @note This will not deallocate the currently active world.
/// @return false on invalid argument
bool world_handler_set_active(struct WorldHandler* handler, struct World* world);

/// @brief Allocates and adds a new world to the worlds vector.
/// @param handler
/// @param config Deepcopied WorldConfig to new world
/// @return false on allocation failure
bool world_handler_new_world(struct WorldHandler* handler, struct WorldConfig config);

/// @brief Deallocates all worlds stored in the handler's worlds vector.
/// @param handler
/// @note This is called on clean close by @see platform.c.
/// @return false on deallocation failure
bool world_handler_free(struct WorldHandler* handler);

/// @brief Update the current active world. Runs world->loop.
/// Handles physics updates, updates shape poses
/// @param handler
/// @return false when some struct member is NULL
bool world_handler_update_active(struct WorldHandler* handler);

#endif // WORLD_HANDLER_H
