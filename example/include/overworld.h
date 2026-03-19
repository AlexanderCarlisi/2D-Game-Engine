#ifndef OVERWORLD_H
#include "platform.h"
#include "logger.h"
#include "color.h"

#define OVERWORLD_HANDLER_INDEX 0

/// @brief Add the Overworld to the WorldHandler
/// @param index in the World Handler to be
/// @see world_handler.h
void overworld_setup_handle(WorldHandler* handler);
WorldConfig overworld_config();

/// Function Definitions for a World
/// @see world.h for information on these functions.
void overworld_init();
void overworld_start();
void overworld_loop();
void overworld_close();
void overworld_free();

#endif // OVERWORLD_H