#ifndef ENGINE_H
#define ENGINE_H

#include "platform.h"

/// @brief Initialize the engine
void engine_start();

/// @brief Update GameLogic, Physics, and Renderer.
/// @param window Collection of Renderer and World to perform with.
bool engine_tick(struct WINDOWINFO* window);

/// @brief Set the running state of the engine
/// @param state Running or Not Running
/// @note setting to false will shut the game down, and clear all resources.
void engine_set_running(bool state);

/// @brief Returns current state of the engine
bool engine_is_running();

/// @brief Closes the Engine, cleaning up resources | Call on Exit
void engine_close();

#endif
