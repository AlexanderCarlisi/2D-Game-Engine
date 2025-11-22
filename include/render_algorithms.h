#ifndef RENDER_ALGO_H
#define RENDER_ALGO_H

#include "shape.h"
#include "render.h"

/// Scanline algorithm for drawing shapes
/// Calls render_draw_pixel
/// @param config
/// @param shape
/// @param color
void render_algo_scanline(struct WindowConfig* config, struct Shape* shape, uint32_t color);

#endif // RENDER_ALGO_H
