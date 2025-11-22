#ifndef RENDER_H
#define RENDER_H

#include "platform.h"
#include "pose.h"

/// @brief Clear the framebuffer with the color
void render_clear(struct WindowConfig* config, uint32_t color);

/// @brief Writes color to the framebuffer at the point
void render_draw_pixel(struct WindowConfig* config, Pose* point, uint32_t color);

/// @brief calls render_draw_pixel whilst interpolating between point 1 and point 2
void render_interpolate_pixel(struct WindowConfig* config, struct Pose* point1, struct Pose* point2, uint32_t color, float alpha);

/// @brief Runs a @see render_algorithms.h 
void render_draw_shape(struct WindowConfig* config, struct Shape* shape, uint32_t color);

/// @brief Draw all active game objects attached to active World from World handler
void render_draw(struct WindowConfig* config, float alpha);

#endif
