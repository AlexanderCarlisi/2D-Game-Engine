#ifndef RENDER_ALGO_H
#define RENDER_ALGO_H

#include "shape.h"
#include "render.h"

/// The first rendering implementation, will be me trying to intuitively
/// draw shapes onto the screen, and see how far I can get, before consulting
/// the wikipedia gods on rendering algorithms.
/// @param config WindowConfig for rendering
/// @param origin Current Position of the GameObject being rendered
/// @param shape Shape to render
/// @param color Color to render Convex Hull as
void render_algo_test(struct WindowConfig* config, struct Shape* shape, uint32_t color);

#endif // RENDER_ALGO_H
